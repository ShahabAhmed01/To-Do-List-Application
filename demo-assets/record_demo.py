"""Regenerate TaskFlow screenshots and the terminal demo GIF.

The script runs the real TaskFlow executable inside a Windows pseudo-terminal
(ConPTY), records its ANSI screen buffer, and renders the result with Pillow.
It is intentionally a development tool: it is not required to build or run
TaskFlow.

Requirements (Windows only, because of pywinpty):
    py -m pip install pillow pyte pywinpty

Build the application first, then run:
    py demo-assets/record_demo.py --binary ./taskflow.exe

The output is written to demo-assets/screenshots and demo-assets/demo.gif.
"""

from __future__ import annotations

import argparse
import ctypes
import os
import shutil
import tempfile
import threading
import time
from pathlib import Path

import pyte
from PIL import Image, ImageDraw, ImageFont
from winpty import PtyProcess

ROWS = 30
COLS = 84
BACKGROUND = "#1e1e1e"
FOREGROUND = "#cccccc"

PALETTE = {
    "default": FOREGROUND,
    "black": "#0c0c0c",
    "red": "#e74856",
    "green": "#16c60c",
    "brown": "#c19c00",
    "yellow": "#f9f1a5",
    "blue": "#3b78ff",
    "magenta": "#b4009e",
    "cyan": "#61d6d6",
    "white": "#cccccc",
    "brightblack": "#767676",
    "brightred": "#ff6b6b",
    "brightgreen": "#4cc94c",
    "brightbrown": "#f9f1a5",
    "brightblue": "#3b78ff",
    "brightmagenta": "#d670d6",
    "brightcyan": "#9aecf0",
    "brightwhite": "#f2f2f2",
}


def _short_path(path: Path) -> str:
    """Return the 8.3 path, if Windows provides one (pywinpty needs it)."""
    buffer = ctypes.create_unicode_buffer(32768)
    length = ctypes.windll.kernel32.GetShortPathNameW(
        str(path), buffer, len(buffer)
    )
    return buffer.value if length else str(path)


def _font_path(name: str) -> str:
    candidates = [
        Path(os.environ.get("WINDIR", r"C:\Windows")) / "Fonts" / name,
        Path("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"),
    ]
    for candidate in candidates:
        if candidate.exists():
            return str(candidate)
    raise FileNotFoundError(f"Could not find a monospace font: {name}")


class Terminal:
    def __init__(self, binary: Path):
        self._temporary = Path(tempfile.gettempdir()) / "taskflow-capture.exe"
        shutil.copy2(binary, self._temporary)
        self._process = PtyProcess.spawn(
            _short_path(self._temporary), dimensions=(ROWS, COLS)
        )
        self._screen = pyte.Screen(COLS, ROWS)
        self._stream = pyte.Stream(self._screen)
        self._chunks: list[str] = []
        self._lock = threading.Lock()
        self._reader = threading.Thread(target=self._read, daemon=True)
        self._reader.start()

    def _read(self) -> None:
        while True:
            try:
                data = self._process.read(65536)
            except Exception:
                break
            if not data:
                break
            with self._lock:
                self._chunks.append(data)

    def pump(self, wait: float = 0.7) -> None:
        time.sleep(wait)
        with self._lock:
            data = "".join(self._chunks)
            self._chunks.clear()
        if data:
            self._stream.feed(data)

    def send(self, text: str, wait: float = 0.7) -> None:
        self._process.write(text)
        self.pump(wait)

    def render(
        self,
        width: int,
        height: int,
        font_size: int,
        offset_x: int,
        offset_y: int,
    ) -> Image.Image:
        image = Image.new("RGB", (width, height), BACKGROUND)
        draw = ImageDraw.Draw(image)
        font = ImageFont.truetype(_font_path("consola.ttf"), font_size)
        bold = ImageFont.truetype(_font_path("consolab.ttf"), font_size)

        cell_width = width / COLS
        cell_height = height / ROWS
        line_width = max(1, int(font_size * 0.07))

        for y in range(ROWS):
            for x in range(COLS):
                cell = self._screen.buffer[y][x]
                char = cell.data
                if char in ("", "\x00", " "):
                    continue

                colour = PALETTE.get(cell.fg, PALETTE["default"])
                px = offset_x + x * cell_width
                py = offset_y + y * cell_height

                # Draw the characters the app uses to build its borders and
                # progress bar as continuous shapes rather than spaced glyphs.
                if char == "═":
                    draw.line(
                        [(px, py + cell_height * 0.32),
                         (px + cell_width, py + cell_height * 0.32)],
                        fill=colour,
                        width=line_width,
                    )
                    draw.line(
                        [(px, py + cell_height * 0.62),
                         (px + cell_width, py + cell_height * 0.62)],
                        fill=colour,
                        width=line_width,
                    )
                elif char == "─":
                    draw.line(
                        [(px, py + cell_height * 0.5),
                         (px + cell_width, py + cell_height * 0.5)],
                        fill=colour,
                        width=line_width,
                    )
                elif char == "█":
                    draw.rectangle(
                        [px, py + cell_height * 0.12,
                         px + cell_width, py + cell_height * 0.88],
                        fill=colour,
                    )
                elif char == "░":
                    step = max(2, int(cell_width / 4))
                    for dot_y in range(
                        int(py + cell_height * 0.12),
                        int(py + cell_height * 0.88),
                        step,
                    ):
                        for dot_x in range(int(px), int(px + cell_width), step):
                            draw.rectangle(
                                [dot_x, dot_y,
                                 min(dot_x + 1, px + cell_width),
                                 min(dot_y + 1, py + cell_height * 0.88)],
                                fill=colour,
                            )
                else:
                    draw.text(
                        (px, py),
                        char,
                        font=bold if cell.bold else font,
                        fill=colour,
                    )

        if not self._screen.cursor.hidden:
            cursor_x = offset_x + self._screen.cursor.x * cell_width
            cursor_y = offset_y + self._screen.cursor.y * cell_height
            draw.rectangle(
                [cursor_x, cursor_y + cell_height * 0.15,
                 cursor_x + max(2, cell_width * 0.08),
                 cursor_y + cell_height * 0.85],
                fill=FOREGROUND,
            )

        return image

    def close(self) -> None:
        try:
            self._process.close()
        finally:
            self._temporary.unlink(missing_ok=True)


def capture_screenshots(binary: Path, output: Path) -> None:
    terminal = Terminal(binary)
    try:
        terminal.pump(1.5)
        terminal.render(1280, 720, 22, 30, 8).save(
            output / "01-main-menu.png", optimize=True
        )

        first = "Buy groceries for the week"
        second = "Record the demo video"
        third = "Review the pull requests"

        terminal.send("1\r")
        terminal.send(first + "\r")
        terminal.send("\r")
        terminal.send("1\r")
        terminal.send(second + "\r")
        terminal.send("\r")

        terminal.send("1\r")
        terminal.send(third + "\r")
        terminal.render(1280, 720, 22, 30, 8).save(
            output / "03-add-task.png", optimize=True
        )
        terminal.send("\r")

        # Mark the second task complete so the view screenshot shows progress.
        terminal.send("3\r")
        terminal.send("\r")
        terminal.send("2\r")
        terminal.send("\r")
        terminal.send("2\r")
        terminal.render(1280, 720, 22, 30, 8).save(
            output / "02-view-tasks.png", optimize=True
        )
        terminal.send("\r")
        terminal.send("0\r")
        terminal.pump(0.5)
    finally:
        terminal.close()


def capture_gif(binary: Path, output: Path) -> None:
    terminal = Terminal(binary)
    frames: list[Image.Image] = []
    durations: list[int] = []

    def snapshot(milliseconds: int) -> None:
        frames.append(terminal.render(960, 540, 16, 24, 6))
        durations.append(milliseconds)

    def add_task(name: str, screenshot: bool = True) -> None:
        terminal.send("1\r")
        terminal.send(name + "\r")
        if screenshot:
            snapshot(700)
        terminal.send("\r")

    try:
        terminal.pump(1.5)
        snapshot(1000)

        # Add the first task with a short typewriter effect.
        terminal.send("1\r")
        terminal.send("Buy gro", 0.3)
        if frames:
            snapshot(350)
        terminal.send("ceries for the week\r", 0.4)
        snapshot(700)
        terminal.send("\r")

        # Add the remaining tasks between frames.
        add_task("Record the demo video", screenshot=False)
        add_task("Review the pull requests", screenshot=False)

        terminal.send("2\r")
        snapshot(1200)
        terminal.send("\r")

        # Mark the second task complete.
        terminal.send("3\r")
        snapshot(550)
        terminal.send("\r")
        terminal.send("2\r")
        snapshot(650)
        terminal.send("\r")

        terminal.send("2\r")
        snapshot(1400)
        terminal.send("\r")

        # Remove the completed task.
        terminal.send("4\r")
        terminal.send("\r")
        snapshot(600)
        terminal.send("2\r")
        snapshot(600)
        terminal.send("\r")

        terminal.send("2\r")
        snapshot(1000)
        terminal.send("\r")

        terminal.send("0\r")
        snapshot(900)
        terminal.pump(0.4)
    finally:
        terminal.close()

    if not frames:
        raise RuntimeError("No frames were captured.")

    palette_frames = [
        frame.convert("P", palette=Image.Palette.ADAPTIVE, colors=64)
        for frame in frames
    ]
    gif_path = output.parent / "demo.gif"
    palette_frames[0].save(
        gif_path,
        save_all=True,
        append_images=palette_frames[1:],
        duration=durations,
        loop=0,
        optimize=True,
        disposal=2,
    )
    print(f"Wrote {gif_path} ({gif_path.stat().st_size / 1024:.0f} KiB)")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--binary",
        type=Path,
        default=Path(__file__).resolve().parents[1] / "taskflow.exe",
        help="Path to a built TaskFlow executable.",
    )
    parser.add_argument(
        "--screenshots-only",
        action="store_true",
        help="Only regenerate the PNG screenshots.",
    )
    parser.add_argument(
        "--gif-only",
        action="store_true",
        help="Only regenerate demo.gif.",
    )
    args = parser.parse_args()

    if not args.binary.exists():
        raise SystemExit(
            f"Binary not found: {args.binary}\n"
            "Build it first, for example:\n"
            '  g++ -std=c++17 -O2 -o taskflow.exe "To-Do List Application.cpp"'
        )

    screenshots = Path(__file__).resolve().parent / "screenshots"
    screenshots.mkdir(parents=True, exist_ok=True)

    if not args.gif_only:
        capture_screenshots(args.binary, screenshots)
        print(f"Wrote screenshots to {screenshots}")

    if not args.screenshots_only:
        capture_gif(args.binary, screenshots)


if __name__ == "__main__":
    try:
        main()
    except ImportError as exc:
        raise SystemExit(
            "Missing dependency. Run: py -m pip install pillow pyte pywinpty"
        ) from exc
