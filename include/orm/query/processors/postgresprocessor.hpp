#pragma once
#ifndef POSTGRESPROCESSOR_HPP
#define POSTGRESPROCESSOR_HPP

#include "orm/query/processors/processor.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Processors
{

    /*! PostgreSql processor, process sql result. */
    class SHAREDLIB_EXPORT PostgresProcessor : public Processor
    {
        Q_DISABLE_COPY(PostgresProcessor)

    public:
        /*! Default constructor. */
        PostgresProcessor() = default;

        /*! Process the results of a column listing query. */
        QStringList processColumnListing(QSqlQuery &query) const override;
    };

} // namespace Orm::Query::Processors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // POSTGRESPROCESSOR_HPP
