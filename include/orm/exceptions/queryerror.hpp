#pragma once
#ifndef QUERYERROR_H
#define QUERYERROR_H

#include <QVariant>

#include "orm/exceptions/sqlerror.hpp"

class QSqlQuery;

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Exceptions
{

    /*! Database query exception. */
    class SHAREDLIB_EXPORT QueryError : public SqlError
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
        const QString m_sql;
        /*! The bindings for the query. */
        const QVector<QVariant> m_bindings;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // QUERYERROR_H
