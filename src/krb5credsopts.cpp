//
// Created by Alexander Bychuk on 16.09.2022.
//
#define KRB5_PRIVATE 1
#include <krb5.h>

#include "krb5credsopts.h"
#include "krb5context.h"
#include "krb5exception.h"

namespace v5 {
CredsOpts::CredsOpts(const Context &context, krb5_get_init_creds_opt *opt) : _context(context), _opt(opt) {}
CredsOpts::~CredsOpts() noexcept { krb5_get_init_creds_opt_free(_context(), _opt); }
void CredsOpts::setForwardable(bool fl) { krb5_get_init_creds_opt_set_forwardable(_opt, (fl) ? 1 : 0); }
void CredsOpts::setProxiable(bool fl) { krb5_get_init_creds_opt_set_proxiable(_opt, (fl) ? 1 : 0); }
void CredsOpts::setTicketLifeTime(krb5_deltat time) { krb5_get_init_creds_opt_set_tkt_life(_opt, time); }
void CredsOpts::setRenewLife(krb5_deltat time) { krb5_get_init_creds_opt_set_renew_life(_opt, time); }
krb5_get_init_creds_opt *CredsOpts::operator()() const { return _opt; }
void CredsOpts::setExpireCallback(krb5_expire_callback_func cb, void *data) {
  krb5_error_code retval = krb5_get_init_creds_opt_set_expire_callback(_context(), _opt, cb, data);
  if (retval != 0) {
    throw KRB5EXCEPTION(retval, _context, "Can't set expire callback");
  }
}
}  // namespace v5
