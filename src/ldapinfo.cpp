#include "ldapinfo.h"
#include <QTextStream>
#include <QStringList>
#include <cstdio>
#include <memory>

template <typename Action>
void execAndDoOnEveryLine(const std::string &execString, const Action &action) {
  std::unique_ptr<FILE, std::function<int(FILE *)>> stream(popen(execString.c_str(), "r"), pclose);

  QTextStream nmOutStream(stream.get());
  QString line;
  do {
    line = nmOutStream.readLine().trimmed();
    if (line.isNull()) {
      break;
    }
    action(line);
  } while (!line.isNull());
}

LdapInfo::LdapInfo(const QString &ldapServer, const QString &principal, const QString &password) : _ldapServer(ldapServer), _principal(principal) {
  if (_ldapServer.isEmpty()) {
    return;
  }
  QString principalName;
  if (!_principal.isEmpty()) {
    auto strs = _principal.split("@");
    if (!strs.isEmpty()) {
      principalName = strs.first();
      _realm = strs.at(1);
    }
  }

  QString domen0;
  QString domen1;
  auto ss = _realm.split(".");
  if (ss.size() == 2) {
    domen0 = ss.at(1);
    domen1 = ss.at(0);
  }
  if (domen1.isEmpty() || domen0.isEmpty()) {
    return;
  }
  _searchString =
      QString("ldapsearch -Z -h %1 -D \"%2\" -w \"%3\" -b \"dc=%4,dc=%5\" \"(sAMAccountName=%6)\" title displayName sn givenName middleName")
          .arg(_ldapServer, _principal, password, domen1, domen0, principalName);
  execAndDoOnEveryLine(_searchString.toStdString(), [this](const QString &line) {
    if (line.contains("title:")) {
      auto ss = line.split(":");
      if (ss.size() == 2) {
        _title = ss.at(1).trimmed();
      }
    }
    if (line.contains("displayName:")) {
      auto ss = line.split(":");
      if (ss.size() == 2) {
        _displayName = ss.at(1).trimmed();
      }
    }
    if (line.contains("sn:")) {
      auto ss = line.split(":");
      if (ss.size() == 2) {
        _surname = ss.at(1).trimmed();
      }
    }
    if (line.contains("givenName:")) {
      auto ss = line.split(":");
      if (ss.size() == 2) {
        _name = ss.at(1).trimmed();
      }
    }
    if (line.contains("middleName:")) {
      auto ss = line.split(":");
      if (ss.size() == 2) {
        _middleName = ss.at(1).trimmed();
      }
    }
  });
}

const QString &LdapInfo::title() const { return _title; }

const QString &LdapInfo::displayName() const { return _displayName; }

const QString &LdapInfo::surname() const { return _surname; }

const QString &LdapInfo::name() const { return _name; }

const QString &LdapInfo::middleName() const { return _middleName; }
