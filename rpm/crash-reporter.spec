# spec file for crash-reporter

%define release         0
%define buildroot       %{_topdir}/%{name}-%{version}-%{release}-root

%define _unpackaged_files_terminate_build 0

BuildRoot:              %{buildroot}
Summary:                Crash Reporting Tool for MeeGo
License:                LGPL
Name:                   crash-reporter
Version:                1.10.0
Release:                %{release}
Prefix:                 /usr
Group:                  Development/Tools
BuildRequires:          gcc-c++, qt5-qtgui-devel, qt5-qtdbus-devel, qt5-qtnetwork-devel, qt5-qttest-devel
Source0:                %{name}-%{version}.tar.gz

%description
Crash Reporting Tool allows users to easily upload crash reports provided by sp-rich-core to centralized location to help developers to fix issues.

%package -n             libcrash-reporter0
Summary:                Crash Reporter library
Group:                  System/Libraries
%description -n         libcrash-reporter0
This package contains various widgets and helper classes for Crash Reporter.

%package                devel
Summary:                crash-reporter development files
Group:                  Development/Libraries
Requires:               crash-reporter
%description            devel
Development headers and libraries for crash-reporter.

%package                tests
Summary:                crash-reporter tests
Requires:               crash-reporter
%description            tests
Test binaries for testing Crash Reporter.

#%package                ui-tests
#Summary:                crash-reporter UI tests
#Requires:               crash-reporter
#%description            ui-tests
#MATTI test cases for Crash Reporter UI.

#%package                l10n-engineering-english
#Summary:                Engineering English translations for crash-reporter
#Requires:               crash-reporter
#%description            l10n-engineering-english
#Engineering English translations for Crash Reporter.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5
make

%install
make install INSTALL_ROOT=%{buildroot}
mkdir %{buildroot}/usr/lib/systemd/user/nemo-mobile-session.target.wants
ln -s -t %{buildroot}/usr/lib/systemd/user/nemo-mobile-session.target.wants ../crash-reporter.service

%files
%doc README COPYING
/usr/bin/crash*
#/usr/lib/duicontrolpanel/*.desktop
#/usr/lib/duicontrolpanel/applets/*crash*.so*
#/usr/lib/crash-reporter/dialogplugins/*
/usr/lib/systemd/user/*
/usr/share/crash-reporter/*
/usr/share/crash-reporter-settings/*
/usr/share/dbus-1/services/*.service
/usr/share/man/man1/*
/usr/share/meegotouch/notifications/eventtypes/*
/usr/share/themes/base/meegotouch/icons/*

%files -n libcrash-reporter0
%doc COPYING
/usr/lib/*crash*.so.*

%files devel
%doc COPYING
/usr/include/crash-reporter/*.h
/usr/lib/*.so
/usr/lib/pkgconfig/*.pc

%files tests
/usr/lib/crash-reporter-tests/*
/usr/share/crash-reporter-tests/*

#%files ui-tests
#/usr/lib/crash-reporter-ui-tests/testdata/*
#/usr/share/crash-reporter-ui-tests/*

#%files l10n-engineering-english
#/usr/share/l10n/meegotouch/crash-reporter.qm

%postun
ldconfig

%changelog
* Tue Jul 29 2013 Jakub Adam <jakub.adam@jollamobile.com> - 1.10.0
- Initial release for Mer
