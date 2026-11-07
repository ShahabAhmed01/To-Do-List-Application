# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).
This project does not have tagged releases yet, so entries are grouped under
"Unreleased".

## [Unreleased]

### Added

- End-to-end smoke tests for Linux (`tests/smoke_test.sh`) and Windows
  (`tests/smoke_test.ps1`), covering quit, a full add/complete/view/remove
  flow, and invalid input.
- GitHub Actions CI that builds with GCC on Ubuntu and MSVC on Windows and runs
  the smoke tests.
- Dependabot configuration for monthly GitHub Actions updates.
- Reproducible demo capture tooling under `demo-assets/`, including screenshots,
  an animated GIF, and a 1280x640 social preview generated from real terminal
  output.
- Contributing, security, code of conduct, and issue/PR templates.

### Fixed

- Replaced the non-portable `system("pause")` calls with a `pauseConsole()`
  helper that reads a line, so the app no longer depends on the Windows shell
  command on Linux.
- Replaced `cin >>` parsing with a line-based `readInt()` helper. Invalid input
  such as `abc` now re-prompts instead of leaving the stream in a failed state
  and looping indefinitely.
- Fixed UTF-8 centering: `printCentered` now takes a string fill and measures
  UTF-8 code points instead of passing a box-drawing character as a single
  `char`, which produced compiler warnings and broken borders.
- Removed emoji and checkmark glyphs that rendered as empty boxes in default
  terminal fonts, while keeping the colored UI and box-drawing frame.
- Removed unused includes, the unused `getCurrentDate()` helper, and an unused
  `setColor` stub on non-Windows builds.
- Added spacing above section titles and validation for unknown removal options.
- Fixed MSVC warnings for console color attributes (`C4244`) and local time
  handling (`C4996`); the Windows CI build now treats warnings as errors.

### Changed

- POSIX screen clearing now uses ANSI escape codes instead of shelling out to
  `clear`.
- On Windows, both the input and output code pages are set to UTF-8.

### Documentation

- Rewrote the README with verified build commands, usage, testing, project
  structure, limitations, and an honest license-pending notice.
