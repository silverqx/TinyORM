#ifndef PHONE_H
#define PHONE_H

#include <orm/tiny/basemodel.hpp>

#include "models/user.hpp"

using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::Relation;

class Phone final : public BaseModel<Phone, User>
{
public:
    friend class BaseModel;

    using BaseModel::BaseModel;

    /*! Get the user that owns the phone. */
    std::unique_ptr<Relation<Phone, User>>
    user()
    {
        return belongsTo<User>();
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "user")
            relationVisited<User>();
    }

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"user", &Phone::user},
    };
};

#endif // PHONE_H
