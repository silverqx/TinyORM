#pragma once
#ifndef MODELS_USER_NORELATIONS_HPP
#define MODELS_USER_NORELATIONS_HPP

#include "orm/tiny/model.hpp"
#include "orm/tiny/softdeletes.hpp"

namespace Models
{

using Orm::Constants::NAME;
using Orm::Constants::NOTE;

using Orm::Tiny::Model;
using Orm::Tiny::SoftDeletes;

class User_NoRelations final : public Model<User_NoRelations>, // NOLINT(bugprone-exception-escape)
                               public SoftDeletes<User_NoRelations>
{
    friend Model;
    using Model::Model;

    /*! The table associated with the model. */
    QString u_table {u"users"_s};

    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable { // NOLINT(cppcoreguidelines-interfaces-global-init)
        NAME,
        u"is_banned"_s,
        NOTE,
    };

    /*! The attributes that should be cast. */
    inline static std::unordered_map<QString, CastItem> u_casts {
        {u"is_banned"_s, CastType::Boolean},
    };
};

} // namespace Models

#endif // MODELS_USER_NORELATIONS_HPP
