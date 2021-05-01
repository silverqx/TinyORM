#ifndef ROLEUSER_H
#define ROLEUSER_H

#include "orm/tiny/model.hpp"

using Orm::Tiny::Relations::BasePivot;

class RoleUser final : public BasePivot<RoleUser>
{
    friend Model<RoleUser>;
    friend BasePivot<RoleUser>;

    using BasePivot<RoleUser>::BasePivot;

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        "active",
    };

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

#endif // ROLEUSER_H
