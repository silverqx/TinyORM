#include <QCoreApplication>
#include <QtSql/QSqlRecord>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/mysqlconnection.hpp"
#include "orm/query/querybuilder.hpp"

#include "databases.hpp"

using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::QMYSQL;
using Orm::Constants::QSQLITE;
using Orm::DB;
using Orm::MySqlConnection;

using QueryBuilder = Orm::Query::Builder;

using TestUtils::Databases;

// TEST exceptions in tests, qt doesn't care about exceptions, totally ignore it, so when the exception is thrown, I didn't get any exception message or something similar, nothing 👿, try to solve it somehow 🤔 silverqx
class tst_DatabaseConnection : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    void pingDatabase() const;

    void isNotMaria_OnMySqlConnection() const;

    void transaction_Commit() const;
    void transaction_RollBack() const;
    void transaction_Commit_Double() const;
    void transaction_RollBack_Double() const;

    void transaction_Savepoints_Commit_AllSuccess() const;
    void transaction_Savepoints_Commit_OneFailed() const;
    void transaction_Savepoints_Commit_AllFailed() const;
    void transaction_Savepoints_Commit_AllFailed_Double() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Create QueryBuilder instance for the given connection. */
    [[nodiscard]] std::shared_ptr<QueryBuilder>
    createQuery(const QString &connection) const;
};

void tst_DatabaseConnection::initTestCase_data() const
{
    const auto &connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg("tst_DatabaseConnection")
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_DatabaseConnection::pingDatabase() const
{
    QFETCH_GLOBAL(QString, connection);

    auto &connection_ = DB::connection(connection);

    if (const auto driverName = connection_.driverName();
        driverName != QMYSQL
    )
        QSKIP(QStringLiteral("The '%1' database driver doesn't support ping command.")
              .arg(driverName).toUtf8().constData(), );

#ifndef TINYORM_MYSQL_PING
    QSKIP("mysql_ping feature was disabled, to pass this test reconfigure the TinyORM "
          "project with the MYSQL_PING preprocessor macro ( -DMYSQL_PING ) for cmake "
          "or with the 'mysql_ping' configuration option ( \"CONFIG+=mysql_ping\" ) "
          "for qmake.", );
#endif

    const auto result = connection_.pingDatabase();

    QVERIFY2(result, "Ping database failed.");
}

void tst_DatabaseConnection::isNotMaria_OnMySqlConnection() const
{
    QFETCH_GLOBAL(QString, connection);

    auto &connection_ = DB::connection(connection);

    if (const auto driverName = connection_.driverName();
        driverName != QMYSQL
    )
        QSKIP(QStringLiteral("The '%1' database driver doesn't implement isMaria() "
                             "method.")
              .arg(driverName).toUtf8().constData(), );

    const auto expected = !(connection == Databases::MYSQL);

    const auto isMaria = dynamic_cast<MySqlConnection &>(connection_).isMaria();

    QCOMPARE(isMaria, expected);
}

void tst_DatabaseConnection::transaction_Commit() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    // Prepare data
    const auto nameValue = QStringLiteral("alibaba");
    const auto noteValue = QStringLiteral("transation commit");

    DB::beginTransaction(connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(DB::connection(connection).inTransaction());

    auto id = builder->from("users").insertGetId({{NAME, nameValue},
                                                  {"note", noteValue}});

    DB::commit(connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(!DB::connection(connection).inTransaction());

    // Check data after commit
    auto query = builder->from("users").find(id);

    QCOMPARE(query.value(ID).value<quint64>(), id);
    QCOMPARE(query.value(NAME).value<QString>(), nameValue);
    QCOMPARE(query.value("note").value<QString>(), noteValue);

    // Clean up
    builder->remove(id);
}

void tst_DatabaseConnection::transaction_RollBack() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    // Prepare data
    const auto nameValue = QStringLiteral("alibaba");
    const auto noteValue = QStringLiteral("transation rollBack");

    DB::beginTransaction(connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(DB::connection(connection).inTransaction());

    auto id = builder->from("users").insertGetId({{NAME, nameValue},
                                                  {"note", noteValue}});

    DB::rollBack(connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(!DB::connection(connection).inTransaction());

    // Check data after commit
    auto query = builder->from("users").find(id);

    QVERIFY(query.isActive());
    QVERIFY(query.record().count() == 4);
    // QSQLITE driver doesn't report a size
    if (DB::connection(connection).driverName() != QSQLITE)
        QVERIFY(query.size() == 0);
}

/* These double types of tests exists because I had a bug when a similar transaction with
   savepoints was called more times in a row and I had Q_ASSERT(m_inTransaction); fail
   in ManagesTransactions::commit().
   Also, I have decided to not extract duplicate  code to private methods, binary size
   will be a little bigger, but test code clarity is more important for me. */

void tst_DatabaseConnection::transaction_Commit_Double() const
{
    QFETCH_GLOBAL(QString, connection);

    quint64 id1 = 0;
    quint64 id2 = 0;

    // First commit
    {
        auto builder = createQuery(connection);

        // Prepare data
        const auto nameValue = QStringLiteral("mařka");
        const auto noteValue = QStringLiteral("transation double-commit");

        DB::beginTransaction(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        id1 = builder->from("users").insertGetId({{NAME, nameValue},
                                                  {"note", noteValue}});

        DB::commit(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        // Check data after commit
        auto query = builder->from("users").find(id1);

        QCOMPARE(query.value(ID).value<quint64>(), id1);
        QCOMPARE(query.value(NAME).value<QString>(), nameValue);
        QCOMPARE(query.value("note").value<QString>(), noteValue);
    }
    // Second commit
    {
        auto builder = createQuery(connection);

        // Prepare data
        const auto nameValue = QStringLiteral("venda");
        const auto noteValue = QStringLiteral("transation double-commit");

        DB::beginTransaction(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        id2 = builder->from("users").insertGetId({{NAME, nameValue},
                                                  {"note", noteValue}});

        DB::commit(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        // Check data after commit
        auto query = builder->from("users").find(id2);

        QCOMPARE(query.value(ID).value<quint64>(), id2);
        QCOMPARE(query.value(NAME).value<QString>(), nameValue);
        QCOMPARE(query.value("note").value<QString>(), noteValue);
    }

    // Clean up
    // Ownerships of the std::shared_ptr<QueryBuilder>
    createQuery(connection)->from("users").remove(id1);
    createQuery(connection)->from("users").remove(id2);
}

void tst_DatabaseConnection::transaction_RollBack_Double() const
{
    QFETCH_GLOBAL(QString, connection);

    // First rollback
    {
        auto builder = createQuery(connection);

        // Prepare data
        const auto nameValue = QStringLiteral("john");
        const auto noteValue = QStringLiteral("transation double-rollBack");

        DB::beginTransaction(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        auto id = builder->from("users").insertGetId({{NAME, nameValue},
                                                      {"note", noteValue}});

        DB::rollBack(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        // Check data after commit
        auto query = builder->from("users").find(id);

        QVERIFY(query.isActive());
        QVERIFY(query.record().count() == 4);
        // QSQLITE driver doesn't report a size
        if (DB::connection(connection).driverName() != QSQLITE)
            QVERIFY(query.size() == 0);
    }
    // Second rollback
    {
        auto builder = createQuery(connection);

        // Prepare data
        const auto nameValue = QStringLiteral("franky");
        const auto noteValue = QStringLiteral("transation double-rollBack");

        DB::beginTransaction(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        auto id = builder->from("users").insertGetId({{NAME, nameValue},
                                                      {"note", noteValue}});

        DB::rollBack(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        // Check data after commit
        auto query = builder->from("users").find(id);

        QVERIFY(query.isActive());
        QVERIFY(query.record().count() == 4);
        // QSQLITE driver doesn't report a size
        if (DB::connection(connection).driverName() != QSQLITE)
            QVERIFY(query.size() == 0);
    }
}

void tst_DatabaseConnection::transaction_Savepoints_Commit_AllSuccess() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    // Prepare data
    const QVector<QString> nameValues {
        QLatin1String("james"),
        QLatin1String("robert"),
        QLatin1String("david"),
    };
    const auto noteValue = QStringLiteral("transation savepoints commit all success");

    // Result ids
    QVector<quint64> idsSuccess;
    idsSuccess.reserve(nameValues.size());

    DB::beginTransaction(connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(DB::connection(connection).inTransaction());

    idsSuccess << builder->from("users").insertGetId({{NAME, nameValues.at(0)},
                                                      {"note", noteValue}});

    DB::savepoint(1, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsSuccess << builder->from("users").insertGetId({{NAME, nameValues.at(1)},
                                                      {"note", noteValue}});

    DB::savepoint(2, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 2);
    QVERIFY(DB::connection(connection).inTransaction());

    idsSuccess << builder->from("users").insertGetId({{NAME, nameValues.at(2)},
                                                      {"note", noteValue}});

    DB::commit(connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(!DB::connection(connection).inTransaction());

    QVERIFY(idsSuccess.size() == 3);

    // Check data after commit
    auto query = builder->from("users")
                 .whereIn(ID, {idsSuccess.constBegin(), idsSuccess.constEnd()})
                 .get();

    // Check query result
    QVERIFY(query.isActive());
    QVERIFY(query.record().count() == 4);
    // QSQLITE driver doesn't report a size
    if (DB::connection(connection).driverName() != QSQLITE)
        QVERIFY(query.size() == 3);

    // Check data
    while (query.next()) {
        const auto id = query.value(ID).value<quint64>();
        const auto nameIndex = idsSuccess.indexOf(id);

        QVERIFY(idsSuccess.contains(id));
        QCOMPARE(query.value(NAME).value<QString>(), nameValues.at(nameIndex));
        QCOMPARE(query.value("note").value<QString>(), noteValue);
    }

    QVERIFY(!query.seek(QSql::BeforeFirstRow));

    // Clean up
    while (query.next())
        // Ownership of the std::shared_ptr<QueryBuilder>
        createQuery(connection)->from("users").remove(query.value(ID).value<quint64>());
}

void tst_DatabaseConnection::transaction_Savepoints_Commit_OneFailed() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    // Prepare data
    const QVector<QString> nameValuesSuccess {
        QLatin1String("james"),
        QLatin1String("david"),
    };
    const QVector<QString> nameValuesFailed {
        QLatin1String("robert"),
    };
    const auto noteValue = QStringLiteral("transation savepoints commit one failed");

    // Result ids
    QVector<quint64> idsSuccess;
    idsSuccess.reserve(2);
    QVector<quint64> idsFailed;
    idsSuccess.reserve(1);

    DB::beginTransaction(connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(DB::connection(connection).inTransaction());

    idsSuccess << builder->from("users").insertGetId({{NAME, nameValuesSuccess.at(0)},
                                                      {"note", noteValue}});

    DB::savepoint(1, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsFailed << builder->from("users").insertGetId({{NAME, nameValuesFailed.at(0)},
                                                     {"note", noteValue}});

    DB::rollbackToSavepoint(1, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(DB::connection(connection).inTransaction());

    DB::savepoint(2, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsSuccess << builder->from("users").insertGetId({{NAME, nameValuesSuccess.at(1)},
                                                      {"note", noteValue}});

    DB::commit(connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(!DB::connection(connection).inTransaction());

    QVERIFY(idsSuccess.size() == 2);
    QVERIFY(idsFailed.size() == 1);

    // Check data after commit
    auto query = builder->from("users")
                 .whereIn(ID, {idsSuccess.constBegin(), idsSuccess.constEnd()})
                 .get();

    // Check query result
    QVERIFY(query.isActive());
    QVERIFY(query.record().count() == 4);
    // QSQLITE driver doesn't report a size
    if (DB::connection(connection).driverName() != QSQLITE)
        QVERIFY(query.size() == 2);

    // Check data
    while (query.next()) {
        const auto id = query.value(ID).value<quint64>();
        const auto nameIndex = idsSuccess.indexOf(id);

        QVERIFY(idsSuccess.contains(id));
        QCOMPARE(query.value(NAME).value<QString>(), nameValuesSuccess.at(nameIndex));
        QCOMPARE(query.value("note").value<QString>(), noteValue);
    }

    QVERIFY(!query.seek(QSql::BeforeFirstRow));

    // Clean up
    while (query.next())
        // Ownership of the std::shared_ptr<QueryBuilder>
        createQuery(connection)->from("users").remove(query.value(ID).value<quint64>());
}

void tst_DatabaseConnection::transaction_Savepoints_Commit_AllFailed() const
{
    QFETCH_GLOBAL(QString, connection);

    auto builder = createQuery(connection);

    // Prepare data
    const QVector<QString> nameValues {
        QLatin1String("james"),
        QLatin1String("robert"),
        QLatin1String("david"),
    };
    const auto noteValue = QStringLiteral("transation savepoints commit all failed");

    // Result ids
    QVector<quint64> idsFailed;
    idsFailed.reserve(nameValues.size());

    DB::beginTransaction(connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(DB::connection(connection).inTransaction());

    DB::savepoint(0, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(0)},
                                                     {"is_banned", true},
                                                     {"note", noteValue}});

    DB::rollbackToSavepoint(0, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(DB::connection(connection).inTransaction());

    DB::savepoint(1, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(1)},
                                                     {"is_banned", true},
                                                     {"note", noteValue}});

    DB::rollbackToSavepoint(1, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(DB::connection(connection).inTransaction());

    DB::savepoint(2, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 1);
    QVERIFY(DB::connection(connection).inTransaction());

    idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(2)},
                                                     {"is_banned", true},
                                                     {"note", noteValue}});

    DB::rollbackToSavepoint(2, connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(DB::connection(connection).inTransaction());

    DB::commit(connection);

    // Check transaction status
    QVERIFY(DB::connection(connection).transactionLevel() == 0);
    QVERIFY(!DB::connection(connection).inTransaction());

    QVERIFY(idsFailed.size() == 3);

    // Check data after commit
    auto query = builder->from("users")
                 .whereIn(ID, {idsFailed.constBegin(), idsFailed.constEnd()})
                 .get();

    // Check query result
    QVERIFY(query.isActive());
    QVERIFY(query.record().count() == 4);
    // QSQLITE driver doesn't report a size
    if (DB::connection(connection).driverName() != QSQLITE)
        QVERIFY(query.size() == 0);
}

void tst_DatabaseConnection::transaction_Savepoints_Commit_AllFailed_Double() const
{
    QFETCH_GLOBAL(QString, connection);

    // First commit
    {
        auto builder = createQuery(connection);

        // Prepare data
        const QVector<QString> nameValues {
            QLatin1String("james"),
            QLatin1String("robert"),
            QLatin1String("david"),
        };
        const auto noteValue =
                QStringLiteral("transation savepoints commit all failed double");

        // Result ids
        QVector<quint64> idsFailed;
        idsFailed.reserve(nameValues.size());

        DB::beginTransaction(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(0, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(0)},
                                                         {"note", noteValue}});

        DB::rollbackToSavepoint(0, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(1, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(1)},
                                                         {"note", noteValue}});

        DB::rollbackToSavepoint(1, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(2, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(2)},
                                                         {"note", noteValue}});

        DB::rollbackToSavepoint(2, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::commit(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        QVERIFY(idsFailed.size() == 3);

        // Check data after commit
        auto query = builder->from("users")
                     .whereIn(ID, {idsFailed.constBegin(), idsFailed.constEnd()})
                     .get();

        // Check query result
        QVERIFY(query.isActive());
        QVERIFY(query.record().count() == 4);
        // QSQLITE driver doesn't report a size
        if (DB::connection(connection).driverName() != QSQLITE)
            QVERIFY(query.size() == 0);
    }
    // Second commit
    {
        auto builder = createQuery(connection);

        // Prepare data
        const QVector<QString> nameValues {
            QLatin1String("james"),
            QLatin1String("robert"),
            QLatin1String("david"),
        };
        const auto noteValue =
                QStringLiteral("transation savepoints commit all failed double");

        // Result ids
        QVector<quint64> idsFailed;
        idsFailed.reserve(nameValues.size());

        DB::beginTransaction(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(0, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(0)},
                                                         {"note", noteValue}});

        DB::rollbackToSavepoint(0, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(1, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(1)},
                                                         {"note", noteValue}});

        DB::rollbackToSavepoint(1, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::savepoint(2, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 1);
        QVERIFY(DB::connection(connection).inTransaction());

        idsFailed << builder->from("users").insertGetId({{NAME, nameValues.at(2)},
                                                         {"note", noteValue}});

        DB::rollbackToSavepoint(2, connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(DB::connection(connection).inTransaction());

        DB::commit(connection);

        // Check transaction status
        QVERIFY(DB::connection(connection).transactionLevel() == 0);
        QVERIFY(!DB::connection(connection).inTransaction());

        QVERIFY(idsFailed.size() == 3);

        // Check data after commit
        auto query = builder->from("users")
                     .whereIn(ID, {idsFailed.constBegin(), idsFailed.constEnd()})
                     .get();

        // Check query result
        QVERIFY(query.isActive());
        QVERIFY(query.record().count() == 4);
        // QSQLITE driver doesn't report a size
        if (DB::connection(connection).driverName() != QSQLITE)
            QVERIFY(query.size() == 0);
    }
}

std::shared_ptr<QueryBuilder>
tst_DatabaseConnection::createQuery(const QString &connection) const
{
    return DB::connection(connection).query();
}

QTEST_MAIN(tst_DatabaseConnection)

#include "tst_databaseconnection.moc"
