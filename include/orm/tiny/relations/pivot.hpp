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
    public:
        friend BaseModel;
        friend BasePivot;

        using BasePivot::BasePivot;

    protected:
        /*! Indicates if the ID is auto-incrementing. */
        bool u_incrementing = false;
        // TODO guarded silverqx
        /*! The attributes that aren't mass assignable. */
//        QStringList u_guarded;
    };

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // PIVOT_H
