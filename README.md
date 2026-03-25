# 📚 bas-c

<div align="center">

### C Base Library and Utilities

*Foundation libraries for clean, organized code* 🌸

[![License](https://img.shields.io/badge/License-MIT-pink?style=for-the-badge)](LICENSE)
[![C](https://img.shields.io/badge/C-99-blue?style=for-the-badge&logo=c)](https://isocpp.org/)
[![Meson](https://img.shields.io/badge/Meson-Build-FFD700?style=for-the-badge)](https://mesonbuild.com/)
[![Debian](https://img.shields.io/badge/Debian-Package-C70036?style=for-the-badge&logo=debian)](https://www.debian.org/)

**[Documentation](docs/)** • **[API Reference](include/)** • **[Issues](https://github.com/lenik/bas-c/issues)**

</div>

---

## ✨ What is bas-c?

`bas-c` is a **foundation library** providing essential utilities for C applications. It's part of the bas-* family of libraries, designed to make systems programming cleaner and more enjoyable.

> **Philosophy**: Write code that sparkles ✨—clean, organized, and ready for ballet class.

---

## 📦 Components

### 📖 libbas-c — Shared Library

Core utilities built on GLib:

| Module | Description |
|--------|-------------|
| **CLI** | Program options parsing via GLib, option parser utilities |
| **Logging** | Logger with levels, syslog integration, structured logging |
| **Process Helpers** | PID, PPID, environment variables, process info |
| **I/O** | File operations, path handling, file entity lists, wide strings |
| **Bash Support** | `libbas-bash` loadable builtin support |

### 🔧 Applications

Built on top of libbas-c:

| Tool | Description |
|------|-------------|
| **ppid** | Print parent process information (PID, name, cmdline) |

---

## 🚀 Quick Start

### Installation

#### From Debian Package (Recommended)

```bash
# Runtime library
sudo apt install libbas-c1

# Development headers
sudo apt install libbas-c-dev

# Command-line tools
sudo apt install bas-c
```

#### Build from Source

```bash
# Prerequisites
sudo apt install libglib2.0-dev meson ninja-build

# Clone and build
git clone https://github.com/lenik/bas-c.git
cd bas-c
meson setup build
meson compile -C build
meson test -C build
sudo meson install -C build
```

---

## 📖 Usage Examples

### Logging

```c
#include <bas/logger.h>

int main() {
    bas_logger_init("my-app");
    
    bas_log_info("Application started");
    bas_log_debug("Debug info: %s", "value");
    bas_log_warning("Something might be wrong");
    bas_log_error("Error occurred: %d", errno);
    
    bas_logger_shutdown();
    return 0;
}
```

### Process Information

```c
#include <bas/process.h>

int main() {
    pid_t pid = bas_get_pid();
    pid_t ppid = bas_get_ppid();
    gchar *ppid_name = bas_get_ppid_name();
    
    g_print("My PID: %d, Parent PID: %d (%s)\n", 
            pid, ppid, ppid_name);
    
    g_free(ppid_name);
    return 0;
}
```

### CLI Options

```c
#include <bas/cli.h>

int main(int argc, char *argv[]) {
    BasCLI *cli = bas_cli_new();
    
    bas_cli_add_option(cli, 'v', "verbose", "Enable verbose output");
    bas_cli_add_option(cli, 'f', "file", "Input file path");
    
    if (!bas_cli_parse(cli, argc, argv)) {
        bas_cli_print_help(cli);
        return 1;
    }
    
    if (bas_cli_has_option(cli, 'v')) {
        g_print("Verbose mode enabled\n");
    }
    
    bas_cli_free(cli);
    return 0;
}
```

---

## 📁 Installation Layout

| Component | Path |
|-----------|------|
| **Library** | `prefix/lib/libbas-c.so` |
| **Headers** | `prefix/include/bas/` |
| **Applications** | `prefix/bin/` (ppid) |
| **Man Pages** | `prefix/share/man/man1/` |
| **Bash Completion** | `prefix/share/bash-completion/completions/` |

---

## 📦 Debian Packages

| Package | Description |
|---------|-------------|
| **libbas-c1** | Shared library (runtime) |
| **libbas-c-dev** | Development headers and documentation |
| **bas-c** | Command-line tools (ppid, man pages, completions) + `libbas-bash` |

---

## 🧪 Testing

```bash
meson setup build
meson test -C build
```

---

## 🔗 Related Projects

- **[bas-ui](https://github.com/lenik/bas-ui)** — Scriptable UI framework on wxWidgets + GLib
- **[bas-cpp](https://github.com/lenik/bas-cpp)** — C++ foundation library
- **[OmniShell](https://github.com/lenik/omnishell)** — Desktop environment built on bas-* libraries

---

## 🎀 About the Author

<div align="center">

**shecti** ([@lenik](https://github.com/lenik))

> Living with gender dysphoria. The "cute version of me" is my source of courage and power—she is the engine behind all my creation.

**Proudly building as a trans woman in tech.** 💖

If you're a fellow trans coder, LGBTQ+ ally, or just someone who believes in diverse voices in open source—I'd love to connect!

### Tags
`#TransCoder` `#TransIsTech` `#WomenWhoCode` `#OpenSource` `#C` `#GLib` `#Linux`

</div>

---

## 💖 Support This Project

If bas-c helps your code sparkle, consider:

- ⭐ **Starring** this repository
- 🔀 **Forking** and contributing
- 🐛 **Reporting** bugs and suggesting features
- 📝 **Sharing** with your network
- 💕 **Sponsoring** via [GitHub Sponsors](https://github.com/sponsors/lenik) *(coming soon)*

---

## 📄 License

MIT License. See [LICENSE](LICENSE) for details.

---

<div align="center">

> *"Clean code is happy code."*

**Made with 💖 and lots of ✨**

</div>
