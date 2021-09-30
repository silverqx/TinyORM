#pragma once
#ifndef TAGGED_HPP
#define TAGGED_HPP

#include "orm/tiny/model.hpp"

using Orm::Tiny::Relations::BasePivot;

class Tagged final : public BasePivot<Tagged>
{
    friend Model<Tagged>;
    friend BasePivot<Tagged>;

    using BasePivot<Tagged>::BasePivot;

    /*! The table associated with the model. */
    QString u_table {"tag_torrent"};

    // TODO add belongsToMany overload, which will not take table argument and obtains a table name from Model::getTable, Eloquent is doing this in the BelongsToMany::resolveTableName() method silverqx
    /*! Indicates if the ID is auto-incrementing. */
//    bool u_incrementing = true;

    // FUTURE u_connection and u_table can't be overriden, the same is true in Eloquent, add support to be able to override these silveqx
};

#endif // TAGGED_HPP
