Summary:                Crash Reporting Tool for Sailfish OS
License:                LGPLv2
Name:                   crash-reporter
Version:                1.15.7
Release:                0
URL:                    https://github.com/mer-qa/crash-reporter
BuildRequires:          qt5-qtdbus-devel
BuildRequires:          qt5-qtdeclarative-devel
BuildRequires:          qt5-qtgui-devel
BuildRequires:          qt5-qtnetwork-devel
BuildRequires:          qt5-qttools-linguist
BuildRequires:          ssu-devel
BuildRequires:          pkgconfig(dbus-1)
BuildRequires:          pkgconfig(libiphb)
BuildRequires:          pkgconfig(libudev)
BuildRequires:          pkgconfig(libsystemd)
BuildRequires:          pkgconfig(mce)
BuildRequires:          pkgconfig(qt5-boostable)
BuildRequires:          pkgconfig(nemonotifications-qt5)
BuildRequires:          pkgconfig(usb-moded-qt5)
BuildRequires:          pkgconfig(systemd)
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
Requires:               %{name} = %{version}-%{release}
%description doc
Man pages for %{name}.

%package ts-devel
Summary: Translation source for crash reporter

%description ts-devel
%{summary}.

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
%{_unitdir}/*
%{_bindir}/%{name}-*
%{_oneshotdir}/*
%{_userunitdir}/*
%attr(0755,root,root) /usr/libexec/crash-reporter-journalspy
%attr(0755,root,root) /usr/libexec/crash-reporter-storagemon
%attr(0755,root,root) /usr/libexec/endurance-collect*
%attr(4755,root,root) /usr/libexec/rich-core-helper
%attr(4750,root,privileged) /usr/libexec/crashreporter-servicehelper
%{_datadir}/%{name}
%{_datadir}/dbus-1/services/*.service

%files -n libcrash-reporter0
%defattr(-,root,root,-)
%license COPYING
%{_libdir}/*crash*.so.*
%{_datadir}/translations/crash-reporter_eng_en.qm

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
%{_docdir}/%{name}-%{version}

%files ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/crash-reporter.ts

%post
systemctl daemon-reload
systemctl-user daemon-reload ||:

%preun
if [ "$1" = 0 ]; then
  systemctl-user stop crash-reporter.service \
                      crash-reporter-storagemon.path \
                      crash-reporter-storagemon.timer \
                      crash-reporter-storagemon.service
  systemctl stop crash-reporter-endurance.service
fi

%postun
if [ "$1" = 0 ]; then
  rm -rf /var/cache/core-dumps/uploadlog /var/cache/core-dumps/endurance-enabled-mark /var/cache/core-dumps/endurance
fi

%post -n libcrash-reporter0
ldconfig

%postun -n libcrash-reporter0
ldconfig
