//
// Created by Alexander Bychuk on 18.09.2022.
//

#define KRB5_PRIVATE 1
#include <krb5.h>

#include "krb5timestamphelper.h"
#include "krb5exception.h"

namespace v5 {
QString TimestampHelper::toString(krb5_timestamp timestamp) {
  static const size_t buflen = 256;
  char buffer[buflen] = {0};
  krb5_error_code retval = krb5_timestamp_to_string(timestamp, &buffer[0], buflen);
  if (retval != 0) {
    throw v5::Exception(retval, "Can't convert timestamp to string", __LINE__, __FILE__);
  }
  return QString::fromLocal8Bit(buffer);
}
}  // namespace v5