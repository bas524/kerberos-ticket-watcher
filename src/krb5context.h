//
// Created by Alexander Bychuk on 17.08.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5CONTEXT_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5CONTEXT_H_

#include <QString>
#include "krb5fwd.h"

namespace v5 {
class CCache;
class Principal;
class CredsOpts;
class Creds;

class Context {
  mutable krb5_context kcontext{};

 public:
  Context();
  ~Context();
  krb5_context operator()() const;
  bool valid() const;
  QString defaultRealm() const;
  krb5_timestamp currentDateTime() const;
  CCache ccache() const;
  Principal principal(const QString &name) const;
  CredsOpts credsOpts() const;
  Creds initCreds(const Principal &principal, const CredsOpts &opts, const QString &password, const QString &initService = QString{});
  krb5_timestamp getPasswordExpiredTimestamp(const Principal &principal) const;
};

}  // namespace v5

#endif  // KRB5_TICKET_WATCHER_SRC_KRB5CONTEXT_H_
