//
// Created by Alexander Bychuk on 25.08.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5TICKET_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5TICKET_H_

#include "krb5fwd.h"
#include <QString>

namespace v5 {
class Creds;

class Ticket {
  Creds &_creds;
  krb5_ticket *_tkt;
  explicit Ticket(Creds &creds);
  friend class Creds;

 public:
  ~Ticket() noexcept;
  QString encriptionTypeName() const;
};
}  // namespace v5

#endif  // KRB5_TICKET_WATCHER_SRC_KRB5TICKET_H_
