#pragma once
#ifndef TAGGED_HPP
#define TAGGED_HPP

#include "orm/tiny/model.hpp"
#include "orm/tiny/relations/basepivot.hpp"

namespace Models
{

using Orm::Tiny::Relations::BasePivot;

class Tagged final : public BasePivot<Tagged>
{
    friend Model;
    friend BasePivot;

    using BasePivot::BasePivot;

    /*! The table associated with the model. */
    QString u_table {"tag_torrent"};

    // TODO add belongsToMany overload, which will not take table argument and obtains a table name from Model::getTable, Eloquent is doing this in the BelongsToMany::resolveTableName() method silverqx
    /*! Indicates if the ID is auto-incrementing. */
//    bool u_incrementing = true;

    // FUTURE u_connection and u_table can't be overriden, the same is true in Eloquent, add support to be able to override these silveqx
};

} // namespace Models

#endif // TAGGED_HPP
