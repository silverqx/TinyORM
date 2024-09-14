#pragma once
#ifndef ORM_CONCERNS_EXPLAINSQUERIES_HPP
#define ORM_CONCERNS_EXPLAINSQUERIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlQuery

#include "orm/macros/export.hpp"

#ifdef TINYORM_USING_QTSQLDRIVERS
#  include "orm/macros/commonnamespace.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Query
{
    class Builder;
}
    using QueryBuilder = Query::Builder;

namespace Concerns
{

    /*! Counts the number of executed queries and the elapsed time of queries. */
    class TINYORM_EXPORT ExplainsQueries
    {
    public:
        /*! Default constructor. */
        inline ExplainsQueries() = default;
        /*! Virtual destructor. */
        inline virtual ~ExplainsQueries() = default;

        /*! Explains the query. */
        TSqlQuery explain();

    private:
        /*! Dynamic cast *this to the QueryBuilder & derived type. */
        QueryBuilder &builder();
    };

} // namespace Concerns
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_EXPLAINSQUERIES_HPP
