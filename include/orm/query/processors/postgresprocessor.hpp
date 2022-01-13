#pragma once
#ifndef ORM_QUERY_PROCESSORS_POSTGRESPROCESSOR_HPP
#define ORM_QUERY_PROCESSORS_POSTGRESPROCESSOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/processors/processor.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Processors
{

    /*! PostgreSql processor, process sql result. */
    class SHAREDLIB_EXPORT PostgresProcessor : public Processor
    {
        Q_DISABLE_COPY(PostgresProcessor)

    public:
        /*! Default constructor. */
        inline PostgresProcessor() = default;
        /*! Virtual destructor. */
        inline ~PostgresProcessor() override = default;

        /*! Process the results of a column listing query. */
        QStringList processColumnListing(QSqlQuery &query) const override;
    };

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_PROCESSORS_POSTGRESPROCESSOR_HPP
