#ifndef USER_H
#define USER_H

#include <orm/tiny/model.hpp>

#include "models/phone.hpp"
#include "models/role.hpp"

using Orm::Tiny::Model;
using Orm::Tiny::Relations::HasOne;
using Orm::Tiny::Relations::Pivot;

class User final : public Model<User, Role, Pivot, Phone>
{
    friend Model;
    using Model::Model;

public:
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

    /*! Get the phone associated with the user. */
    std::unique_ptr<HasOne<User, Phone>>
    phone()
    {
        return hasOne<Phone>();
    }

private:
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"roles", [](auto &v) { v(&User::roles); }},
        {"phone", [](auto &v) { v(&User::phone); }},
    };

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

#endif // USER_H
