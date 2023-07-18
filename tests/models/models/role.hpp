#pragma once
#ifndef MODELS_ROLE_HPP
#define MODELS_ROLE_HPP

#include "orm/tiny/relations/pivot.hpp"

#include "models/roleuser.hpp"
#include "models/roleuser_appends.hpp"
#include "models/user.hpp"

namespace Models
{

using Orm::Constants::NAME;

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;

class User;

// NOLINTNEXTLINE(misc-no-recursion, bugprone-exception-escape)
class Role final : public Model<Role, User, RoleUser, RoleUser_Appends, Pivot>
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
        "added_on",
    };

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;

    /*! The storage format of the model's date columns. */
    inline static QString u_dateFormat {QLatin1Char('U')};

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {"added_on"};
};

} // namespace Models

#endif // MODELS_ROLE_HPP
