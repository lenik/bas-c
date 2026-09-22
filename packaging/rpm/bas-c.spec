# Version is injected by packaging/rpm/Makefile via `zfr version`.
# RPM Version cannot contain '-'; use `zfr version -r` (hyphens → '_').
# srcversion is the unsanitized Meson/git version and names the tarball.
%{!?version:%global version 0.0.0}
%{!?srcversion:%global srcversion %{version}}

Name:           bas-c
Version:        %{version}
Release:        1%{?dist}
Summary:        C base library (runtime)

License:        AGPL-3.0-or-later
URL:            https://github.com/lenik/bas-c
Packager:       Lenik (谢继雷) <bas-c@bodz.net>
Source0:        %{name}-%{srcversion}.tar.xz
# RPM-only patches live under packaging/rpm/*.patch (none required currently).

BuildRequires:  meson
BuildRequires:  ninja-build
BuildRequires:  pkgconf
BuildRequires:  gcc
BuildRequires:  gcc-c++
BuildRequires:  glib2-devel
BuildRequires:  libcurl-devel
BuildRequires:  openssl-devel
BuildRequires:  zlib-devel
BuildRequires:  libicu-devel
BuildRequires:  gettext
BuildRequires:  asciidoctor
# Debian Build-Depends "bash-builtins" → bash + staged headers (see build-rpm).
BuildRequires:  bash

%description
Shared library providing base utilities: CLI (program/options), logging,
process helpers, I/O, and bash loadable builtin support.

%prep
%autosetup -n %{name}-%{srcversion} -p1

%build
meson setup build \
    --prefix=%{_prefix} \
    --bindir=%{_bindir} \
    --libdir=%{_libdir} \
    --datadir=%{_datadir} \
    --mandir=%{_mandir} \
    --sysconfdir=%{_sysconfdir} \
    --localstatedir=%{_localstatedir} \
    --buildtype=plain
meson compile -C build

%install
meson install -C build --destdir=%{buildroot}

%files
%{_bindir}/*
%{_libdir}/libbas-c.so*
%{_libdir}/libbas-c.a
# libbas-bash is optional when bash loadable headers are absent
%{_libdir}/libbas-bash.so*
%{_libdir}/pkgconfig/bas-c.pc
%{_libdir}/pkgconfig/bas-c-static.pc
%{_includedir}/bas/
%{_datadir}/bas-c/
%{_datadir}/bash-completion/completions/*
%{_mandir}/man1/ppid.1*
%{_datadir}/locale/*/LC_MESSAGES/*.mo
%{_datadir}/doc/libbas-c/

%changelog
* Sun Sep 20 2026 Lenik (谢继雷) <bas-c@bodz.net>
- Apply packaging/rpm/*.patch via Patch0 + %autosetup (prefer bash.pc).

* Sun Sep 20 2026 Lenik (谢继雷) <bas-c@bodz.net>
- Use RHEL-style BuildRequires; ship libraries/headers in the RPM file list.
- Drop circular libbas-c-dev BuildRequires.

* Thu Aug 20 2026 Lenik (谢继雷) <lenik@bodz.net>
- Align spec with debian/control (Meson, AGPL-3.0-or-later).
- Version comes from `zfr version`, the same method meson.build uses.
