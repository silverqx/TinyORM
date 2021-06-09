#ifndef LOG_HPP
#define LOG_HPP

#include "orm/types/boundvalues.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
namespace Types
{

    /*! Query Log record. */
    struct Log
    {
        /*! Type of the query in log record. */
        enum struct Type
        {
            UNDEFINED = -1,
            NORMAL,
            TRANSACTION,
        };

        /*! Executed query. */
        QString query;
        /*! Map of bound values. */
        BoundValues boundValues {};
        /*! Type of the query in log record. */
        Type type = Type::UNDEFINED;
        /*! Order of the query log record. */
        std::size_t order = 0;
        /*! Query execution time. */
        qint64 elapsed = -1;
        /*! Size of the result (number of rows returned). */
        int results = -1;
        /*! Number of rows affected by the query. */
        int affected = -1;
    };

} // namespace Orm::Types

    using Log = Types::Log;

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // LOG_HPP
