#pragma once
#ifndef ORM_QUERY_PROCESSORS_MYSQLPROCESSOR_HPP
#define ORM_QUERY_PROCESSORS_MYSQLPROCESSOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/processors/processor.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Processors
{

    /*! MySQL processor, process SQL results. */
    class MySqlProcessor final : public Processor
    {
        Q_DISABLE_COPY_MOVE(MySqlProcessor)

    public:
        /*! Default constructor. */
        MySqlProcessor() = default;
        /*! Virtual destructor. */
        ~MySqlProcessor() final = default;
    };

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_PROCESSORS_MYSQLPROCESSOR_HPP
