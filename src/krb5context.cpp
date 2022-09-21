//
// Created by Alexander Bychuk on 17.08.2022.
//

#define KRB5_PRIVATE 1
#include <krb5.h>

#include "krb5context.h"
#include "krb5exception.h"
#include "krb5ccache.h"
#include "krb5principal.h"
#include "krb5credsopts.h"
#include "krb5principal.h"

#include <QString>

namespace v5 {
Context::Context() {
  krb5_error_code err = krb5_init_context(&kcontext);
  if (err) {
    throw KRB5EXCEPTION(err, "Error at krb5_init_context");
  }
}
Context::~Context() {
  krb5_free_context(kcontext);
  kcontext = nullptr;
}
krb5_context Context::operator()() { return kcontext; }
bool Context::valid() const { return kcontext == nullptr; }
QString Context::defaultRealm() const {
  char *realm = nullptr;
  krb5_get_default_realm(kcontext, &realm);
  QString defRealm(realm);
  krb5_free_default_realm(kcontext, realm);
  return defRealm;
}
krb5_timestamp Context::currentDateTime() const {
  krb5_timestamp now;
  int e = krb5_timeofday(kcontext, &now);
  if (e != 0) {
    throw KRB5EXCEPTION(e, QString("Cannot get current time: %1").arg(strerror(e)));
  }
  return now;
}
CCache Context::ccache() { return CCache(*this); }
Principal Context::principal(const QString &name) {
  krb5_principal principal = nullptr;
  krb5_error_code retval = krb5_parse_name(kcontext, name.toStdString().c_str(), &principal);
  if (retval != 0) {
    throw KRB5EXCEPTION(retval, "can't get principal by name");
  }
  return Principal(*this, principal);
}
CredsOpts Context::credsOpts() {
  krb5_get_init_creds_opt *opts = nullptr;
  krb5_error_code retval = krb5_get_init_creds_opt_alloc(kcontext, &opts);
  if (retval != 0) {
    KRB5EXCEPTION(retval, "can't init credsopts");
  }
  return CredsOpts(*this, opts);
}
Creds Context::initCreds(Principal &principal, CredsOpts &opts, const QString &password, const QString &initService) {
  qDebug("call initCredential");
  auto creds = std::make_unique<krb5_creds>();
  std::string tmpStrSvc = initService.toStdString();
  const char *in_tkt_service = initService.isEmpty() ? nullptr : tmpStrSvc.c_str();
  krb5_error_code retval =
      krb5_get_init_creds_password(kcontext, creds.get(), principal(), password.toUtf8().data(), nullptr, nullptr, 0, in_tkt_service, opts());
  if (retval) {
    throw KRB5EXCEPTION(retval, "Can't init creds with password");
  }
  return Creds(*this, std::move(creds));
}
}  // namespace v5