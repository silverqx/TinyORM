#pragma once
#ifndef MODELS_DATETIME_HPP
#define MODELS_DATETIME_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Datetime final : public Model<Datetime> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;

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
    inline static const QStringList u_dates { // NOLINT(cppcoreguidelines-interfaces-global-init)
        "datetime",
        "datetime_tz",
        "timestamp",
        "timestamp_tz",
        "date",
    };

public:
    /*! The storage format of the model's time columns. */
    inline static QString u_timeFormat {"HH:mm:ss"};
};

} // namespace Models

#endif // MODELS_DATETIME_HPP
