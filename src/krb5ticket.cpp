//
// Created by Alexander Bychuk on 25.08.2022.
//

#define KRB5_PRIVATE 1
#include <krb5.h>
#include "krb5ticket.h"
#include "krb5creds.h"
#include "krb5exception.h"
#include "krb5context.h"

namespace v5 {
v5::Ticket::Ticket(Creds& creds) : _creds(creds), _tkt(nullptr) {
  krb5_error_code retval = krb5_decode_ticket(&_creds._creds->ticket, &_tkt);
  if (retval) {
    throw KRB5EXCEPTION(retval, _creds._context, "Can't decode ticket");
  }
}
v5::Ticket::~Ticket() noexcept { krb5_free_ticket(_creds._context(), _tkt); }
QString v5::Ticket::encriptionTypeName() const {
  char buf[100];
  QString result;
  krb5_error_code retval = krb5_enctype_to_string(_tkt->enc_part.enctype, buf, sizeof(buf));
  if (retval) {
    result = QString("etype %1").arg(_tkt->enc_part.enctype);
  } else {
    result = QString::fromLocal8Bit(buf);
  }
  return result;
}
}  // namespace v5
