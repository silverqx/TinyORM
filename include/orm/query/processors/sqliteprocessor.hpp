#pragma once
#ifndef ORM_QUERY_PROCESSORS_SQLITEPROCESSOR_HPP
#define ORM_QUERY_PROCESSORS_SQLITEPROCESSOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/processors/processor.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Processors
{

    /*! SQLite processor, process sql result. */
    class SHAREDLIB_EXPORT SQLiteProcessor : public Processor
    {
        Q_DISABLE_COPY(SQLiteProcessor)

        /*! Alias for the SqlQuery. */
        using SqlQuery = Orm::Types::SqlQuery;

    public:
        /*! Default constructor. */
        inline SQLiteProcessor() = default;
        /*! Virtual destructor. */
        inline ~SQLiteProcessor() override = default;

        /*! Process the results of a column listing query. */
        QStringList processColumnListing(SqlQuery &query) const override;
    };

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_PROCESSORS_SQLITEPROCESSOR_HPP
