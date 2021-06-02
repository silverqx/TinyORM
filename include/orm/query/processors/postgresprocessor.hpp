#ifndef POSTGRESPROCESSOR_H
#define POSTGRESPROCESSOR_H

#include "orm/query/processors/processor.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Processors
{

    class SHAREDLIB_EXPORT PostgresProcessor : public Processor
    {
        Q_DISABLE_COPY(PostgresProcessor)

    public:
        PostgresProcessor() = default;

        /*! Process the results of a column listing query. */
        QStringList processColumnListing(QSqlQuery &query) const override;
    };

} // namespace Orm::Query::Processors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // POSTGRESPROCESSOR_H
