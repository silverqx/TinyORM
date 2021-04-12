#ifndef PIVOT_H
#define PIVOT_H

#include "orm/tiny/relations/basepivot.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{

    class SHAREDLIB_EXPORT Pivot : public BasePivot<Pivot>
    {
        // TODO study, or stackoverflow friend X vs friend X<Pivot>, if template parameter deduced or all BaseXyz template instances are friends? silverqx
        friend Model<Pivot>;
        friend BasePivot<Pivot>;

        using BasePivot<Pivot>::BasePivot;

    protected:
        /*! Indicates if the ID is auto-incrementing. */
        bool u_incrementing = false;
        // FEATURE guarded silverqx
        /*! The attributes that aren't mass assignable. */
//        QStringList u_guarded;
    };

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // PIVOT_H
