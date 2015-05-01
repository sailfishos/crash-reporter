# spec file for crash-reporter

%define release         0
%define buildroot       %{_topdir}/%{name}-%{version}-%{release}-root

BuildRoot:              %{buildroot}
Summary:                Crash Reporting Tool for Mer
License:                LGPL
Name:                   crash-reporter
Version:                1.10.64
Release:                %{release}
Prefix:                 /usr
Group:                  Development/Tools
URL:                    https://github.com/mer-qa/crash-reporter
BuildRequires:          qt5-qtdbus-devel
BuildRequires:          qt5-qtdeclarative-devel
BuildRequires:          qt5-qtgui-devel
BuildRequires:          qt5-qtnetwork-devel
BuildRequires:          qt5-qttools-linguist
BuildRequires:          qt5-qtxml-devel
BuildRequires:          ssu-devel
BuildRequires:          pkgconfig(dbus-1)
BuildRequires:          pkgconfig(libiphb)
BuildRequires:          pkgconfig(libudev)
BuildRequires:          pkgconfig(mce)
BuildRequires:          pkgconfig(qt5-boostable)
Requires:               sp-rich-core >= 1.71.2
Requires:               sp-endurance
Requires:               oneshot
Requires:               qt5-qttools-qdbus
Requires:               sailfishsilica-qt5 >= 0.13.40
Conflicts:              quick-feedback < 0.0.18
%{_oneshot_requires_post}
Source0:                %{name}-%{version}.tar.gz

%description
Crash Reporting Tool allows users to easily upload crash reports provided by sp-rich-core to centralized location to help developers to fix issues.

%package -n             libcrash-reporter0
Summary:                Crash Reporter library
Group:                  System/Libraries
Requires:               crash-reporter-config
%description -n         libcrash-reporter0
This package contains various widgets and helper classes for Crash Reporter.

%package -n             crash-reporter-config-nemo
Summary:                Crash Reporter default configuration
Group:                  Development/Tools
BuildArch:              noarch
Provides:               crash-reporter-config
%description -n         crash-reporter-config-nemo
Crash Reporter default setting files.

%package -n             jolla-settings-crash-reporter
Summary:                Crash Reporter Settings UI for Jolla Sailfish OS
Group:                  System/GUI/Other
Requires:               %{name} = %{version}-%{release}
%description -n         jolla-settings-crash-reporter
Crash Reporter settings plugin for the Sailfish OS Settings application.

%package -n             libcrash-reporter0-devel
Summary:                Development files for crash-reporter
Group:                  Development/Libraries
Requires:               libcrash-reporter0 = %{version}-%{release}
%description -n         libcrash-reporter0-devel
Development headers and libraries for crash-reporter.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5
make %{?jobs:-j%jobs}

%install
%qmake5_install

%files
%defattr(-,root,root,-)
%doc README COPYING
%config %{_sysconfdir}/dbus-1/system.d/crash-reporter-dbus.conf
/lib/systemd/system/*
/usr/bin/crash*
/usr/lib/oneshot.d/*
/usr/lib/systemd/user/*
%attr(0755,root,root) /usr/libexec/crash-reporter-journalspy
%attr(0755,root,root) /usr/libexec/endurance-collect*
%attr(4755,root,root) /usr/libexec/rich-core-helper
%dir /usr/share/crash-reporter
/usr/share/crash-reporter/*
/usr/share/dbus-1/services/*.service
/usr/share/man/man1/*
/usr/share/lipstick/notificationcategories/*

%files -n libcrash-reporter0
%defattr(-,root,root,-)
%doc COPYING
/usr/lib/*crash*.so.*
/usr/share/translations/*

%files -n crash-reporter-config-nemo
%defattr(-,root,root,-)
%dir /usr/share/crash-reporter-settings
/usr/share/crash-reporter-settings/*

%files -n jolla-settings-crash-reporter
%defattr(-,root,root,-)
/usr/lib/qt5/qml/*
%dir /usr/share/jolla-settings/pages/crash-reporter
/usr/share/jolla-settings/pages/crash-reporter/*
/usr/share/jolla-settings/entries/*

%files -n libcrash-reporter0-devel
%defattr(-,root,root,-)
%doc COPYING
/usr/include/crash-reporter/*.h
/usr/lib/*.so
/usr/lib/pkgconfig/*.pc

%post
# Remove timer unit symlink potentially left by old installation.
rm -f /etc/systemd/system/basic.target.wants/crash-reporter-endurance.timer
# Remove any endurance service symlinks in /etc. The service is now permanently
# enabled in /lib/systemd.
rm -f /etc/systemd/system/basic.target.wants/crash-reporter-endurance.service \
  /etc/systemd/system/multi-user.target.wants/crash-reporter-endurance.service
rmdir --ignore-fail-on-non-empty /etc/systemd/system/multi-user.target.wants
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
