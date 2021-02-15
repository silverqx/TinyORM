#ifndef POST_H
#define POST_H

class Post;

#include <orm/tiny/basemodel.hpp>

#include "models/comment.hpp"
#include "models/user.hpp"

using Orm::Tiny::BaseModel;
using Orm::Tiny::Relations::Relation;

class Post final : public BaseModel<Post, Comment, User>
{
public:
    friend class BaseModel;

    using BaseModel::BaseModel;

    /*! Get the comments for the blog post. */
    std::unique_ptr<Relation<Post, Comment>>
    comments()
    {
        return hasMany<Comment>();
    }

    std::unique_ptr<Relation<Post, User>>
    user()
    {
        return belongsTo<User>("foreign_key");
    }

private:
    /*! The visitor to obtain a type for Related template parameter. */
    void relationVisitor(const QString &relation)
    {
        if (relation == "comments")
            relationVisited<Comment>();
    }

    /*! Map of relation names to methods. */
    QHash<QString, std::any> u_relations {
        {"comments", &Post::comments},
    };
};

#endif // POST_H
