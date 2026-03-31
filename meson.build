project('bas-c', 'c', version: '1.0.0', default_options: ['warning_level=0'])

pkgdatadir = get_option('datadir') / meson.project_name()

fs = import('fs')
cc = meson.get_compiler('c')

# Disable all warnings (wx 3.0 and deps are noisy)
c_nowarn = cc.get_supported_arguments(['-w', '/w'])

# zlib for ZIP support
zlib_dep = dependency('zlib', required: true)

# ICU for charset conversion (CharsetEncoder, CharsetDecoder, etc.)
icu_uc_dep = dependency('icu-uc', required: true)

# GLib for scriptable UI stack (GList)
glib_dep = dependency('glib-2.0', required: true)

# SDK libraries - detect architecture and use appropriate library directory
target_cpu = build_machine.cpu_family()

# Get C compiler for finding libraries
# (cc defined above)

# OpenSSL (crypto + ssl for HTTPS)
openssl_dep = dependency('openssl', required: true)

# libcurl for HTTP/HTTPS/FTP/FTPS (curl license = permissive)
libcurl_dep = dependency('libcurl', required: true)

# Include path for src/ so "log/log.h", "base/string_list.h" etc. resolve
exe_inc = [include_directories('src')]

# Bash headers for loadable builtins (bb-util.c).
bash_dep = dependency('bash', required: true)
bash_includedir = bash_dep.get_variable(pkgconfig: 'includedir')
# Like bash_dep but only the first includedir (avoids shadowing system <getopt.h>).
bash_dep_1 = declare_dependency(
    include_directories: include_directories(
        join_paths(bash_includedir, 'bash'),
        join_paths(bash_includedir, 'bash/include'),
        is_system: true,
    ),
    compile_args: ['-DSHELL', '-fPIC'],
)

# zlib is used for decompression, but we'll parse ZIP manually
c_args = c_nowarn + ['-DLOGGER_NAME=bas']

c_sources = files(
    'src/module.c',
    'src/bas/base/stdio.c',
    'src/bas/base/str.c',
    'src/bas/base/string_list.c',
    'src/bas/base/wstring.c',
    'src/bas/base/wstring_list.c',
    'src/bas/cli/args.c',
    'src/bas/cli/cli_parser.c',
    'src/bas/cli/opt_parser.c',
    'src/bas/cli/program.c',
    'src/bas/io/file.c',
    'src/bas/io/fileent_list.c',
    'src/bas/io/path.c',
    'src/bas/io/wfileent_list.c',
    'src/bas/locale/unicode.c',
    'src/bas/log/log.c',
    'src/bas/log/logger.c',
    'src/bas/log/syslog.c',
    'src/bas/proc/dbgaction.c',
    'src/bas/proc/dbgthread.c',
    'src/bas/proc/env.c',
    'src/bas/proc/format_backtrace.c',
    'src/bas/proc/format_gdb.c',
    'src/bas/proc/pid.c',
    'src/bas/proc/stackdump.c',
    'src/bas/util/prefix_map.c',
)

# Executable (optional wx media/webview only when found)
exe_deps = [
    glib_dep,
    icu_uc_dep,
    libcurl_dep,
    openssl_dep,
    zlib_dep,
]

override_options = ['b_lundef=false']  # Allows undefined symbols at link time

lib_bas_c = library(
    'bas-c',
    c_sources,
    dependencies: exe_deps,
    include_directories: exe_inc,
    c_args: c_args,
    override_options: override_options,
    version: meson.project_version(),
    soversion: '1',
    install: true,
)

lib_bas_c_static = static_library(
    'bas-c',
    c_sources,
    dependencies: exe_deps,
    include_directories: exe_inc,
    c_args: c_args,
    override_options: override_options,
    install: true,
)

bas_c_dep = declare_dependency(
    link_with: lib_bas_c,
    include_directories: exe_inc,
    dependencies: exe_deps,
)

bas_c_static_dep = declare_dependency(
    link_with: lib_bas_c_static,
    include_directories: exe_inc,
    dependencies: exe_deps,
)

meson.override_dependency('bas-c', bas_c_dep)
meson.override_dependency('bas-c-static', bas_c_static_dep)

# Bash loadable: libbas-bash (bb-util)
bas_bash = shared_library(
    'bas-bash',
    'src/bas/bash/bb-util.c',
    dependencies: [bash_dep_1, bas_c_dep],
    include_directories: exe_inc,
    version: meson.project_version(),
    soversion: meson.project_version().split('.')[0],
    install: true,
)

# ---- pkg-config ----
pkgconfig = import('pkgconfig')
pkgconfig.generate(
    lib_bas_c,
    name: 'bas-c',
    description: 'C base library: CLI, logging, process helpers, I/O, bash loadable builtin support',
    version: meson.project_version(),
    subdirs: 'bas',
    requires: ['glib-2.0', 'libcurl', 'openssl', 'zlib', 'icu-uc'],
)

pkgconfig.generate(
    lib_bas_c_static,
    name: 'bas-c-static',
    description: 'C base library: CLI, logging, process helpers, I/O, bash loadable builtin support',
    version: meson.project_version(),
    subdirs: 'bas',
    requires: ['glib-2.0', 'libcurl', 'openssl', 'zlib', 'icu-uc'],
)

# ---- Applications (install to bindir) ----
# Auto-discover all app/*.c files and build each as a standalone program.
# Each app links with libbas-c and defines LOGGER_NAME to the program name.
app_find = run_command(
    'find',
    'app',
    '-maxdepth', '1',
    '-type', 'f',
    '-name', '*.c',
    check: false,
)
app_sources = []
if app_find.returncode() == 0 and app_find.stdout().strip() != ''
    app_sources = app_find.stdout().strip().split('\n')
endif

foreach app_src : app_sources
    app_name = fs.replace_suffix(app_src.split('/')[-1], '')
    if app_name == 'ppid'
        install_dir = get_option('bindir')
    else
        install_dir = pkgdatadir / 'app'
    endif
    executable(
        app_name,
        app_src,
        'src/module.c',
        dependencies: exe_deps + [bas_c_dep],
        include_directories: exe_inc,
        c_args: c_nowarn + ['-DLOGGER_NAME=' + app_name],
        install: true,
        install_dir : install_dir,
    )
endforeach

# Manpages: app/*.1 -> install to mandir
find_man = run_command('find', 'app', '-maxdepth', '1', '-name', '*.1', check: false)
if find_man.returncode() == 0
    foreach line : find_man.stdout().strip().split('\n')
        if line == ''
            continue
        endif
        install_man(line)
    endforeach
endif

# ---- Bash completion (install to datadir/bash-completion/completions) ----
find_bash = run_command('find', 'app', '-maxdepth', '1', '-name', '*.bash', check: false)
app_bash_list = []
if find_bash.returncode() == 0
    foreach line : find_bash.stdout().strip().split('\n')
        if line == ''
            continue
        endif
        completion_name = fs.replace_suffix(line.split('/')[-1], '')
        install_data(
            line,
            rename: completion_name,
            install_dir: get_option('datadir') / 'bash-completion' / 'completions',
            install_mode: 'rw-r--r--',
        )
    endforeach
endif

# ---- Headers for libbas-c package (includedir/bas-c) ----
find_other_cmd = run_command(
    'find',
    'src',
    '-name', '*.h',
    '-o',
    '-name', '*.hpp',
    '-o',
    '-type', 'd',
    '-o',
    '-print',
    check: true,
)
other_files = find_other_cmd.stdout().strip().split('\n')

other_list = []
foreach f : other_files
    other_list += f.substring(4)
endforeach

install_subdir(
    'src',
    install_dir: get_option('includedir'),
    exclude_files: other_list,
    strip_directory: true,
)

# ---- Docs for libbas-c ----
install_data(
    'README.md',
    install_dir: get_option('datadir') / 'doc' / 'libbas-c',
    install_mode: 'rw-r--r--',
)

# Unit tests generator
# Find all *_test.c files and build them automatically using the 'includes' utility
# Similar to Makefile pattern rule: %_test: %_test.c
#   sources: %_test.c $(includes -e -c %_test.c)
#   cmdline: $(CC) $(CFLAGS) -o $@ $*_test.c `includes -c $*_test.c` -licuuc -licui18n

# List of all test source files (manually enumerated since Meson lacks fs module)
# This list is automatically generated from: find src -name '*_test.c' | sort
all_test_files = [
    'bas/base/str_test.c',
    'bas/cli/program_test.c',
    'bas/log/log_test.c',
]

# Build each test using the 'includes' utility to get source dependencies
foreach test_rel_path : all_test_files
    test_c = 'src/' + test_rel_path

    # Extract test name (e.g., ByteBuffer_test from src/io/ByteBuffer_test.c)
    path_parts = test_rel_path.split('/')
    test_file = path_parts[path_parts.length() - 1]
    test_name = fs.replace_suffix(test_file, '')

    # Use 'includes' utility to get implementation dependencies
    # 'includes -e -c test.c' returns source files, one per line (not -I include paths)
    includes_result = run_command('includes', '-e', '-c', test_c, check: false)

    impl_deps = []
    if includes_result.returncode() == 0
        # Parse the output: each line is a .c/.c file path
        output = includes_result.stdout()
        lines = output.split('\n')
        foreach line : lines
            line = line.strip()
            # Keep non-empty lines that are .c/.c files
            if line.endswith('.c') or line.endswith('.c')
                impl_deps += line
            endif
        endforeach
    endif

    # Combine test file with implementation dependencies from 'includes'
    all_sources = [test_c] + impl_deps

    # Create the executable (LOGGER_NAME so program.h and logger.c work in tests)
    test_exe = executable(
        test_name,
        all_sources,
        'src/module.c',
        include_directories: exe_inc,
        dependencies: exe_deps + [bas_c_dep],
        c_args: c_nowarn + ['-DLOGGER_NAME=test'],
    )

    # Register the test with meson
    test(test_name, test_exe)
endforeach
