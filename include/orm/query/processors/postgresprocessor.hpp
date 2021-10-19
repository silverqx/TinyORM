#pragma once
#ifndef POSTGRESPROCESSOR_HPP
#define POSTGRESPROCESSOR_HPP

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
        PostgresProcessor() = default;

        /*! Process the results of a column listing query. */
        QStringList processColumnListing(QSqlQuery &query) const override;
    };

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE

#endif // POSTGRESPROCESSOR_HPP
