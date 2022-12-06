//
// Created by Alexander Bychuk on 25.08.2022.
//

#define KRB5_PRIVATE 1
#include <krb5.h>
#include "krb5context.h"
#include "krb5creds.h"
#include "krb5ccache.h"
#include "krb5exception.h"
#include "krb5cursor.h"
#include <iostream>

namespace v5 {
Cursor::Cursor(v5::CCache &cCache) : _cCache(cCache), _cur(nullptr) {
  krb5_error_code code = krb5_cc_start_seq_get(_cCache.context()(), _cCache(), &_cur);
  if (code) {
    throw KRB5EXCEPTION(code, _cCache.context(), "Error while starting to retrieve tickets");
  }
}
Cursor::~Cursor() noexcept {
  krb5_error_code code = krb5_cc_end_seq_get(_cCache.context()(), _cCache(), &_cur);
  if (code) {
    std::cerr << "Error while finishing ticket retrieval" << std::endl;
  } else {
    krb5_flags flags = KRB5_TC_OPENCLOSE; /* turns on OPENCLOSE mode */
    code = krb5_cc_set_flags(_cCache.context()(), _cCache(), flags);
    if (code) {
      std::cerr << "Error while closing ccache" << std::endl;
    }
  }
}
Cursor::Iterator::Iterator(v5::Cursor &cursor, std::unique_ptr<Creds> creds, krb5_error_code code)
    : _cursor(cursor), _creds(std::move(creds)), _code(code) {}
Cursor::Iterator::~Iterator() noexcept {}
Creds &Cursor::Iterator::operator*() { return *_creds; }
Cursor::Iterator Cursor::next() {
  auto cred = std::make_unique<krb5_creds>();
  krb5_error_code code = krb5_cc_next_cred(_cCache.context()(), _cCache(), &_cur, cred.get());

  if (code == KRB5_CC_END) {
    return Cursor::Iterator(*this, {}, code);
  }
  if (code != 0) {
    throw KRB5EXCEPTION(code, _cCache.context(), "Error while retrieving a ticket");
  }
  return Cursor::Iterator(*this, std::make_unique<Creds>(Creds(_cCache.context(), std::move(cred))), code);
}
Cursor::Iterator &Cursor::Iterator::operator++() {
  auto item = _cursor.next();
  this->_code = item._code;
  this->_creds = std::move(item._creds);
  return *this;
}
Cursor::Iterator Cursor::begin() { return next(); }
Cursor::Iterator Cursor::end() { return Cursor::Iterator(*this, {}, KRB5_CC_END); }
bool Cursor::Iterator::operator==(const Cursor::Iterator &o) const { return (_code == o._code); }
bool Cursor::Iterator::operator!=(const Cursor::Iterator &o) const { return !operator==(o); }
}  // namespace v5
