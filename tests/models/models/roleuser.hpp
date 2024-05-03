#pragma once
#ifndef MODELS_ROLEUSER_HPP
#define MODELS_ROLEUSER_HPP

#include "orm/tiny/relations/basepivot.hpp"

namespace Models
{

using Orm::Tiny::Relations::BasePivot;

class RoleUser final : public BasePivot<RoleUser> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    friend BasePivot;

    using BasePivot::BasePivot;

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;

    /*! The attributes that should be cast. */
    inline static std::unordered_map<QString, CastItem> u_casts {
        {"active", CastType::Boolean},
    };
};

} // namespace Models

#endif // MODELS_ROLEUSER_HPP
