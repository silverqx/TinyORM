#pragma once
#ifndef ORM_EXCEPTIONS_QUERYERROR_HPP
#define ORM_EXCEPTIONS_QUERYERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/exceptions/sqlerror.hpp"

class QSqlQuery;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! TinyORM Database query exception. */
    class SHAREDLIB_EXPORT QueryError : public SqlError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! const char * constructor. */
        QueryError(const char *message, const QSqlQuery &query,
                   const QVector<QVariant> &bindings = {});
        /*! QString constructor. */
        QueryError(const QString &message, const QSqlQuery &query,
                   const QVector<QVariant> &bindings = {});

        /*! Get the SQL for the query. */
        const QString &getSql() const;
        /*! Get the bindings for the query. */
        const QVector<QVariant> &getBindings() const;

    protected:
        /*! Format the Qt SQL error message. */
        QString formatMessage(const char *message, const QSqlQuery &query);

        /*! The SQL for the query. */
        QString m_sql;
        /*! The bindings for the query. */
        QVector<QVariant> m_bindings;
    };

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_QUERYERROR_HPP
