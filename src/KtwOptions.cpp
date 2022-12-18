//
// Created by Alexander Bychuk on 18.12.2022.
//

#include "KtwOptions.h"
#include "krb5_tw_gettext.h"
namespace ktw {
TimeUnit::TimeUnit(krb5_deltat time, ktw::TmUnit unit) : _time(time), _unit(unit), _unitName(tmUnitTostring(unit)) {}
krb5_deltat TimeUnit::time() const { return _time; }
void TimeUnit::setTime(krb5_deltat time) { _time = time; }
TmUnit TimeUnit::unit() const { return _unit; }
void TimeUnit::setUnit(TmUnit unit) {
  _unit = unit;
  _unitName = tmUnitTostring(_unit);
}
QString TimeUnit::tmUnitTostring(TmUnit unit) {
  QString result;
  switch (unit) {
    case TmUnit::SECONDS:
      result = ki18n("seconds");
      break;
    case TmUnit::MINUTES:
      result = ki18n("minutes");
      break;
    case TmUnit::HOURS:
      result = ki18n("hours");
      break;
    case TmUnit::DAYS:
      result = ki18n("days");
      break;
    case TmUnit::MICROSECONDS:
      result = ki18n("microseconds");
      break;
    case TmUnit::UNDEFINED:
      result = ki18n("undefined");
      break;
  }
  return result;
}
TmUnit TimeUnit::tmUnitFromText(const QString &text) {
  if (text == ki18n("days")) return TmUnit::DAYS;
  if (text == ki18n("hours")) return TmUnit::HOURS;
  if (text == ki18n("minutes")) return TmUnit::MINUTES;
  if (text == ki18n("seconds")) return TmUnit::SECONDS;
  if (text == ki18n("microseconds")) return TmUnit::MICROSECONDS;
  return TmUnit::UNDEFINED;
}
const QString &TimeUnit::unitName() const { return _unitName; }
krb5_deltat TimeUnit::ticketTime() const {
  krb5_deltat result = 0;
  switch (_unit) {
    case TmUnit::SECONDS:
      result = _time;
      break;
    case TmUnit::MINUTES:
      result = _time * 60;
      break;
    case TmUnit::HOURS:
      result = _time * 60 * 60;
      break;
    case TmUnit::DAYS:
      result = _time * 60 * 60 * 24;
      break;
    case TmUnit::MICROSECONDS:
      result = _time / 1000;
      break;
    case TmUnit::UNDEFINED:
      result = -1;
      break;
  }
  return result;
}
krb5_deltat TimeUnit::as(TmUnit unit) const {
  krb5_deltat secsTime = ticketTime();
  krb5_deltat result = 0;
  switch (unit) {
    case TmUnit::SECONDS:
      result = secsTime;
      break;
    case TmUnit::MINUTES:
      result = secsTime / 60;
      break;
    case TmUnit::HOURS:
      result = secsTime / 60 / 60;
      break;
    case TmUnit::DAYS:
      result = secsTime / 60 / 60 / 24;
      break;
    case TmUnit::MICROSECONDS:
      result = secsTime * 1000;
      break;
    case TmUnit::UNDEFINED:
      // do nothing
      break;
  }
  return result;
}
QMap<QString, QVariant> Options::toKeyValueProps() const {
  QMap<QString, QVariant> result;
  result.insert("forwardable", forwardable);
  result.insert("proxiable", proxiable);
  result.insert("lifetime.time", lifetime.time());
  result.insert("lifetime.unit", static_cast<int>(lifetime.unit()));
  result.insert("renewtime.time", renewtime.time());
  result.insert("renewtime.unit", static_cast<int>(renewtime.unit()));
  result.insert("promptInterval.time", promptInterval.time());
  result.insert("promptInterval.unit", static_cast<int>(promptInterval.unit()));
  return result;
}
}  // namespace ktw