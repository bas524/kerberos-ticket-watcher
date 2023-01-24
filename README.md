# Kerberos Ticket Watcher

## Description

krb5-ticket-watcher is a tray applet for watching, renewing, and reinitializing Kerberos tickets.
It's very useful in corporate network for linux-workstations which should be in the AD-domain.

Application read configuration from kerberos, for example, ```/etc/krb5.cfg```

Application automatically saves credential. On linux with libsecret, on macos with keychain, all casess implemented by qt5keychain.

Application automatically saves and reads options to/from ~/.config/krb5tw/options.cfg, sample,

```ini
[General]
forwardable=true
lifetime.time=8
lifetime.unit=hours
promptInterval.time=31
promptInterval.unit=minutes
proxiable=true
renewtime.time=13
renewtime.unit=days
ldap.server=dc.domain.com
```

## Code

This is a fork of [source forge project](https://sourceforge.net/projects/krb5ticketwatch/) with patches from AltLinux team.

## Screenshots

* Simple login window
![LoginWindow](screenshots/login-w-simple.png?raw=true "kerberos-ticket-watcher")

* Extend login window
![LoginWindowE](screenshots/login-w-extend.png?raw=true "kerberos-ticket-watcher")

* Main window and menu
Icon in the system tray shows how many days left before password will be expired
![MainWindow](screenshots/main-w.png?raw=true "kerberos-ticket-watcher")
