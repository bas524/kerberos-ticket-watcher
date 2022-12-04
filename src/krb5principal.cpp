//
// Created by Alexander Bychuk on 17.08.2022.
//
#define KRB5_PRIVATE 1
#include <krb5.h>

#include "krb5principal.h"
#include "krb5exception.h"
#include "krb5context.h"
#include "krb5ccache.h"

namespace v5 {
Principal::Principal(CCache& cCache) : _context(cCache.context()) {
  krb5_error_code retval = krb5_cc_get_principal(_context(), cCache(), &_principal);
  if (retval != 0) {
    throw KRB5EXCEPTION(retval, _context, "Cant' get principal from ccache");
  }
}
Principal::Principal(Context& context, krb5_principal principal) : _context(context), _principal(principal) {}
Principal::~Principal() {
  if (_principal) {
    krb5_free_principal(_context(), _principal);
  }
}
Principal::Principal(Principal&& o) noexcept : _context(o._context), _principal(o._principal) { o._principal = nullptr; }
Principal::Principal(CCache& cCache, krb5_principal principal) : _context(cCache.context()), _principal(principal) {}

QString Principal::realm() const { return QString::fromLocal8Bit(_principal->realm.data, (int)_principal->realm.length); }

Principal Principal::buildTargetPrincipal() const {
  krb5_principal targetPrincipal = nullptr;
  auto realmName = realm().toStdString();

  krb5_error_code retval = krb5_build_principal_ext(
      _context(), &targetPrincipal, realmName.length(), realmName.data(), KRB5_TGS_NAME_SIZE, KRB5_TGS_NAME, realmName.length(), realmName.data(), 0);
  if (retval != 0) {
    throw KRB5EXCEPTION(retval, _context, "Can't build target principal");
  }
  return Principal(_context, targetPrincipal);
}

krb5_principal Principal::operator()() const { return _principal; }
QString Principal::name() const {
  char* defname = nullptr;
  krb5_error_code retval = krb5_unparse_name(_context(), _principal, &defname);
  if (retval != 0) {
    throw KRB5EXCEPTION(retval, _context, "Error while unparsing principal name");
  }
  QString result(defname);
  krb5_free_unparsed_name(_context(), defname);
  return result;
}

}  // namespace v5
