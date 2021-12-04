#pragma once
#ifndef ORM_LOG_HPP
#define ORM_LOG_HPP

#include <QVariant>

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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
        QVector<QVariant> boundValues;
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

} // namespace Types

    using Log = Types::Log;

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_LOG_HPP
