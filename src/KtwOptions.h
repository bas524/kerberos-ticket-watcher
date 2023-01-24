//
// Created by Alexander Bychuk on 18.12.2022.
//

#ifndef KRB5_TICKET_WATCHER_KTWOPTIONS_H
#define KRB5_TICKET_WATCHER_KTWOPTIONS_H

#include <QString>
#include <QMap>
#include <QVariant>
#include "krb5fwd.h"
#include "tmunit.h"

namespace ktw {

class TimeUnit {
  krb5_deltat _time;
  TmUnit _unit;
  QString _unitName;

 public:
  enum class LocaleOpt { USE = 0, DONT_USE = 1 };
  TimeUnit(krb5_deltat time, TmUnit unit);
  TimeUnit(const TimeUnit &) = default;
  TimeUnit(TimeUnit &&) = default;
  TimeUnit &operator=(const TimeUnit &) = default;
  TimeUnit &operator=(TimeUnit &&) = default;
  ~TimeUnit() = default;

  krb5_deltat time() const;
  void setTime(krb5_deltat time);
  TmUnit unit() const;
  void setUnit(TmUnit unit);
  const QString &unitName() const;
  krb5_deltat ticketTime() const;
  krb5_deltat as(TmUnit unit) const;
  static QString tmUnitTostring(TmUnit unit, LocaleOpt lopt = LocaleOpt::USE);
  static TmUnit tmUnitFromText(const QString &text, LocaleOpt lopt = LocaleOpt::USE);
};

struct Options {
  bool forwardable = true;
  bool proxiable = false;
  TimeUnit lifetime = TimeUnit(0, TmUnit::HOURS);
  TimeUnit renewtime = TimeUnit(0, TmUnit::DAYS);
  TimeUnit promptInterval = TimeUnit(31, TmUnit::MINUTES);
  QMap<QString, QVariant> toKeyValueProps() const;
  static Options fromKeyValueProps(const QMap<QString, QVariant> &props);
  QString ldapServer;
};
}  // namespace ktw
#endif  // KRB5_TICKET_WATCHER_KTWOPTIONS_H
