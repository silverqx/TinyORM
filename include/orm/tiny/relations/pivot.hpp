#pragma once
#ifndef PIVOT_HPP
#define PIVOT_HPP

#include "orm/tiny/relations/basepivot.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
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
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // PIVOT_HPP
