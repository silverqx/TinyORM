#ifndef ROLE_H
#define ROLE_H

#include <orm/tiny/basemodel.hpp>

class User; // Forward declaration to avoid cyclic dependency

#include "models/roleuser.hpp"

using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::Pivot;
using Orm::Tiny::Relations::Relation;

class Role final : public BaseModel<Role, User, RoleUser>
{
    friend BaseModel;
    using BaseModel::BaseModel;

public:
    /*! The users that belong to the role. */
    std::unique_ptr<Relation<Role, User>>
    users()
    {
        return belongsToMany<User>();
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
};

#endif // ROLE_H
