#pragma once
#ifndef MODELS_ROLE_HPP
#define MODELS_ROLE_HPP

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Constants::AddedOn;
using Orm::Constants::NAME;

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;

class RoleUser;
class RoleUser_Appends;
class User;

class Role final : public Model<Role, User, RoleUser, RoleUser_Appends, Pivot> // NOLINT(bugprone-exception-escape, misc-no-recursion)
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
        AddedOn,
    };

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;

    /*! The storage format of the model's date columns. */
    inline static QString u_dateFormat {QLatin1Char('U')};

    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {AddedOn};
};

} // namespace Models

#endif // MODELS_ROLE_HPP
