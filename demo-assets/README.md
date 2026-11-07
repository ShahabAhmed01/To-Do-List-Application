# Demo assets

This directory contains the visual assets used in the README and the scripts
that regenerate them.

| File | Purpose |
| --- | --- |
| `screenshots/01-main-menu.png` | Main menu, 1280x720 |
| `screenshots/02-view-tasks.png` | Task list with progress bar, 1280x720 |
| `screenshots/03-add-task.png` | Add-task confirmation, 1280x720 |
| `demo.gif` | Animated walkthrough, 960x540, 10 frames |
| `social-preview.png` | GitHub social preview, 1280x640 |
| `record_demo.py` | Captures the screenshots and GIF from the real app |
| `make_social_preview.py` | Composes the social preview from a real screenshot |

The screenshots and GIF are captured from a real TaskFlow process running in a
Windows pseudo-terminal (ConPTY). Text and colors come from the program's
actual output; they are not mocked up.

## Regenerating

The capture scripts require Windows because they use `pywinpty`. They are
development tools and are not needed to build or run TaskFlow.

```powershell
py -m pip install pillow pyte pywinpty

# Build the app first.
g++ -std=c++17 -O2 -o taskflow.exe "To-Do List Application.cpp"

# Screenshots and GIF.
py demo-assets/record_demo.py --binary .\taskflow.exe

# Social preview from the regenerated screenshot.
py demo-assets/make_social_preview.py
```

`record_demo.py` accepts `--screenshots-only` and `--gif-only` to regenerate
one kind of asset. It renders with Consolas from the Windows font directory.

## Notes

- The UI uses only ASCII labels plus box-drawing and progress-bar characters,
  so the captures render reliably in default terminal fonts.
- If you change visible text or layout, regenerate the screenshots, GIF, and
  social preview so the README stays accurate.
- The committed assets are optimized PNG/GIF files. Re-running the scripts may
  produce slightly different file sizes due to timestamps in the rendered
  header.
