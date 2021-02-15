#ifndef COMMENT_H
#define COMMENT_H

#include <orm/tiny/basemodel.hpp>

#include "models/post.hpp"

using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::Relation;

class Comment final : public BaseModel<Comment, Post>
{
public:
    friend class BaseModel;

    using BaseModel::BaseModel;

    /*! Get the post that owns the comment. */
    std::unique_ptr<Relation<Comment, Post>>
    post()
    {
        return belongsTo<Post>();
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "post")
            relationVisited<Post>();
    }

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"post", &Comment::post},
    };

    /*! All of the relationships to be touched. */
    QStringList u_touches {"post"};
};

//class Comment final : public BaseModel<Comment, Post, User>
//{
//public:
//    friend class BaseModel;

//    using BaseModel::BaseModel;

//    /*! Get the post that owns the comment. */
//    std::unique_ptr<Relation<Comment, Post>>
//    post()
//    {
//        return belongsTo<Post>();
//    }
//    std::unique_ptr<Relation<Comment, User>>
//    user()
//    {
//        return belongsTo<User>();
//    }

//private:
//    /*! The visitor to obtain a type for Related template parameter. */
//    void relationVisitor(const QString &relation)
//    {
//        if (relation == "post")
//            relationVisited<Post>();
//        else if (relation == "user")
//            relationVisited<User>();
//    }

//    /*! Map of relation names to methods. */
//    QHash<QString, std::any> u_relations {
//        {"post", &Comment::post},
//        {"user", &Comment::user},
//    };
//};

#endif // COMMENT_H
