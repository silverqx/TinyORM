#pragma once
#ifndef MODELS_ROLEUSER_HPP
#define MODELS_ROLEUSER_HPP

#include "orm/tiny/relations/basepivot.hpp"

namespace Models
{

using Orm::Tiny::Relations::BasePivot;

class RoleUser final : public BasePivot<RoleUser>
{
    friend Model;
    friend BasePivot;

    using BasePivot::BasePivot;

    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;
};

} // namespace Models

#endif // MODELS_ROLEUSER_HPP
