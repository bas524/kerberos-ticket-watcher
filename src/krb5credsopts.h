//
// Created by Alexander Bychuk on 16.09.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5CREDSOPTS_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5CREDSOPTS_H_

#include "krb5fwd.h"

namespace v5 {
class Context;
class CredsOpts {
  const Context &_context;
  krb5_get_init_creds_opt *_opt;
  CredsOpts(const Context &context, krb5_get_init_creds_opt *opt);
  friend class Context;

 public:
  ~CredsOpts() noexcept;
  void setForwardable(bool fl);
  void setProxiable(bool fl);
  void setTicketLifeTime(krb5_deltat time);
  void setRenewLife(krb5_deltat time);
  krb5_get_init_creds_opt *operator()() const;
  void setExpireCallback(krb5_expire_callback_func cb, void *data);
};
}  // namespace v5
#endif  // KRB5_TICKET_WATCHER_SRC_KRB5CREDSOPTS_H_
