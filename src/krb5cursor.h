//
// Created by Alexander Bychuk on 25.08.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5CURSOR_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5CURSOR_H_

#include "krb5fwd.h"
#include "krb5creds.h"

namespace v5 {
class CCache;

class Cursor {
  CCache &_cCache;
  krb5_cc_cursor _cur;
  explicit Cursor(CCache &cCache);
  friend class CCache;

 public:
  ~Cursor() noexcept;
  class Iterator {
    Cursor &_cursor;
    std::unique_ptr<Creds> _creds;
    krb5_error_code _code;
    Iterator(Cursor &cursor, std::unique_ptr<Creds> creds, krb5_error_code code);
    friend class Cursor;

   public:
    Iterator(const Iterator &) = delete;
    Iterator(Iterator &&) = default;
    ~Iterator() noexcept;
    Creds &operator*();
    Iterator &operator++();
    bool operator==(const Iterator &o) const;
    bool operator!=(const Iterator &o) const;
  };

  Iterator begin();
  Iterator end();

 private:
  Iterator next();
};
}  // namespace v5

#endif  // KRB5_TICKET_WATCHER_SRC_KRB5CURSOR_H_
