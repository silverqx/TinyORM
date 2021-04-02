#ifndef ROLEUSER_H
#define ROLEUSER_H

#include "orm/tiny/relations/basepivot.hpp"

using Orm::Tiny::Relations::BasePivot;

class RoleUser final : public BasePivot<RoleUser>
{
    friend BaseModel<RoleUser>;
    friend BasePivot<RoleUser>;

    using BasePivot<RoleUser>::BasePivot;
};

#endif // ROLEUSER_H
