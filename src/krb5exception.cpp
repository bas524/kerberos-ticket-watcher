//
// Created by Alexander Bychuk on 17.08.2022.
//

#define KRB5_PRIVATE 1
#include <krb5.h>

#include "krb5exception.h"
#include "krb5context.h"
namespace v5 {
Exception::Exception(krb5_error_code retval, std::string what, int line, const char *file)
    : _retval(retval), _what(std::move(what)), _line(line), _file(file) {
  _message = std::string(_file).append(":").append(std::to_string(_line)).append(" ").append(_what);
}
Exception::Exception(krb5_error_code retval, const QString &what, int line, const char *file)
    : _retval(retval), _what(what.toStdString()), _line(line), _file(file) {}
Exception::Exception(krb5_error_code retval, const char *what, int line, const char *file) : _retval(retval), _what(what), _line(line), _file(file) {}
const char *Exception::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW { return _message.c_str(); }
krb5_error_code Exception::retval() const { return _retval; }
QString Exception::krb5ErrorMessage(Context &context) const {
  const char *message = krb5_get_error_message(context(), _retval);
  QString msg(message);
  krb5_free_error_message(context(), message);
  return msg;
}
}  // namespace v5