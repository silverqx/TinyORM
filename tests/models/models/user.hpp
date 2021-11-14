#pragma once
#ifndef USER_HPP
#define USER_HPP

#include "orm/tiny/model.hpp"
#include "orm/tiny/relations/pivot.hpp"

#include "models/phone.hpp"
#include "models/role.hpp"
#include "models/torrent.hpp"

namespace Models
{

using Orm::Tiny::Model;
using Orm::Tiny::Relations::HasOne;
using Orm::Tiny::Relations::HasMany;
using Orm::Tiny::Relations::Pivot;

class Phone;
class Role;

// NOLINTNEXTLINE(misc-no-recursion)
class User final : public Model<User, Phone, Role, Torrent, Pivot>
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

    /*! Get torrents associated with the user. */
    std::unique_ptr<HasMany<User, Torrent>>
    torrents()
    {
        return hasMany<Torrent>();
    }

private:
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"roles",    [](auto &v) { v(&User::roles); }},
        {"phone",    [](auto &v) { v(&User::phone); }},
        {"torrents", [](auto &v) { v(&User::torrents); }},
    };

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

} // namespace Models

#endif // USER_HPP
