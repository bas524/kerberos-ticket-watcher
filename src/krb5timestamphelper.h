//
// Created by Alexander Bychuk on 18.09.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5TIMESTAMPHELPER_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5TIMESTAMPHELPER_H_

#include "krb5fwd.h"
#include <QString>

namespace v5 {
class TimestampHelper {
 public:
  static QString toString(krb5_timestamp timestamp);
};
}  // namespace v5

#endif  // KRB5_TICKET_WATCHER_SRC_KRB5TIMESTAMPHELPER_H_
