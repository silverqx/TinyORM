#pragma once
#ifndef SQLITEPROCESSOR_HPP
#define SQLITEPROCESSOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/processors/processor.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Processors
{

    /*! SQLite processor, process sql result. */
    class SHAREDLIB_EXPORT SQLiteProcessor : public Processor
    {
        Q_DISABLE_COPY(SQLiteProcessor)

    public:
        /*! Default constructor. */
        SQLiteProcessor() = default;

        /*! Process the results of a column listing query. */
        QStringList processColumnListing(QSqlQuery &query) const override;
    };

} // namespace Orm::Query::Processors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SQLITEPROCESSOR_HPP
