#pragma once
#ifndef ORM_DRIVERS_SQLDATABASEMANAGER_HPP
#define ORM_DRIVERS_SQLDATABASEMANAGER_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDatabase;
    class SqlDriver;

    /*! Database connections manager. */
    class TINYDRIVERS_EXPORT SqlDatabaseManager
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

        /*! Obtain the given connection. */
        static SqlDatabase
        database(const QString &connection = defaultConnection, bool open = true);
        /*! Determine whether the manager contains the given connection. */
        static bool
        contains(const QString &connection = defaultConnection);

        /*! Register a new database connection using the given driver name. */
        static SqlDatabase
        addDatabase(const QString &driver, const QString &connection = defaultConnection);
        /*! Register a new database connection using the given driver name. */
        static SqlDatabase
        addDatabase(QString &&driver, const QString &connection = defaultConnection);
        /*! Register a new database connection using the given driver instance. */
        static SqlDatabase
        addDatabase(std::unique_ptr<SqlDriver> &&driver,
                    const QString &connection = defaultConnection);

        /*! Remove the given database connection from the manager. */
        static void
        removeDatabase(const QString &connection);

        /*! Clone the given connection and register it as a new database connection. */
        static SqlDatabase
        cloneDatabase(const SqlDatabase &other, const QString &connection);
        /*! Clone the given connection and register it as a new database connection. */
        static SqlDatabase
        cloneDatabase(const QString &otherConnection, const QString &connection);

        /* Getters / Setters */
        /*! Get all of the available database drivers. */
        static QStringList drivers();
        /*! Get all managed connection names. */
        static QStringList connectionNames();

        /*! Determine whether the given driver is available. */
        static bool isDriverAvailable(const QString &name);

    private:
        /*! Throw an exception if the driver is nullptr. */
        static void throwIfDriverIsNullptr(const std::unique_ptr<SqlDriver> &driver,
                                           const QString &connection);
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDATABASEMANAGER_HPP
