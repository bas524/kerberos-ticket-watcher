//
// Created by Alexander Bychuk on 17.08.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5PRINCIPAL_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5PRINCIPAL_H_

#include "krb5fwd.h"
#include <QString>

namespace v5 {
class Context;
class CCache;

class Principal {
  const Context &_context;
  krb5_principal _principal = nullptr;
  explicit Principal(const CCache &cCache);
  Principal(const Context &context, krb5_principal principal);
  Principal(const CCache &cCache, krb5_principal principal);
  friend class CCache;
  friend class Context;

 public:
  Principal(Principal &&o) noexcept;
  Principal(const Principal &) = delete;
  ~Principal();
  Principal buildTargetPrincipal() const;
  QString realm() const;
  krb5_principal operator()() const;
  QString name() const;
};
}  // namespace v5

#endif  // KRB5_TICKET_WATCHER_SRC_KRB5PRINCIPAL_H_
