#include <QCoreApplication>
#include <QTest>

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlRecord

#include "orm/db.hpp"
#include "orm/exceptions/multiplecolumnsselectederror.hpp"
#include "orm/mysqlconnection.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"
#include "macros.hpp"

using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::TZ00;
using Orm::Constants::UTC;
using Orm::Constants::qt_timezone;
using Orm::Constants::timezone_;

using Orm::DB;
using Orm::Exceptions::MultipleColumnsSelectedError;
using Orm::MySqlConnection;
using Orm::QtTimeZoneConfig;

using QueryBuilder = Orm::Query::Builder;
using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

// TEST exceptions in tests, qt doesn't care about exceptions, totally ignore it, so when the exception is thrown, I didn't get any exception message or something similar, nothing 👿, try to solve it somehow 🤔 silverqx
class tst_DatabaseConnection : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    void pingDatabase() const;

    void isNotMaria_OnMySqlConnection() const;
    void isMaria_OnMariaConnection() const;

    void transaction_Commit() const;
    void transaction_RollBack() const;
    void transaction_Commit_Double() const;
    void transaction_RollBack_Double() const;

    void transaction_Savepoints_Commit_AllSuccess() const;
    void transaction_Savepoints_Commit_OneFailed() const;
    void transaction_Savepoints_Commit_AllFailed() const;
    void transaction_Savepoints_Commit_AllFailed_Double() const;

    void timezone_And_qt_timezone() const;

    void scalar() const;
    void scalar_EmptyResult() const;
    void scalar_MultipleColumnsSelectedError() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Create QueryBuilder instance for the given connection. */
    [[nodiscard]] static std::shared_ptr<QueryBuilder>
    createQuery(const QString &connection);
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_DatabaseConnection::initTestCase_data() const
{
    const auto connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_DatabaseConnection::pingDatabase() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &connectionRef = DB::connection(connection);

    if (const auto driverName = connectionRef.driverName();
        driverName != QMYSQL
    )
        QSKIP(sl("The '%1' database driver doesn't support ping command.")
              .arg(driverName).toUtf8().constData(), );

#ifndef TINYORM_MYSQL_PING
    QSKIP("mysql_ping feature was disabled, to pass this test reconfigure the TinyORM "
          "project with the MYSQL_PING preprocessor macro ( -DMYSQL_PING ) for cmake "
          "or with the 'mysql_ping' configuration option ( \"CONFIG+=mysql_ping\" ) "
          "for qmake.", );
#endif

    const auto result = connectionRef.pingDatabase();

    QVERIFY2(result, "Ping database failed.");
}

void tst_DatabaseConnection::isNotMaria_OnMySqlConnection() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (connection != Databases::MYSQL)
        QSKIP(sl("The '%1' connection is not the connection to the MySQL database.")
              .arg(connection).toUtf8().constData(), );

    const auto isMaria = dynamic_cast<MySqlConnection &>(DB::connection(connection))
                         .isMaria();

    QCOMPARE(isMaria, false);
}

void tst_DatabaseConnection::isMaria_OnMariaConnection() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (connection != Databases::MARIADB)
        QSKIP(sl("The '%1' connection is not the connection to the MariaDB database.")
              .arg(connection).toUtf8().constData(), );

    const auto isMaria = dynamic_cast<MySqlConnection &>(DB::connection(connection))
                         .isMaria();

    QCOMPARE(isMaria, true);
}

void tst_DatabaseConnection::transaction_Commit() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto builder = createQuery(connection);

    // Prepare data
    const auto nameValue = sl("alibaba");
    const auto noteValue = sl("transation commit");

    DB::beginTransaction(connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(DB::connection(connection).inTransaction());

    auto id = builder->from("users").insertGetId({{NAME, nameValue},
                                                  {NOTE, noteValue}});

    DB::commit(connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(!DB::connection(connection).inTransaction());

    // Check data after commit
    auto query = builder->from("users").find(id);

    QCOMPARE(query.value(ID).value<quint64>(), id);
    QCOMPARE(query.value(NAME).value<QString>(), nameValue);
    QCOMPARE(query.value(NOTE).value<QString>(), noteValue);

    // Clean up
    builder->remove(id);
}

void tst_DatabaseConnection::transaction_RollBack() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto builder = createQuery(connection);

    // Prepare data
    const auto nameValue = sl("alibaba");
    const auto noteValue = sl("transation rollBack");

    DB::beginTransaction(connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(DB::connection(connection).inTransaction());

    auto id = builder->from("users").insertGetId({{NAME, nameValue},
                                                  {NOTE, noteValue}});

    DB::rollBack(connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(!DB::connection(connection).inTransaction());

    // Check data after commit
    auto query = builder->from("users").find(id);

    QVERIFY(query.isActive());
    QCOMPARE(query.record().count(), 7);
    // QSQLITE driver doesn't report a size
    if (DB::driverName(connection) != QSQLITE)
        QCOMPARE(query.size(), 0);
}

/* These double types of tests exists because I had a bug when a similar transaction with
   savepoints was called more times in a row and I had Q_ASSERT(m_inTransaction); fail
   in ManagesTransactions::commit().
   Also, I have decided to not extract duplicate  code to private methods, binary size
   will be a little bigger, but test code clarity is more important for me. */

void tst_DatabaseConnection::transaction_Commit_Double() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    quint64 id1 = 0;
    quint64 id2 = 0;

    // First commit
    {
        auto builder = createQuery(connection);

        // Prepare data
        const auto nameValue = sl("mařka");
        const auto noteValue = sl("transation double-commit");

        DB::beginTransaction(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        id1 = builder->from("users").insertGetId({{NAME, nameValue},
                                                  {NOTE, noteValue}});

        DB::commit(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        // Check data after commit
        auto query = builder->from("users").find(id1);

        QCOMPARE(query.value(ID).value<quint64>(), id1);
        QCOMPARE(query.value(NAME).value<QString>(), nameValue);
        QCOMPARE(query.value(NOTE).value<QString>(), noteValue);
    }
    // Second commit
    {
        auto builder = createQuery(connection);

        // Prepare data
        const auto nameValue = sl("venda");
        const auto noteValue = sl("transation double-commit");

        DB::beginTransaction(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        id2 = builder->from("users").insertGetId({{NAME, nameValue},
                                                  {NOTE, noteValue}});

        DB::commit(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        // Check data after commit
        auto query = builder->from("users").find(id2);

        QCOMPARE(query.value(ID).value<quint64>(), id2);
        QCOMPARE(query.value(NAME).value<QString>(), nameValue);
        QCOMPARE(query.value(NOTE).value<QString>(), noteValue);
    }

    // Clean up
    // Ownerships of the std::shared_ptr<QueryBuilder>
    createQuery(connection)->from("users").remove(id1);
    createQuery(connection)->from("users").remove(id2);
}

void tst_DatabaseConnection::transaction_RollBack_Double() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // First rollback
    {
        auto builder = createQuery(connection);

        // Prepare data
        const auto nameValue = sl("john");
        const auto noteValue = sl("transation double-rollBack");

        DB::beginTransaction(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        auto id = builder->from("users").insertGetId({{NAME, nameValue},
                                                      {NOTE, noteValue}});

        DB::rollBack(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        // Check data after commit
        auto query = builder->from("users").find(id);

        QVERIFY(query.isActive());
        QCOMPARE(query.record().count(), 7);
        // QSQLITE driver doesn't report a size
        if (DB::driverName(connection) != QSQLITE)
            QCOMPARE(query.size(), 0);
    }
    // Second rollback
    {
        auto builder = createQuery(connection);

        // Prepare data
        const auto nameValue = sl("franky");
        const auto noteValue = sl("transation double-rollBack");

        DB::beginTransaction(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        auto id = builder->from("users").insertGetId({{NAME, nameValue},
                                                      {NOTE, noteValue}});

        DB::rollBack(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        // Check data after commit
        auto query = builder->from("users").find(id);

        QVERIFY(query.isActive());
        QCOMPARE(query.record().count(), 7);
        // QSQLITE driver doesn't report a size
        if (DB::driverName(connection) != QSQLITE)
            QCOMPARE(query.size(), 0);
    }
}

void tst_DatabaseConnection::transaction_Savepoints_Commit_AllSuccess() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto builder = createQuery(connection);

    // Prepare data
    const QList<QString> nameValues {
        QLatin1String("james"),
        QLatin1String("robert"),
        QLatin1String("david"),
    };
    const auto noteValue = sl("transation savepoints commit all success");

    // Result ids
    QList<quint64> idsSuccess;
    idsSuccess.reserve(nameValues.size());

    DB::beginTransaction(connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(DB::connection(connection).inTransaction());

    idsSuccess << builder->from("users").insertGetId({{NAME, nameValues.at(0)},
                                                      {NOTE, noteValue}});

    DB::savepoint(1, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsSuccess << builder->from("users").insertGetId({{NAME, nameValues.at(1)},
                                                      {NOTE, noteValue}});

    DB::savepoint(2, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 2);
    QVERIFY(DB::connection(connection).inTransaction());

    idsSuccess << builder->from("users").insertGetId({{NAME, nameValues.at(2)},
                                                      {NOTE, noteValue}});

    DB::commit(connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(!DB::connection(connection).inTransaction());

    QCOMPARE(idsSuccess.size(), 3);

    // Check data after commit
    auto query = builder->from("users")
                 .whereIn(ID, {idsSuccess.constBegin(), idsSuccess.constEnd()})
                 .get();

    // Check query result
    QVERIFY(query.isActive());
    QCOMPARE(query.record().count(), 7);
    // QSQLITE driver doesn't report a size
    if (DB::driverName(connection) != QSQLITE)
        QCOMPARE(query.size(), 3);

    // Check data
    while (query.next()) {
        const auto id = query.value(ID).value<quint64>();
        const auto nameIndex = idsSuccess.indexOf(id);

        QVERIFY(idsSuccess.contains(id));
        QCOMPARE(query.value(NAME).value<QString>(), nameValues.at(nameIndex));
        QCOMPARE(query.value(NOTE).value<QString>(), noteValue);
    }

    QVERIFY(!query.seek(TCursorPosition::BeforeFirstRow));

    // Clean up
    while (query.next())
        // Ownership of the std::shared_ptr<QueryBuilder>
        createQuery(connection)->from("users").remove(query.value(ID).value<quint64>());
}

void tst_DatabaseConnection::transaction_Savepoints_Commit_OneFailed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto builder = createQuery(connection);

    // Prepare data
    const QList<QString> nameValuesSuccess {
        QLatin1String("james"),
        QLatin1String("david"),
    };
    const QList<QString> nameValuesFailed {
        QLatin1String("robert"),
    };
    const auto noteValue = sl("transation savepoints commit one failed");

    // Result ids
    QList<quint64> idsSuccess;
    idsSuccess.reserve(2);
    QList<quint64> idsFailed;
    idsSuccess.reserve(1);

    DB::beginTransaction(connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(DB::connection(connection).inTransaction());

    idsSuccess << builder->from("users").insertGetId({{NAME, nameValuesSuccess.at(0)},
                                                      {NOTE, noteValue}});

    DB::savepoint(1, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsFailed << builder->from("users").insertGetId({{NAME, nameValuesFailed.at(0)},
                                                     {NOTE, noteValue}});

    DB::rollbackToSavepoint(1, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(DB::connection(connection).inTransaction());

    DB::savepoint(2, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsSuccess << builder->from("users").insertGetId({{NAME, nameValuesSuccess.at(1)},
                                                      {NOTE, noteValue}});

    DB::commit(connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(!DB::connection(connection).inTransaction());

    QCOMPARE(idsSuccess.size(), 2);
    QCOMPARE(idsFailed.size(), 1);

    // Check data after commit
    auto query = builder->from("users")
                 .whereIn(ID, {idsSuccess.constBegin(), idsSuccess.constEnd()})
                 .get();

    // Check query result
    QVERIFY(query.isActive());
    QCOMPARE(query.record().count(), 7);
    // QSQLITE driver doesn't report a size
    if (DB::driverName(connection) != QSQLITE)
        QCOMPARE(query.size(), 2);

    // Check data
    while (query.next()) {
        const auto id = query.value(ID).value<quint64>();
        const auto nameIndex = idsSuccess.indexOf(id);

        QVERIFY(idsSuccess.contains(id));
        QCOMPARE(query.value(NAME).value<QString>(), nameValuesSuccess.at(nameIndex));
        QCOMPARE(query.value(NOTE).value<QString>(), noteValue);
    }

    QVERIFY(!query.seek(TCursorPosition::BeforeFirstRow));

    // Clean up
    while (query.next())
        // Ownership of the std::shared_ptr<QueryBuilder>
        createQuery(connection)->from("users").remove(query.value(ID).value<quint64>());
}

void tst_DatabaseConnection::transaction_Savepoints_Commit_AllFailed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto builder = createQuery(connection);

    // Prepare data
    const QList<QString> nameValues {
        QLatin1String("james"),
        QLatin1String("robert"),
        QLatin1String("david"),
    };
    const auto noteValue = sl("transation savepoints commit all failed");

    // Result ids
    QList<quint64> idsFailed;
    idsFailed.reserve(nameValues.size());

    DB::beginTransaction(connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(DB::connection(connection).inTransaction());

    DB::savepoint(0, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(0)},
                                                     {"is_banned", true},
                                                     {NOTE, noteValue}});

    DB::rollbackToSavepoint(0, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(DB::connection(connection).inTransaction());

    DB::savepoint(1, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(1)},
                                                     {"is_banned", true},
                                                     {NOTE, noteValue}});

    DB::rollbackToSavepoint(1, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(DB::connection(connection).inTransaction());

    DB::savepoint(2, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(2)},
                                                     {"is_banned", true},
                                                     {NOTE, noteValue}});

    DB::rollbackToSavepoint(2, connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(DB::connection(connection).inTransaction());

    DB::commit(connection);

    // Check transaction status
    QCOMPARE(DB::connection(connection).transactionLevel(), 0);
    QVERIFY(!DB::connection(connection).inTransaction());

    QCOMPARE(idsFailed.size(), 3);

    // Check data after commit
    auto query = builder->from("users")
                 .whereIn(ID, {idsFailed.constBegin(), idsFailed.constEnd()})
                 .get();

    // Check query result
    QVERIFY(query.isActive());
    QCOMPARE(query.record().count(), 7);
    // QSQLITE driver doesn't report a size
    if (DB::driverName(connection) != QSQLITE)
        QCOMPARE(query.size(), 0);
}

void tst_DatabaseConnection::transaction_Savepoints_Commit_AllFailed_Double() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // First commit
    {
        auto builder = createQuery(connection);

        // Prepare data
        const QList<QString> nameValues {
            QLatin1String("james"),
            QLatin1String("robert"),
            QLatin1String("david"),
        };
        const auto noteValue = sl("transation savepoints commit all failed double");

        // Result ids
        QList<quint64> idsFailed;
        idsFailed.reserve(nameValues.size());

        DB::beginTransaction(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(0, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(0)},
                                                         {NOTE, noteValue}});

        DB::rollbackToSavepoint(0, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(1, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(1)},
                                                         {NOTE, noteValue}});

        DB::rollbackToSavepoint(1, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(2, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(2)},
                                                         {NOTE, noteValue}});

        DB::rollbackToSavepoint(2, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::commit(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        QCOMPARE(idsFailed.size(), 3);

        // Check data after commit
        auto query = builder->from("users")
                     .whereIn(ID, {idsFailed.constBegin(), idsFailed.constEnd()})
                     .get();

        // Check query result
        QVERIFY(query.isActive());
        QCOMPARE(query.record().count(), 7);
        // QSQLITE driver doesn't report a size
        if (DB::driverName(connection) != QSQLITE)
            QCOMPARE(query.size(), 0);
    }
    // Second commit
    {
        auto builder = createQuery(connection);

        // Prepare data
        const QList<QString> nameValues {
            QLatin1String("james"),
            QLatin1String("robert"),
            QLatin1String("david"),
        };
        const auto noteValue = sl("transation savepoints commit all failed double");

        // Result ids
        QList<quint64> idsFailed;
        idsFailed.reserve(nameValues.size());

        DB::beginTransaction(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(0, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(0)},
                                                         {NOTE, noteValue}});

        DB::rollbackToSavepoint(0, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(1, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(1)},
                                                         {NOTE, noteValue}});

        DB::rollbackToSavepoint(1, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(2, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(2)},
                                                         {NOTE, noteValue}});

        DB::rollbackToSavepoint(2, connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::commit(connection);

        // Check transaction status
        QCOMPARE(DB::connection(connection).transactionLevel(), 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        QCOMPARE(idsFailed.size(), 3);

        // Check data after commit
        auto query = builder->from("users")
                     .whereIn(ID, {idsFailed.constBegin(), idsFailed.constEnd()})
                     .get();

        // Check query result
        QVERIFY(query.isActive());
        QCOMPARE(query.record().count(), 7);
        // QSQLITE driver doesn't report a size
        if (DB::driverName(connection) != QSQLITE)
            QCOMPARE(query.size(), 0);
    }
}

void tst_DatabaseConnection::timezone_And_qt_timezone() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &connectionRef = DB::connection(connection);

    // Original config. qt_timezone
    QCOMPARE(DB::originalConfigValue(qt_timezone, connection),
             QVariant::fromValue(Orm::TTimeZone::UTC));

    // Connection qt_timezone (parsed)
    QCOMPARE(connectionRef.getQtTimeZone(), QtTimeZoneConfig::utc());

    // Time zone for the database connection (session variable)
    const auto timeZone = connectionRef.getConfig(timezone_);

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(timeZone, QVariant(TZ00));

    else if (driverName == QPSQL)
        QCOMPARE(timeZone, QVariant(UTC));

    else if (driverName == QSQLITE) {
        QVERIFY(!timeZone.isValid());
        QVERIFY(!connectionRef.hasConfig(timezone_));

    } else
        Q_UNREACHABLE();
}

void tst_DatabaseConnection::scalar() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto name = DB::connection(connection).scalar(
                    "select name from torrents order by id");

    QCOMPARE(name, QVariant(QString("test1")));
}

void tst_DatabaseConnection::scalar_EmptyResult() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto name = DB::connection(connection).scalar(
                    "select name from torrents where id = ?", {0});

    QVERIFY(!name.isValid() && name.isNull());
}

void tst_DatabaseConnection::scalar_MultipleColumnsSelectedError() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    TVERIFY_THROWS_EXCEPTION(MultipleColumnsSelectedError,
                             DB::connection(connection).scalar(
                                 "select id, name from torrents order by id"));
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

std::shared_ptr<QueryBuilder>
tst_DatabaseConnection::createQuery(const QString &connection)
{
    return DB::connection(connection).query();
}

QTEST_MAIN(tst_DatabaseConnection)

#include "tst_databaseconnection.moc"
