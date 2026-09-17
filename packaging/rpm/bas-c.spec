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
URL:            https://github.com/lenik/zephyr
Packager:       Lenik (谢继雷) <lenik@bodz.net>
Source0:        %{name}-%{srcversion}.tar.xz

BuildRequires:  meson
BuildRequires:  pkg-config
BuildRequires:  bash-builtins
BuildRequires:  libglib2.0-dev
BuildRequires:  libcurl4-openssl-dev
BuildRequires:  libssl-dev
BuildRequires:  zlib1g-dev
BuildRequires:  libicu-dev
BuildRequires:  ninja-build
BuildRequires:  asciidoctor
BuildRequires:  libbas-c-dev

%description
Shared library providing base utilities: CLI (program/options), logging,
process helpers, I/O, and bash loadable builtin support.

%prep
%setup -q -n %{name}-%{srcversion}

%build
meson setup build \
    --prefix=%{_prefix} \
    --bindir=%{_bindir} \
    --datadir=%{_datadir} \
    --mandir=%{_mandir} \
    --sysconfdir=%{_sysconfdir} \
    --localstatedir=%{_localstatedir} \
    --buildtype=plain
meson compile -C build

%install
meson install -C build --destdir=%{buildroot}

%files
%{_mandir}/man1/ppid.1*
%{_datadir}/bas-c/
%{_datadir}/locale/*/LC_MESSAGES/bas_c.mo
%{_datadir}/doc/bas-c/

%changelog
* Thu Aug 20 2026 Lenik (谢继雷) <lenik@bodz.net>
- Align spec with debian/control (Meson, AGPL-3.0-or-later).
- Version comes from `zfr version`, the same method meson.build uses.
