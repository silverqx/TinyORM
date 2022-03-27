#include <QCoreApplication>

#if defined(_WIN32)
#include <qt_windows.h>
#endif

#include <orm/db.hpp>

using Orm::Constants::H127001;
using Orm::Constants::P3306;
using Orm::Constants::QMYSQL;
using Orm::Constants::SYSTEM;
using Orm::Constants::UTF8MB4;
using Orm::Constants::charset_;
using Orm::Constants::collation_;
using Orm::Constants::database_;
using Orm::Constants::driver_;
using Orm::Constants::engine_;
using Orm::Constants::host_;
using Orm::Constants::InnoDB;
using Orm::Constants::isolation_level;
using Orm::Constants::options_;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::prefix_;
using Orm::Constants::prefix_indexes;
using Orm::Constants::strict_;
using Orm::Constants::timezone_;
using Orm::Constants::username_;

int main(int /*unused*/, char */*unused*/[])
//int main(int argc, char *argv[])
{
//    QCoreApplication app(argc, argv);
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
//    SetConsoleOutputCP(1250);
#endif

    QCoreApplication::setOrganizationName("TinyORM");
    QCoreApplication::setOrganizationDomain("tinyorm.org");
    QCoreApplication::setApplicationName("tom");

    qDebug() << "tom";

    auto dm = Orm::DB::create({
        {driver_,    QMYSQL},
        {host_,      qEnvironmentVariable("DB_MYSQL_HOST", H127001)},
        {port_,      qEnvironmentVariable("DB_MYSQL_PORT", P3306)},
        {database_,  qEnvironmentVariable("DB_MYSQL_DATABASE", "")},
        {username_,  qEnvironmentVariable("DB_MYSQL_USERNAME", "")},
        {password_,  qEnvironmentVariable("DB_MYSQL_PASSWORD", "")},
        {charset_,   qEnvironmentVariable("DB_MYSQL_CHARSET", UTF8MB4)},
        {collation_, qEnvironmentVariable("DB_MYSQL_COLLATION",
                                          QStringLiteral("utf8mb4_0900_ai_ci"))},
        // CUR add timezone names to the MySQL server and test them silverqx
        {timezone_,       SYSTEM},
        {prefix_,         ""},
        {prefix_indexes,  true},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")},
        {engine_,         InnoDB},
        {options_,        QVariantHash()},
    });

//    return QCoreApplication::exec();
    return 0;
}
