#include <QCoreApplication>
#include <QTest>

#include "orm/drivers/exceptions/logicerror.hpp"
#include "orm/drivers/sqlquery.hpp"
#include "orm/drivers/sqlrecord.hpp"

#ifdef TINYDRIVERS_MYSQL_DRIVER
#  include "orm/drivers/mysql/version.hpp"
#endif

#include "orm/constants.hpp"
#include "orm/utils/nullvariant.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

using Qt::StringLiterals::operator""_s;

using Orm::Constants::AddedOn;
using Orm::Constants::CREATED_AT;
using Orm::Constants::DELETED_AT;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;
using Orm::Constants::UPDATED_AT;
using Orm::Constants::dummy_NONEXISTENT;

using Orm::Drivers::Exceptions::LogicError;
using Orm::Drivers::SqlQuery;
using Orm::Drivers::SqlRecord;

using enum Orm::Drivers::CursorPosition;
using enum Orm::Drivers::NumericalPrecisionPolicy;

using Orm::Utils::NullVariant;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_SqlQuery_Normal : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase() const;

    void select_All() const;
    void select_EmptyResultSet() const;
    void select_FetchingSameResultSet() const;
    void select_WithWhere() const;

    void select_IsNull() const;

    void select_Aggregate_Count() const;

    void select_recordCached_WithDefaultValues() const;
    void select_recordCached_WithoutDefaultValues() const;

    void seeking() const;

    void finish_And_detachFromResultSet() const;

    void select_reExecute_SameQuery() const;
    void select_executeAnotherQuery_OnSameInstance() const;

    void select_reExecute_SameQuery_AfterFinish() const;
    void select_executeAnotherQuery_OnSameInstance_AfterFinish() const;

    void insert_update_delete() const;

    void clear() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Create QueryBuilder instance for the given connection. */
    [[nodiscard]] inline static SqlQuery createQuery(const QString &connection);
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_SqlQuery_Normal::initTestCase() const
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

/*! Comparable structure to store the ID and custom templated type. */
template<typename T>
struct IdAndCustomType
{
    /*! Record's ID column. */
    quint64 id;
    /*! Any column. */
    T custom;

    /*! Equality comparison operator for the IdAndCustomType. */
    bool operator==(const IdAndCustomType &) const = default;
};

void tst_SqlQuery_Normal::select_All() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    const auto query = u"select id, name from users order by id"_s;
    const auto ok = users.exec(query);

    // Check everything what can be checked for this basic query (default configuration)
    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(users.isSelect());
    QVERIFY(!users.isValid());
    QCOMPARE(users.at(), BeforeFirstRow);
    const auto querySize = users.size();
    QCOMPARE(querySize, 5);
    QVERIFY(!users.isEmpty());
    // Behaves the same as the size() for SELECT queries
    QCOMPARE(users.numRowsAffected(), 5);
    QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
    QCOMPARE(users.executedQuery(), query);
    QCOMPARE(users.lastInsertId(), QVariant());

    QVERIFY(users.boundValues().isEmpty());

    // Verify the result
    QList<IdAndCustomType<QString>> expected {
        {1, "andrej"}, {2, "silver"}, {3, "peter"}, {4, "jack"}, {5, "obiwan"},
    };
    QList<IdAndCustomType<QString>> actual;
    actual.reserve(querySize);

    while (users.next()) {
        QVERIFY(users.isValid());
        QVERIFY(!users.isNull(0));
        QVERIFY(!users.isNull(1));
        QVERIFY(!users.isNull(ID));
        QVERIFY(!users.isNull(NAME));
        // Number of fields
        const auto &record = users.recordCached();
        QCOMPARE(record.count(), 2);
        QVERIFY(record.contains(ID));
        QVERIFY(record.contains(NAME));

        actual.emplaceBack(users.value(ID).value<quint64>(),
                           users.value(NAME).value<QString>());
    }
    QCOMPARE(actual, expected);
    QCOMPARE(users.at(), AfterLastRow);
}

void tst_SqlQuery_Normal::select_EmptyResultSet() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    const auto query = u"select id, name from users where name = '%1'"_s
                       .arg(dummy_NONEXISTENT);
    const auto ok = users.exec(query);

    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(users.isSelect());
    QVERIFY(!users.isValid());
    QCOMPARE(users.at(), BeforeFirstRow);
    const auto querySize = users.size();
    QCOMPARE(querySize, 0);
    QVERIFY(users.isEmpty());
    // Behaves the same as the size() for SELECT queries
    QCOMPARE(users.numRowsAffected(), 0);
    QCOMPARE(users.executedQuery(), query);
    QCOMPARE(users.lastInsertId(), QVariant());

    QVERIFY(users.boundValues().isEmpty());

    QVERIFY(!users.next());
    QCOMPARE(users.at(), BeforeFirstRow);
    QVERIFY(!users.previous());
    QCOMPARE(users.at(), BeforeFirstRow);
    QVERIFY(!users.first());
    QCOMPARE(users.at(), BeforeFirstRow);
    QVERIFY(!users.last());
    QCOMPARE(users.at(), BeforeFirstRow);
    QVERIFY(!users.seek(1));
    QCOMPARE(users.at(), BeforeFirstRow);
}

void tst_SqlQuery_Normal::select_FetchingSameResultSet() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    const auto query = u"select id, name from users where id <= 3"_s;
    const auto ok = users.exec(query);

    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(users.isSelect());
    QVERIFY(!users.isValid());
    QCOMPARE(users.at(), BeforeFirstRow);
    const auto querySize = users.size();
    QCOMPARE(querySize, 3);
    QVERIFY(!users.isEmpty());
    // Behaves the same as the size() for SELECT queries
    QCOMPARE(users.numRowsAffected(), 3);
    QCOMPARE(users.executedQuery(), query);
    QCOMPARE(users.lastInsertId(), QVariant());

    QVERIFY(users.boundValues().isEmpty());

    QVERIFY(users.first());
    QVERIFY(users.first());

    QVERIFY(users.last());
    QVERIFY(users.last());

    QVERIFY(users.seek(1));
    QVERIFY(users.seek(1));
}

void tst_SqlQuery_Normal::select_WithWhere() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    const auto query = u"select id, name from users where id < 4 order by id"_s;
    const auto ok = users.exec(query);

    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(users.isSelect());
    QVERIFY(!users.isValid());
    QCOMPARE(users.at(), BeforeFirstRow);
    const auto querySize = users.size();
    QCOMPARE(querySize, 3);
    // Behaves the same as the size() for SELECT queries
    QCOMPARE(users.numRowsAffected(), 3);
    QCOMPARE(users.executedQuery(), query);

    QVERIFY(users.boundValues().isEmpty());

    // Verify the result
    QList<IdAndCustomType<QString>> expected {
        {1, u"andrej"_s}, {2, u"silver"_s}, {3, u"peter"_s},
    };
    QList<IdAndCustomType<QString>> actual;
    actual.reserve(querySize);

    while (users.next()) {
        QVERIFY(users.isValid());
        QVERIFY(!users.isNull(0));
        QVERIFY(!users.isNull(1));
        QVERIFY(!users.isNull(ID));
        QVERIFY(!users.isNull(NAME));
        // Number of fields
        const auto &record = users.recordCached();
        QCOMPARE(record.count(), 2);
        QVERIFY(record.contains(ID));
        QVERIFY(record.contains(NAME));

        actual.emplaceBack(users.value(ID).value<quint64>(),
                           users.value(NAME).value<QString>());
    }
    QCOMPARE(actual, expected);
    QCOMPARE(users.at(), AfterLastRow);
}

void tst_SqlQuery_Normal::select_IsNull() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    const auto query = u"select id, note from users order by id"_s;
    const auto ok = users.exec(query);

    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(users.isSelect());
    QVERIFY(!users.isValid());
    QCOMPARE(users.at(), BeforeFirstRow);
    const auto querySize = users.size();
    QCOMPARE(querySize, 5);
    QCOMPARE(users.executedQuery(), query);

    // Verify the result
    QList<IdAndCustomType<bool>> expected {
        {1, true}, {2, true}, {3, false}, {4, false}, {5, false},
    };
    QList<IdAndCustomType<bool>> actual;
    actual.reserve(querySize);

    while (users.next()) {
        QVERIFY(users.isValid());
        // Number of fields
        const auto &record = users.recordCached();
        QCOMPARE(record.count(), 2);
        QVERIFY(record.contains(ID));
        QVERIFY(record.contains(NOTE));

        actual.emplaceBack(users.value(ID).value<quint64>(),
                           users.isNull(NOTE));
    }
    QCOMPARE(actual, expected);
    QCOMPARE(users.at(), AfterLastRow);
}

void tst_SqlQuery_Normal::select_Aggregate_Count() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    const auto query = u"select count(id) as aggregate from users where id < 3"_s;
    auto ok = users.exec(query);

    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(users.isSelect());
    QVERIFY(!users.isValid());
    QCOMPARE(users.at(), BeforeFirstRow);
    const auto querySize = users.size();
    QCOMPARE(querySize, 1);
    QCOMPARE(users.executedQuery(), query);

    // Verify the result
    ok = users.first();
    QVERIFY(ok);
    QVERIFY(users.isValid());
    // Number of fields
    const auto &record = users.recordCached();
    QCOMPARE(record.count(), 1);
    QVERIFY(record.contains("aggregate"));

    QCOMPARE(users.value("aggregate").value<quint64>(), 2);
}

// This is an overkill, but it tests everything, I think I have a lot of free time 😁😅
void tst_SqlQuery_Normal::select_recordCached_WithDefaultValues() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto torrents = createQuery(connection);

    static const QString
    query = u"select id, user_id, name, size, progress, added_on, note "
             "from torrents "
             "where id between 2 and 4 "
             "order by id"_s;

    const auto ok = torrents.exec(query);
    QVERIFY(ok);

    QVERIFY(torrents.isActive());
    QVERIFY(torrents.isSelect());
    QVERIFY(!torrents.isValid());
    QCOMPARE(torrents.at(), BeforeFirstRow);
    const auto querySize = torrents.size();
    QCOMPARE(querySize, 3);
    // Behaves the same as the size() for SELECT queries
    QCOMPARE(torrents.numRowsAffected(), 3);
    QCOMPARE(torrents.numericalPrecisionPolicy(), LowPrecisionDouble);
    QCOMPARE(torrents.executedQuery(), query);
    QCOMPARE(torrents.lastInsertId(), QVariant());

    // Populate values to compare
    // Verify the result
    // Field values related
    QList<QList<QVariant>> expectedValues {
        {2, 1, u"test2"_s, 12, 200,
         QDateTime({2020, 8, 2}, {20, 11, 10}, QTimeZone::UTC), NullVariant::QString()},
        {3, 1, u"test3"_s, 13, 300,
         QDateTime({2020, 8, 3}, {20, 11, 10}, QTimeZone::UTC), NullVariant::QString()},
        {4, 1, u"test4"_s, 14, 400,
         QDateTime({2020, 8, 4}, {20, 11, 10}, QTimeZone::UTC),
         u"after update revert updated_at"_s}
    };
    QList<QList<QVariant>> actualValues;
    actualValues.reserve(querySize);

    // Actual NULL values (QVariant)
    QList<QList<bool>> expectedNullValues {
        {false, false, false, false, false, false, true},
        {false, false, false, false, false, false, true},
        {false, false, false, false, false, false, false},
    };
    QList<QList<bool>> actualNullValues;
    actualNullValues.reserve(querySize);

    // Column definitions related
    static const auto Torrents = u"torrents"_s;
    static const QStringList fieldNames({
        ID, "user_id", NAME, SIZE_, Progress, AddedOn, NOTE,
    });
    static const auto fieldsCount = fieldNames.size();

    QList<bool> expectedAutoIncrements {true, false, false, false, false, false, false};
    QList<bool> actualAutoIncrements;
    actualAutoIncrements.reserve(fieldsCount);

    // NULL in the table definition (not the QVariant value itself)
    QList<bool> expectedNullColumns {false, true, false, false, false, false, true};
    QList<bool> actualNullColumns;
    actualNullColumns.reserve(fieldsCount);

    QList<QMetaType> expectedMetaTypes { // clazy:exclude=missing-typeinfo
        QMetaType::fromType<quint64>(), QMetaType::fromType<quint64>(),
        QMetaType::fromType<QString>(), QMetaType::fromType<quint64>(),
        QMetaType::fromType<ushort>(),  QMetaType::fromType<QDateTime>(),
        QMetaType::fromType<QString>(),
    };
    QList<QMetaType> actualMetaTypes; // clazy:exclude=missing-typeinfo
    actualMetaTypes.reserve(fieldsCount);

    static const auto BIGINT = u"BIGINT"_s;
    QList<QString> expectedSqlTypeNames {
        BIGINT, BIGINT, "VARCHAR", BIGINT, "SMALLINT", "DATETIME", "VARCHAR",
    };
    QList<QString> actualSqlTypeNames;
    actualSqlTypeNames.reserve(fieldsCount);

    QList<qint64> expectedLengths {20, 20, 1020, 20, 5, 19, 1020};
    QList<qint64> actualLengths;
    actualLengths.reserve(fieldsCount);

    QList<qint64> expectedPrecisions {0, 0, 0, 0, 0, 0, 0};
    QList<qint64> actualPrecisions;
    actualPrecisions.reserve(fieldsCount);

    QList<QVariant> expectedDefaultValues = std::invoke([&connection]() -> QList<QVariant>
    {
        static const auto NULL_ = u"NULL"_s;
        static const auto Zero  = u"0"_s;

        /* MySQL and MariaDB have different values in the COLUMN_DEFAULT column:
           MySQL: NULL, "CURRENT_TIMESTAMP"
           MariaDB: "NULL", "current_timestamp()"
           MariaDB has string "NULL" in COLUMN_DEFAULT column if IS_NULLABLE="YES",
           MySQL uses normal SQL NULL and you must check the IS_NULLABLE column
           to find out if a column is nullable.
           Also, MySQL returns QByteArray because it has set the BINARY attribute
           on the COLUMN_DEFAULT column because it uses the utf8mb3_bin collation,
           the flags=144 (BLOB_FLAG, BINARY_FLAG). I spent a lot of time on this to
           find out. 🤔
           MariaDB uses the utf8mb3_general_ci so it returns the QString,
           flags=4112 (BLOB_FLAG, NO_DEFAULT_VALUE_FLAG). */
        if (connection == Databases::MYSQL_DRIVERS)
            return {NullVariant::QByteArray(), NullVariant::QByteArray(),
                    NullVariant::QByteArray(), Zero.toUtf8(), Zero.toUtf8(),
                    u"CURRENT_TIMESTAMP"_s.toUtf8(), NullVariant::QByteArray()};

        if (connection == Databases::MARIADB_DRIVERS)
            return {NullVariant::QString(), NULL_, NullVariant::QString(), Zero, Zero,
                    u"current_timestamp()"_s, NULL_};

        Q_UNREACHABLE();
    });
    QList<QVariant> actualDefaultValues;
    actualDefaultValues.reserve(fieldsCount);

    while (torrents.next()) {
        QVERIFY(torrents.isValid());
        // Number of fields
        const auto &record = torrents.recordCached(true);
        const auto recordCount = record.count();
        QCOMPARE(recordCount, 7);

        /* All cached SqlRecord-s must return the same memory address, the second
           recordCached(false) WITHOUT the Default Column Values also returns
           a memory address of the same SqlRecord instance because Default Column Values
           were already populated and there is no reason dropping this instance and
           re-populate it without Default Column Values, or to clear these Default Column
           Values. 🤔😁 */
        QCOMPARE(std::addressof(torrents.recordCached(true)), std::addressof(record));
        QCOMPARE(std::addressof(torrents.recordCached(false)), std::addressof(record));

        // Column definitions related
        QCOMPARE(record.fieldNames(), fieldNames);
        for (SqlRecord::size_type i = 0; i < recordCount; ++i) {
            const auto field = record.field(i);
            QVERIFY(field.isValid());
            QCOMPARE(field.tableName(), Torrents);

            actualAutoIncrements << field.isAutoIncrement();
            actualNullColumns    << field.isNullColumn();
            actualMetaTypes      << field.metaType();
            actualSqlTypeNames   << field.sqlTypeName();
            actualLengths        << field.length();
            actualPrecisions     << field.precision();
            actualDefaultValues  << field.defaultValue();
        }

        QCOMPARE(actualAutoIncrements, expectedAutoIncrements);
        QCOMPARE(actualNullColumns, expectedNullColumns);
        QCOMPARE(actualMetaTypes, expectedMetaTypes);
        QCOMPARE(actualSqlTypeNames, expectedSqlTypeNames);
        QCOMPARE(actualLengths, expectedLengths);
        QCOMPARE(actualPrecisions, expectedPrecisions);
        QCOMPARE(actualDefaultValues, expectedDefaultValues);

        // Clear before the next loop
        actualAutoIncrements.clear();
        actualNullColumns.clear();
        actualMetaTypes.clear();
        actualSqlTypeNames.clear();
        actualLengths.clear();
        actualPrecisions.clear();
        actualDefaultValues.clear();

        // Field values related
        actualValues << QList<QVariant>({torrents.value(ID).value<quint64>(),
                                         torrents.value("user_id").value<quint64>(),
                                         torrents.value(NAME).value<QString>(),
                                         torrents.value(SIZE_).value<quint64>(),
                                         torrents.value(Progress).value<uint>(),
                                         torrents.value(AddedOn).value<QDateTime>(),
                                         torrents.value(NOTE).value<QString>()});
        actualNullValues << QList<bool>({torrents.isNull(ID),
                                         torrents.isNull("user_id"),
                                         torrents.isNull(NAME),
                                         torrents.isNull(SIZE_),
                                         torrents.isNull(Progress),
                                         torrents.isNull(AddedOn),
                                         torrents.isNull(NOTE)});
    }

    // Verify what left (all at once)
    QCOMPARE(actualValues, expectedValues);
    QCOMPARE(actualNullValues, expectedNullValues);
    QCOMPARE(torrents.at(), AfterLastRow);
}

void tst_SqlQuery_Normal::select_recordCached_WithoutDefaultValues() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto torrents = createQuery(connection);

    static const QString
    query = u"select id, user_id, name, size, progress, added_on, note "
             "from torrents "
             "where id between 2 and 4 "
             "order by id"_s;

    const auto ok = torrents.exec(query);
    QVERIFY(ok);

    QVERIFY(torrents.isActive());
    QVERIFY(torrents.isSelect());
    QVERIFY(!torrents.isValid());
    QCOMPARE(torrents.at(), BeforeFirstRow);
    const auto querySize = torrents.size();
    QCOMPARE(querySize, 3);
    // Behaves the same as the size() for SELECT queries
    QCOMPARE(torrents.numRowsAffected(), 3);
    QCOMPARE(torrents.numericalPrecisionPolicy(), LowPrecisionDouble);
    QCOMPARE(torrents.executedQuery(), query);
    QCOMPARE(torrents.lastInsertId(), QVariant());

    // Populate values to compare
    // Verify the result
    // Field values related
    QList<QList<QVariant>> expectedValues {
        {2, 1, u"test2"_s, 12, 200,
         QDateTime({2020, 8, 2}, {20, 11, 10}, QTimeZone::UTC), NullVariant::QString()},
        {3, 1, u"test3"_s, 13, 300,
         QDateTime({2020, 8, 3}, {20, 11, 10}, QTimeZone::UTC), NullVariant::QString()},
        {4, 1, u"test4"_s, 14, 400,
         QDateTime({2020, 8, 4}, {20, 11, 10}, QTimeZone::UTC),
         u"after update revert updated_at"_s}
    };
    QList<QList<QVariant>> actualValues;
    actualValues.reserve(querySize);
    // After re-populated Default Column Values
    QList<QList<QVariant>> actualValues2;
    actualValues2.reserve(querySize);

    // Actual NULL values (QVariant)
    QList<QList<bool>> expectedNullValues {
        {false, false, false, false, false, false, true},
        {false, false, false, false, false, false, true},
        {false, false, false, false, false, false, false},
    };
    QList<QList<bool>> actualNullValues;
    actualNullValues.reserve(querySize);
    // After re-populated Default Column Values
    QList<QList<bool>> actualNullValues2;
    actualNullValues2.reserve(querySize);

    // Column definitions related
    static const auto Torrents = u"torrents"_s;
    static const QStringList fieldNames({
        ID, "user_id", NAME, SIZE_, Progress, AddedOn, NOTE,
    });
    static const auto fieldsCount = fieldNames.size();

    QList<bool> expectedAutoIncrements {true, false, false, false, false, false, false};
    QList<bool> actualAutoIncrements;
    actualAutoIncrements.reserve(fieldsCount);

    // NULL in the table definition (not the QVariant value itself)
    QList<bool> expectedNullColumns {false, true, false, false, false, false, true};
    QList<bool> actualNullColumns;
    actualNullColumns.reserve(fieldsCount);

    QList<QMetaType> expectedMetaTypes { // clazy:exclude=missing-typeinfo
        QMetaType::fromType<quint64>(), QMetaType::fromType<quint64>(),
        QMetaType::fromType<QString>(), QMetaType::fromType<quint64>(),
        QMetaType::fromType<ushort>(),  QMetaType::fromType<QDateTime>(),
        QMetaType::fromType<QString>(),
    };
    QList<QMetaType> actualMetaTypes; // clazy:exclude=missing-typeinfo
    actualMetaTypes.reserve(fieldsCount);

    static const auto BIGINT = u"BIGINT"_s;
    QList<QString> expectedSqlTypeNames {
        BIGINT, BIGINT, "VARCHAR", BIGINT, "SMALLINT", "DATETIME", "VARCHAR",
    };
    QList<QString> actualSqlTypeNames;
    actualSqlTypeNames.reserve(fieldsCount);

    QList<qint64> expectedLengths {20, 20, 1020, 20, 5, 19, 1020};
    QList<qint64> actualLengths;
    actualLengths.reserve(fieldsCount);

    QList<qint64> expectedPrecisions {0, 0, 0, 0, 0, 0, 0};
    QList<qint64> actualPrecisions;
    actualPrecisions.reserve(fieldsCount);

    QList<QVariant> expectedDefaultValues = std::invoke([&connection]() -> QList<QVariant>
    {
        static const auto NULL_ = u"NULL"_s;
        static const auto Zero  = u"0"_s;

        /* MySQL and MariaDB have different values in the COLUMN_DEFAULT column:
           MySQL: NULL, "CURRENT_TIMESTAMP"
           MariaDB: "NULL", "current_timestamp()"
           MariaDB has string "NULL" in COLUMN_DEFAULT column if IS_NULLABLE="YES",
           MySQL uses normal SQL NULL and you must check the IS_NULLABLE column
           to find out if a column is nullable.
           Also, MySQL returns QByteArray because it has set the BINARY attribute
           on the COLUMN_DEFAULT column because it uses the utf8mb3_bin collation,
           the flags=144 (BLOB_FLAG, BINARY_FLAG). I spent a lot of time on this to
           find out. 🤔
           MariaDB uses the utf8mb3_general_ci so it returns the QString,
           flags=4112 (BLOB_FLAG, NO_DEFAULT_VALUE_FLAG). */
        if (connection == Databases::MYSQL_DRIVERS)
            return {NullVariant::QByteArray(), NullVariant::QByteArray(),
                    NullVariant::QByteArray(), Zero.toUtf8(), Zero.toUtf8(),
                    u"CURRENT_TIMESTAMP"_s.toUtf8(), NullVariant::QByteArray()};

        if (connection == Databases::MARIADB_DRIVERS)
            return {NullVariant::QString(), NULL_, NullVariant::QString(), Zero, Zero,
                    u"current_timestamp()"_s, NULL_};

        Q_UNREACHABLE();
    });
    QList<QVariant> actualDefaultValues;
    actualDefaultValues.reserve(fieldsCount);

    while (torrents.next()) {
        QVERIFY(torrents.isValid());
        // Number of fields                        // Don't uncomment to test the default argument
        const auto &record = torrents.recordCached(/*false*/);
        const auto recordCount = record.count();
        QCOMPARE(recordCount, 7);

        /* All cached SqlRecord-s must return the same memory address, see also note
           in the previous test method. */
        QCOMPARE(std::addressof(torrents.recordCached(false)), std::addressof(record));

        // Column definitions related
        QCOMPARE(record.fieldNames(), fieldNames);
        for (SqlRecord::size_type i = 0; i < recordCount; ++i) {
            const auto field = record.field(i);
            QVERIFY(field.isValid());
            QCOMPARE(field.tableName(), Torrents);

            actualAutoIncrements << field.isAutoIncrement();
            actualNullColumns    << field.isNullColumn();
            actualMetaTypes      << field.metaType();
            actualSqlTypeNames   << field.sqlTypeName();
            actualLengths        << field.length();
            actualPrecisions     << field.precision();
            // All Default Column Values must be QVariant(Invalid)
            QVERIFY(!field.defaultValue().isValid());
        }

        QCOMPARE(actualAutoIncrements, expectedAutoIncrements);
        QCOMPARE(actualNullColumns, expectedNullColumns);
        QCOMPARE(actualMetaTypes, expectedMetaTypes);
        QCOMPARE(actualSqlTypeNames, expectedSqlTypeNames);
        QCOMPARE(actualLengths, expectedLengths);
        QCOMPARE(actualPrecisions, expectedPrecisions);

        // Clear before the next loop
        actualAutoIncrements.clear();
        actualNullColumns.clear();
        actualMetaTypes.clear();
        actualSqlTypeNames.clear();
        actualLengths.clear();
        actualPrecisions.clear();

        // Field values related
        actualValues << QList<QVariant>({torrents.value(ID).value<quint64>(),
                                         torrents.value("user_id").value<quint64>(),
                                         torrents.value(NAME).value<QString>(),
                                         torrents.value(SIZE_).value<quint64>(),
                                         torrents.value(Progress).value<uint>(),
                                         torrents.value(AddedOn).value<QDateTime>(),
                                         torrents.value(NOTE).value<QString>()});
        actualNullValues << QList<bool>({torrents.isNull(ID),
                                         torrents.isNull("user_id"),
                                         torrents.isNull(NAME),
                                         torrents.isNull(SIZE_),
                                         torrents.isNull(Progress),
                                         torrents.isNull(AddedOn),
                                         torrents.isNull(NOTE)});

        /* Re-populate the Default Column Values on the cached SqlRecord, it of course
           must be done after all the previous tests as the last thing to test it
           correctly. */
        QCOMPARE(std::addressof(torrents.recordCached(true)), std::addressof(record));

        /* Verify re-populated Default Column Values, OK I reinvoke the same test logic
           again as everything should stay the same, to correctly test it. */

        // Column definitions related
        QCOMPARE(record.fieldNames(), fieldNames);
        for (SqlRecord::size_type i = 0; i < recordCount; ++i) {
            const auto field = record.field(i);
            QVERIFY(field.isValid());
            QCOMPARE(field.tableName(), Torrents);

            actualAutoIncrements << field.isAutoIncrement();
            actualNullColumns    << field.isNullColumn();
            actualMetaTypes      << field.metaType();
            actualSqlTypeNames   << field.sqlTypeName();
            actualLengths        << field.length();
            actualPrecisions     << field.precision();
            actualDefaultValues  << field.defaultValue();
        }

        QCOMPARE(actualAutoIncrements, expectedAutoIncrements);
        QCOMPARE(actualNullColumns, expectedNullColumns);
        QCOMPARE(actualMetaTypes, expectedMetaTypes);
        QCOMPARE(actualSqlTypeNames, expectedSqlTypeNames);
        QCOMPARE(actualLengths, expectedLengths);
        QCOMPARE(actualPrecisions, expectedPrecisions);
        // This is the actual test
        QCOMPARE(actualDefaultValues, expectedDefaultValues);

        // Clear before the next loop
        actualAutoIncrements.clear();
        actualNullColumns.clear();
        actualMetaTypes.clear();
        actualSqlTypeNames.clear();
        actualLengths.clear();
        actualPrecisions.clear();
        actualDefaultValues.clear();

        // Field values related
        actualValues2 << QList<QVariant>({torrents.value(ID).value<quint64>(),
                                          torrents.value("user_id").value<quint64>(),
                                          torrents.value(NAME).value<QString>(),
                                          torrents.value(SIZE_).value<quint64>(),
                                          torrents.value(Progress).value<uint>(),
                                          torrents.value(AddedOn).value<QDateTime>(),
                                          torrents.value(NOTE).value<QString>()});
        actualNullValues2 << QList<bool>({torrents.isNull(ID),
                                          torrents.isNull("user_id"),
                                          torrents.isNull(NAME),
                                          torrents.isNull(SIZE_),
                                          torrents.isNull(Progress),
                                          torrents.isNull(AddedOn),
                                          torrents.isNull(NOTE)});
    }

    // Verify what left (all at once)
    QCOMPARE(actualValues, expectedValues);
    QCOMPARE(actualValues2, expectedValues);
    QCOMPARE(actualNullValues, expectedNullValues);
    QCOMPARE(actualNullValues2, expectedNullValues);
    QCOMPARE(torrents.at(), AfterLastRow);
}

void tst_SqlQuery_Normal::seeking() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    const auto query = u"select id from users order by id"_s;
    const auto ok = users.exec(query);

    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(users.isSelect());
    QVERIFY(!users.isValid());
    const auto querySize = users.size();
    QCOMPARE(querySize, 5);

    // Verify the result
    QList<quint64> expected {1, 2, 3, 4, 5};
    QList<quint64> actual;
    actual.reserve(querySize);

    // First verify if we have the correct result set
    while (users.next()) {
        QVERIFY(!users.isNull(ID));
        // Number of fields
        const auto &record = users.recordCached();
        QCOMPARE(record.count(), 1);
        QVERIFY(record.contains(ID));

        actual.emplaceBack(users.value(ID).value<quint64>());
    }
    QCOMPARE(actual, expected);

    QCOMPARE(users.at(), AfterLastRow);

    // The following tests test all possible seeking code branches

    /* Testing first(), last(), next(), previous() */
    QVERIFY(users.first());
    QCOMPARE(users.at(), 0);
    QCOMPARE(users.value<quint64>(ID), 1);

    QVERIFY(!users.previous());
    QCOMPARE(users.at(), BeforeFirstRow);
    QVERIFY(!users.previous());
    QCOMPARE(users.at(), BeforeFirstRow);

    QVERIFY(users.next());
    QCOMPARE(users.at(), 0);
    QCOMPARE(users.value<quint64>(ID), 1);

    QVERIFY(users.last());
    QCOMPARE(users.at(), 4);
    QCOMPARE(users.value<quint64>(ID), 5);

    QVERIFY(!users.next());
    QCOMPARE(users.at(), AfterLastRow);
    QVERIFY(!users.next());
    QCOMPARE(users.at(), AfterLastRow);

    QVERIFY(users.previous());
    QCOMPARE(users.at(), 4);
    QCOMPARE(users.value<quint64>(ID), 5);

    QVERIFY(users.previous());
    QCOMPARE(users.at(), 3);
    QCOMPARE(users.value<quint64>(ID), 4);

    /* Testing seek() - seekArbitrary() */
    // In mapSeekToFetch()
    QVERIFY(!users.seek(std::numeric_limits<int>::max(), false));
    QCOMPARE(users.at(), AfterLastRow);
    // In seekArbitrary()
    QVERIFY(!users.seek(-3, false));
    QCOMPARE(users.at(), BeforeFirstRow);

    // Branch - fetch()
    QVERIFY(users.seek(4, false));
    QCOMPARE(users.at(), 4);
    QCOMPARE(users.value<quint64>(ID), 5);

    // Branch - fetchPrevious()
    QVERIFY(users.seek(3, false));
    QCOMPARE(users.at(), 3);
    QCOMPARE(users.value<quint64>(ID), 4);

    // The BeforeFirstRow branch for fetchPrevious() is unreachable with non-relative seek

    // Branch - fetchNext()
    QVERIFY(users.seek(4, false));
    QCOMPARE(users.at(), 4);
    QCOMPARE(users.value<quint64>(ID), 5);

    QVERIFY(!users.seek(5, false));
    QCOMPARE(users.at(), AfterLastRow);

    /* From here things start to be a little messy because relative seek() has
       a lot of branches. */

    /* Testing relative seek() - seekRelative() */
    // AfterLastRow
    QCOMPARE(users.at(), AfterLastRow);
    QVERIFY(!users.seek(0, true));
    QCOMPARE(users.at(), AfterLastRow);
    QVERIFY(!users.seek(1, true));
    QCOMPARE(users.at(), AfterLastRow);

    // BeforeFirstRow
    // Prepare
    QVERIFY(users.first());
    QCOMPARE(users.at(), 0);
    QVERIFY(!users.previous());
    QCOMPARE(users.at(), BeforeFirstRow);
    // Test
    QVERIFY(!users.seek(0, true));
    QCOMPARE(users.at(), BeforeFirstRow);
    QVERIFY(!users.seek(-1, true));
    QCOMPARE(users.at(), BeforeFirstRow);

    // default: BeforeFirstRow
    // Prepare
    QVERIFY(users.seek(2, false));
    QCOMPARE(users.at(), 2);
    // Test
    QVERIFY(!users.seek(-3, true));
    QCOMPARE(users.at(), BeforeFirstRow);

    // Legend: -- means or distinguishes the main sub-section

    /* Testing relative seek() - fetchNext(), fetchPrevious(), and fetch() */
    // -- BeforeFirstRow in seekRelative()

    // Branches - fetchNext() and fetchPrevious() are unreachable

    // Branch - fetch()
    QVERIFY(users.seek(5, true));
    QCOMPARE(users.at(), 4);
    QCOMPARE(users.value<quint64>(ID), 5);

    // AfterLastRow in mapSeekToFetch()
    // Prepare
    QVERIFY(users.first());
    QCOMPARE(users.at(), 0);
    QVERIFY(!users.previous());
    QCOMPARE(users.at(), BeforeFirstRow);
    // Test
    QVERIFY(!users.seek(6, true));
    QCOMPARE(users.at(), AfterLastRow);

    // -- AfterLastRow in seekRelative()

    // Branches - fetchPrevious(), fetchNext() and AfterLastRow in mapSeekToFetch() are unreachable

    // Branch - fetch()
    QVERIFY(users.seek(-4, true));
    QCOMPARE(users.at(), 1);
    QCOMPARE(users.value<quint64>(ID), 2);

    // -- default: in seekRelative()
    // Branch - fetchNext()
    QVERIFY(users.seek(1, true));
    QCOMPARE(users.at(), 2);
    QCOMPARE(users.value<quint64>(ID), 3);

    // Branch - fetchPrevious()
    QVERIFY(users.seek(-1, true));
    QCOMPARE(users.at(), 1);
    QCOMPARE(users.value<quint64>(ID), 2);

    // Branch - fetch()
    QVERIFY(users.seek(2, true));
    QCOMPARE(users.at(), 3);
    QCOMPARE(users.value<quint64>(ID), 4);

    // AfterLastRow in mapSeekToFetch()
    QVERIFY(!users.seek(2, true));
    QCOMPARE(users.at(), AfterLastRow);
}

void tst_SqlQuery_Normal::finish_And_detachFromResultSet() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    const auto query = u"select id, name from users order by id"_s;
    const auto ok = users.exec(query);

    // Check everything what can be checked for this basic query (default configuration)
    QVERIFY(ok);
    QVERIFY(users.isActive());
    QVERIFY(users.isSelect());
    QVERIFY(!users.isValid());
    const auto querySize = users.size();
    QCOMPARE(querySize, 5);
    // Behaves the same as the size() for SELECT queries
    QCOMPARE(users.numRowsAffected(), 5);
    QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
    QCOMPARE(users.executedQuery(), query);
    QCOMPARE(users.lastInsertId(), QVariant());

    QVERIFY(users.boundValues().isEmpty());

    // Verify the result
    QList<IdAndCustomType<QString>> expected {
        {1, "andrej"}, {2, "silver"}, {3, "peter"}, {4, "jack"}, {5, "obiwan"},
    };
    QList<IdAndCustomType<QString>> actual;
    actual.reserve(querySize);

    while (users.next()) {
        QVERIFY(users.isValid());
        QVERIFY(!users.isNull(0));
        QVERIFY(!users.isNull(1));
        QVERIFY(!users.isNull(ID));
        QVERIFY(!users.isNull(NAME));
        // Number of fields
        const auto &record = users.recordCached();
        QCOMPARE(record.count(), 2);
        QVERIFY(record.contains(ID));
        QVERIFY(record.contains(NAME));

        actual.emplaceBack(users.value(ID).value<quint64>(),
                           users.value(NAME).value<QString>());
    }
    QCOMPARE(actual, expected);

    users.finish();

    QVERIFY(!users.isActive());
    QVERIFY(users.isSelect());
    QVERIFY(!users.isValid());
    QCOMPARE(users.at(), BeforeFirstRow);
    QVERIFY_THROWS_EXCEPTION(LogicError, users.size());
    // Behaves the same as the size() for SELECT queries
    QVERIFY_THROWS_EXCEPTION(LogicError, users.numRowsAffected());
    QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
    QCOMPARE(users.executedQuery(), query);
    QVERIFY_THROWS_EXCEPTION(LogicError, users.lastInsertId());

    QVERIFY(!users.next());
    QVERIFY(!users.previous());
    QVERIFY(!users.first());
    QVERIFY(!users.last());
    QVERIFY(!users.seek(1));
}

void tst_SqlQuery_Normal::select_reExecute_SameQuery() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    const auto query = u"select id, name from users order by id"_s;

    QList<IdAndCustomType<QString>> expected {
        {1, "andrej"}, {2, "silver"}, {3, "peter"}, {4, "jack"}, {5, "obiwan"},
    };

    // Execute first time
    {
        const auto ok = users.exec(query);

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(users.isSelect());
        QVERIFY(!users.isValid());
        QCOMPARE(users.at(), BeforeFirstRow);
        const auto querySize = users.size();
        QCOMPARE(querySize, 5);
        // Behaves the same as the size() for SELECT queries
        QCOMPARE(users.numRowsAffected(), 5);
        QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(users.executedQuery(), query);
        QCOMPARE(users.lastInsertId(), QVariant());

        QVERIFY(users.boundValues().isEmpty());

        // Verify the result
        QList<IdAndCustomType<QString>> actual;
        actual.reserve(querySize);

        while (users.next()) {
            QVERIFY(users.isValid());
            QVERIFY(!users.isNull(0));
            QVERIFY(!users.isNull(1));
            QVERIFY(!users.isNull(ID));
            QVERIFY(!users.isNull(NAME));
            // Number of fields
            const auto &record = users.recordCached();
            QCOMPARE(record.count(), 2);
            QVERIFY(record.contains(ID));
            QVERIFY(record.contains(NAME));

            actual.emplaceBack(users.value(ID).value<quint64>(),
                               users.value(NAME).value<QString>());
        }
        QCOMPARE(actual, expected);
        QCOMPARE(users.at(), AfterLastRow);
    }

    // Re-execute second time
    {
        const auto ok = users.exec(query);

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(users.isSelect());
        QVERIFY(!users.isValid());
        QCOMPARE(users.at(), BeforeFirstRow);
        const auto querySize = users.size();
        QCOMPARE(querySize, 5);
        // Behaves the same as the size() for SELECT queries
        QCOMPARE(users.numRowsAffected(), 5);
        QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(users.executedQuery(), query);
        QCOMPARE(users.lastInsertId(), QVariant());

        QVERIFY(users.boundValues().isEmpty());

        // Verify the result
        QList<IdAndCustomType<QString>> actual;
        actual.reserve(querySize);

        while (users.next()) {
            QVERIFY(users.isValid());
            QVERIFY(!users.isNull(0));
            QVERIFY(!users.isNull(1));
            QVERIFY(!users.isNull(ID));
            QVERIFY(!users.isNull(NAME));
            // Number of fields
            const auto &record = users.recordCached();
            QCOMPARE(record.count(), 2);
            QVERIFY(record.contains(ID));
            QVERIFY(record.contains(NAME));

            actual.emplaceBack(users.value(ID).value<quint64>(),
                               users.value(NAME).value<QString>());
        }
        QCOMPARE(actual, expected);
        QCOMPARE(users.at(), AfterLastRow);
    }
}

void tst_SqlQuery_Normal::select_executeAnotherQuery_OnSameInstance() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto query = createQuery(connection);

    // Execute first time
    {
        const auto queryString = u"select id, name from users order by id"_s;
        const auto ok = query.exec(queryString);

        QVERIFY(ok);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(query.at(), BeforeFirstRow);
        const auto querySize = query.size();
        QCOMPARE(querySize, 5);
        // Behaves the same as the size() for SELECT queries
        QCOMPARE(query.numRowsAffected(), 5);
        QCOMPARE(query.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(query.executedQuery(), queryString);
        QCOMPARE(query.lastInsertId(), QVariant());

        QVERIFY(query.boundValues().isEmpty());

        // Verify the result
        QList<IdAndCustomType<QString>> expected {
            {1, "andrej"}, {2, "silver"}, {3, "peter"}, {4, "jack"}, {5, "obiwan"},
        };
        QList<IdAndCustomType<QString>> actual;
        actual.reserve(querySize);

        while (query.next()) {
            QVERIFY(query.isValid());
            QVERIFY(!query.isNull(0));
            QVERIFY(!query.isNull(1));
            QVERIFY(!query.isNull(ID));
            QVERIFY(!query.isNull(NAME));
            // Number of fields
            const auto &record = query.recordCached();
            QCOMPARE(record.count(), 2);
            QVERIFY(record.contains(ID));
            QVERIFY(record.contains(NAME));

            actual.emplaceBack(query.value(ID).value<quint64>(),
                               query.value(NAME).value<QString>());
        }
        QCOMPARE(actual, expected);
        QCOMPARE(query.at(), AfterLastRow);
    }

    // Re-execute second time
    {
        const auto queryString = u"select * from roles order by id"_s;
        const auto ok = query.exec(queryString);

        QVERIFY(ok);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(query.at(), BeforeFirstRow);
        const auto querySize = query.size();
        QCOMPARE(querySize, 3);
        // Behaves the same as the size() for SELECT queries
        QCOMPARE(query.numRowsAffected(), 3);
        QCOMPARE(query.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(query.executedQuery(), queryString);
        QCOMPARE(query.lastInsertId(), QVariant());

        QVERIFY(query.boundValues().isEmpty());

        // Verify the result
        QList<IdAndCustomType<QString>> expected {
            {1, "role one"}, {2, "role two"}, {3, "role three"},
        };
        QList<IdAndCustomType<QString>> actual;
        actual.reserve(querySize);

        while (query.next()) {
            QVERIFY(query.isValid());
            QVERIFY(!query.isNull(0));
            QVERIFY(!query.isNull(1));
            QVERIFY(!query.isNull(ID));
            QVERIFY(!query.isNull(NAME));
            // Number of fields
            const auto &record = query.recordCached();
            QCOMPARE(record.count(), 3);
            QVERIFY(record.contains(ID));
            QVERIFY(record.contains(NAME));
            QVERIFY(record.contains(AddedOn));

            actual.emplaceBack(query.value(ID).value<quint64>(),
                               query.value(NAME).value<QString>());
        }
        QCOMPARE(actual, expected);
        QCOMPARE(query.at(), AfterLastRow);
    }
}

void tst_SqlQuery_Normal::select_reExecute_SameQuery_AfterFinish() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    const auto query = u"select id, name from users order by id"_s;

    QList<IdAndCustomType<QString>> expected {
        {1, "andrej"}, {2, "silver"}, {3, "peter"}, {4, "jack"}, {5, "obiwan"},
    };

    // Execute first time
    {
        const auto ok = users.exec(query);

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(users.isSelect());
        QVERIFY(!users.isValid());
        QCOMPARE(users.at(), BeforeFirstRow);
        const auto querySize = users.size();
        QCOMPARE(querySize, 5);
        // Behaves the same as the size() for SELECT queries
        QCOMPARE(users.numRowsAffected(), 5);
        QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(users.executedQuery(), query);
        QCOMPARE(users.lastInsertId(), QVariant());

        QVERIFY(users.boundValues().isEmpty());

        // Verify the result
        QList<IdAndCustomType<QString>> actual;
        actual.reserve(querySize);

        while (users.next()) {
            QVERIFY(users.isValid());
            QVERIFY(!users.isNull(0));
            QVERIFY(!users.isNull(1));
            QVERIFY(!users.isNull(ID));
            QVERIFY(!users.isNull(NAME));
            // Number of fields
            const auto &record = users.recordCached();
            QCOMPARE(record.count(), 2);
            QVERIFY(record.contains(ID));
            QVERIFY(record.contains(NAME));

            actual.emplaceBack(users.value(ID).value<quint64>(),
                               users.value(NAME).value<QString>());
        }
        QCOMPARE(actual, expected);
        QCOMPARE(users.at(), AfterLastRow);
    }

    // Finish (detachFromResultSet())
    {
        users.finish();

        QVERIFY(!users.isActive());
        QVERIFY(users.isSelect());
        QVERIFY(!users.isValid());
        QCOMPARE(users.at(), BeforeFirstRow);
        QVERIFY_THROWS_EXCEPTION(LogicError, users.size());
        // Behaves the same as the size() for SELECT queries
        QVERIFY_THROWS_EXCEPTION(LogicError, users.numRowsAffected());
        QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(users.executedQuery(), query);
        QVERIFY_THROWS_EXCEPTION(LogicError, users.lastInsertId());

        QVERIFY(!users.next());
        QVERIFY(!users.previous());
        QVERIFY(!users.first());
        QVERIFY(!users.last());
        QVERIFY(!users.seek(1));
    }

    // Re-execute second time
    {
        const auto ok = users.exec(query);

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(users.isSelect());
        QVERIFY(!users.isValid());
        QCOMPARE(users.at(), BeforeFirstRow);
        const auto querySize = users.size();
        QCOMPARE(querySize, 5);
        // Behaves the same as the size() for SELECT queries
        QCOMPARE(users.numRowsAffected(), 5);
        QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(users.executedQuery(), query);
        QCOMPARE(users.lastInsertId(), QVariant());

        QVERIFY(users.boundValues().isEmpty());

        // Verify the result
        QList<IdAndCustomType<QString>> actual;
        actual.reserve(querySize);

        while (users.next()) {
            QVERIFY(users.isValid());
            QVERIFY(!users.isNull(0));
            QVERIFY(!users.isNull(1));
            QVERIFY(!users.isNull(ID));
            QVERIFY(!users.isNull(NAME));
            // Number of fields
            const auto &record = users.recordCached();
            QCOMPARE(record.count(), 2);
            QVERIFY(record.contains(ID));
            QVERIFY(record.contains(NAME));

            actual.emplaceBack(users.value(ID).value<quint64>(),
                               users.value(NAME).value<QString>());
        }
        QCOMPARE(actual, expected);
        QCOMPARE(users.at(), AfterLastRow);
    }
}

void tst_SqlQuery_Normal::select_executeAnotherQuery_OnSameInstance_AfterFinish() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto query = createQuery(connection);

    // Execute first time
    const auto queryString = u"select id, name from users order by id"_s;
    {
        const auto ok = query.exec(queryString);

        QVERIFY(ok);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(query.at(), BeforeFirstRow);
        const auto querySize = query.size();
        QCOMPARE(querySize, 5);
        // Behaves the same as the size() for SELECT queries
        QCOMPARE(query.numRowsAffected(), 5);
        QCOMPARE(query.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(query.executedQuery(), queryString);
        QCOMPARE(query.lastInsertId(), QVariant());

        QVERIFY(query.boundValues().isEmpty());

        // Verify the result
        QList<IdAndCustomType<QString>> expected {
            {1, "andrej"}, {2, "silver"}, {3, "peter"}, {4, "jack"}, {5, "obiwan"},
        };
        QList<IdAndCustomType<QString>> actual;
        actual.reserve(querySize);

        while (query.next()) {
            QVERIFY(query.isValid());
            QVERIFY(!query.isNull(0));
            QVERIFY(!query.isNull(1));
            QVERIFY(!query.isNull(ID));
            QVERIFY(!query.isNull(NAME));
            // Number of fields
            const auto &record = query.recordCached();
            QCOMPARE(record.count(), 2);
            QVERIFY(record.contains(ID));
            QVERIFY(record.contains(NAME));

            actual.emplaceBack(query.value(ID).value<quint64>(),
                               query.value(NAME).value<QString>());
        }
        QCOMPARE(actual, expected);
        QCOMPARE(query.at(), AfterLastRow);
    }

    // Finish (detachFromResultSet())
    {
        query.finish();

        QVERIFY(!query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(query.at(), BeforeFirstRow);
        QVERIFY_THROWS_EXCEPTION(LogicError, query.size());
        // Behaves the same as the size() for SELECT queries
        QVERIFY_THROWS_EXCEPTION(LogicError, query.numRowsAffected());
        QCOMPARE(query.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(query.executedQuery(), queryString);
        QVERIFY_THROWS_EXCEPTION(LogicError, query.lastInsertId());

        QVERIFY(!query.next());
        QVERIFY(!query.previous());
        QVERIFY(!query.first());
        QVERIFY(!query.last());
        QVERIFY(!query.seek(1));
    }

    // Re-execute second time
    {
        const auto queryString2 = u"select * from roles order by id"_s;
        const auto ok = query.exec(queryString2);

        QVERIFY(ok);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(query.at(), BeforeFirstRow);
        const auto querySize = query.size();
        QCOMPARE(querySize, 3);
        // Behaves the same as the size() for SELECT queries
        QCOMPARE(query.numRowsAffected(), 3);
        QCOMPARE(query.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(query.executedQuery(), queryString2);
        QCOMPARE(query.lastInsertId(), QVariant());

        QVERIFY(query.boundValues().isEmpty());

        // Verify the result
        QList<IdAndCustomType<QString>> expected {
            {1, "role one"}, {2, "role two"}, {3, "role three"},
        };
        QList<IdAndCustomType<QString>> actual;
        actual.reserve(querySize);

        while (query.next()) {
            QVERIFY(query.isValid());
            QVERIFY(!query.isNull(0));
            QVERIFY(!query.isNull(1));
            QVERIFY(!query.isNull(ID));
            QVERIFY(!query.isNull(NAME));
            // Number of fields
            const auto &record = query.recordCached();
            QCOMPARE(record.count(), 3);
            QVERIFY(record.contains(ID));
            QVERIFY(record.contains(NAME));
            QVERIFY(record.contains(AddedOn));

            actual.emplaceBack(query.value(ID).value<quint64>(),
                               query.value(NAME).value<QString>());
        }
        QCOMPARE(actual, expected);
        QCOMPARE(query.at(), AfterLastRow);
    }
}

/* I will test the INSERT, UPDATE, and DELETE in the one test method, it's nothing
   wrong about it (it's not absolutely cosher though), it also tests executing more
   queries on the same SqlQuery instance. */
void tst_SqlQuery_Normal::insert_update_delete() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);
    quint64 lastInsertedId = 0;

    // INSERT a new row into the users table
    {
        const auto query =
                u"insert into users "
                   "(name, is_banned, note, created_at, updated_at, deleted_at) "
                 "values ('ashen one', 1, 'test drivers INSERT', "
                   "'2023-05-11T11:52:53', '2023-05-12T11:52:53', null)"_s;
        const auto ok = users.exec(query);

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(!users.isSelect());
        QVERIFY(!users.isValid());
        QVERIFY_THROWS_EXCEPTION(LogicError, users.size());
        QCOMPARE(users.numRowsAffected(), 1);
        QCOMPARE(users.executedQuery(), query);

        lastInsertedId = users.lastInsertId().value<quint64>();
        QVERIFY(lastInsertedId > 5);
    }

    const auto columnNames = std::to_array({ID, NAME, u"is_banned"_s, NOTE, CREATED_AT,
                                            UPDATED_AT, DELETED_AT});

    // Verify the INSERT
    {
        const auto query = u"select * from users where id = %1"_s.arg(lastInsertedId);
        auto ok = users.exec(query);

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(users.isSelect());
        QVERIFY(!users.isValid());
        const auto querySize = users.size();
        QCOMPARE(querySize, 1);
        QCOMPARE(users.executedQuery(), query);

        // Verify the result
        ok = users.first();
        QVERIFY(ok);
        QVERIFY(users.isValid());
        // Number of fields
        const auto &record = users.recordCached();
        QCOMPARE(record.count(), 7);
        for (const auto &column : columnNames)
            QVERIFY(record.contains(column));

        // Tests if the QVariant has the correct type will be done in other test methods
        QCOMPARE(users.value(NAME)       .value<QString>(),   u"ashen one"_s);
        QCOMPARE(users.value("is_banned").value<bool>(),      true);
        QCOMPARE(users.value(NOTE)       .value<QString>(),   u"test drivers INSERT"_s);
#if tinymysql_lib_utc_qdatetime >= 20240618
        QCOMPARE(users.value(CREATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 11},
                                                                        {11, 52, 53},
                                                                        QTimeZone::UTC));
        QCOMPARE(users.value(UPDATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 12},
                                                                        {11, 52, 53},
                                                                        QTimeZone::UTC));
#else
        QCOMPARE(users.value(CREATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 11},
                                                                        {11, 52, 53}));
        QCOMPARE(users.value(UPDATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 12},
                                                                        {11, 52, 53}));
#endif
        QVERIFY(users.isNull(DELETED_AT));
        QCOMPARE(users.value(DELETED_AT), NullVariant::QDateTime());
    }

    // UPDATE
    {
        const auto query = u"update users set name = '%1', is_banned = %2 "
                              "where id = %3"_s
                           .arg("micah").arg(0).arg(lastInsertedId);
        auto ok = users.exec(query);

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(!users.isSelect());
        QVERIFY(!users.isValid());
        QVERIFY_THROWS_EXCEPTION(LogicError, users.size());
        QCOMPARE(users.numRowsAffected(), 1);
        QCOMPARE(users.executedQuery(), query);
    }

    // Verify the UPDATE
    {
        const auto query = u"select * from users where id = %1"_s.arg(lastInsertedId);
        auto ok = users.exec(query);

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(users.isSelect());
        QVERIFY(!users.isValid());
        const auto querySize = users.size();
        QCOMPARE(querySize, 1);
        QCOMPARE(users.executedQuery(), query);

        // Verify the result
        ok = users.first();
        QVERIFY(ok);
        QVERIFY(users.isValid());
        // Number of fields
        const auto &record = users.recordCached();
        QCOMPARE(record.count(), 7);
        for (const auto &column : columnNames)
            QVERIFY(record.contains(column));

        // Tests if the QVariant has the correct type will be done in other test methods
        QCOMPARE(users.value(NAME)       .value<QString>(),   u"micah"_s);
        QCOMPARE(users.value("is_banned").value<bool>(),      false);
        QCOMPARE(users.value(NOTE)       .value<QString>(),   u"test drivers INSERT"_s);
#if tinymysql_lib_utc_qdatetime >= 20240618
        QCOMPARE(users.value(CREATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 11},
                                                                        {11, 52, 53},
                                                                        QTimeZone::UTC));
        QCOMPARE(users.value(UPDATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 12},
                                                                        {11, 52, 53},
                                                                        QTimeZone::UTC));
#else
        QCOMPARE(users.value(CREATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 11},
                                                                        {11, 52, 53}));
        QCOMPARE(users.value(UPDATED_AT) .value<QDateTime>(), QDateTime({2023, 05, 12},
                                                                        {11, 52, 53}));
#endif
        QVERIFY(users.isNull(DELETED_AT));
        QCOMPARE(users.value(DELETED_AT), NullVariant::QDateTime());
    }

    // Restore and also test the DELETE
    {
        const auto query = u"delete from users where id = %1"_s.arg(lastInsertedId);
        auto ok = users.exec(query);

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(!users.isSelect());
        QVERIFY(!users.isValid());
        QVERIFY_THROWS_EXCEPTION(LogicError, users.size());
        QCOMPARE(users.numRowsAffected(), 1);
        QCOMPARE(users.executedQuery(), query);
    }

    // Verify the DELETE
    {
        const auto query = u"select id from users where id = %1"_s.arg(lastInsertedId);
        auto ok = users.exec(query);

        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(users.isSelect());
        QVERIFY(!users.isValid());
        const auto querySize = users.size();
        QCOMPARE(querySize, 0);
        QCOMPARE(users.executedQuery(), query);

        // Verify the result
        ok = users.first();
        QVERIFY(!ok);
        // Check also this, must stay the same
        QVERIFY(users.isActive());
        QVERIFY(users.isSelect());
        QVERIFY(!users.isValid());
    }
}

void tst_SqlQuery_Normal::clear() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto users = createQuery(connection);

    // First do a normal select query
    {
        const auto query = u"select id, name from users order by id"_s;
        const auto ok = users.exec(query);

        // Check everything what can be checked for this basic query (default configuration)
        QVERIFY(ok);
        QVERIFY(users.isActive());
        QVERIFY(users.isSelect());
        QVERIFY(!users.isValid());
        QCOMPARE(users.at(), BeforeFirstRow);
        const auto querySize = users.size();
        QCOMPARE(querySize, 5);
        // Behaves the same as the size() for SELECT queries
        QCOMPARE(users.numRowsAffected(), 5);
        QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(users.executedQuery(), query);
        QCOMPARE(users.lastInsertId(), QVariant());

        QVERIFY(users.boundValues().isEmpty());

        // Verify the result
        QList<IdAndCustomType<QString>> expected {
            {1, "andrej"}, {2, "silver"}, {3, "peter"}, {4, "jack"}, {5, "obiwan"},
        };
        QList<IdAndCustomType<QString>> actual;
        actual.reserve(querySize);

        while (users.next()) {
            QVERIFY(users.isValid());
            QVERIFY(!users.isNull(0));
            QVERIFY(!users.isNull(1));
            QVERIFY(!users.isNull(ID));
            QVERIFY(!users.isNull(NAME));
            // Number of fields
            const auto &record = users.recordCached();
            QCOMPARE(record.count(), 2);
            QVERIFY(record.contains(ID));
            QVERIFY(record.contains(NAME));

            actual.emplaceBack(users.value(ID).value<quint64>(),
                               users.value(NAME).value<QString>());
        }
        QCOMPARE(actual, expected);
        QCOMPARE(users.at(), AfterLastRow);
    }

    // Verify SqlQuery::clear()
    {
        users.clear();

        // Comparing SqlQuery-ies isn't possible so I have to check all possible manually

        QVERIFY(!users.isActive());
        QVERIFY(!users.isSelect());
        QVERIFY(!users.isValid());
        QCOMPARE(users.at(), BeforeFirstRow);
        QVERIFY_THROWS_EXCEPTION(LogicError, users.isEmpty());
        QVERIFY_THROWS_EXCEPTION(LogicError, users.size());
        QVERIFY_THROWS_EXCEPTION(LogicError, users.numRowsAffected());
        QCOMPARE(users.numericalPrecisionPolicy(), LowPrecisionDouble);
        QCOMPARE(users.executedQuery(), QString());
        QVERIFY_THROWS_EXCEPTION(LogicError, users.lastInsertId());

        QVERIFY(users.boundValues().isEmpty());
    }
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

SqlQuery tst_SqlQuery_Normal::createQuery(const QString &connection)
{
   return SqlQuery(Databases::driversConnection(connection));
}

QTEST_MAIN(tst_SqlQuery_Normal)

#include "tst_sqlquery_normal.moc"
