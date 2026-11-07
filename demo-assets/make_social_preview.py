"""Create the 1280x640 GitHub social preview image.

The image uses a real TaskFlow screenshot, not a mockup. Regenerate the
screenshot first if the UI changes:

    py demo-assets/record_demo.py --binary ./taskflow.exe
    py demo-assets/make_social_preview.py
"""

from __future__ import annotations

import os
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

WIDTH, HEIGHT = 1280, 640
BACKGROUND = "#14161a"
PANEL = "#1e1e1e"
BORDER = "#343a40"
TEXT = "#f2f2f2"
MUTED = "#b8c0cc"
CYAN = "#9aecf0"
PURPLE = "#d670d6"
LINK = "#7aa2ff"


def font(name: str, size: int) -> ImageFont.FreeTypeFont:
    candidates = [
        Path(os.environ.get("WINDIR", r"C:\Windows")) / "Fonts" / name,
        Path("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"),
    ]
    for candidate in candidates:
        if candidate.exists():
            return ImageFont.truetype(str(candidate), size)
    raise FileNotFoundError(f"Could not find a monospace font: {name}")


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    screenshot_path = root / "demo-assets" / "screenshots" / "02-view-tasks.png"
    if not screenshot_path.exists():
        raise SystemExit(
            f"Missing {screenshot_path}. Run record_demo.py first."
        )

    image = Image.new("RGB", (WIDTH, HEIGHT), BACKGROUND)
    draw = ImageDraw.Draw(image)

    # Soft horizontal accent behind the title area.
    for x in range(0, 560):
        alpha = int(18 * (1 - x / 560))
        draw.line([(x, 0), (x, HEIGHT)], fill=(20 + alpha, 26 + alpha, 34 + alpha))

    draw.rectangle([64, 96, 70, 544], fill=CYAN)

    title_font = font("consolab.ttf", 78)
    subtitle_font = font("consola.ttf", 22)
    body_font = font("consola.ttf", 20)
    chip_font = font("consolab.ttf", 16)
    link_font = font("consola.ttf", 18)

    draw.text((102, 92), "TASKFLOW", font=title_font, fill=TEXT)
    draw.text(
        (102, 196),
        "A colorful C++17 console to-do manager",
        font=subtitle_font,
        fill=CYAN,
    )
    draw.text(
        (102, 250),
        "Add tasks, track progress, and finish work",
        font=body_font,
        fill=MUTED,
    )
    draw.text((102, 278), "from your terminal.", font=body_font, fill=MUTED)

    chips = [("C++17", CYAN), ("Windows", PURPLE), ("Linux", PURPLE)]
    chip_x = 102
    for label, colour in chips:
        text_width = draw.textlength(label, font=chip_font)
        draw.rounded_rectangle(
            [chip_x, 354, chip_x + text_width + 28, 386],
            radius=8,
            outline=colour,
            width=1,
            fill="#1d232b",
        )
        draw.text(
            (chip_x + 14, 361),
            label,
            font=chip_font,
            fill=colour,
        )
        chip_x += text_width + 46

    draw.text(
        (102, 560),
        "github.com/ShahabAhmed01/To-Do-List-Application",
        font=link_font,
        fill=LINK,
    )

    screenshot = Image.open(screenshot_path).convert("RGB")
    target_width = 600
    target_height = round(screenshot.height * target_width / screenshot.width)
    screenshot = screenshot.resize((target_width, target_height), Image.LANCZOS)

    panel_x = 620
    panel_y = (HEIGHT - target_height) // 2
    draw.rounded_rectangle(
        [panel_x - 3, panel_y - 3, panel_x + target_width + 3,
         panel_y + target_height + 3],
        radius=10,
        fill=PANEL,
        outline=BORDER,
        width=2,
    )
    image.paste(screenshot, (panel_x, panel_y))

    output = root / "demo-assets" / "social-preview.png"
    image.save(output, optimize=True)
    print(f"Wrote {output} ({output.stat().st_size / 1024:.0f} KiB)")


if __name__ == "__main__":
    main()
