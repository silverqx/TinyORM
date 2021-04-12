#ifndef TAGGED_H
#define TAGGED_H

#include "orm/tiny/relations/basepivot.hpp"

using Orm::Tiny::Relations::BasePivot;

class Tagged final : public BasePivot<Tagged>
{
    friend Model<Tagged>;
    friend BasePivot<Tagged>;

    using BasePivot<Tagged>::BasePivot;

    // TODO add belongsToMany overload, which will not take table argument and obtains a table name from Model::getTable, Eloquent is doing this in the BelongsToMany::resolveTableName() method silverqx
    /*! Indicates if the ID is auto-incrementing. */
//    bool u_incrementing = true;

    // TODO future u_connection and u_table can't be overriden, the same is true in Eloquent, add support to be able to override these silveqx

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        "active",
    };
};

#endif // TAGGED_H
