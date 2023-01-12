//
// Created by Alexander Bychuk on 17.08.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5CCACHE_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5CCACHE_H_

#include "krb5fwd.h"
#include <memory>
#include <QString>
#include "krb5cursor.h"

namespace v5 {
class Context;
class Principal;

class CCache {
  bool _destroyed = false;
  const Context &_context;
  krb5_ccache _ccache = nullptr;
  explicit CCache(const Context &context);
  friend class Creds;
  friend class Context;

 public:
  CCache(CCache &&) = default;
  ~CCache();
  const Context &context() const;
  krb5_ccache operator()() const;
  Principal getPrincipal() const;
  krb5_error_code destroy();
  krb5_error_code setFlags(krb5_flags flags);
  QString type() const;
  QString name() const;
  Cursor cursor();
  Creds renewCredentials(const Principal &principal) const;
};
}  // namespace v5

#endif  // KRB5_TICKET_WATCHER_SRC_KRB5CCACHE_H_
