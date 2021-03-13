#ifndef ROLEUSER_H
#define ROLEUSER_H

#include "orm/tiny/relations/basepivot.hpp"

using Orm::Tiny::Relations::BasePivot;

class RoleUser final : public BasePivot<RoleUser>
{
    friend BaseModel;
    friend BasePivot;

    using BasePivot::BasePivot;
};

#endif // ROLEUSER_H
