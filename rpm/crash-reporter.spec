# spec file for crash-reporter

%define release         0
%define buildroot       %{_topdir}/%{name}-%{version}-%{release}-root

BuildRoot:              %{buildroot}
Summary:                Crash Reporting Tool for Mer
License:                LGPLv2.1
Name:                   crash-reporter
Version:                1.10.64
Release:                %{release}
Prefix:                 /usr
Group:                  Development/Tools
URL:                    https://github.com/mer-qa/crash-reporter
BuildRequires:          groff
BuildRequires:          qt5-qtdbus-devel
BuildRequires:          qt5-qtdeclarative-devel
BuildRequires:          qt5-qtgui-devel
BuildRequires:          qt5-qtnetwork-devel
BuildRequires:          qt5-qttools-linguist
BuildRequires:          ssu-devel
BuildRequires:          pkgconfig(dbus-1)
BuildRequires:          pkgconfig(libiphb)
BuildRequires:          pkgconfig(libudev)
BuildRequires:          pkgconfig(mce)
BuildRequires:          pkgconfig(qt5-boostable)
BuildRequires:          pkgconfig(nemonotifications-qt5)
Requires:               sp-rich-core >= 1.71.2
Requires:               sp-endurance
Requires:               oneshot
Requires:               qt5-qttools-qdbus
Requires:               sailfishsilica-qt5 >= 0.27.0
Requires:               ssu
Requires:               ssu-sysinfo
Conflicts:              quick-feedback < 0.0.18
%{_oneshot_requires_post}
Source0:                %{name}-%{version}.tar.gz

%description
Crash Reporting Tool allows users to easily upload crash reports provided by sp-rich-core to centralized location to help developers to fix issues.

%package -n             libcrash-reporter0
Summary:                Crash Reporter library
Requires:               crash-reporter-config
%description -n         libcrash-reporter0
This package contains various widgets and helper classes for Crash Reporter.

%package -n             crash-reporter-config-nemo
Summary:                Crash Reporter default configuration
BuildArch:              noarch
Provides:               crash-reporter-config
%description -n         crash-reporter-config-nemo
Crash Reporter default setting files.

%package -n             jolla-settings-crash-reporter
Summary:                Crash Reporter Settings UI for Jolla Sailfish OS
Requires:               %{name} = %{version}-%{release}
%description -n         jolla-settings-crash-reporter
Crash Reporter settings plugin for the Sailfish OS Settings application.

%package -n             libcrash-reporter0-devel
Summary:                Development files for crash-reporter
Requires:               libcrash-reporter0 = %{version}-%{release}
%description -n         libcrash-reporter0-devel
Development headers and libraries for crash-reporter.

%package doc
Summary:                Documentation for %{name}
Group:                  Documentation
Requires:               %{name} = %{version}-%{release}
%description doc
Man pages for %{name}.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5
make %{?_smp_mflags}

%install
%qmake5_install

mkdir -p %{buildroot}%{_docdir}/%{name}-%{version}
install -m0644 -t %{buildroot}%{_docdir}/%{name}-%{version} README

%files
%defattr(-,root,root,-)
%license COPYING
/lib/systemd/system/*
%{_bindir}/%{name}-*
%{_libdir}/oneshot.d/*
%{_libdir}/systemd/user/*
%attr(0755,root,root) /usr/libexec/crash-reporter-journalspy
%attr(0755,root,root) /usr/libexec/endurance-collect*
%attr(4755,root,root) /usr/libexec/rich-core-helper
%attr(4750,root,privileged) /usr/libexec/crashreporter-servicehelper
%{_datadir}/%{name}
%{_datadir}/dbus-1/services/*.service

%files -n libcrash-reporter0
%defattr(-,root,root,-)
%license COPYING
%{_libdir}/*crash*.so.*
%{_datadir}/translations/*

%files -n crash-reporter-config-nemo
%defattr(-,root,root,-)
%license COPYING
%{_datadir}/crash-reporter-settings

%files -n jolla-settings-crash-reporter
%defattr(-,root,root,-)
%{_libdir}/qt5/qml/*
%{_datadir}/jolla-settings/pages/%{name}
%{_datadir}/jolla-settings/entries/*

%files -n libcrash-reporter0-devel
%defattr(-,root,root,-)
%{_includedir}/%{name}
%{_libdir}/*crash*.so
%{_libdir}/pkgconfig/*.pc

%files doc
%defattr(-,root,root,-)
%{_mandir}/man1/%{name}-*
%{_docdir}/%{name}-%{version}

%post
systemctl daemon-reload
## on first install
#if [ "$1" -eq 1 ]; then
#	add-oneshot --user --now crash-reporter-service-default
#fi

%preun
if [ "$1" = 0 ]; then
  su nemo -c "systemctl --user stop crash-reporter.service"
  systemctl stop crash-reporter-endurance.service
fi

%postun
if [ "$1" = 0 ]; then
  rm -rf /var/cache/core-dumps/{uploadlog,endurance-enabled-mark,endurance}
fi

%post -n libcrash-reporter0
ldconfig

%postun -n libcrash-reporter0
ldconfig
