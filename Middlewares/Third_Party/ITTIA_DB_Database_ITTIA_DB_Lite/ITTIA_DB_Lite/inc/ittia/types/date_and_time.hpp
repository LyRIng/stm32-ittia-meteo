/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2005-2025 by ITTIA L.L.C. All rights reserved.      */
/*                                                                        */
/*  This software is copyrighted by and is the sole property of ITTIA     */
/*  L.L.C.  All rights, title, ownership, or other interests in the       */
/*  software remain the property of ITTIA L.L.C.  This software may only  */
/*  be used in accordance with the corresponding license agreement.  Any  */
/*  unauthorized use, duplication, transmission, distribution, or         */
/*  disclosure of this software is expressly forbidden.                   */
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */
/*  written consent of ITTIA L.L.C.                                       */
/*                                                                        */
/*  ITTIA L.L.C. reserves the right to modify this software without       */
/*  notice.                                                               */
/*                                                                        */
/*  info@ittia.com                                                        */
/*  https://www.ittia.com                                                 */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

#ifndef ITTIA_TYPES_DATE_AND_TIME_H
#define ITTIA_TYPES_DATE_AND_TIME_H

#include <assert.h>
#include <ittia/db/db_coltype.h>
#include <ittia/types/bind_traits.hpp>


inline bool operator==(const db_date_t &a, const db_date_t &b) {
    return a.year == b.year && a.month == b.month && a.day == b.day;
}

inline bool operator!=(const db_date_t &a, const db_date_t &b) {
    return !(a == b);
}

inline bool operator<(const db_date_t &a, const db_date_t &b) {
    return a.year == b.year ? (a.month == b.month ? a.day < b.day : a.month < b.month) : a.year < b.year;
}

inline bool operator>(const db_date_t &a, const db_date_t &b) {
    return a.year == b.year ? (a.month == b.month ? a.day > b.day : a.month > b.month) : a.year > b.year;
}

inline bool operator<=(const db_date_t &a, const db_date_t &b) {
    return a == b || a < b;
}

inline bool operator>=(const db_date_t &a, const db_date_t &b) {
    return a == b || a > b;
}

inline bool operator==(const db_time_t &a, const db_time_t &b) {
    return a.hour == b.hour && a.minute == b.minute && a.second == b.second;
}

inline bool operator!=(const db_time_t &a, const db_time_t &b) {
    return !(a == b);
}

inline bool operator<(const db_time_t &a, const db_time_t &b) {
    return a.hour == b.hour ? (a.minute == b.minute ? a.second < b.second : a.minute < b.minute) : a.hour < b.hour;
}

inline bool operator>(const db_time_t &a, const db_time_t &b) {
    return a.hour == b.hour ? (a.minute == b.minute ? a.second > b.second : a.minute > b.minute) : a.hour > b.hour;
}

inline bool operator<=(const db_time_t &a, const db_time_t &b) {
    return a == b || a < b;
}

inline bool operator>=(const db_time_t &a, const db_time_t &b) {
    return a == b || a > b;
}

inline bool operator==(const db_datetime_t &a, const db_datetime_t &b) {
    return a.date == b.date && a.time == b.time;
}

inline bool operator!=(const db_datetime_t &a, const db_datetime_t &b) {
    return !(a == b);
}

inline bool operator<(const db_datetime_t &a, const db_datetime_t &b) {
    return a.date == b.date ? a.time < b.time : a.date < b.date;
}

inline bool operator>(const db_datetime_t &a, const db_datetime_t &b) {
    return a.date == b.date ? a.time > b.time : a.date > b.date;
}

inline bool operator<=(const db_datetime_t &a, const db_datetime_t &b) {
    return a == b || a < b;
}

inline bool operator>=(const db_datetime_t &a, const db_datetime_t &b) {
    return a == b || a > b;
}

inline bool operator==(const db_timestamp_t &a, const db_timestamp_t &b) {
    return a.datetime == b.datetime && a.microsec == b.microsec;
}

inline bool operator!=(const db_timestamp_t &a, const db_timestamp_t &b) {
    return !(a == b);
}

inline bool operator<(const db_timestamp_t &a, const db_timestamp_t &b) {
    return a.datetime == b.datetime ? a.microsec < b.microsec : a.datetime < b.datetime;
}

inline bool operator>(const db_timestamp_t &a, const db_timestamp_t &b) {
    return a.datetime == b.datetime ? a.microsec > b.microsec : a.datetime > b.datetime;
}

inline bool operator<=(const db_timestamp_t &a, const db_timestamp_t &b) {
    return a == b || a < b;
}

inline bool operator>=(const db_timestamp_t &a, const db_timestamp_t &b) {
    return a == b || a > b;
}


namespace ittia {
namespace types {

struct Date : public db_date_t
{
    Date()
        : db_date_t()
    {
    }

    Date(uint16_t year_, uint16_t month_, uint16_t day_)
        : db_date_t()
    {
        db_date_t::year = year_;
        db_date_t::month = month_;
        db_date_t::day = day_;
    }

    Date(const db_date_t& o)
        : db_date_t(o)
    {
    }

    Date & operator=(const db_date_t& o)
    {
        (void)db_date_t::operator=(o);
        return *this;
    }
};

struct Time : public db_time_t
{
    Time()
        : db_time_t()
    {
    }

    Time(uint16_t hour_, uint16_t minute_, uint16_t second_)
        : db_time_t()
    {
        db_time_t::hour = hour_;
        db_time_t::minute = minute_;
        db_time_t::second = second_;
    }

    Time(const db_time_t& o)
        : db_time_t(o)
    {
    }

    Time & operator=(const db_time_t& o)
    {
        (void)db_time_t::operator=(o);
        return *this;
    }
};

struct DateTime : public db_datetime_t
{
    DateTime()
        : db_datetime_t()
    {
    }

    DateTime(db_date_t date_, db_time_t time_)
        : db_datetime_t()
    {
        db_datetime_t::date = date_;
        db_datetime_t::time = time_;
    }

    DateTime(uint16_t year_, uint16_t month_, uint16_t day_, uint16_t hour_, uint16_t minute_, uint16_t second_)
        : db_datetime_t()
    {
        db_datetime_t::date.year = year_;
        db_datetime_t::date.month = month_;
        db_datetime_t::date.day = day_;
        db_datetime_t::time.hour = hour_;
        db_datetime_t::time.minute = minute_;
        db_datetime_t::time.second = second_;
    }

    DateTime(const db_datetime_t& o)
        : db_datetime_t(o)
    {
    }

    DateTime & operator=(const db_datetime_t& o)
    {
        (void)db_datetime_t::operator=(o);
        return *this;
    }

    ::ittia::types::Date extract_date() const {
        return db_datetime_t::date;
    }

    ::ittia::types::Time extract_time() const {
        return db_datetime_t::time;
    }

    uint16_t year() const { return db_datetime_t::date.year; }
    uint16_t month() const { return db_datetime_t::date.month; }
    uint16_t day() const { return db_datetime_t::date.day; }
    uint16_t hour() const { return db_datetime_t::time.hour; }
    uint16_t minute() const { return db_datetime_t::time.minute; }
    uint16_t second() const { return db_datetime_t::time.second; }
};

struct Timestamp : public db_timestamp_t
{
    Timestamp()
        : db_timestamp_t()
    {
    }

    Timestamp(db_datetime_t datetime_, uint32_t microsec_)
        : db_timestamp_t()
    {
        db_timestamp_t::datetime = datetime_;
        db_timestamp_t::microsec = microsec_;
    }

    Timestamp(db_date_t date_, db_time_t time_, uint32_t microsec_)
        : db_timestamp_t()
    {
        db_timestamp_t::datetime.date = date_;
        db_timestamp_t::datetime.time = time_;
        db_timestamp_t::microsec = microsec_;
    }

    Timestamp(uint16_t year_, uint16_t month_, uint16_t day_, uint16_t hour_, uint16_t minute_, uint16_t second_, uint32_t microsec_)
        : db_timestamp_t()
    {
        db_timestamp_t::datetime.date.year = year_;
        db_timestamp_t::datetime.date.month = month_;
        db_timestamp_t::datetime.date.day = day_;
        db_timestamp_t::datetime.time.hour = hour_;
        db_timestamp_t::datetime.time.minute = minute_;
        db_timestamp_t::datetime.time.second = second_;
        db_timestamp_t::microsec = microsec_;
    }

    Timestamp(const db_timestamp_t& o)
        : db_timestamp_t(o)
    {
    }

    Timestamp & operator=(const db_timestamp_t& o)
    {
        (void)db_timestamp_t::operator=(o);
        return *this;
    }

    ::ittia::types::DateTime extract_datetime() const {
        return db_timestamp_t::datetime;
    }

    ::ittia::types::Date extract_date() const {
        return db_timestamp_t::datetime.date;
    }

    ::ittia::types::Time extract_time() const {
        return db_timestamp_t::datetime.time;
    }

    uint16_t year() const { return db_timestamp_t::datetime.date.year; }
    uint16_t month() const { return db_timestamp_t::datetime.date.month; }
    uint16_t day() const { return db_timestamp_t::datetime.date.day; }
    uint16_t hour() const { return db_timestamp_t::datetime.time.hour; }
    uint16_t minute() const { return db_timestamp_t::datetime.time.minute; }
    uint16_t second() const { return db_timestamp_t::datetime.time.second; }
};

class IntervalDayToSecond
{
public:
    IntervalDayToSecond()
        : interval_(0)
    {
    }

    operator const db_interval_t&() const { return interval_; }
    operator db_interval_t&() { return interval_; }

    template<typename T> T to_microseconds() const { return interval_ / 1LL; }
    template<typename T> T to_seconds() const { return interval_ / 1000000LL; }
    template<typename T> T to_minutes() const { return interval_ / 60000000LL; }
    template<typename T> T to_hours() const { return interval_ / 3600000000LL; }
    template<typename T> T to_days() const { return interval_ / 86400000000LL; }

    template<typename T> static IntervalDayToSecond from_microseconds(T n) { return IntervalDayToSecond(n * 1LL); }
    template<typename T> static IntervalDayToSecond from_seconds(T n) { return IntervalDayToSecond(n * 1000000LL); }
    template<typename T> static IntervalDayToSecond from_minutes(T n) { return IntervalDayToSecond(n * 60000000LL); }
    template<typename T> static IntervalDayToSecond from_hours(T n) { return IntervalDayToSecond(n * 3600000000LL); }
    template<typename T> static IntervalDayToSecond from_days(T n) { return IntervalDayToSecond(n * 86400000000LL); }


    IntervalDayToSecond & operator+=(const IntervalDayToSecond &rhs) {
        interval_ += rhs.interval_;
        return *this;
    }

    const IntervalDayToSecond operator+(const IntervalDayToSecond &other) const {
        return IntervalDayToSecond(*this) += other;
    }

    IntervalDayToSecond & operator-=(const IntervalDayToSecond &rhs) {
        interval_ -= rhs.interval_;
        return *this;
    }

    const IntervalDayToSecond operator-(const IntervalDayToSecond &other) const {
        return IntervalDayToSecond(*this) -= other;
    }

    IntervalDayToSecond & operator*=(const int64_t &rhs) {
        interval_ *= rhs;
        return *this;
    }

    const IntervalDayToSecond operator*(const int64_t &other) const {
        return IntervalDayToSecond(*this) *= other;
    }

    IntervalDayToSecond & operator/=(const int64_t &rhs) {
        interval_ /= rhs;
        return *this;
    }

    const IntervalDayToSecond operator/(const int64_t &other) const {
        return IntervalDayToSecond(*this) /= other;
    }

    IntervalDayToSecond & operator*=(const float64_t &rhs) {
        interval_ = static_cast<db_interval_t>(static_cast<float64_t>(interval_) * rhs);
        return *this;
    }

    const IntervalDayToSecond operator*(const float64_t &other) const {
        return IntervalDayToSecond(*this) *= other;
    }

    IntervalDayToSecond & operator/=(const float64_t &rhs) {
        interval_ = static_cast<db_interval_t>(static_cast<float64_t>(interval_) / rhs);
        return *this;
    }

    const IntervalDayToSecond operator/(const float64_t &other) const {
        return IntervalDayToSecond(*this) /= other;
    }

    const IntervalDayToSecond operator-() const {
        return IntervalDayToSecond(-interval_);
    }

    bool operator==(const IntervalDayToSecond &other) const {
        return interval_ == other.interval_;
    }

    bool operator!=(const IntervalDayToSecond &other) const {
        return interval_ != other.interval_;
    }

    bool operator<(const IntervalDayToSecond &other) const {
        return interval_ < other.interval_;
    }

    bool operator>(const IntervalDayToSecond &other) const {
        return interval_ > other.interval_;
    }

    bool operator<=(const IntervalDayToSecond &other) const {
        return interval_ <= other.interval_;
    }

    bool operator>=(const IntervalDayToSecond &other) const {
        return interval_ >= other.interval_;
    }

private:
    IntervalDayToSecond(const db_interval_t& interval)
        : interval_(interval)
    {
    }

    IntervalDayToSecond& operator=(const db_interval_t&);

private:
    db_interval_t interval_;
};

template<> inline float64_t IntervalDayToSecond::to_microseconds<float64_t>() const { return interval_ / 1.0; }
template<> inline float64_t IntervalDayToSecond::to_seconds<float64_t>() const { return interval_ / 1000000.0; }
template<> inline float64_t IntervalDayToSecond::to_minutes<float64_t>() const { return interval_ / 60000000.0; }
template<> inline float64_t IntervalDayToSecond::to_hours<float64_t>() const { return interval_ / 3600000000.0; }
template<> inline float64_t IntervalDayToSecond::to_days<float64_t>() const { return interval_ / 86400000000.0; }

template<> inline IntervalDayToSecond IntervalDayToSecond::from_microseconds<float64_t>(float64_t n) { return IntervalDayToSecond(static_cast<db_interval_t>(n * 1.0)); }
template<> inline IntervalDayToSecond IntervalDayToSecond::from_seconds<float64_t>(float64_t n) { return IntervalDayToSecond(static_cast<db_interval_t>(n * 1000000.0)); }
template<> inline IntervalDayToSecond IntervalDayToSecond::from_minutes<float64_t>(float64_t n) { return IntervalDayToSecond(static_cast<db_interval_t>(n * 60000000.0)); }
template<> inline IntervalDayToSecond IntervalDayToSecond::from_hours<float64_t>(float64_t n) { return IntervalDayToSecond(static_cast<db_interval_t>(n * 3600000000.0)); }
template<> inline IntervalDayToSecond IntervalDayToSecond::from_days<float64_t>(float64_t n) { return IntervalDayToSecond(static_cast<db_interval_t>(n * 86400000000.0)); }

class IntervalYearToMonth
{
public:
    IntervalYearToMonth()
        : interval_(0)
    {
    }

    operator const db_interval_t&() const { return interval_; }
    operator db_interval_t&() { return interval_; }

    template<typename T> T to_months() const { return interval_ / 1LL; }
    template<typename T> T to_years() const { return interval_ / 12LL; }

    template<typename T> static IntervalYearToMonth from_months(T n) { return IntervalYearToMonth(n * 1LL); }
    template<typename T> static IntervalYearToMonth from_years(T n) { return IntervalYearToMonth(n * 12LL); }

    IntervalYearToMonth & operator+=(const IntervalYearToMonth &rhs) {
        interval_ += rhs.interval_;
        return *this;
    }

    const IntervalYearToMonth operator+(const IntervalYearToMonth &other) const {
        return IntervalYearToMonth(*this) += other;
    }

    IntervalYearToMonth & operator-=(const IntervalYearToMonth &rhs) {
        interval_ -= rhs.interval_;
        return *this;
    }

    const IntervalYearToMonth operator-(const IntervalYearToMonth &other) const {
        return IntervalYearToMonth(*this) -= other;
    }

    IntervalYearToMonth & operator*=(const int64_t &rhs) {
        interval_ *= rhs;
        return *this;
    }

    const IntervalYearToMonth operator*(const int64_t &other) const {
        return IntervalYearToMonth(*this) *= other;
    }

    IntervalYearToMonth & operator/=(const int64_t &rhs) {
        interval_ /= rhs;
        return *this;
    }

    const IntervalYearToMonth operator/(const int64_t &other) const {
        return IntervalYearToMonth(*this) /= other;
    }

    IntervalYearToMonth & operator*=(const float64_t &rhs) {
        interval_ = static_cast<db_interval_t>(static_cast<float64_t>(interval_) * rhs);
        return *this;
    }

    const IntervalYearToMonth operator*(const float64_t &other) const {
        return IntervalYearToMonth(*this) *= other;
    }

    IntervalYearToMonth & operator/=(const float64_t &rhs) {
        interval_ = static_cast<db_interval_t>(static_cast<float64_t>(interval_) / rhs);
        return *this;
    }

    const IntervalYearToMonth operator/(const float64_t &other) const {
        return IntervalYearToMonth(*this) /= other;
    }

    const IntervalYearToMonth operator-() const {
        return IntervalYearToMonth(-interval_);
    }

    bool operator==(const IntervalYearToMonth &other) const {
        return interval_ == other.interval_;
    }

    bool operator!=(const IntervalYearToMonth &other) const {
        return interval_ != other.interval_;
    }

    bool operator<(const IntervalYearToMonth &other) const {
        return interval_ < other.interval_;
    }

    bool operator>(const IntervalYearToMonth &other) const {
        return interval_ > other.interval_;
    }

    bool operator<=(const IntervalYearToMonth &other) const {
        return interval_ <= other.interval_;
    }

    bool operator>=(const IntervalYearToMonth &other) const {
        return interval_ >= other.interval_;
    }

private:
    IntervalYearToMonth(const db_interval_t& interval)
        : interval_(interval)
    {
    }

    IntervalYearToMonth& operator=(const db_interval_t& interval);

private:
    db_interval_t interval_;
};

template<> inline float64_t IntervalYearToMonth::to_months<float64_t>() const { return interval_ / 1.0; }
template<> inline float64_t IntervalYearToMonth::to_years<float64_t>() const { return interval_ / 12.0; }

template<> inline IntervalYearToMonth IntervalYearToMonth::from_months<float64_t>(float64_t n) { return IntervalYearToMonth(static_cast<db_interval_t>(n * 1.0)); }
template<> inline IntervalYearToMonth IntervalYearToMonth::from_years<float64_t>(float64_t n) { return IntervalYearToMonth(static_cast<db_interval_t>(n * 12.0)); }

class TimestampUsec
{
public:
    TimestampUsec(const db_timestamp_usec_t& value_p = 0) : timestamp_(value_p) {}
    operator const db_timestamp_usec_t&() const { return timestamp_; }
    operator db_timestamp_usec_t&() { return timestamp_; }

private:
    db_timestamp_usec_t timestamp_;
};

template <> struct BindTraits<Date> : public InheritedBindTraits<Date, db_date_t>{};
template <> struct BindTraits<Time> : public InheritedBindTraits<Time, db_time_t>{};
template <> struct BindTraits<DateTime> : public InheritedBindTraits<DateTime, db_datetime_t>{};
template <> struct BindTraits<Timestamp> : public InheritedBindTraits<Timestamp, db_timestamp_t>{};

template <> struct BindTraits<IntervalDayToSecond> : public CastBindTraits<IntervalDayToSecond, db_interval_t, DB_VARTYPE_INTERVAL_DAY_TO_SECOND_TAG>{};
template <> struct BindTraits<IntervalYearToMonth> : public CastBindTraits<IntervalYearToMonth, db_interval_t, DB_VARTYPE_INTERVAL_YEAR_TO_MONTH_TAG>{};
template <> struct BindTraits<TimestampUsec> : public CastBindTraits<TimestampUsec, db_timestamp_usec_t, DB_VARTYPE_TIMESTAMP_USEC_TAG>{};

}
}

#endif // ITTIA_TYPES_DATE_AND_TIME_H
