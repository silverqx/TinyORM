#pragma once
#ifndef MODELS_ROLEUSER_APPENDS_HPP
#define MODELS_ROLEUSER_APPENDS_HPP

#include "orm/tiny/relations/basepivot.hpp"

namespace Models
{

using Orm::Tiny::Relations::BasePivot;

// NOLINTNEXTLINE(misc-no-recursion, bugprone-exception-escape)
class RoleUser_Appends final : public BasePivot<RoleUser_Appends>
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
            return getAttribute<bool>("active");
        });
    }

private:
    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;

    /*! The attributes that should be cast. */
    inline static std::unordered_map<QString, CastItem> u_casts {
        {"active", CastType::Boolean},
    };

    /* Appends */
    /*! Map of mutator names to methods. */
    inline static const QHash<QString, MutatorFunction> u_mutators {
        {"is_active", &RoleUser_Appends::isActive},
    };

    /*! The attributes that should be appended during serialization. */
    std::set<QString> u_appends {
        "is_active",
    };
};

} // namespace Models

#endif // MODELS_ROLEUSER_APPENDS_HPP
