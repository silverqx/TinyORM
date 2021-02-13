# TinyORM: Relationships

- [Introduction](#introduction)
- [Defining Relationships](#defining-relationships)
    - [One To One](#one-to-one)
    - [One To Many](#one-to-many)
    - [One To Many (Inverse) / Belongs To](#one-to-many-inverse)
- [Many To Many Relationships](#many-to-many)
    - [Retrieving Intermediate Table Columns](#retrieving-intermediate-table-columns)
    - [Filtering Queries Via Intermediate Table Columns](#filtering-queries-via-intermediate-table-columns)
    - [Defining Custom Intermediate Table Models](#defining-custom-intermediate-table-models)
- [Querying Relations](#querying-relations)
    - [Relationship Methods Vs. Dynamic Properties](#relationship-methods-vs-dynamic-properties)
- [Eager Loading](#eager-loading)
    - [Constraining Eager Loads](#constraining-eager-loads)
    - [Lazy Eager Loading](#lazy-eager-loading)
- [Inserting & Updating Related Models](#inserting-and-updating-related-models)
- [Touching Parent Timestamps](#touching-parent-timestamps)

<a name="introduction"></a>
## Introduction

Database tables are often related to one another. For example, a blog post may have many comments or an order could be related to the user who placed it. TinyORM makes managing and working with these relationships easy, and supports a variety of common relationships:

<div class="content-list" markdown="1">
- [One To One](#one-to-one)
- [One To Many](#one-to-many)
- [Many To Many](#many-to-many)
</div>

<a name="defining-relationships"></a>
## Defining Relationships

TinyORM relationships are defined as methods on your TinyORM model classes. Since relationships also serve as powerful [query builders](/docs/{{version}}/query-builder), defining relationships as methods provides powerful method chaining and querying capabilities. For example, we may chain additional query constraints on this `posts` relationship:

    user->posts()->where("active", 1).get();

But, before diving too deep into using relationships, let's learn how to define each type of relationship supported by TinyORM.

<a name="one-to-one"></a>
### One To One

A one-to-one relationship is a very basic type of database relationship. For example, a `User` model might be associated with one `Phone` model. To define this relationship, we will place a `phone` method on the `User` model. The `phone` method should call the `hasOne` method and return its result. The `hasOne` method is available to your model via the model's `Orm::Tiny::BaseModel<Model, Relations...>` base class:

    #ifndef USER_H
    #define USER_H

    #include "orm/tiny/basemodel.hpp"

    #include "models/phone.hpp"

    using Orm::Tiny::BaseModel;
    using Orm::Tiny::Relations::Relation;

    class User final : public BaseModel<User, Phone>
    {
    public:
        friend class BaseModel;

        using BaseModel::BaseModel;

        /*! Get the phone associated with the user. */
        std::unique_ptr<Relation<User, Phone>>
        phone()
        {
            return hasOne<Phone>();
        }

    private:
        /*! The visitor to obtain a type for Related template parameter. */
        void relationVisitor(const QString &relation)
        {
            if (relation == "phone")
                relationVisited<Phone>();
        }

        /*! Map of relation names to methods. */
        QHash<QString, std::any> u_relations {
            {"phone", &User::phone},
        };
    };

    #endif // USER_H

The first template argument provided to the `hasOne` method is the type-id of the related model class. Once the relationship is defined, we may retrieve the related record using BaseModel's `getRelationValue` method. Dynamic properties allow you to access relationship methods as if they were properties defined on the model:

    auto phone = User::find(1)->getRelationValue<Phone, Orm::One>("phone");

TinyORM determines the foreign key of the relationship based on the parent model name. In this case, the `Phone` model is automatically assumed to have a `user_id` foreign key. If you wish to override this convention, you may pass a first argument to the `hasOne` method:

    return hasOne<Phone>("foreign_key");

Additionally, TinyORM assumes that the foreign key should have a value matching the primary key column of the parent. In other words, TinyORM will look for the value of the user's `id` column in the `user_id` column of the `Phone` record. If you would like the relationship to use a primary key value other than `id` or your model's `u_primaryKey` data member, you may pass a second argument to the `hasOne` method:

    return hasOne<Phone>("foreign_key", "local_key");

<a name="one-to-one-defining-the-inverse-of-the-relationship"></a>
#### Defining The Inverse Of The Relationship

So, we can access the `Phone` model from our `User` model. Next, let's define a relationship on the `Phone` model that will let us access the user that owns the phone. We can define the inverse of a `hasOne` relationship using the `belongsTo` method:

    #ifndef PHONE_H
    #define PHONE_H

    #include "orm/tiny/basemodel.hpp"

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

When invoking the `user` method, TinyORM will attempt to find a `User` model that has an `id` which matches the `user_id` column on the `Phone` model.

TinyORM determines the foreign key name by examining the name of the `Related` template parameter and suffixing the method name with `_id`. So, in this case, TinyORM assumes that the `Phone` model has a `user_id` column.

 However, if the foreign key on the `Phone` model is not `user_id`, you may pass a custom key name as the first argument to the `belongsTo` method:

    using Orm::Tiny::Relations::Relation;

    /*! Get the user that owns the phone. */
    std::unique_ptr<Relation<Phone, User>>
    user()
    {
        return belongsTo<User>("foreign_key");
    }

If the parent model does not use `id` as its primary key, or you wish to find the associated model using a different column, you may pass a second argument to the `belongsTo` method specifying the parent table's custom key:

    /*! Get the user that owns the phone. */
    std::unique_ptr<Relation<Phone, User>>
    user()
    {
        return belongsTo<User>("foreign_key", "owner_key");
    }

The third `belongsTo` parameter is the relation name, if you pass it, the foreign key name will be determined from it. By convention, TinyORM will "snake case" this relation name  and suffix it with a `_` followed by the name of the parent model's primary key column, the `__func__` predefined identifier is ideal for this:

    /*! Get the user that owns the phone. */
    std::unique_ptr<Relation<Phone, User>>
    someUser()
    {
        return belongsTo<User>({}, {}, __func__); // the foreign key will be some_user_id
    }

<a name="one-to-many"></a>
### One To Many

A one-to-many relationship is used to define relationships where a single model is the parent to one or more child models. For example, a blog post may have an infinite number of comments. Like all other TinyORM relationships, one-to-many relationships are defined by defining a method on your TinyORM model:

    #ifndef POST_H
    #define POST_H

    #include "orm/tiny/basemodel.hpp"

    #include "models/comment.hpp"

    using Orm::Tiny::BaseModel;
    using Orm::Tiny::Relations::Relation;

    class Post final : public BaseModel<Post, Comment>
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

Remember, TinyORM will automatically determine the proper foreign key column for the `Comment` model. By convention, TinyORM will take the "snake case" name of the parent model and suffix it with `_id`. So, in this example, TinyORM will assume the foreign key column on the `Comment` model is `post_id`.

Once the relationship method has been defined, we can access the `QVector<Related *>` of related comments by BaseModel's `getRelationValue` method:

    #include "models/post.hpp";

    auto comments = Post::find(1)->getRelationValue<Comment>("comments");

    for (auto *comment : comments) {
        //
    }

Since all relationships also serve as query builders, you may add further constraints to the relationship query by calling the `comments` method and continuing to chain conditions onto the query, only simple `where` methods are proxied now:

    auto comment = Post::find(1)->comments()
                        ->where("title", "foo")
                        .first();

Like the `hasOne` method, you may also override the foreign and local keys by passing additional arguments to the `hasMany` method:

    return hasMany<Comment>("foreign_key");

    return hasMany<Comment>("foreign_key", "local_key");

<a name="one-to-many-inverse"></a>
### One To Many (Inverse) / Belongs To

Now that we can access all of a post's comments, let's define a relationship to allow a comment to access its parent post. To define the inverse of a `hasMany` relationship, define a relationship method on the child model which calls the `belongsTo` method:

    #ifndef COMMENT_H
    #define COMMENT_H

    #include "orm/tiny/basemodel.hpp"

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
    };

    #endif // COMMENT_H

Once the relationship has been defined, we can retrieve a comment's parent post by BaseModel's `getRelationValue` method:

    #include "models/comment.hpp"

    auto comment = Comment::find(1);

    return comment->getRelationValue<Post, Orm::One>("post")->getAttribute("title").toString();

In the example above, TinyORM will attempt to find a `Post` model that has an `id` which matches the `post_id` column on the `Comment` model.

TinyORM determines the foreign key name by examining the name of the `Related` template parameter and suffixing the method name with a `_` followed by the name of the parent model's primary key column. So, in this case, TinyORM assumes that the `Post` model's foreign key on the `comments` table is `post_id`.

However, if the foreign key for your relationship does not follow these conventions, you may pass a custom foreign key name as the first argument to the `belongsTo` method:

    using Orm::Tiny::Relations::Relation;

    /*! Get the post that owns the comment. */
    std::unique_ptr<Relation<Comment, Post>>
    post()
    {
        return belongsTo<Post>("foreign_key");
    }

If your parent model does not use `id` as its primary key, or you wish to find the associated model using a different column, you may pass a second argument to the `belongsTo` method specifying your parent table's custom key:

    using Orm::Tiny::Relations::Relation;

    /*! Get the post that owns the comment. */
    std::unique_ptr<Relation<Comment, Post>>
    post()
    {
        return belongsTo<Post>("foreign_key", "owner_key");
    }

The third `belongsTo` parameter is the relation name, if you pass it, the foreign key name will be determined from it. By convention, TinyORM will "snake case" this relation name  and suffix it with a `_` followed by the name of the parent model's primary key column, the `__func__` predefined identifier is ideal for this:

    /*! Get the post that owns the comment. */
    std::unique_ptr<Relation<Comment, Post>>
    somePost()
    {
        return belongsTo<Post>({}, {}, __func__); // the foreign key will be some_post_id
    }

<a name="many-to-many"></a>
## Many To Many Relationships

Many-to-many relations are slightly more complicated than `hasOne` and `hasMany` relationships. An example of a many-to-many relationship is a user that has many roles and those roles are also shared by other users in the application. For example, a user may be assigned the role of "Author" and "Editor"; however, those roles may also be assigned to other users as well. So, a user has many roles and a role has many users.

<a name="many-to-many-table-structure"></a>
#### Table Structure

To define this relationship, three database tables are needed: `users`, `roles`, and `role_user`. The `role_user` table is derived from the alphabetical order of the related model names and contains `user_id` and `role_id` columns. This table is used as an intermediate table linking the users and roles.

Remember, since a role can belong to many users, we cannot simply place a `user_id` column on the `roles` table. This would mean that a role could only belong to a single user. In order to provide support for roles being assigned to multiple users, the `role_user` table is needed. We can summarize the relationship's table structure like so:

    users
        id - integer
        name - string

    roles
        id - integer
        name - string

    role_user
        user_id - integer
        role_id - integer

<a name="many-to-many-model-structure"></a>
#### Model Structure

Many-to-many relationships are defined by writing a method that returns the result of the `belongsToMany` method. The `belongsToMany` method is provided by the `Illuminate\Database\Eloquent\Model` base class that is used by all of your application's TinyORM models. For example, let's define a `roles` method on our `User` model. The first argument passed to this method is the name of the related model class:

    <?php

    namespace App\Models;

    use Illuminate\Database\Eloquent\Model;

    class User extends Model
    {
        /**
         * The roles that belong to the user.
         */
        public function roles()
        {
            return $this->belongsToMany(Role::class);
        }
    }

Once the relationship is defined, you may access the user's roles using the `roles` dynamic relationship property:

    use App\Models\User;

    $user = User::find(1);

    foreach ($user->roles as $role) {
        //
    }

Since all relationships also serve as query builders, you may add further constraints to the relationship query by calling the `roles` method and continuing to chain conditions onto the query:

    $roles = User::find(1)->roles()->orderBy('name')->get();

To determine the table name of the relationship's intermediate table, TinyORM will join the two related model names in alphabetical order. However, you are free to override this convention. You may do so by passing a second argument to the `belongsToMany` method:

    return $this->belongsToMany(Role::class, 'role_user');

In addition to customizing the name of the intermediate table, you may also customize the column names of the keys on the table by passing additional arguments to the `belongsToMany` method. The third argument is the foreign key name of the model on which you are defining the relationship, while the fourth argument is the foreign key name of the model that you are joining to:

    return $this->belongsToMany(Role::class, 'role_user', 'user_id', 'role_id');

<a name="many-to-many-defining-the-inverse-of-the-relationship"></a>
#### Defining The Inverse Of The Relationship

To define the "inverse" of a many-to-many relationship, you should define a method on the related model which also returns the result of the `belongsToMany` method. To complete our user / role example, let's define the `users` method on the `Role` model:

    <?php

    namespace App\Models;

    use Illuminate\Database\Eloquent\Model;

    class Role extends Model
    {
        /**
         * The users that belong to the role.
         */
        public function users()
        {
            return $this->belongsToMany(User::class);
        }
    }

As you can see, the relationship is defined exactly the same as its `User` model counterpart with the exception of referencing the `App\Models\User` model. Since we're reusing the `belongsToMany` method, all of the usual table and key customization options are available when defining the "inverse" of many-to-many relationships.

<a name="retrieving-intermediate-table-columns"></a>
### Retrieving Intermediate Table Columns

As you have already learned, working with many-to-many relations requires the presence of an intermediate table. TinyORM provides some very helpful ways of interacting with this table. For example, let's assume our `User` model has many `Role` models that it is related to. After accessing this relationship, we may access the intermediate table using the `pivot` attribute on the models:

    use App\Models\User;

    $user = User::find(1);

    foreach ($user->roles as $role) {
        echo $role->pivot->created_at;
    }

Notice that each `Role` model we retrieve is automatically assigned a `pivot` attribute. This attribute contains a model representing the intermediate table.

By default, only the model keys will be present on the `pivot` model. If your intermediate table contains extra attributes, you must specify them when defining the relationship:

    return $this->belongsToMany(Role::class)->withPivot('active', 'created_by');

If you would like your intermediate table to have `created_at` and `updated_at` timestamps that are automatically maintained by TinyORM, call the `withTimestamps` method when defining the relationship:

    return $this->belongsToMany(Role::class)->withTimestamps();

> {note} Intermediate tables that utilize TinyORM's automatically maintained timestamps are required to have both `created_at` and `updated_at` timestamp columns.

<a name="customizing-the-pivot-attribute-name"></a>
#### Customizing The `pivot` Attribute Name

As noted previously, attributes from the intermediate table may be accessed on models via the `pivot` attribute. However, you are free to customize the name of this attribute to better reflect its purpose within your application.

For example, if your application contains users that may subscribe to podcasts, you likely have a many-to-many relationship between users and podcasts. If this is the case, you may wish to rename your intermediate table attribute to `subscription` instead of `pivot`. This can be done using the `as` method when defining the relationship:

    return $this->belongsToMany(Podcast::class)
                    ->as('subscription')
                    ->withTimestamps();

Once the custom intermediate table attribute has been specified, you may access the intermediate table data using the customized name:

    $users = User::with('podcasts')->get();

    foreach ($users->flatMap->podcasts as $podcast) {
        echo $podcast->subscription->created_at;
    }

<a name="filtering-queries-via-intermediate-table-columns"></a>
### Filtering Queries Via Intermediate Table Columns

You can also filter the results returned by `belongsToMany` relationship queries using the `wherePivot`, `wherePivotIn`, and `wherePivotNotIn` methods when defining the relationship:

    return $this->belongsToMany(Role::class)
                    ->wherePivot('approved', 1);

    return $this->belongsToMany(Role::class)
                    ->wherePivotIn('priority', [1, 2]);

    return $this->belongsToMany(Role::class)
                    ->wherePivotNotIn('priority', [1, 2]);

<a name="defining-custom-intermediate-table-models"></a>
### Defining Custom Intermediate Table Models

If you would like to define a custom model to represent the intermediate table of your many-to-many relationship, you may call the `using` method when defining the relationship. Custom pivot models give you the opportunity to define additional methods on the pivot model.

Custom many-to-many pivot models should extend the `Illuminate\Database\Eloquent\Relations\Pivot` class while custom polymorphic many-to-many pivot models should extend the `Illuminate\Database\Eloquent\Relations\MorphPivot` class. For example, we may define a `Role` model which uses a custom `RoleUser` pivot model:

    <?php

    namespace App\Models;

    use Illuminate\Database\Eloquent\Model;

    class Role extends Model
    {
        /**
         * The users that belong to the role.
         */
        public function users()
        {
            return $this->belongsToMany(User::class)->using(RoleUser::class);
        }
    }

When defining the `RoleUser` model, you should extend the `Illuminate\Database\Eloquent\Relations\Pivot` class:

    <?php

    namespace App\Models;

    use Illuminate\Database\Eloquent\Relations\Pivot;

    class RoleUser extends Pivot
    {
        //
    }

> {note} Pivot models may not use the `SoftDeletes` trait. If you need to soft delete pivot records consider converting your pivot model to an actual TinyORM model.

<a name="custom-pivot-models-and-incrementing-ids"></a>
#### Custom Pivot Models And Incrementing IDs

If you have defined a many-to-many relationship that uses a custom pivot model, and that pivot model has an auto-incrementing primary key, you should ensure your custom pivot model class defines an `incrementing` property that is set to `true`.

    /**
     * Indicates if the IDs are auto-incrementing.
     *
     * @var bool
     */
    public $incrementing = true;

<a name="querying-relations"></a>
## Querying Relations

Since all TinyORM relationships are defined via methods, you may call those methods to obtain an instance of the relationship without actually executing a query to load the related models. In addition, all types of TinyORM relationships also serve as [query builders](/docs/{{version}}/query-builder), allowing you to continue to chain constraints onto the relationship query before finally executing the SQL query against your database.

For example, imagine a blog application in which a `User` model has many associated `Post` models:

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

You may query the `posts` relationship and add additional constraints to the relationship like so:

    #include "models/user.hpp"

    auto user = User::find(1);

    user->posts()->whereEq("active", 1).get();

You are able to use any of the Laravel [query builder's](/docs/{{version}}/query-builder) methods on the relationship, so be sure to explore the query builder documentation to learn about all of the methods that are available to you.

So far, not all query builder methods are proxied on the relation base class.

<a name="chaining-orwhere-clauses-after-relationships"></a>
#### Chaining `orWhere` Clauses After Relationships

As demonstrated in the example above, you are free to add additional constraints to relationships when querying them. However, use caution when chaining `orWhere` clauses onto a relationship, as the `orWhere` clauses will be logically grouped at the same level as the relationship constraint:

    user->posts()
        ->whereEq("active", 1)
        .orWhere("votes", ">=", 100)
        .get();

The example above will generate the following SQL. As you can see, the `or` clause instructs the query to return _any_ user with greater than 100 votes. The query is no longer constrained to a specific user:

```sql
select *
from posts
where user_id = ? and active = 1 or votes >= 100
```

In most situations, you should use [logical groups](/docs/{{version}}/query-builder#logical-grouping) to group the conditional checks between parentheses:

    user->posts()
        ->where([](auto &query) {
            return query.whereEq("active", 1)
                        .orWhere("votes", ">=", 100);
        })
        .get();

The example above will produce the following SQL. Note that the logical grouping has properly grouped the constraints and the query remains constrained to a specific user:

```sql
select *
from posts
where user_id = ? and (active = 1 or votes >= 100)
```

<a name="relationship-methods-vs-dynamic-properties"></a>
### Relationship Methods Vs. Dynamic Properties

If you do not need to add additional constraints to an TinyORM relationship query, you may access the relationship as if it were a property. For example, continuing to use our `User` and `Post` example models, we may access all of a user's posts like so:

    use App\Models\User;

    $user = User::find(1);

    foreach ($user->posts as $post) {
        //
    }

Dynamic relationship properties perform "lazy loading", meaning they will only load their relationship data when you actually access them. Because of this, developers often use [eager loading](#eager-loading) to pre-load relationships they know will be accessed after loading the model. Eager loading provides a significant reduction in SQL queries that must be executed to load a model's relations.

<a name="eager-loading"></a>
## Eager Loading

When accessing TinyORM relationships by BaseModel's `getRelationValue` method, the related models are "lazy loaded". This means the relationship data is not actually loaded until you first access them. However, TinyORM can "eager load" relationships at the time you query the parent model. Eager loading alleviates the "N + 1" query problem. To illustrate the N + 1 query problem, consider a `Book` model that "belongs to" to an `Author` model:

    using Orm::Tiny::BaseModel;
    using Orm::Tiny::Relations::Relation;

    class Book final : public BaseModel<Book, Author>
    {
    public:
        friend class BaseModel;

        using BaseModel::BaseModel;

        /*! Get the author that wrote the book. */
        std::unique_ptr<Relation<Book, Author>>
        author()
        {
            return belongsTo<Author>();
        }

    private:
        /*! The visitor to obtain a type for Related template parameter. */
        void relationVisitor(const QString &relation)
        {
            if (relation == "author")
                relationVisited<Author>();
        }

        /*! Map of relation names to methods. */
        QHash<QString, std::any> u_relations {
            {"author", &Book::author},
        };
    };

Now, let's retrieve all books and their authors:

    #include <QDebug>

    #include "models/book.hpp"

    auto books = Book::all();

    for (const auto &book : books)
        qDebug() << book.getRelationValue<Author, Orm::One>("author")
                        ->getAttribute("name").toString();

This loop will execute one query to retrieve all of the books within the database table, then another query for each book in order to retrieve the book's author. So, if we have 25 books, the code above would run 26 queries: one for the original book, and 25 additional queries to retrieve the author of each book.

Thankfully, we can use eager loading to reduce this operation to just two queries. When building a query, you may specify which relationships should be eager loaded using the `with` method:

    auto books = Book::with("author")->get();

    for (const auto &book : books)
        qDebug() << book.getRelation<Author, Orm::One>("author")
                        ->getAttribute("name").toString();

For this operation, only two queries will be executed - one query to retrieve all of the books and one query to retrieve all of the authors for all of the books:

```sql
select * from books

select * from authors where id in (1, 2, 3, 4, 5, ...)
```

<a name="eager-loading-multiple-relationships"></a>
#### Eager Loading Multiple Relationships

Sometimes you may need to eager load several different relationships. To do so, just pass a `QVector<WithItem>` of relationships to the `with` method:

    auto books = Book::with({{"author"}, {"publisher"}})->get();

<a name="nested-eager-loading"></a>
#### Nested Eager Loading

To eager a relationship's relationships, you may use "dot" syntax. For example, let's eager load all of the book's authors and all of the author's personal contacts:

    auto books = Book::with("author.contacts")->get();

<a name="eager-loading-by-default"></a>
#### Eager Loading By Default

Sometimes you might want to always load some relationships when retrieving a model. To accomplish this, you may define a `u_with` data member on the model:

    using Orm::Tiny::BaseModel;
    using Orm::Tiny::Relations::Relation;

    class Book final : public BaseModel<Book, Author>
    {
    public:
        friend class BaseModel;

        using BaseModel::BaseModel;

        /*! Get the author that wrote the book. */
        std::unique_ptr<Relation<Book, Author>>
        author()
        {
            return belongsTo<Author>();
        }

    private:
        /*! The visitor to obtain a type for Related template parameter. */
        void relationVisitor(const QString &relation)
        {
            if (relation == "author")
                relationVisited<Author>();
        }

        /*! Map of relation names to methods. */
        QHash<QString, std::any> u_relations {
            {"author", &Book::author},
        };

        /*! The relationships that should always be loaded. */
        QVector<Orm::WithItem> u_with {
            {"author"},
        };
    };

If you would like to remove an item from the `u_with` data member for a single query, you may use the `without` method:

    auto books = Book::without("author")->get();

<a name="lazy-eager-loading"></a>
### Lazy Eager Loading

Sometimes you may need to eager load a relationship after the parent model has already been retrieved. For example, this may be useful if you need to dynamically decide whether to load related models:

    #include "models/book.hpp"

    auto book = Book::find(1);

    if (someCondition)
        book->load("author");

You may load more relationships at once, to do so, just pass a `QVector<WithItem>` of relationships to the `load` method:

    book->load({{"author"}, {"publisher"}});

So far, this only works on models, not on containers returned from BaseModel's `get` or `all` methods.

<a name="inserting-and-updating-related-models"></a>
## Inserting & Updating Related Models

<a name="the-push-method"></a>
#### Recursively Saving Models & Relationships

If you would like to `save` your model and all of its associated relationships, you may use the `push` method. In this example, the `Post` model will be saved as well as its comments and the comment's authors:

    auto post = Post::find(1);

    post->getRelationValue("comments").at(0)->setAttribute("message", "Message");

    post->getRelationValue("comments").first()
        .getRelationValue("author")->setAttribute("name", "Author Name");

    post->push();

<a name="touching-parent-timestamps"></a>
## Touching Parent Timestamps

When a model defines a `belongsTo` relationship to another model, such as a `Comment` which belongs to a `Post`, it is sometimes helpful to update the parent's timestamp when the child model is updated.

For example, when a `Comment` model is updated, you may want to automatically "touch" the `updated_at` timestamp of the owning `Post` so that it is set to the current date and time. To accomplish this, you may add a `u_touches` data member to your child model containing the names of the relationships that should have their `updated_at` timestamps updated when the child model is updated:

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

> {note} Parent model timestamps will only be updated if the child model is updated using TinyORM's `save` method.
