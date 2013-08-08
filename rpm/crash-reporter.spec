# spec file for crash-reporter

%define release         0
%define buildroot       %{_topdir}/%{name}-%{version}-%{release}-root

%define _unpackaged_files_terminate_build 0

BuildRoot:              %{buildroot}
Summary:                Crash Reporting Tool for MeeGo
License:                LGPL
Name:                   crash-reporter
Version:                1.10.3
Release:                %{release}
Prefix:                 /usr
Group:                  Development/Tools
BuildRequires:          gcc-c++, qt5-qtgui-devel, qt5-qtdbus-devel, qt5-qtnetwork-devel, qt5-qttest-devel, qt5-qttools-linguist, ssu-devel
Source0:                %{name}-%{version}.tar.gz

%description
Crash Reporting Tool allows users to easily upload crash reports provided by sp-rich-core to centralized location to help developers to fix issues.

%package -n             libcrash-reporter0
Summary:                Crash Reporter library
Group:                  System/Libraries
%description -n         libcrash-reporter0
This package contains various widgets and helper classes for Crash Reporter.

%package -n             jolla-settings-crash-reporter
Summary:                Crash Reporter Settings UI for Jolla Sailfish OS
Group:                  System/GUI/Other
Requires:               qt5-qtsvg-plugin-imageformat-svg
%description -n         jolla-settings-crash-reporter
Crash Reporter settings plugin for the Sailfish OS Settings application.

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

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5
make

%install
%qmake5_install

%files
%doc README COPYING
/usr/bin/crash*
#/usr/lib/duicontrolpanel/*.desktop
#/usr/lib/duicontrolpanel/applets/*crash*.so*
#/usr/lib/crash-reporter/dialogplugins/*
/usr/lib/systemd/user/*
/usr/share/crash-reporter/*
/usr/share/dbus-1/services/*.service
/usr/share/man/man1/*
/usr/share/meegotouch/notifications/eventtypes/*

%files -n libcrash-reporter0
%doc COPYING
/usr/lib/*crash*.so.*
/usr/share/crash-reporter-settings/*
/usr/share/translations/*

%files -n jolla-settings-crash-reporter
/usr/lib/qt5/qml/*
/usr/share/jolla-settings/pages/crash-reporter/*
/usr/share/jolla-settings/entries/*
/usr/share/themes/*

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

%post -n libcrash-reporter0
ldconfig

%postun -n libcrash-reporter0
ldconfig
