#ifndef ROLEUSER_H
#define ROLEUSER_H

#include "orm/tiny/relations/basepivot.hpp"

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
};

#endif // ROLEUSER_H
