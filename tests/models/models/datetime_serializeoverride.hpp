#pragma once
#ifndef MODELS_DATETIME_SERIALIZEOVERRIDE_HPP
#define MODELS_DATETIME_SERIALIZEOVERRIDE_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Casts::Attribute;
using Orm::Tiny::Model;

// NOLINTNEXTLINE(bugprone-exception-escape)
class Datetime_SerializeOverride final : public Model<Datetime_SerializeOverride>
{
    friend Model;
    using Model::Model;

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

private:
    /*! Prepare a date for vector, map, or JSON serialization. */
    inline static QString serializeDate(const QDate date)
    {
        return date.toString("dd.MM.yyyy");
    }

    /*! Prepare a datetime for vector, map, or JSON serialization. */
    inline static QString serializeDateTime(const QDateTime &datetime)
    {
        return datetime.toUTC().toString("dd.MM.yyyy HH:mm:ss.z t");
    }

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        "datetime",
        "datetime_tz",
        "timestamp",
        "timestamp_tz",
        "date",
    };

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;

public:
    /*! The attributes that should be mutated to dates. */
    inline static QStringList u_dates {}; // NOLINT(cppcoreguidelines-interfaces-global-init)

private:
    /* Appends */
    /*! Map of mutator names to methods. */
    inline static const QHash<QString, MutatorFunction> u_mutators {
        {"datetime_test", &Datetime_SerializeOverride::datetimeTest},
        {"date_test",     &Datetime_SerializeOverride::dateTest},
    };
};

} // namespace Models

#endif // MODELS_DATETIME_SERIALIZE_OVERRIDE_HPP
