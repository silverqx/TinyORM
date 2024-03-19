#pragma once
#ifndef ORM_QUERY_PROCESSORS_POSTGRESPROCESSOR_HPP
#define ORM_QUERY_PROCESSORS_POSTGRESPROCESSOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/processors/processor.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Processors
{

    /*! PostgreSQL processor, process SQL results. */
    class PostgresProcessor final : public Processor
    {
        Q_DISABLE_COPY_MOVE(PostgresProcessor)

    public:
        /*! Default constructor. */
        PostgresProcessor() = default;
        /*! Virtual destructor. */
        ~PostgresProcessor() final = default;
    };

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_PROCESSORS_POSTGRESPROCESSOR_HPP
