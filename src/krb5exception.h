//
// Created by Alexander Bychuk on 17.08.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5EXCEPTION_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5EXCEPTION_H_

#include <exception>
#include <string>
#include <QString>
#include "krb5fwd.h"

namespace v5 {
class Context;
class Exception : public std::exception {
  krb5_error_code _retval = 0;
  std::string _what;
  int _line;
  const std::string _file;
  std::string _message;

 public:
  Exception(krb5_error_code retval, std::string what, int line, const char *file);
  Exception(krb5_error_code retval, const QString &what, int line, const char *file);
  Exception(krb5_error_code retval, const char *what, int line, const char *file);
  Exception(const Exception &) _GLIBCXX_NOTHROW = default;
  Exception(Exception &&) _GLIBCXX_NOTHROW = default;
  ~Exception() override = default;
  const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
  krb5_error_code retval() const;
  QString krb5ErrorMessage(Context &context) const;
};
}  // namespace v5

#define KRB5EXCEPTION(code, w) v5::Exception(code, w, __LINE__, __FILE__)

#endif  // KRB5_TICKET_WATCHER_SRC_KRB5EXCEPTION_H_