#ifndef USER_H
#define USER_H

#include <orm/tiny/model.hpp>

#include "models/role.hpp"

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Relations::Relation;

class User final : public Model<User, Role, Pivot>
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

private:
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"roles", [](auto &v) { v(&User::roles); }},
    };
};

#endif // USER_H
