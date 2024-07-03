#pragma once
#ifndef ORM_CONCERNS_EXPLAINQUERIES_HPP
#define ORM_CONCERNS_EXPLAINQUERIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlQuery

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

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
    class TINYORM_EXPORT ExplainQueries
    {
    public:
        /*! Default constructor. */
        inline ExplainQueries() = default;
        /*! Virtual destructor. */
        inline virtual ~ExplainQueries() = default;

        /*! Explains the query. */
        TSqlQuery explain();

    private:
        /*! Dynamic cast *this to the QueryBuilder & derived type. */
        QueryBuilder &builder();
    };

} // namespace Concerns
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_EXPLAINQUERIES_HPP
