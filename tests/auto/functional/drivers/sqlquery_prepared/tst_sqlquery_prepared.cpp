#include <QCoreApplication>
#include <QtTest>

#include "orm/drivers/sqldatabase.hpp"
#include "orm/drivers/sqlquery.hpp"
#include "orm/drivers/sqlrecord.hpp"

#include "orm/constants.hpp"
#include "orm/utils/nullvariant.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

using Orm::Constants::CREATED_AT;
using Orm::Constants::DELETED_AT;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::UPDATED_AT;

using Orm::Drivers::SqlDatabase;
using Orm::Drivers::SqlQuery;

using Orm::Utils::NullVariant;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_SqlQuery_Prepared : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

    /*! Expose the NumericalPrecisionPolicy enum. */
    using enum Orm::Drivers::NumericalPrecisionPolicy;

private Q_SLOTS:
    void initTestCase() const;

    void select_All() const;
    void select_WithWhere() const;

    void select_IsNull() const;

    void select_Aggregate_Count() const;

    void insert_update_delete() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Create QueryBuilder instance for the given connection. */
    [[nodiscard]] SqlQuery
    createQuery(const QString &connection) const;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_SqlQuery_Prepared::initTestCase() const
{
    const auto connections = Databases::createDriversConnections();

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                          .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

/*! Comparable structure to store the ID and custom the templated type. */
template<typename T>
struct IdAndCustomType
{
    /*! Record's ID column. */
    quint64 id;
    /*! Any column. */
    T custom;

    /*! Equality comparison operator for the IdAndCustomType. */
    inline bool operator==(const IdAndCustomType &) const = default;
};

void tst_SqlQuery_Prepared::select_All() const
{
    QFETCH_GLOBAL(QString, connection);

    auto users = createQuery(connection);

    const auto query = sl("select id, name from users order by id");
    auto ok = users.prepare(query);
    QVERIFY(ok);

    // Test bound values
    QVERIFY(users.boundValues().isEmpty());

    ok = users.exec();

    // Check everything what can be checked for this basic query (default configuration)
    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(!users.isValid());
    QVERIFY(users.isSelect());
    const auto querySize = users.size();
    QCOMPARE(querySize, 5);
    // Behaves the same as the size() for SELECT queries
    QCOMPARE(users.numRowsAffected(), 5);
    QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
    QCOMPARE(users.executedQuery(), query);
    QCOMPARE(users.lastInsertId(), QVariant());

    // Verify the result
    QVector<IdAndCustomType<QString>> expected {
        {1, "andrej"}, {2, "silver"}, {3, "peter"}, {4, "jack"}, {5, "obiwan"},
    };
    QVector<IdAndCustomType<QString>> actual;
    actual.reserve(querySize);

    while (users.next()) {
        QVERIFY(users.isValid());
        QVERIFY(!users.isNull(0));
        QVERIFY(!users.isNull(1));
        QVERIFY(!users.isNull(ID));
        QVERIFY(!users.isNull(NAME));
        // Number of fields
        const auto record = users.record();
        QCOMPARE(record.count(), 2);
        QVERIFY(record.contains(ID));
        QVERIFY(record.contains(NAME));

        actual.emplaceBack(users.value(ID).value<quint64>(),
                           users.value(NAME).value<QString>());
    }
    QCOMPARE(actual, expected);
}

void tst_SqlQuery_Prepared::select_WithWhere() const
{
    QFETCH_GLOBAL(QString, connection);

    auto users = createQuery(connection);

    const auto query = sl("select id, name from users where id < ? order by id");
    auto ok = users.prepare(query);
    QVERIFY(ok);

    users.addBindValue(4);

    // Test bound values
    const auto boundValues = users.boundValues();
    QCOMPARE(boundValues.size(), 1);
    QCOMPARE(boundValues, QVariantList {4});

    ok = users.exec();

    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(!users.isValid());
    QVERIFY(users.isSelect());
    const auto querySize = users.size();
    QCOMPARE(querySize, 3);
    // Behaves the same as the size() for SELECT queries
    QCOMPARE(users.numRowsAffected(), 3);
    QCOMPARE(users.executedQuery(), query);

    // Verify the result
    QVector<IdAndCustomType<QString>> expected {
        {1, "andrej"}, {2, "silver"}, {3, "peter"},
    };
    QVector<IdAndCustomType<QString>> actual;
    actual.reserve(querySize);

    while (users.next()) {
        QVERIFY(users.isValid());
        QVERIFY(!users.isNull(0));
        QVERIFY(!users.isNull(1));
        QVERIFY(!users.isNull(ID));
        QVERIFY(!users.isNull(NAME));
        // Number of fields
        const auto record = users.record();
        QCOMPARE(record.count(), 2);
        QVERIFY(record.contains(ID));
        QVERIFY(record.contains(NAME));

        actual.emplaceBack(users.value(ID).value<quint64>(),
                           users.value(NAME).value<QString>());
    }
    QCOMPARE(actual, expected);
}

void tst_SqlQuery_Prepared::select_IsNull() const
{
    QFETCH_GLOBAL(QString, connection);

    auto users = createQuery(connection);

    const auto query = sl("select id, note from users order by id");
    auto ok = users.prepare(query);
    QVERIFY(ok);

    // Test bound values
    QVERIFY(users.boundValues().isEmpty());

    ok = users.exec();

    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(!users.isValid());
    QVERIFY(users.isSelect());
    const auto querySize = users.size();
    QCOMPARE(querySize, 5);
    QCOMPARE(users.executedQuery(), query);

    // Verify the result
    QVector<IdAndCustomType<bool>> expected {
        {1, true}, {2, true}, {3, false}, {4, false}, {5, false},
    };
    QVector<IdAndCustomType<bool>> actual;
    actual.reserve(querySize);

    while (users.next()) {
        QVERIFY(users.isValid());
        // Number of fields
        const auto record = users.record();
        QCOMPARE(record.count(), 2);
        QVERIFY(record.contains(ID));
        QVERIFY(record.contains(NOTE));

        actual.emplaceBack(users.value(ID).value<quint64>(),
                           users.isNull(NOTE));
    }
    QCOMPARE(actual, expected);
}

void tst_SqlQuery_Prepared::select_Aggregate_Count() const
{
    QFETCH_GLOBAL(QString, connection);

    auto users = createQuery(connection);

    const auto query = sl("select count(id) as aggregate from users where id < ?");
    auto ok = users.prepare(query);
    QVERIFY(ok);

    users.addBindValue(3);

    // Test bound values
    const auto boundValues = users.boundValues();
    QCOMPARE(boundValues.size(), 1);
    QCOMPARE(boundValues, QVariantList {3});

    ok = users.exec();

    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(!users.isValid());
    QVERIFY(users.isSelect());
    const auto querySize = users.size();
    QCOMPARE(querySize, 1);
    QCOMPARE(users.executedQuery(), query);

    // Verify the result
    ok = users.first();
    QVERIFY(ok);
    QVERIFY(users.isValid());
    // Number of fields
    const auto record = users.record();
    QCOMPARE(record.count(), 1);
    QVERIFY(record.contains("aggregate"));

    QCOMPARE(users.value("aggregate").value<quint64>(), 2);
}

/* I will test the INSERT, UPDATE, and DELETE in the one test method, it's nothing
   wrong about it (it's not absolutely cosher though), it also tests executing more
   queries on the same SqlQuery instance. */
void tst_SqlQuery_Prepared::insert_update_delete() const
{
    QFETCH_GLOBAL(QString, connection);

    auto users = createQuery(connection);
    quint64 lastInsertedId = 0;

    // INSERT a new row into the users table
    {
        const auto query =
                sl("insert into users "
                     "(name, is_banned, note, created_at, updated_at, deleted_at) "
                   "values (?, ?, ?, ?, ?, ?)");
        auto ok = users.prepare(query);
        QVERIFY(ok);

        users.addBindValue(sl("ashen one"));
        users.addBindValue(1);
        users.addBindValue(sl("test drivers INSERT"));
        users.addBindValue(sl("2023-05-11T11:52:53"));
        users.addBindValue(sl("2023-05-12T11:52:53"));
        users.addBindValue(NullVariant::QDateTime());

        // Test bound values
        const auto boundValues = users.boundValues();
        QCOMPARE(boundValues.size(), 6);
        QCOMPARE(boundValues,
                 (QVariantList {sl("ashen one"), 1, sl("test drivers INSERT"),
                                sl("2023-05-11T11:52:53"), sl("2023-05-12T11:52:53"),
                                NullVariant::QDateTime()}));

        ok = users.exec();

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(!users.isValid());
        QVERIFY(!users.isSelect());
        QCOMPARE(users.size(), -1);
        QCOMPARE(users.numRowsAffected(), 1);
        QCOMPARE(users.executedQuery(), query);

        lastInsertedId = users.lastInsertId().value<quint64>();
        QVERIFY(lastInsertedId > 5);
    }

    const auto columnNames = std::to_array({ID, NAME, sl("is_banned"), NOTE, CREATED_AT,
                                            UPDATED_AT, DELETED_AT});

    // Verify the INSERT
    {
        const auto query = sl("select * from users where id = ?");
        auto ok = users.prepare(query);
        QVERIFY(ok);

        users.addBindValue(lastInsertedId);

        // Test bound values
        const auto boundValues = users.boundValues();
        QCOMPARE(boundValues.size(), 1);
        QCOMPARE(boundValues, QVariantList {lastInsertedId});

        ok = users.exec();

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(!users.isValid());
        QVERIFY(users.isSelect());
        const auto querySize = users.size();
        QCOMPARE(querySize, 1);
        QCOMPARE(users.executedQuery(), query);

        // Verify the result
        ok = users.first();
        QVERIFY(ok);
        QVERIFY(users.isValid());
        // Number of fields
        const auto record = users.record();
        QCOMPARE(record.count(), 7);
        for (const auto &column : columnNames)
            QVERIFY(record.contains(column));

        // Tests if the QVariant has the correct type will be done in other test methods
        QCOMPARE(users.value(NAME)       .value<QString>(),   sl("ashen one"));
        QCOMPARE(users.value("is_banned").value<bool>(),      true);
        QCOMPARE(users.value(NOTE)       .value<QString>(),   sl("test drivers INSERT"));
        QCOMPARE(users.value(CREATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 11},
                                                                        {11, 52, 53}));
        QCOMPARE(users.value(UPDATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 12},
                                                                        {11, 52, 53}));
        QVERIFY(users.isNull(DELETED_AT));
        QCOMPARE(users.value(DELETED_AT), NullVariant::QDateTime());
    }

    // UPDATE
    {
        const auto query = sl("update users set name = ?, is_banned = ? where id = ?");
        auto ok = users.prepare(query);
        QVERIFY(ok);

        users.addBindValue(sl("micah"));
        users.addBindValue(0);
        users.addBindValue(lastInsertedId);

        // Test bound values
        const auto boundValues = users.boundValues();
        QCOMPARE(boundValues.size(), 3);
        QCOMPARE(boundValues, (QVariantList {sl("micah"), 0, lastInsertedId}));

        ok = users.exec();

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(!users.isValid());
        QVERIFY(!users.isSelect());
        QCOMPARE(users.size(), -1);
        QCOMPARE(users.numRowsAffected(), 1);
        QCOMPARE(users.executedQuery(), query);
    }

    // Verify the UPDATE
    {
        const auto query = sl("select * from users where id = ?");
        auto ok = users.prepare(query);
        QVERIFY(ok);

        users.addBindValue(lastInsertedId);

        // Test bound values
        const auto boundValues = users.boundValues();
        QCOMPARE(boundValues.size(), 1);
        QCOMPARE(boundValues, QVariantList {lastInsertedId});

        ok = users.exec();

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(!users.isValid());
        QVERIFY(users.isSelect());
        const auto querySize = users.size();
        QCOMPARE(querySize, 1);
        QCOMPARE(users.executedQuery(), query);

        // Verify the result
        ok = users.first();
        QVERIFY(ok);
        QVERIFY(users.isValid());
        // Number of fields
        const auto record = users.record();
        QCOMPARE(record.count(), 7);
        for (const auto &column : columnNames)
            QVERIFY(record.contains(column));

        // Tests if the QVariant has the correct type will be done in other test methods
        QCOMPARE(users.value(NAME)       .value<QString>(),   sl("micah"));
        QCOMPARE(users.value("is_banned").value<bool>(),      false);
        QCOMPARE(users.value(NOTE)       .value<QString>(),   sl("test drivers INSERT"));
        QCOMPARE(users.value(CREATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 11},
                                                                        {11, 52, 53}));
        QCOMPARE(users.value(UPDATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 12},
                                                                        {11, 52, 53}));
        QVERIFY(users.isNull(DELETED_AT));
        QCOMPARE(users.value(DELETED_AT), NullVariant::QDateTime());
    }

    // Restore and also test the DELETE
    {
        const auto query = sl("delete from users where id = ?");
        auto ok = users.prepare(query);
        QVERIFY(ok);

        users.addBindValue(lastInsertedId);

        // Test bound values
        const auto boundValues = users.boundValues();
        QCOMPARE(boundValues.size(), 1);
        QCOMPARE(boundValues, QVariantList {lastInsertedId});

        ok = users.exec();

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(!users.isValid());
        QVERIFY(!users.isSelect());
        QCOMPARE(users.size(), -1);
        QCOMPARE(users.numRowsAffected(), 1);
        QCOMPARE(users.executedQuery(), query);
    }

    // Verify the DELETE
    {
        const auto query = sl("select id from users where id = ?");
        auto ok = users.prepare(query);
        QVERIFY(ok);

        users.addBindValue(lastInsertedId);

        // Test bound values
        const auto boundValues = users.boundValues();
        QCOMPARE(boundValues.size(), 1);
        QCOMPARE(boundValues, QVariantList {lastInsertedId});

        ok = users.exec();

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(!users.isValid());
        QVERIFY(users.isSelect());
        const auto querySize = users.size();
        QCOMPARE(querySize, 0);
        QCOMPARE(users.executedQuery(), query);

        // Verify the result
        ok = users.first();
        QVERIFY(!ok);
        QVERIFY(!users.isValid());
        // Check also this, must stay the same
        QVERIFY(users.isActive());
        QVERIFY(users.isSelect());
    }
}

/* private */

SqlQuery tst_SqlQuery_Prepared::createQuery(const QString &connection) const
{
    return SqlQuery(Databases::connection(connection));
}

QTEST_MAIN(tst_SqlQuery_Prepared)

#include "tst_sqlquery_prepared.moc"
