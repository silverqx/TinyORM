#pragma once
#ifndef MODELS_ROLEUSER_APPENDS_HPP
#define MODELS_ROLEUSER_APPENDS_HPP

#include "orm/tiny/relations/basepivot.hpp"

namespace Models
{

using Orm::Tiny::Relations::BasePivot;

class RoleUser_Appends final : public BasePivot<RoleUser_Appends> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    friend BasePivot;

    using BasePivot::BasePivot;

protected:
    /*! Accessor active attribute alias (to test a custom pivot with appends). */
    Attribute isActive() const noexcept
    {
        return Attribute::make(/* get */ [this]() -> QVariant
        {
            return getAttribute<bool>(u"active"_s);
        });
    }

private:
    /*! The table associated with the model. */
    QString u_table {u"role_user"_s}; // Not used in belongsToMany()

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;

    /*! The attributes that should be cast. */
    inline static std::unordered_map<QString, CastItem> u_casts {
        {u"active"_s, CastType::Boolean},
    };

    /* Appends */
    /*! Map of mutator names to methods. */
    inline static const QHash<QString, MutatorFunction> u_mutators {
        {u"is_active"_s, &RoleUser_Appends::isActive},
    };

    /*! The attributes that should be appended during serialization. */
    std::set<QString> u_appends {
        u"is_active"_s,
    };
};

} // namespace Models

#endif // MODELS_ROLEUSER_APPENDS_HPP
