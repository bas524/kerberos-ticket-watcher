//
// Created by Alexander Bychuk on 18.12.2022.
//

#ifndef KRB5_TICKET_WATCHER_TMUNIT_H
#define KRB5_TICKET_WATCHER_TMUNIT_H

namespace ktw {

enum class TmUnit { UNDEFINED = -1, MICROSECONDS = 0, SECONDS = 1, MINUTES, HOURS, DAYS };

}

#endif  // KRB5_TICKET_WATCHER_TMUNIT_H
