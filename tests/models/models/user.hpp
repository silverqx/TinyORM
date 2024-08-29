#pragma once
#ifndef MODELS_USER_HPP
#define MODELS_USER_HPP

#include "orm/tiny/relations/pivot.hpp"
#include "orm/tiny/softdeletes.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::NOTE;

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::SoftDeletes;

class Phone;
class Role;
class RoleUser;
class RoleUser_Appends;
class Torrent;

class User final : public Model<User, Phone, Role, Torrent, Pivot>, // NOLINT(bugprone-exception-escape, misc-no-recursion)
                   public SoftDeletes<User>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a phone associated with the user. */
    std::unique_ptr<HasOne<User, Phone>>
    phone()
    {
        return hasOne<Phone>();
    }

    /*! The roles that belong to the user. */
    std::unique_ptr<BelongsToMany<User, Role, RoleUser>>
    roles()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Role, RoleUser>();

        relation->as("subscription")
                 .withPivot("active");

        return relation;
    }

    /*! The roles that belong to the user (to test a custom pivot with appends). */
    std::unique_ptr<BelongsToMany<User, Role, RoleUser_Appends>>
    roles_appends()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Role, RoleUser_Appends>();

        relation->as("subscription")
                 .withPivot("active");

        return relation;
    }

    /*! Get torrents associated with the user. */
    std::unique_ptr<HasMany<User, Torrent>>
    torrents()
    {
        return hasMany<Torrent>();
    }

private:
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"roles",         [](auto &v) { v(&User::roles); }},
        {"roles_appends", [](auto &v) { v(&User::roles_appends); }},
        {"phone",         [](auto &v) { v(&User::phone); }},
        {"torrents",      [](auto &v) { v(&User::torrents); }},
    };

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        "is_banned",
        NOTE,
    };

    /*! The attributes that should be cast. */
    inline static std::unordered_map<QString, CastItem> u_casts {
        {"is_banned", CastType::Boolean},
    };
};

} // namespace Models

#endif // MODELS_USER_HPP
