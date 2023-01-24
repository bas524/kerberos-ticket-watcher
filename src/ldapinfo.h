//
// Created by Alexander Bychuk on 18.12.2022.
//

#ifndef KRB5_TICKET_WATCHER_LDAPINFO_H
#define KRB5_TICKET_WATCHER_LDAPINFO_H

#include <QString>

class LdapInfo {
  QString _ldapServer;
  QString _principal;
  QString _realm;
  QString _searchString;
  QString _title;
  QString _displayName;
  QString _surname;
  QString _name;
  QString _middleName;

 public:
  LdapInfo(const QString &ldapServer, const QString &principal, const QString &password);
  const QString &title() const;
  const QString &displayName() const;
  const QString &surname() const;
  const QString &name() const;
  const QString &middleName() const;
};

#endif  // KRB5_TICKET_WATCHER_LDAPINFO_H
