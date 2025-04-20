Name:           showtime
Version:        1.0.0
Release:        1%{?dist}
Summary:        ASCII Analog Clock for Terminal

License:        MIT
URL:            https://github.com/yourusername/showtime
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc, make
Requires:       glibc

%description
A simple analog clock written in C using terminal characters.

%prep
%setup -q

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make install PREFIX=$RPM_BUILD_ROOT/usr/local/bin

%clean
rm -rf $RPM_BUILD_ROOT

%files
/usr/local/bin/showtime

%changelog
* Sat May 17 2025 T1antina  <xuanwonaturo@gmail.com> - 1.0.0-1
- Initial release
