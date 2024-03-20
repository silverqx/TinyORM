#pragma once
#ifndef MODELS_DATETIME_SERIALIZEOVERRIDE_HPP
#define MODELS_DATETIME_SERIALIZEOVERRIDE_HPP

#include "orm/tiny/model.hpp"

class tst_Model_Serialization;

namespace Models
{

using Orm::Tiny::Model;

// NOLINTNEXTLINE(bugprone-exception-escape)
class Datetime_SerializeOverride final : public Model<Datetime_SerializeOverride>
{
    friend Model;
    using Model::Model;

#ifdef TINYORM_TESTS_CODE
    friend tst_Model_Serialization;
#endif

protected:
    /*! Accessor for datetime (used in tests to test accessor with serializeDateTime). */
    Attribute datetimeTest() const noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return Attribute::make(/* get */ [this]() -> QVariant
        {
            return asDateTime(getAttribute("datetime")).addMonths(1);
        });
    }

    /*! Accessor for date (used in tests to test accessor with serializeDate()). */
    Attribute dateTest() const noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return Attribute::make(/* get */ [this]() -> QVariant
        {
            return asDate(getAttribute("date")).addMonths(1);
        });
    }

    /*! Accessor for time (used in tests to test accessor with serializeTime()). */
    Attribute timeTest() const noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return Attribute::make(/* get */ [this]() -> QVariant
        {
            return asTime(getAttribute("time")).addSecs(5);
        });
    }

private:
    /*! Prepare a date for vector, map, or JSON serialization. */
    static QString serializeDate(const QDate date)
    {
        return date.toString("dd.MM.yyyy");
    }

    /*! Prepare a datetime for vector, map, or JSON serialization. */
    static QString serializeDateTime(const QDateTime &datetime)
    {
        return datetime.toUTC().toString("dd.MM.yyyy HH:mm:ss.z t");
    }

    /*! Prepare a time for vector, map, or JSON serialization. */
    static QString serializeTime(const QTime time)
    {
        /* .z reports the seconds to full available (millisecond) precision without
           trailing zeroes, so if ms is .123 it reports also .123, what means there is
           no rounding support for now and we can't force the output to .1 only. */
        return time.toString("HH-mm-ss.z");
    }

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        "datetime",
        "datetime_tz",
        "timestamp",
        "timestamp_tz",
        "date",
        "time",
        "time_ms",
    };

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;

    /*! The attributes that should be mutated to dates. */
    inline static QStringList u_dates {}; // NOLINT(cppcoreguidelines-interfaces-global-init)

    /* Appends */
    /*! Map of mutator names to methods. */
    inline static const QHash<QString, MutatorFunction> u_mutators {
        {"datetime_test", &Datetime_SerializeOverride::datetimeTest},
        {"date_test",     &Datetime_SerializeOverride::dateTest},
        {"time_test",     &Datetime_SerializeOverride::timeTest},
    };

public:
    /*! The storage format of the model's time columns. */
    inline static QString u_timeFormat {"HH:mm:ss"};
};

} // namespace Models

#endif // MODELS_DATETIME_SERIALIZE_OVERRIDE_HPP
