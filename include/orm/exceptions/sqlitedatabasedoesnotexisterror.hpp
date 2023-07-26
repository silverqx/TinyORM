#pragma once
#ifndef ORM_EXCEPTIONS_SQLITEDATABASEDOESNOTEXISTERROR_HPP
#define ORM_EXCEPTIONS_SQLITEDATABASEDOESNOTEXISTERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! TinyORM SQLite database doesn't exist exception. */
    class SQLiteDatabaseDoesNotExistError : public InvalidArgumentError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Path constructor (QString). */
        inline explicit SQLiteDatabaseDoesNotExistError(const QString &path);

        /*! Get the path to the database. */
        inline const QString &getPath() const noexcept;

    protected:
        /*! The path to the database. */
        QString m_path;
    };

    /* public */

    SQLiteDatabaseDoesNotExistError::SQLiteDatabaseDoesNotExistError(const QString &path)
        : InvalidArgumentError(QStringLiteral(
                                   "SQLite Database file '%1' does not exist, please "
                                   "insert an absolute path to the database.")
                               .arg(path).toUtf8().constData())
        , m_path(path)
    {}

    const QString &SQLiteDatabaseDoesNotExistError::getPath() const noexcept
    {
        return m_path;
    }

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_SQLITEDATABASEDOESNOTEXISTERROR_HPP
