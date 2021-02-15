#ifndef USER_H
#define USER_H

class User;

#include <orm/tiny/basemodel.hpp>

#include "models/post.hpp"

using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::Relation;

class User final : public BaseModel<User, Post>
{
public:
    friend class BaseModel;

    using BaseModel::BaseModel;

    /*! Get all of the posts for the user. */
    std::unique_ptr<Relation<User, Post>>
    posts()
    {
        return hasMany<Post>();
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "posts")
            relationVisited<Post>();
    }

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"posts", &User::posts},
    };
};

//#include "models/phone.hpp"

//using Orm::Tiny::BaseModel;
//using Orm::Tiny::Relations::Relation;

//class User final : public BaseModel<User, Phone>
//{
//public:
//    friend class BaseModel;

//    using BaseModel::BaseModel;

//    /*! Get the phone associated with the user. */
//    std::unique_ptr<Relation<User, Phone>>
//    phone()
//    {
//        return hasOne<Phone>();
//    }

//private:
//    /*! The visitor to obtain a type for Related template parameter. */
//    void relationVisitor(const QString &relation)
//    {
//        if (relation == "phone")
//            relationVisited<Phone>();
//    }

//    /*! Map of relation names to methods. */
//    QHash<QString, std::any> u_relations {
//        {"phone", &User::phone},
//    };
//};

#endif // USER_H
