//
// Created by Alexander Bychuk on 17.08.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5EXCEPTION_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5EXCEPTION_H_

#include <exception>
#include <string>
#include <list>
#include <QString>
#include "krb5fwd.h"

namespace v5 {
class Context;
class Exception : public std::exception {
  krb5_error_code _retval = 0;
  std::string _what;
  int _line;
  std::string _file;
  std::string _message;
  std::string _krb5ErrMessage;
  std::list<std::string> _stackTraces;

  void makeErrorMessage(Context &context);

 public:
  Exception(krb5_error_code retval, std::string what, int line, const char *file);
  Exception(krb5_error_code retval, Context &context, std::string what, int line, const char *file);
  Exception(krb5_error_code retval, Context &context, const QString &what, int line, const char *file);
  Exception(krb5_error_code retval, Context &context, const char *what, int line, const char *file);
  Exception(const Exception &) = default;
  Exception(Exception &&) = default;
  ~Exception() override = default;
  const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
  krb5_error_code retval() const;
  QString krb5ErrorMessage() const;
  static std::string krb5ErrorMessage(krb5_error_code retval, Context &context);
  const std::string &simpleWhat() const;
  void rethrow() const;
};
}  // namespace v5

#define KRB5EXCEPTION(code, context, w) v5::Exception(code, context, w, __LINE__, __FILE__)

#endif  // KRB5_TICKET_WATCHER_SRC_KRB5EXCEPTION_H_