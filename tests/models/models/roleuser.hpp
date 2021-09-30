#pragma once
#ifndef ROLEUSER_HPP
#define ROLEUSER_HPP

#include "orm/tiny/model.hpp"

using Orm::Tiny::Relations::BasePivot;

class RoleUser final : public BasePivot<RoleUser>
{
    friend Model<RoleUser>;
    friend BasePivot<RoleUser>;

    using BasePivot<RoleUser>::BasePivot;

    /*! Indicates if the model should be timestamped. */
    bool u_timestamps = false;
};

#endif // ROLEUSER_HPP
