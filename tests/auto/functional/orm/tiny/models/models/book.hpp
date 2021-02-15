#ifndef BOOK_HPP
#define BOOK_HPP

#include "models/user.hpp"

using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::Relation;
using Orm::WithItem;

class Book final : public BaseModel<Book, User>
{
public:
    friend class BaseModel;

    using BaseModel::BaseModel;

    /*! Get the user that wrote the book. */
    std::unique_ptr<Relation<Book, User>>
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
        {"user", &Book::user},
    };

    /*! The relationships that should always be loaded. */
    QVector<WithItem> u_with {
        {"user"},
    };
};

//using Orm::Tiny::BaseModel;
//using Orm::Tiny::Relations::Relation;

//class Book final : public BaseModel<Book, User>
//{
//public:
//    friend class BaseModel;

//    using BaseModel::BaseModel;

//    /*! Get the user that wrote the book. */
//    std::unique_ptr<Relation<Book, User>>
//    user()
//    {
//        return belongsTo<User>();
//    }

//private:
//    /*! The visitor to obtain a type for Related template parameter. */
//    void relationVisitor(const QString &relation)
//    {
//        if (relation == "user")
//            relationVisited<User>();
//    }

//    /*! Map of relation names to methods. */
//    QHash<QString, std::any> u_relations {
//        {"user", &Book::user},
//    };
//};

#endif // BOOK_HPP
