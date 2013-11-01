# spec file for crash-reporter

%define release         0
%define buildroot       %{_topdir}/%{name}-%{version}-%{release}-root

%define _unpackaged_files_terminate_build 0

BuildRoot:              %{buildroot}
Summary:                Crash Reporting Tool for Mer
License:                LGPL
Name:                   crash-reporter
Version:                1.10.29
Release:                %{release}
Prefix:                 /usr
Group:                  Development/Tools
URL:                    https://github.com/mer-qa/crash-reporter
BuildRequires:          qt5-qtdbus-devel
BuildRequires:          qt5-qtdeclarative-devel
BuildRequires:          qt5-qtgui-devel
BuildRequires:          qt5-qtnetwork-devel
BuildRequires:          qt5-qttest-devel
BuildRequires:          qt5-qttools-linguist
BuildRequires:          qt5-qtxml-devel
BuildRequires:          ssu-devel
Requires:               sp-rich-core >= 1.71.2
Requires:               oneshot
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

%package                devel
Summary:                Development files for crash-reporter
Group:                  Development/Libraries
Requires:               %{name} = %{version}-%{release}
%description            devel
Development headers and libraries for crash-reporter.

%package                tests
Summary:                Tests for crash-reporter
Requires:               %{name} = %{version}-%{release}
%description            tests
Test binaries for testing Crash Reporter.

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
/usr/bin/crash*
/usr/lib/oneshot.d/*
/usr/lib/systemd/user/*
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
/usr/share/crash-reporter-settings/*

%files -n jolla-settings-crash-reporter
%defattr(-,root,root,-)
/usr/lib/qt5/qml/*
/usr/share/jolla-settings/pages/crash-reporter/*
/usr/share/jolla-settings/entries/*

%files devel
%defattr(-,root,root,-)
%doc COPYING
/usr/include/crash-reporter/*.h
/usr/lib/*.so
/usr/lib/pkgconfig/*.pc

%files tests
%defattr(-,root,root,-)
/usr/lib/crash-reporter-tests/*
/usr/share/crash-reporter-tests/*

#%post
## on first install
#if [ "$1" -eq 1 ]; then
#	add-oneshot --user --now crash-reporter-service-default
#fi

%post -n libcrash-reporter0
ldconfig

%postun -n libcrash-reporter0
ldconfig
