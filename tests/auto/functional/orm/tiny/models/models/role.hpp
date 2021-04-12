#ifndef ROLE_H
#define ROLE_H

#include <orm/tiny/model.hpp>

class User; // Forward declaration to avoid cyclic dependency

#include "models/roleuser.hpp"

using Orm::Tiny::Model;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Relations::Relation;

class Role final : public Model<Role, User, RoleUser>
{
    friend Model;
    using Model::Model;

public:
    /*! The users that belong to the role. */
    std::unique_ptr<Relation<Role, User>>
    users()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<User>();
        dynamic_cast<BelongsToMany<Role, User> &>(*relation)
                .withPivot("active");

        return relation;
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "users")
            relationVisited<User>();
        else if (relation == "subscription") // Pivot
            relationVisited<RoleUser>();
    }

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"users", &Role::users},
    };

    /*! The attributes that are mass assignable. */
    inline static QStringList u_fillable {
        "name",
    };
};

#endif // ROLE_H
