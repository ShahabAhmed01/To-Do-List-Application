# Contributing to TaskFlow

Thanks for taking the time to contribute. TaskFlow is a small, beginner-friendly
C++17 console project, and focused fixes and improvements are welcome.

## Before you start

- Check the [issue tracker](https://github.com/ShahabAhmed01/To-Do-List-Application/issues)
  for existing reports and discussions.
- For larger changes, open an issue first so the approach can be discussed.
- Keep the project dependency-free and buildable from a single translation unit
  unless an issue explicitly calls for a different structure.
- Do not commit generated binaries, editor settings, or demo assets unrelated to
  your change.

## Development setup

You need a C++17 compiler. No third-party libraries are required.

Build on Windows with MinGW:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror -o taskflow.exe "To-Do List Application.cpp"
```

Build on Linux:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror -o taskflow "To-Do List Application.cpp"
```

Run the smoke tests:

```bash
# Linux
bash tests/smoke_test.sh ./taskflow
```

```powershell
# Windows
pwsh -File tests/smoke_test.ps1 -Binary ./taskflow.exe
```

## Making a change

1. Fork the repository and create a branch from `main`.
2. Make a focused change. Avoid unrelated reformatting or refactors.
3. Build with the strict flags above and run both applicable smoke tests.
4. Update the README or CHANGELOG if behavior changes.
5. Commit with a clear message, for example `fix: keep input usable after invalid choice`.
6. Open a pull request and describe what changed and how you tested it.

## Code style

- C++17, standard library only.
- Match the existing formatting, naming, and `#ifdef _WIN32` platform pattern.
- Prefer the existing `readInt` and `pauseConsole` helpers over raw `cin >>`
  or `system("pause")`.
- Keep console output ASCII except for the box-drawing characters already used
  for the UI frame and progress bar; avoid emoji so output stays readable in
  default terminal fonts.
- Do not add dead code or unused includes.

## Testing expectations

A pull request should include:

- The build command and compiler version used.
- The smoke test command and its result.
- Manual steps to reproduce the changed behavior, if the smoke tests do not
  cover it.
- Screenshots or a terminal recording for visible UI changes when practical.

CI runs GCC on Ubuntu and MSVC on Windows. A pull request is ready when both
jobs pass.

## Reporting bugs and requesting features

Use the issue templates. Include your operating system, compiler and version,
the exact build command, the input sequence, and what you expected versus what
happened. Please do not paste secrets, tokens, or private data.

Security issues should be reported privately as described in
[`SECURITY.md`](SECURITY.md).

## License of contributions

The repository does not have a license yet. By submitting a contribution you
agree that it may be distributed under the license the maintainer later
chooses for this project. If that is not acceptable, please wait until a
license is published before contributing.
