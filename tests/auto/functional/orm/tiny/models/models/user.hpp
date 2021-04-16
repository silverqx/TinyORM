#ifndef USER_H
#define USER_H

#include <orm/tiny/model.hpp>

#include "models/role.hpp"

using Orm::Tiny::Model;
using Orm::Tiny::Relations::BelongsToMany;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Relations::Relation;

class User final : public Model<User, Role, Pivot>
{
    friend Model;
    using Model::Model;

public:
    /*! The roles that belong to the user. */
    std::unique_ptr<Relation<User, Role>>
    roles()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<Role, RoleUser>();

        relation->as("subscription")
                .withPivot("active");

        return relation;
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "roles")
            relationVisited<Role>();
        else if (relation == "pivot") // Pivot
            relationVisited<Pivot>();
    }

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"roles", &User::roles},
    };
};

#endif // USER_H
