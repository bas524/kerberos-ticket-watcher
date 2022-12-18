//
// Created by Alexander Bychuk on 17.08.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5CREDS_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5CREDS_H_

#include "krb5fwd.h"
#include "krb5ticket.h"
#include <memory>
#include <QStringList>
#include "tmunit.h"

namespace v5 {
class Context;
class CCache;
class Principal;

class Creds {
  Context &_context;
  std::unique_ptr<krb5_creds> _creds;
  Creds(Context &context, std::unique_ptr<krb5_creds> creds);
  void retriveCreds(CCache &cCache, Principal &principal, Principal &targetPrincipal);
  friend class Context;
  friend class Cursor;
  friend class Ticket;
  friend class CCache;

 public:
  using LifeTimeDuration = ktw::TmUnit;
  Creds(Creds &&) = default;
  static Creds FromCCache(Context &context);
  ~Creds();

  bool isForwardable() const;
  bool isForwarded() const;
  bool isProxyable() const;
  bool isProxy() const;
  bool isMayPostdate() const;
  bool isPostdated() const;
  bool isInvalid() const;
  bool isRenewable() const;
  bool isInitial() const;
  bool isHWAuth() const;
  bool isPreAuth() const;
  bool hasTransitionPolicy() const;
  bool isOkAsDelegate() const;
  bool isAnonimous() const;
  krb5_timestamp ticketEndTime() const;
  krb5_timestamp ticketStartTime() const;
  krb5_timestamp ticketRenewTillTime() const;
  krb5_deltat ticketLifeTime() const;
  krb5_deltat ticketRenewTime() const;
  std::pair<krb5_deltat, LifeTimeDuration> lifeTimeDuration() const;
  std::pair<krb5_deltat, LifeTimeDuration> renewTimeDuration() const;
  QString clientName() const;
  QString serverName() const;
  bool hasStartTime() const;
  void setStartTimeInAuthTime();
  QString encryptionTypeName() const;
  QStringList adresses() const;
  Ticket ticket();
  void storeInCacheFor(Principal &principal);
  void changePassword(const QString &newPassword);
  bool operator==(const Creds &o) const;
};

}  // namespace v5
#endif  // KRB5_TICKET_WATCHER_SRC_KRB5CREDS_H_
