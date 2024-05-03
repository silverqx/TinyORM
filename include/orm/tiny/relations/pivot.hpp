#pragma once
#ifndef ORM_TINY_RELATIONS_PIVOT_HPP
#define ORM_TINY_RELATIONS_PIVOT_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/relations/basepivot.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Tiny::Relations
{

    /*! Basic Pivot class. */
    class Pivot : public BasePivot<Pivot> // NOLINT(bugprone-exception-escape, misc-no-recursion)
    {
        // TODO study, or stackoverflow friend X vs friend X<Pivot>, if template parameter deduced or all BaseXyz template instances are friends? silverqx
        friend Model<Pivot>;
        friend BasePivot<Pivot>;

        /*! Inherit constructors. */
        using BasePivot<Pivot>::BasePivot;

    protected:
        /*! Indicates if the ID is auto-incrementing. */
        bool u_incrementing = false;

        /*! The attributes that aren't mass assignable, by default all attributes are
            mass assignable for Pivots. */
        T_THREAD_LOCAL
        inline static QStringList u_guarded;
    };

} // namespace Tiny::Relations

    /*! Alias for the Relations::Pivot, shortcut alias. */
    using Tiny::Relations::Pivot; // NOLINT(misc-unused-using-decls)

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_PIVOT_HPP
