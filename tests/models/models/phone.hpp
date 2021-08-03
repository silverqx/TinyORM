#pragma once
#ifndef PHONE_HPP
#define PHONE_HPP

#include <orm/tiny/model.hpp>

using Orm::Tiny::Model;
using Orm::Tiny::Relations::BelongsTo;

class User;

class Phone final : public Model<Phone, User>
{
    friend Model;
    using Model::Model;

public:
    /*! Get a user that owns the phone. */
    std::unique_ptr<BelongsTo<Phone, User>>
    user()
    {
        return belongsTo<User>();
    }

private:
    /*! The table associated with the model. */
    QString u_table {"user_phones"};

    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
        {"user", [](auto &v) { v(&Phone::user); }},
    };

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

#endif // PHONE_HPP
