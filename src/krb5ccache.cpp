//
// Created by Alexander Bychuk on 17.08.2022.
//

#define KRB5_PRIVATE 1
#include <krb5.h>

#include "krb5ccache.h"
#include "krb5context.h"
#include "krb5exception.h"
#include "krb5principal.h"

#include <memory>

namespace v5 {
CCache::CCache(Context& context) : _context(context) {
  krb5_error_code retval = krb5_cc_default(_context(), &_ccache);
  if (retval != 0) {
    throw KRB5EXCEPTION(retval, _context, "Can't generate ccache from default");
  }
}
CCache::~CCache() {
  if (!_destroyed) {
    krb5_cc_close(_context(), _ccache);
  }
}
Context& CCache::context() { return _context; }
krb5_ccache CCache::operator()() const { return _ccache; }
Principal CCache::getPrincipal() { return Principal(*this); }
krb5_error_code CCache::destroy() {
  int code = krb5_cc_destroy(_context(), _ccache);
  _destroyed = true;
  if (code != 0 && code != KRB5_FCC_NOFILE) {
    return code;
  }
  return 0;
}
krb5_error_code CCache::setFlags(krb5_flags flags) { return krb5_cc_set_flags(_context(), _ccache, flags); }
QString CCache::type() const { return QString(krb5_cc_get_type(_context(), _ccache)); }
QString CCache::name() const { return QString(krb5_cc_get_name(_context(), _ccache)); }
Cursor CCache::cursor() { return Cursor(*this); }
Creds CCache::renewCredentials(const Principal& principal) {
  auto my_creds = std::make_unique<krb5_creds>();

  krb5_error_code retval = krb5_get_renewed_creds(_context(), my_creds.get(), principal(), _ccache, nullptr);

  qDebug("krb5_get_renewed_creds returned: %d", retval);
  if (retval) {
    krb5_free_cred_contents(_context(), my_creds.get());
    throw KRB5EXCEPTION(retval, _context, "Didn't get renewed creds");
  }

  retval = krb5_cc_initialize(_context(), _ccache, principal());
  if (retval) {
    krb5_free_cred_contents(_context(), my_creds.get());
    throw KRB5EXCEPTION(retval, _context, "Can't initialize cc");
  }

  retval = krb5_cc_store_cred(_context(), _ccache, my_creds.get());
  if (retval) {
    krb5_free_cred_contents(_context(), my_creds.get());
    throw KRB5EXCEPTION(retval, _context, "Can't store credds into cc");
  }
  return Creds(_context, std::move(my_creds));
}
}  // namespace v5
