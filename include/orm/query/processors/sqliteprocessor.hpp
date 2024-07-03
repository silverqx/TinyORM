#pragma once
#ifndef ORM_QUERY_PROCESSORS_SQLITEPROCESSOR_HPP
#define ORM_QUERY_PROCESSORS_SQLITEPROCESSOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/processors/processor.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Processors
{

    /*! SQLite processor, process SQL results. */
    class TINYORM_EXPORT SQLiteProcessor final : public Processor
    {
        Q_DISABLE_COPY_MOVE(SQLiteProcessor)

        /*! Alias for the SqlQuery. */
        using SqlQuery = Orm::Types::SqlQuery;

    public:
        /*! Default constructor. */
        SQLiteProcessor() = default;
        /*! Virtual destructor. */
        ~SQLiteProcessor() final = default;

        /*! Process the results of a column listing query. */
        QStringList processColumnListing(SqlQuery &query) const final;
    };

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_PROCESSORS_SQLITEPROCESSOR_HPP
