//
// Created by Alexander Bychuk on 17.08.2022.
//

#define KRB5_PRIVATE 1
#include <krb5.h>

#include "krb5exception.h"
#include "krb5context.h"
#include "stacktrace.h"
#include <sstream>

namespace v5 {
Exception::Exception(krb5_error_code retval, std::string what, int line, const char *file)
    : _retval(retval), _what(std::move(what)), _line(line), _file(file) {
  _message = std::string(_file).append(":").append(std::to_string(_line)).append(" ").append(_what).append(" krb5:").append(std::to_string(retval));
  std::stringstream ss;
  print_stacktrace(ss);
  _stackTraces.push_back(ss.str());
}
Exception::Exception(krb5_error_code retval, Context &context, std::string what, int line, const char *file)
    : _retval(retval), _what(std::move(what)), _line(line), _file(file) {
  makeErrorMessage(context);
}
Exception::Exception(krb5_error_code retval, Context &context, const QString &what, int line, const char *file)
    : _retval(retval), _what(what.toStdString()), _line(line), _file(file) {
  makeErrorMessage(context);
}
Exception::Exception(krb5_error_code retval, Context &context, const char *what, int line, const char *file)
    : _retval(retval), _what(what), _line(line), _file(file) {
  makeErrorMessage(context);
}
void Exception::makeErrorMessage(Context &context) {
  _krb5ErrMessage = krb5ErrorMessage(_retval, context);
  _message = std::string(_file).append(":").append(std::to_string(_line)).append(" ").append(_what).append(" krb5:").append(_krb5ErrMessage);
  std::stringstream ss;
  print_stacktrace(ss);
  _stackTraces.push_back(ss.str());
}
const char *Exception::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW { return _message.c_str(); }
krb5_error_code Exception::retval() const { return _retval; }
std::string Exception::krb5ErrorMessage(krb5_error_code retval, Context &context) {
  const char *message = krb5_get_error_message(context(), retval);
  std::string msg(message);
  krb5_free_error_message(context(), message);
  return msg;
}
QString Exception::krb5ErrorMessage() const { return QString::fromStdString(_krb5ErrMessage); }
const std::string &Exception::simpleWhat() const { return _what; }
void Exception::rethrow() const {
  Exception ex(*this);
  std::stringstream ss;
  print_stacktrace(ss);
  ex._stackTraces.push_back(ss.str());
  throw ex;
}
}  // namespace v5