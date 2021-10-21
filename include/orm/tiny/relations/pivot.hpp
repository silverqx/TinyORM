#pragma once
#ifndef ORM_PIVOT_HPP
#define ORM_PIVOT_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/relations/basepivot.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{

    /*! Basic Pivot class. */
    class SHAREDLIB_EXPORT Pivot : public BasePivot<Pivot>
    {
        // TODO study, or stackoverflow friend X vs friend X<Pivot>, if template parameter deduced or all BaseXyz template instances are friends? silverqx
        friend Model<Pivot>;
        friend BasePivot<Pivot>;

        /*! Inherit constructors. */
        using BasePivot<Pivot>::BasePivot;

    protected:
        /*! Indicates if the ID is auto-incrementing. */
        bool u_incrementing = false;
        /*! The attributes that aren't mass assignable. */
        inline static QStringList u_guarded;
    };

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_PIVOT_HPP
