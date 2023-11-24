#pragma once
#ifndef ORM_DRIVERS_SQLDATABASEMANAGER_HPP
#define ORM_DRIVERS_SQLDATABASEMANAGER_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDatabase;
    class SqlDriver;

    class SHAREDLIB_EXPORT SqlDatabaseManager
    {
    protected:
        /*! Default constructor. */
        inline SqlDatabaseManager() = default;

    public:
        /*! Default destructor. */
        inline ~SqlDatabaseManager() = default;

        /*! Default Database Connection Name, used as default value in method declarations
            only. */
        static const QString defaultConnection;

        static SqlDatabase
        database(const QString &connection = defaultConnection, bool open = true);
        static bool
        contains(const QString &connection = defaultConnection);

        static SqlDatabase
        addDatabase(const QString &driver, const QString &connection = defaultConnection);
        static SqlDatabase
        addDatabase(std::unique_ptr<SqlDriver> &&driver,
                    const QString &connection = defaultConnection);

        static void
        removeDatabase(const QString &connection);

        static SqlDatabase
        cloneDatabase(const SqlDatabase &other, const QString &connection);
        static SqlDatabase
        cloneDatabase(const QString &otherConnection, const QString &connection);

        /* Getters / Setters */
        static QStringList drivers();
        static QStringList connectionNames();

        static bool isDriverAvailable(const QString &name);
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDATABASEMANAGER_HPP
