#pragma once
#ifndef ROLEUSER_HPP
#define ROLEUSER_HPP

#include "orm/tiny/relations/basepivot.hpp"

namespace Models
{

using Orm::Tiny::Relations::BasePivot;

class RoleUser final : public BasePivot<RoleUser>
{
    friend Model;
    friend BasePivot;

    using BasePivot::BasePivot;

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

} // namespace Models

#endif // ROLEUSER_HPP
