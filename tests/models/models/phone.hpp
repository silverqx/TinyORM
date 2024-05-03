#pragma once
#ifndef MODELS_PHONE_HPP
#define MODELS_PHONE_HPP

#include "orm/tiny/model.hpp"

#include "models/user.hpp"

namespace Models
{

using Orm::Tiny::Model;

class User;

class Phone final : public Model<Phone, User> // NOLINT(bugprone-exception-escape, misc-no-recursion)
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

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;
};

} // namespace Models

#endif // MODELS_PHONE_HPP
