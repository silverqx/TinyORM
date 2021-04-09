#ifndef SQLITEBUILDER_H
#define SQLITEBUILDER_H

#include "orm/schema/builder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema
{

    class SHAREDLIB_EXPORT SQLiteBuilder : public Builder
    {
        Q_DISABLE_COPY(SQLiteBuilder)

    public:
        using Builder::Builder;
    };

} // namespace Orm::Schema
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SQLITEBUILDER_H
