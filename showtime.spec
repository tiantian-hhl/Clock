Name:           showtime
Version:        1.0.0
Release:        4%{?dist}
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

* Mon May 19 2025 T1antian <xuanwonaturo@gmail.com> - 1.0-4
-draw: use color to distinguish hands

* Mon May 19 2025 T1antian <xuanwonaturo@gmail.com> - 1.0-3
- Improve terminal cleanup on exit to avoid screen clutter
- Handle Ctrl+C (SIGINT) more gracefully by restoring cursor and clearing screen
- Refactor output flow to avoid overlapping with RPM install output

* Sun May 18 2025 T1antian <xuanwonaturo@gmail.com> - 1.0-2
- Improved clock aesthetics and digit alignment
- Added SIGINT handler for clean exit on Ctrl+C
- Refactored drawing functions for better modularity
- Updated help and version options

* Sat May 17 2025 T1antina  <xuanwonaturo@gmail.com> - 1.0.0-1
- Initial release
