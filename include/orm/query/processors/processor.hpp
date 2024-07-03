#pragma once
#ifndef ORM_QUERY_PROCESSORS_PROCESSOR_HPP
#define ORM_QUERY_PROCESSORS_PROCESSOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QStringList>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Types
{
    class SqlQuery;
}

namespace Query::Processors
{

    /*! Base processor class, process SQL results. */
    class TINYORM_EXPORT Processor
    {
        Q_DISABLE_COPY_MOVE(Processor)

        /*! Alias for the SqlQuery. */
        using SqlQuery = Orm::Types::SqlQuery;

    public:
        /*! Default constructor. */
        Processor() = default;
        /*! Pure virtual destructor. */
        inline virtual ~Processor() = 0;

        /*! Process the results of a column listing query. */
        virtual QStringList processColumnListing(SqlQuery &query) const;
    };

    /* public */

    Processor::~Processor() = default;

} // namespace Query::Processors
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_PROCESSORS_PROCESSOR_HPP
