project('bas-c', 'c',
  version : '1.0.0',
  default_options : ['warning_level=0'])

fs = import('fs')
cc = meson.get_compiler('c')

# Disable all warnings (wx 3.0 and deps are noisy)
c_nowarn = cc.get_supported_arguments(['-w', '/w'])

# zlib for ZIP support
zlib_dep = dependency('zlib', required : true)

# ICU for charset conversion (CharsetEncoder, CharsetDecoder, etc.)
icu_uc_dep = dependency('icu-uc', required : true)

# GLib for scriptable UI stack (GList)
glib_dep = dependency('glib-2.0', required : true)


# SDK libraries - detect architecture and use appropriate library directory
target_cpu = build_machine.cpu_family()

# Get C compiler for finding libraries
# (cc defined above)

# OpenSSL (crypto + ssl for HTTPS)
openssl_dep = dependency('openssl', required : true)

# libcurl for HTTP/HTTPS/FTP/FTPS (curl license = permissive)
libcurl_dep = dependency('libcurl', required : true)

# Include path for src/ so "log/log.h", "base/string_list.h" etc. resolve
exe_inc = [ include_directories('src') ]

# Bash headers for loadable builtins (bb-util.c).
bash_dep = dependency('bash', required : true)
bash_includedir = bash_dep.get_variable(pkgconfig : 'includedir')
# Like bash_dep but only the first includedir (avoids shadowing system <getopt.h>).
bash_dep_1 = declare_dependency(
  include_directories : include_directories(
    join_paths(bash_includedir, 'bash'),
    join_paths(bash_includedir, 'bash/include'),
    is_system : true,
  ),
  compile_args : ['-DSHELL', '-fPIC'],
)

# zlib is used for decompression, but we'll parse ZIP manually

# ppid.c has main() and is a standalone executable, not library code
c_sources = files(
    'src/proc/dbgaction.c',
    'src/proc/dbgthread.c',
    'src/proc/env.c',
    'src/proc/format_backtrace.c',
    'src/proc/format_gdb.c',
    'src/proc/pid.c',
    'src/proc/stackdump.c',
    'src/io/file.c',
    'src/io/fileent_list.c',
    'src/io/path.c',
    'src/io/wfileent_list.c',
    'src/base/str.c',
    'src/base/stdio.c',
    'src/base/string_list.c',
    'src/base/wstring.c',
    'src/cli/opt_parser.c',
    'src/cli/args.c',
    'src/cli/program.c',
    'src/log/logger.c',
    'src/log/log.c',
    'src/log/syslog.c',
)

# Executable (optional wx media/webview only when found)
exe_deps = [
  glib_dep,
  icu_uc_dep,
  libcurl_dep,
  openssl_dep,
  zlib_dep,
]

lib_bas_c = library('bas-c',
  c_sources,
  dependencies : exe_deps,
  include_directories : exe_inc,
  c_args : c_nowarn + ['-DLOGGER_NAME=bas'],
  version : meson.project_version(),
  soversion : '1',
  install : true,
)

# Bash loadable: libbas-bash (bb-util)
bas_bash = shared_library('bas-bash',
  'src/bash/bb-util.c',
  link_with : lib_bas_c,
  dependencies : [bash_dep_1],
  include_directories : exe_inc,
  version : meson.project_version(),
  soversion : '1',
  install : true,
)

# ---- pkg-config ----
pkgconfig = import('pkgconfig')
pkgconfig.generate(
  lib_bas_c,
  name : 'bas-c',
  description : 'C base library: CLI, logging, process helpers, I/O, bash loadable builtin support',
  version : meson.project_version(),
  subdirs : 'bas',
  requires : ['glib-2.0', 'libcurl', 'openssl', 'zlib', 'icu-uc'],
)

# ---- Applications (install to bindir) ----
# Auto-discover all app/*.c files and build each as a standalone program.
# Each app links with libbas-c and defines LOGGER_NAME to the program name.
app_find = run_command('find', 'app', '-maxdepth', '1', '-type', 'f', '-name', '*.c', check : false)
app_sources = []
if app_find.returncode() == 0 and app_find.stdout().strip() != ''
  app_sources = app_find.stdout().strip().split('\n')
endif

foreach app_src : app_sources
  app_name = fs.replace_suffix(app_src.split('/')[-1], '')
  executable(app_name,
    app_src,
    link_with : lib_bas_c,
    dependencies : exe_deps,
    include_directories : exe_inc,
    c_args : c_nowarn + [ '-DLOGGER_NAME=' + app_name ],
    install : true,
  )
endforeach

# ---- Man pages (install to mandir) ----
install_man('app/ppid.1')

# ---- Bash completion (install to datadir/bash-completion/completions) ----
install_data(
  'app/ppid.bash',
  rename : 'ppid',
  install_dir : get_option('datadir') / 'bash-completion' / 'completions',
  install_mode : 'rw-r--r--',
)

# ---- Headers for libbas-c package (includedir/bas-c) ----
find_other_cmd = run_command('find', '.',
          '-name', '*.h',
    '-o', '-name', '*.hpp',
    '-o', '-type', 'd',
    '-o', '-print',
    check: true)
other_files = find_other_cmd.stdout().strip().split('\n')

other_list = []
foreach f : other_files
    other_list += f.substring(4)
endforeach

install_subdir('src',
  install_dir : get_option('includedir') / 'bas',
  exclude_files : other_list,
  strip_directory : true,
)

# ---- Docs for libbas-c ----
install_data('README.md',
  install_dir : get_option('datadir') / 'doc' / 'libbas-c',
  install_mode : 'rw-r--r--',
)

# Unit tests generator
# Find all *_test.c files and build them automatically using the 'includes' utility
# Similar to Makefile pattern rule: %_test: %_test.c 
#   sources: %_test.c $(includes -e -c %_test.c)
#   cmdline: $(CC) $(CFLAGS) -o $@ $*_test.c `includes -c $*_test.c` -licuuc -licui18n

# List of all test source files (manually enumerated since Meson lacks fs module)
# This list is automatically generated from: find src -name '*_test.c' | sort
all_test_files = [
    'base/str_test.c',
    'cli/program_test.c',
    'log/log_test.c',
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
  includes_result = run_command('includes', '-e', '-c', test_c, check : false)
  
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
  test_exe = executable(test_name,
    all_sources,
    include_directories : exe_inc,
    dependencies : exe_deps,
    c_args : c_nowarn + ['-DLOGGER_NAME=' + test_name]
  )
  
  # Register the test with meson
  test(test_name, test_exe)
endforeach
