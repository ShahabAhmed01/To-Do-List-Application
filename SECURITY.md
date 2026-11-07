# Security Policy

## Supported versions

TaskFlow has no tagged releases yet. Security fixes are applied to the `main`
branch and the current development branch. Older snapshots are not supported.

## Reporting a vulnerability

Please do **not** open a public issue for a security problem.

Use GitHub's private vulnerability reporting form:

https://github.com/ShahabAhmed01/To-Do-List-Application/security/advisories/new

Include:

- A description of the issue and its impact.
- Steps to reproduce, including the operating system and compiler.
- The affected commit or branch.
- Any suggested fix.

You can expect an initial acknowledgement as soon as the maintainer is
available. If the report is valid, a fix will be prepared on a branch and
disclosed after it is merged.

## Scope

TaskFlow is a local console application. It does not use the network, read or
write task files, or handle credentials. The most relevant classes of issues
are:

- Crashes or hangs caused by crafted console input.
- Out-of-bounds access, integer overflow, or undefined behavior in task
  handling.
- Build or CI configuration that could expose repository credentials.

Issues in third-party tools used only for demo capture or CI are out of scope
for this project and should be reported to the relevant upstream project.

## Safe harbor

Good-faith security research that follows this policy will not be pursued or
reported. Please avoid privacy violations, data destruction, and disruption of
other users while testing.
