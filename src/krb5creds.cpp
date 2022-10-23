//
// Created by Alexander Bychuk on 17.08.2022.
//

#define KRB5_PRIVATE 1
#include <krb5.h>
#include "krb5_tw_gettext.h"
#include <QHostAddress>
#include "krb5context.h"
#include "krb5creds.h"
#include "krb5principal.h"
#include "krb5ccache.h"
#include "krb5exception.h"

namespace v5 {
Creds::Creds(Context& context, std::unique_ptr<krb5_creds> creds) : _context(context), _creds(std::move(creds)) {}
Creds::~Creds() { krb5_free_cred_contents(_context(), _creds.get()); }
void Creds::retriveCreds(CCache& cCache, Principal& principal, Principal& targetPrincipal) {
  krb5_creds mcreds;
  memset(_creds.get(), 0, sizeof(krb5_creds));
  memset(&mcreds, 0, sizeof(mcreds));
  mcreds.client = principal();
  mcreds.server = targetPrincipal();
  krb5_error_code retval = krb5_cc_retrieve_cred(_context(), cCache(), 0, &mcreds, _creds.get());
  if (retval != 0) {
    throw KRB5EXCEPTION(retval, _context, "Can't retrive creds from ccache");
  }
}
Creds Creds::FromCCache(Context& context) {
  CCache cCache(context);
  Principal principal = cCache.getPrincipal();
  Principal targetPrincipal = principal.buildTargetPrincipal();
  Creds creds(context, std::make_unique<krb5_creds>());
  creds.retriveCreds(cCache, principal, targetPrincipal);
  return creds;
}
bool Creds::isForwardable() const { return _creds->ticket_flags & TKT_FLG_FORWARDABLE; }
bool Creds::isForwarded() const { return _creds->ticket_flags & TKT_FLG_FORWARDED; }
bool Creds::isProxyable() const { return _creds->ticket_flags & TKT_FLG_PROXIABLE; }
bool Creds::isProxy() const { return _creds->ticket_flags & TKT_FLG_PROXY; }
bool Creds::isMayPostdate() const { return _creds->ticket_flags & TKT_FLG_MAY_POSTDATE; }
bool Creds::isPostdated() const { return _creds->ticket_flags & TKT_FLG_POSTDATED; }
bool Creds::isInvalid() const { return _creds->ticket_flags & TKT_FLG_INVALID; }
bool Creds::isRenewable() const { return _creds->ticket_flags & TKT_FLG_RENEWABLE; }
bool Creds::isInitial() const { return _creds->ticket_flags & TKT_FLG_INITIAL; }
bool Creds::isHWAuth() const { return _creds->ticket_flags & TKT_FLG_HW_AUTH; }
bool Creds::isPreAuth() const { return _creds->ticket_flags & TKT_FLG_PRE_AUTH; }
bool Creds::hasTransitionPolicy() const { return _creds->ticket_flags & TKT_FLG_TRANSIT_POLICY_CHECKED; }
bool Creds::isOkAsDelegate() const { return _creds->ticket_flags & TKT_FLG_OK_AS_DELEGATE; }
bool Creds::isAnonimous() const { return _creds->ticket_flags & TKT_FLG_ANONYMOUS; }
krb5_timestamp Creds::ticketEndTime() const { return _creds->times.endtime; }
krb5_timestamp Creds::ticketStartTime() const { return _creds->times.starttime; }
krb5_timestamp Creds::ticketRenewTillTime() const { return _creds->times.renew_till; }
krb5_deltat Creds::ticketLifeTime() const { return _creds->times.endtime - _creds->times.starttime; }
krb5_deltat Creds::ticketRenewTime() const {
  if (_creds->times.renew_till == 0) {
    return -1;
  }
  return _creds->times.renew_till - _creds->times.starttime;
}
std::pair<krb5_deltat, Creds::LifeTimeDuration> Creds::lifeTimeDuration() const {
  auto ticketLifetime = ticketLifeTime();
  krb5_deltat lifetime = ticketLifetime;
  LifeTimeDuration duration = LifeTimeDuration::SECONDS;

  if ((lifetime = (krb5_deltat)ticketLifetime / (60 * 60 * 24)) > 0) {
    duration = LifeTimeDuration::DAYS;
  } else if ((lifetime = (krb5_deltat)ticketLifetime / (60 * 60)) > 0) {
    duration = LifeTimeDuration::HOURS;
  } else if ((lifetime = (krb5_deltat)ticketLifetime / (60)) > 0) {
    duration = LifeTimeDuration::MINUTES;
  }
  return std::make_pair(lifetime, duration);
}
std::pair<krb5_deltat, Creds::LifeTimeDuration> Creds::renewTimeDuration() const {
  krb5_deltat ticketrenewtime = ticketRenewTime();
  krb5_deltat renewtime = ticketrenewtime;
  LifeTimeDuration duration = LifeTimeDuration::SECONDS;

  if (ticketrenewtime != -1) {
    if ((renewtime = (krb5_deltat)(ticketrenewtime / (60 * 60 * 24))) > 0) {
      duration = LifeTimeDuration::DAYS;
    } else if ((renewtime = (krb5_deltat)(ticketrenewtime / (60 * 60))) > 0) {
      duration = LifeTimeDuration::HOURS;
    } else if ((renewtime = (krb5_deltat)(ticketrenewtime / (60))) > 0) {
      duration = LifeTimeDuration::MINUTES;
    }
  }
  return std::make_pair(renewtime, duration);
}
QString Creds::clientName() const {
  char* name;
  QString result;
  krb5_error_code retval = krb5_unparse_name(_context(), _creds->client, &name);
  if (retval) {
    throw KRB5EXCEPTION(retval, _context, "Error while unparsing client name");
  }
  result = name;
  krb5_free_unparsed_name(_context(), name);
  return result;
}
QString Creds::serverName() const {
  char* name;
  QString result;
  krb5_error_code retval = krb5_unparse_name(_context(), _creds->server, &name);
  if (retval) {
    throw KRB5EXCEPTION(retval, _context, "Error while unparsing server name");
  }
  result = name;
  krb5_free_unparsed_name(_context(), name);
  return result;
}
bool Creds::hasStartTime() const { return _creds->times.starttime != 0; }
void Creds::setStartTimeInAuthTime() { _creds->times.starttime = _creds->times.authtime; }
QString Creds::encryptionTypeName() const {
  char buf[100];
  krb5_error_code retval = krb5_enctype_to_string(_creds->keyblock.enctype, buf, sizeof(buf));
  if (retval) {
    /* XXX if there's an error != EINVAL, I should probably report it */
    sprintf(buf, "etype %d", _creds->keyblock.enctype);
  }
  return {buf};
}
QStringList Creds::adresses() const {
  if (!_creds->addresses || !_creds->addresses[0]) {
    return {};
  }

  auto oneAddr = [](krb5_address* a) -> QString {
    QHostAddress addr;
    switch (a->addrtype) {
      case ADDRTYPE_INET: {
        if (a->length != 4) {
          return ki18n("Broken address (type %1 length %2)").arg(a->addrtype).arg(a->length);
        }
        quint32 ad;
        memcpy(&ad, a->contents, 4);
        addr = QHostAddress(ad);
      } break;
      case ADDRTYPE_INET6: {
        if (a->length != 16) {
          return ki18n("Broken address (type %1 length %2)").arg(a->addrtype).arg(a->length);
        }
        Q_IPV6ADDR ad;
        memcpy(&ad, a->contents, 16);
        addr = QHostAddress(ad);

      } break;
      default:
        return ki18n("Unknown address type %1").arg(a->addrtype);
    }
    if (addr.isNull()) {
      return ki18n("(none)");
    }
    return addr.toString();
  };

  QStringList addresses;
  for (int i = 0; _creds->addresses[i] != nullptr; i++) {
    addresses.append(oneAddr(_creds->addresses[i]));
  }
  return addresses;
}
Ticket Creds::ticket() { return Ticket(*this); }
void Creds::storeInCacheFor(Principal& principal) {
  CCache cCache = _context.ccache();

  krb5_error_code retval = krb5_cc_initialize(_context(), cCache(), principal());
  if (retval) {
    throw KRB5EXCEPTION(retval, _context, "Can't initialize cc");
  }

  retval = krb5_cc_store_cred(_context(), cCache(), _creds.get());
  if (retval) {
    throw KRB5EXCEPTION(retval, _context, "Can't store cc");
  }
}
void Creds::changePassword(const QString& newPassword) {
  int result_code;
  krb5_data result_code_string, result_string;
  krb5_error_code retval =
      krb5_change_password(_context(), _creds.get(), newPassword.toUtf8().data(), &result_code, &result_code_string, &result_string);
  if (retval != 0) {
    QString hrMessage;
    char* messageOut = nullptr;
    retval = krb5_chpw_message(_context(), (const krb5_data*)&result_string, &messageOut);
    if (retval == 0) {
      hrMessage = QString::fromLocal8Bit(messageOut);
    } else {
      hrMessage = QString::fromLocal8Bit(result_string.data, result_string.length);
    }
    krb5_free_string(_context(), messageOut);

    QString errorStr =
        QString("Can't change password, error (%1) : %2").arg(QString::fromLocal8Bit(result_code_string.data, result_code_string.length), hrMessage);
    krb5_free_data_contents(_context(), &result_code_string);
    krb5_free_data_contents(_context(), &result_string);
    throw KRB5EXCEPTION(retval, _context, errorStr);
  }
  krb5_free_data_contents(_context(), &result_code_string);
  krb5_free_data_contents(_context(), &result_string);
}
bool Creds::operator==(const Creds& o) const { return _creds == o._creds; }
}  // namespace v5