#pragma once
#ifndef MODELS_DATETIME_HPP
#define MODELS_DATETIME_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

class Datetime final : public Model<Datetime>
{
    friend Model;
    using Model::Model;

private:
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

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates { // NOLINT(cppcoreguidelines-interfaces-global-init)
        "datetime",
        "datetime_tz",
        "timestamp",
        "timestamp_tz",
        "date",
    };
};

} // namespace Models

#endif // MODELS_DATETIME_HPP
