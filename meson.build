# warning_level=1: Meson < 0.40 rejects warning_level=0; -w still used via c_args.
project('bas-c', 'c', version: '1.0.1', default_options: ['warning_level=1'])

# Feature gates (Meson version).
# - string path `/` operator: since 0.49.0
# - fs module: since 0.53.0
# - override_dependency(): since 0.54.0
# - target override_options: since 0.40.0
# - run_command(..., check:): since 0.47.0
# - compiler.get_supported_arguments(): since 0.43.0
meson_has_path_op = meson.version().version_compare('>=0.49.0')
meson_has_fs = meson.version().version_compare('>=0.53.0')
meson_has_override_dep = meson.version().version_compare('>=0.54.0')
meson_has_override_options = meson.version().version_compare('>=0.40.0')
meson_has_run_check = meson.version().version_compare('>=0.47.0')
meson_has_supported_args = meson.version().version_compare('>=0.43.0')
meson_has_strip_dir = meson.version().version_compare('>=0.45.0')
meson_has_install_mode = meson.version().version_compare('>=0.38.0')

# Install layout:
#   Meson >= 0.49: get_option + `/` path joins.
#   Older Meson: join_paths (string `/` is 0.49+; still honors --prefix).
if meson_has_path_op
    prefix = get_option('prefix')
    bindir = prefix / get_option('bindir')
    datadir = prefix / get_option('datadir')
    localedir = prefix / get_option('localedir')
    includedir = prefix / get_option('includedir')
    libdir = prefix / get_option('libdir')
    pkgdatadir = datadir / meson.project_name()
    pkgdocdir = datadir / 'doc' / 'libbas-c'
    bash_completions_dir = datadir / 'bash-completion' / 'completions'
    pkgconfig_dir = libdir / 'pkgconfig'
    pkg_app_dir = pkgdatadir / 'app'
else
    prefix = get_option('prefix')
    bindir = join_paths(prefix, get_option('bindir'))
    datadir = join_paths(prefix, get_option('datadir'))
    localedir = join_paths(prefix, get_option('localedir'))
    includedir = join_paths(prefix, get_option('includedir'))
    libdir = join_paths(prefix, get_option('libdir'))
    pkgdatadir = join_paths(datadir, meson.project_name())
    pkgdocdir = join_paths(datadir, 'doc', 'libbas-c')
    bash_completions_dir = join_paths(datadir, 'bash-completion', 'completions')
    pkgconfig_dir = join_paths(libdir, 'pkgconfig')
    pkg_app_dir = join_paths(pkgdatadir, 'app')
endif

if meson_has_fs
    fs = import('fs')
endif

cc = meson.get_compiler('c')

# Disable all warnings (wx 3.0 and deps are noisy)
if meson_has_supported_args
    c_nowarn = cc.get_supported_arguments(['-w', '/w'])
else
    c_nowarn = ['-w']
endif

# SDK libraries - detect architecture and use appropriate library directory
target_cpu = build_machine.cpu_family()

# Optional subprojects (present when bas-c/subprojects is wired correctly).
have_bash_builtins_sub = false
have_icu_config_sub = false
if meson_has_fs
    have_bash_builtins_sub = fs.is_dir('subprojects/bash-builtins')
    have_icu_config_sub = fs.is_dir('subprojects/icu-config')
else
    have_bash_builtins_sub = run_command(
        'sh', '-c', 'test -d "$1" && echo yes || echo no',
        'sh', 'subprojects/bash-builtins',
    ).stdout().strip() == 'yes'
    have_icu_config_sub = run_command(
        'sh', '-c', 'test -d "$1" && echo yes || echo no',
        'sh', 'subprojects/icu-config',
    ).stdout().strip() == 'yes'
endif

# Prefer get_variable() so Meson < 0.54 (no override_dependency) still works.
if have_bash_builtins_sub
    bash_builtins_sp = subproject('bash-builtins')
    bash_builtins_dep = bash_builtins_sp.get_variable('bash_builtins_dep')
else
    bash_builtins_dep = dependency('bash-builtins', required: true)
endif

if have_icu_config_sub
    icu_config_sp = subproject('icu-config')
    icu_uc_dep = icu_config_sp.get_variable('icu_uc_dep')
else
    icu_uc_dep = dependency('icu-uc', required: true)
endif

# GLib for scriptable UI stack (GList)
glib_dep = dependency('glib-2.0', required: true)

# libcurl for HTTP/HTTPS/FTP/FTPS (curl license = permissive)
libcurl_dep = dependency('libcurl', required: true)

# OpenSSL (crypto + ssl for HTTPS)
openssl_dep = dependency('openssl', required: true)

# zlib for ZIP support
zlib_dep = dependency('zlib', required: true)

# pthreads (dbgthread); not always pulled transitively on older glibc/toolchains.
thread_dep = dependency('threads')

# Include path for src/ so "log/log.h", "base/string_list.h" etc. resolve
exe_inc = [include_directories('src')]

# zlib is used for decompression, but we'll parse ZIP manually
c_args = c_nowarn + [
    '-DLOGGER_NAME=bas',
    '-DTEXT_DOMAIN=bas_c',
    '-DLOCALEDIR="@0@"'.format(localedir),
]

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
    'src/bas/locale/i18n.c',
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
    thread_dep,
]

if meson_has_override_options
    lib_bas_c = library(
        'bas-c',
        c_sources,
        dependencies: exe_deps,
        include_directories: exe_inc,
        c_args: c_args,
        override_options: ['b_lundef=false'],
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
        override_options: ['b_lundef=false'],
        install: true,
    )
else
    lib_bas_c = library(
        'bas-c',
        c_sources,
        dependencies: exe_deps,
        include_directories: exe_inc,
        c_args: c_args,
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
        install: true,
    )
endif

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

if meson_has_override_dep
    meson.override_dependency('bas-c', bas_c_dep)
    meson.override_dependency('bas-c-static', bas_c_static_dep)
endif

# Bash loadable: libbas-bash (bb-util)
bas_bash = shared_library(
    'bas-bash',
    'src/bas/bash/bb-util.c',
    dependencies: [bash_builtins_dep, bas_c_dep],
    include_directories: exe_inc,
    version: meson.project_version(),
    soversion: meson.project_version().split('.')[0],
    install: true,
)

# ---- pkg-config ----
# Positional library arg for generate() needs Meson >= 0.46.
pkgconfig = import('pkgconfig')
meson_has_pkg_pos = meson.version().version_compare('>=0.46.0')
meson_has_pkg_uninst = meson.version().version_compare('>=0.54.0')
if meson_has_pkg_pos
    pkgconfig.generate(
        lib_bas_c,
        name: 'bas-c',
        description: 'C base library: CLI, logging, process helpers, I/O, bash loadable builtin support',
        version: meson.project_version(),
        # headers live at ${includedir}/bas/... (include <bas/...>)
        requires: ['glib-2.0', 'libcurl', 'openssl', 'zlib'],
    )
else
    pkgconfig.generate(
        libraries: lib_bas_c,
        name: 'bas-c',
        description: 'C base library: CLI, logging, process helpers, I/O, bash loadable builtin support',
        version: meson.project_version(),
        # headers live at ${includedir}/bas/... (include <bas/...>)
        requires: ['glib-2.0', 'libcurl', 'openssl', 'zlib'],
    )
endif

# Static .pc: use explicit .a path so `pkg-config --libs bas-c-static` links the
# archive instead of -lbas-c (which would prefer libbas-c.so when both exist).
# icu-uc is in Requires (not Libs.private) so static consumers get ICU without
# needing `pkg-config --static`.
if meson_has_pkg_uninst
    pkgconfig.generate(
        name: 'bas-c-static',
        filebase: 'bas-c-static',
        description: 'C base library: CLI, logging, process helpers, I/O, bash loadable builtin support',
        version: meson.project_version(),
        # headers live at ${includedir}/bas/... (include <bas/...>)
        install_dir: pkgconfig_dir,
        requires: ['glib-2.0', 'libcurl', 'openssl', 'zlib', 'icu-uc'],
        libraries: ['${bas_c_static_lib}'],
        variables: ['bas_c_static_lib=${libdir}/libbas-c.a'],
        uninstalled_variables: ['bas_c_static_lib=${prefix}/libbas-c.a'],
    )
elif meson_has_pkg_pos
    pkgconfig.generate(
        lib_bas_c_static,
        name: 'bas-c-static',
        filebase: 'bas-c-static',
        description: 'C base library: CLI, logging, process helpers, I/O, bash loadable builtin support',
        version: meson.project_version(),
        # headers live at ${includedir}/bas/... (include <bas/...>)
        requires: ['glib-2.0', 'libcurl', 'openssl', 'zlib', 'icu-uc'],
    )
else
    pkgconfig.generate(
        libraries: lib_bas_c_static,
        name: 'bas-c-static',
        filebase: 'bas-c-static',
        description: 'C base library: CLI, logging, process helpers, I/O, bash loadable builtin support',
        version: meson.project_version(),
        # headers live at ${includedir}/bas/... (include <bas/...>)
        requires: ['glib-2.0', 'libcurl', 'openssl', 'zlib', 'icu-uc'],
    )
endif


# ---- Applications (install to bindir) ----
# Auto-discover all app/*.c files and build each as a standalone program.
if meson_has_run_check
    app_find = run_command(
        'find',
        'app',
        '-maxdepth', '1',
        '-type', 'f',
        '-name', '*.c',
        check: false,
    )
    app_find_ok = app_find.returncode() == 0
    app_find_out = app_find.stdout().strip()
else
    app_find = run_command(
        'sh', '-c',
        'find app -maxdepth 1 -type f -name "*.c" 2>/dev/null || true',
    )
    app_find_ok = true
    app_find_out = app_find.stdout().strip()
endif

app_sources = []
if app_find_ok and app_find_out != ''
    app_sources = app_find_out.split('\n')
endif

foreach app_src : app_sources
    app_parts = app_src.split('/')
    app_base = app_parts[app_parts.length() - 1]
    if meson_has_fs
        app_name = fs.replace_suffix(app_base, '')
    else
        app_name = app_base.split('.')[0]
    endif
    if app_name == 'ppid'
        app_install_dir = bindir
    else
        app_install_dir = pkg_app_dir
    endif
    executable(
        app_name,
        app_src,
        'src/module.c',
        dependencies: exe_deps + [bas_c_dep],
        include_directories: exe_inc,
        c_args: c_nowarn + [
            '-DLOGGER_NAME=' + app_name,
            '-DTEXT_DOMAIN=bas_c',
            '-DLOCALEDIR="@0@"'.format(localedir),
        ],
        install: true,
        install_dir: app_install_dir,
    )
endforeach

# Manpages: app/*.1 -> install to mandir
if meson_has_run_check
    find_man = run_command('find', 'app', '-maxdepth', '1', '-name', '*.1', check: false)
    find_man_ok = find_man.returncode() == 0
    find_man_out = find_man.stdout().strip()
else
    find_man = run_command(
        'sh', '-c',
        'find app -maxdepth 1 -name "*.1" 2>/dev/null || true',
    )
    find_man_ok = true
    find_man_out = find_man.stdout().strip()
endif
if find_man_ok and find_man_out != ''
    foreach line : find_man_out.split('\n')
        if line != ''
            install_man(line)
        endif
    endforeach
endif

# ---- Bash completion (install to datadir/bash-completion/completions) ----
if meson_has_run_check
    find_bash = run_command('find', 'app', '-maxdepth', '1', '-name', '*.bash', check: false)
    find_bash_ok = find_bash.returncode() == 0
    find_bash_out = find_bash.stdout().strip()
else
    find_bash = run_command(
        'sh', '-c',
        'find app -maxdepth 1 -name "*.bash" 2>/dev/null || true',
    )
    find_bash_ok = true
    find_bash_out = find_bash.stdout().strip()
endif
if find_bash_ok and find_bash_out != ''
    foreach line : find_bash_out.split('\n')
        if line != ''
            bash_parts = line.split('/')
            bash_base = bash_parts[bash_parts.length() - 1]
            if meson_has_fs
                completion_name = fs.replace_suffix(bash_base, '')
            else
                completion_name = bash_base.split('.')[0]
            endif
            if meson_has_install_mode
                install_data(
                    line,
                    rename: completion_name,
                    install_dir: bash_completions_dir,
                    install_mode: 'rw-r--r--',
                )
            else
                install_data(
                    line,
                    rename: completion_name,
                    install_dir: bash_completions_dir,
                )
            endif
        endif
    endforeach
endif

# ---- Headers for libbas-c package (includedir/bas) ----
if meson_has_run_check
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
else
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
    )
endif
other_files = find_other_cmd.stdout().strip().split('\n')

other_list = []
foreach f : other_files
    # Strip leading "src/" without string.startswith (Meson >= 0.51).
    f_parts = f.split('/')
    if f_parts.length() > 1 and f_parts[0] == 'src'
        other_list += f.split('src/')[1]
    endif
endforeach

if meson_has_strip_dir
    install_subdir(
        'src',
        install_dir: includedir,
        exclude_files: other_list,
        strip_directory: true,
    )
else
    # Meson < 0.45 has no strip_directory; keep the `src` directory name out by
    # installing `src/bas` (and other top-level trees) directly under includedir.
    install_subdir(
        'src/bas',
        install_dir: includedir,
    )
endif

# ---- Docs for libbas-c ----
if meson_has_install_mode
    install_data(
        'README.md',
        install_dir: pkgdocdir,
        install_mode: 'rw-r--r--',
    )
else
    install_data(
        'README.md',
        install_dir: pkgdocdir,
    )
endif

# Unit tests generator
# List of all test source files (manually enumerated)
all_test_files = [
    'bas/base/str_test.c',
    'bas/cli/program_test.c',
    'bas/log/log_test.c',
]

foreach test_rel_path : all_test_files
    test_c = 'src/' + test_rel_path

    path_parts = test_rel_path.split('/')
    test_file = path_parts[path_parts.length() - 1]
    if meson_has_fs
        test_name = fs.replace_suffix(test_file, '')
    else
        test_name = test_file.split('.')[0]
    endif

    if meson_has_run_check
        includes_result = run_command('includes', '-e', '-c', test_c, check: false)
        includes_ok = includes_result.returncode() == 0
        includes_out = includes_result.stdout()
    else
        includes_result = run_command(
            'sh', '-c',
            'includes -e -c "$1" 2>/dev/null || true',
            'sh',
            test_c,
        )
        includes_ok = true
        includes_out = includes_result.stdout()
    endif

    impl_deps = []
    if includes_ok
        lines = includes_out.split('\n')
        foreach line : lines
            # str.strip/endswith need newer Meson; trim via split for 0.37.
            line_parts = line.split(' ')
            trimmed = ''
            foreach p : line_parts
                if p != ''
                    if trimmed == ''
                        trimmed = p
                    else
                        trimmed = trimmed + ' ' + p
                    endif
                endif
            endforeach
            # Prefer last path-like token if includes printed extra text.
            toks = trimmed.split('\t')
            candidate = toks[toks.length() - 1]
            c_parts = candidate.split('.')
            if c_parts.length() >= 2 and c_parts[c_parts.length() - 1] == 'c'
                impl_deps += candidate
            endif
        endforeach
    endif

    all_sources = [test_c] + impl_deps

    test_exe = executable(
        test_name,
        all_sources,
        'src/module.c',
        include_directories: exe_inc,
        dependencies: exe_deps + [bas_c_dep],
        c_args: c_nowarn + [
            '-DLOGGER_NAME=test',
            '-DTEXT_DOMAIN=bas_c',
            '-DLOCALEDIR="@0@"'.format(localedir),
        ],
    )

    test(test_name, test_exe)
endforeach

# Optional gettext trees (may be absent on minimal checkouts).
# i18n/project_source_root need Meson >= 0.50; skip on older (e.g. stretch 0.37).
if meson_has_fs
    have_po = fs.is_dir('po')
else
    have_po = run_command(
        'sh', '-c', 'test -d po && echo yes || echo no',
    ).stdout().strip() == 'yes'
endif
if have_po and meson.version().version_compare('>=0.50.0')
    subdir('po')
endif

# run_target with a multiline shell script emits broken build.ninja on Meson 0.37.
if meson.version().version_compare('>=0.50.0')
    run_target(
        'posync',
        command: [
            'bash',
            '-euc', '''
                cd "@SOURCE_ROOT@/po"
                poflags=(--no-wrap)
                xgettext "${poflags[@]}" --from-code=UTF-8 --keyword=_ --keyword=N_ --keyword=OPTION:3 --keyword=OPTARG:3 --language=C --directory=.. --output=bas_c.pot --files-from=POTFILES
                while IFS= read -r lang; do
                    [ -n "$lang" ] || continue
                    case "$lang" in
                        '#'* ) continue ;;
                    esac
                    po_file="$lang.po"
                    if [ ! -f "$po_file" ]; then
                        msginit "${poflags[@]}" --no-translator --input=bas_c.pot --locale="$lang" --output-file="$po_file"
                    fi
                    msgmerge "${poflags[@]}" --update --backup=none "$po_file" bas_c.pot
                    msgattrib "${poflags[@]}" --no-obsolete --output-file="$po_file" "$po_file"
                done < LINGUAS
            ''',
        ],
    )
endif

