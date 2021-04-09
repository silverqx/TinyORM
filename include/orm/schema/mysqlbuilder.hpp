#ifndef MYSQLBUILDER_H
#define MYSQLBUILDER_H

#include "orm/schema/builder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema
{

    class SHAREDLIB_EXPORT MySqlBuilder : public Builder
    {
        Q_DISABLE_COPY(MySqlBuilder)

    public:
        using Builder::Builder;

        /*! Get the column listing for a given table. */
        QStringList getColumnListing(const QString &table) const override;
    };

} // namespace Orm::Schema
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MYSQLBUILDER_H
