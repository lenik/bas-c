# bas-c — C base library and utilities

This project provides:

- **libbas-c** — shared library with base utilities:
  - CLI (program/options via GLib, option parser)
  - Logging (logger, log levels, syslog)
  - Process helpers (pid, ppid, env)
  - I/O (file, path, fileent lists, wide strings)
  - Bash loadable builtin support (`libbas-bash` from `bb-util`)

- **Applications** (built on libbas-c):
  - `ppid` — print parent process information

## Build (Meson)

```sh
meson setup build
meson compile -C build
meson test -C build
meson install -C build   # or DESTDIR=... for packaging
```

## Install layout

- **Library**: `prefix/lib/libbas-c.so`
- **Headers**: `prefix/include/bas/`
- **Apps**: `prefix/bin/` (`ppid`)
- **Man pages**: `prefix/share/man/man1/`
- **Bash completion**: `prefix/share/bash-completion/completions/`

## Debian packages

- **libbas-c1**: shared library (runtime)
- **libbas-c-dev**: headers and docs
- **bas-c**: command-line tools (ppid, man pages, completions), plus `libbas-bash` bash loadable
