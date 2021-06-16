#pragma once
#ifndef ROLE_H
#define ROLE_H

#include <orm/tiny/model.hpp>

#include "models/roleuser.hpp"

using Orm::Tiny::Model;
using Orm::Tiny::Relations::BelongsToMany;
using Orm::Tiny::Relations::Pivot;

class User;

class Role final : public Model<Role, User, RoleUser>
{
    friend Model;
    using Model::Model;

public:
    /*! The users that belong to the role. */
    std::unique_ptr<BelongsToMany<Role, User>>
    users()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<User>();

        relation->withPivot("active");

        return relation;
    }

private:
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"users", [](auto &v) { v(&Role::users); }},
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        "name",
    };

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

#endif // ROLE_H
