#pragma once
#ifndef MODELS_TAGGED_HPP
#define MODELS_TAGGED_HPP

#include "orm/tiny/relations/basepivot.hpp"

namespace Models
{

using Orm::Tiny::CastItem;
using Orm::Tiny::CastType;
using Orm::Tiny::Relations::BasePivot;

// NOLINTNEXTLINE(bugprone-exception-escape)
class Tagged final : public BasePivot<Tagged>
{
    friend Model;
    friend BasePivot;

    using BasePivot::BasePivot;

    /*! The table associated with the model. */
    QString u_table {"tag_torrent"};

    /*! Indicates if the ID is auto-incrementing. */
//    bool u_incrementing = true;

    /*! The attributes that should be cast. */
    inline static std::unordered_map<QString, CastItem> u_casts {
        {"active", CastType::Boolean},
    };

    /* Below is true, only when obtaining pivot records from the database during
       the lazy or eager loading with the Custom Pivot models only.
       It's not true eg. if you call Tagged::create()/save()/update()/..., in all this
       cases the Ignored u_xyz data members are taken into account normally❗

       Ignored  : u_connection, u_timestamps, CREATED_AT, UPDATED_AT
       Accepted : u_attributes, u_dates, u_dateFormat, u_fillable, u_guarded,
                  u_incrementing, u_table

       Notes : u_connection - inferred from the parent model
               u_timestamps - true if attributes contain both CREATED_AT and UPDATED_AT
               CREATED/UPDATED_AT - inferred from the parent model, can be overridden
                                    using the withTimestamps() method
    */
};

} // namespace Models

#endif // MODELS_TAGGED_HPP
