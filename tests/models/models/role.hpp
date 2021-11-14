#pragma once
#ifndef MODELS_ROLE_HPP
#define MODELS_ROLE_HPP

#include "orm/tiny/model.hpp"

#include "models/roleuser.hpp"
#include "models/user.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Tiny::Model;
using Orm::Tiny::Relations::BelongsToMany;

class User;

// NOLINTNEXTLINE(misc-no-recursion)
class Role final : public Model<Role, User, RoleUser>
{
    friend Model;
    using Model::Model;

public:
    /*! Get users that belong to the role. */
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
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
    };

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

} // namespace Models

#endif // MODELS_ROLE_HPP
