#pragma once
#ifndef MYSQLPROCESSOR_HPP
#define MYSQLPROCESSOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/processors/processor.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Processors
{

    /*! MySql processor, process sql result. */
    class SHAREDLIB_EXPORT MySqlProcessor : public Processor
    {
        Q_DISABLE_COPY(MySqlProcessor)

    public:
        /*! Default constructor. */
        MySqlProcessor() = default;

        /*! Process the results of a column listing query. */
        QStringList processColumnListing(QSqlQuery &query) const override;
    };

} // namespace Orm::Query::Processors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MYSQLPROCESSOR_HPP
