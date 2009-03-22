Name: krb5-ticket-watcher
Version: 1.0.2
Release: alt1
Summary: A Tray Applet for Watching, Renewing, and Reinitializing Kerberos Tickets
Url: http://sourceforge.net/projects/krb5ticketwatch
License: %gpl2plus
Group: System/X11

Packager: Andriy Stepanov <stanv@altlinux.ru>

Source: %name-%version.tar

BuildRequires: kde-common-devel rpm-build-licenses libkrb5-devel libkeyutils-devel
# Automatically added by buildreq on Thu Mar 19 2009
BuildRequires: cmake gcc-c++ libcom_err-devel libqt3-devel libqt4-devel

%description
A tray applet for watching, renewing, and reinitializing Kerberos
tickets.

%prep
%setup

%build
%add_optflags -I%_includedir/krb5
%K4build

%install
%K4install

%files
%_bindir/krb5-ticket-watcher
%_pixmapsdir/krb5-ticket-watcher.png
%_desktopdir/krb5-ticket-watcher.desktop
%{_datadir}/locale/*/*/*.mo
%doc COPYING Changes News TODO 

%changelog
* Sun Mar 22 2009 Andriy Stepanov <stanv@altlinux.ru> 1.0.2-alt1
- New version.

* Fri Mar 20 2009 Andriy Stepanov <stanv@altlinux.ru> 1.0.1-alt1
- Initial import to Sisyphus
