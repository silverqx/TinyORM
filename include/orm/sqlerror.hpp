#ifndef SQLERROR_H
#define SQLERROR_H

#include <QtSql/QSqlError>

#include "orm/ormruntimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SqlError : public OrmRuntimeError
    {
    public:
        SqlError(const char *message, const QSqlError &error);
        SqlError(const QString &message, const QSqlError &error);

        /*! Get the original Qt SQL error. */
        const QSqlError &getSqlError() const;

    protected:
        /*! Internal ctor for use from descendants to avoid an error message formatting. */
        SqlError(const QString &message, const QSqlError &error, int);

        /*! Format the Qt SQL error message. */
        QString formatMessage(const char *message, const QSqlError &error) const;

        /*! The Qt SQL error instance. */
        const QSqlError m_sqlError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SQLERROR_H
