#include <QCoreApplication>
#include <QTest>

#include "orm/drivers/sqlrecord.hpp"

#include "orm/constants.hpp"
#include "orm/utils/nullvariant.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Constants::ID;
using Orm::Constants::NOTE;
using Orm::Constants::SIZE_;

using Orm::Drivers::SqlRecord;

using Orm::Utils::NullVariant;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_SqlDatabase : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase() const;

    void table_record_WithDefaultValues() const;
    void table_record_WithoutDefaultValues() const;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_SqlDatabase::initTestCase() const
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

// This is an overkill, but it tests everything, I think I have a lot of free time 😁😅
void tst_SqlDatabase::table_record_WithDefaultValues() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    static const auto EmptyWithDefaultValues = u"empty_with_default_values"_s;

    const auto db = Databases::driversConnection(connection);
    QVERIFY(db.isValid());
    QVERIFY(db.isOpen());
    QVERIFY(!db.isOpenError());
                                                        // Don't uncomment to test the default argument
    const auto record = db.record(EmptyWithDefaultValues/*, true*/);

    // Verify the record
    QVERIFY(!record.isEmpty());
    const auto recordCount = record.count();
    QCOMPARE(recordCount, 6);

    // Populate values to compare
    // Column definitions related only
    QList<bool> expectedAutoIncrements {true, false, false, false, false, false};
    QList<bool> actualAutoIncrements;
    actualAutoIncrements.reserve(recordCount);

    // NULL in the table definition (not the QVariant value itself)
    QList<bool> expectedNullColumns {false, true, false, true, false, true};
    QList<bool> actualNullColumns;
    actualNullColumns.reserve(recordCount);

    QList<QMetaType> expectedMetaTypes { // clazy:exclude=missing-typeinfo
        QMetaType::fromType<quint64>(), QMetaType::fromType<quint64>(),
        QMetaType::fromType<quint64>(), QMetaType::fromType<double>(),
        QMetaType::fromType<QDateTime>(), QMetaType::fromType<QString>(),
    };
    QList<QMetaType> actualMetaTypes; // clazy:exclude=missing-typeinfo
    actualMetaTypes.reserve(recordCount);

    static const auto BIGINT = u"BIGINT"_s;
    QList<QString> expectedSqlTypeNames {
        BIGINT, BIGINT, BIGINT, u"DECIMAL"_s, u"DATETIME"_s, u"VARCHAR"_s,
    };
    QList<QString> actualSqlTypeNames;
    actualSqlTypeNames.reserve(recordCount);

    QList<qint64> expectedLengths {20, 20, 20, 10, 19, 1020};
    QList<qint64> actualLengths;
    actualLengths.reserve(recordCount);

    QList<qint64> expectedPrecisions {0, 0, 0, 2, 0, 0};
    QList<qint64> actualPrecisions;
    actualPrecisions.reserve(recordCount);

    QList<QVariant> expectedValues {
        NullVariant::ULongLong(), NullVariant::ULongLong(), NullVariant::ULongLong(),
        NullVariant::Double() ,NullVariant::QDateTime(), NullVariant::QString(),
    };
    QList<QVariant> actualValues;
    actualValues.reserve(recordCount);

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
                    Zero.toUtf8(), u"100.12"_s.toUtf8(), u"CURRENT_TIMESTAMP"_s.toUtf8(),
                    NullVariant::QByteArray()};

        if (connection == Databases::MARIADB_DRIVERS)
            return {NullVariant::QString(), NULL_, Zero, u"100.12"_s,
                    u"current_timestamp()"_s, NULL_};

        Q_UNREACHABLE();
    });
    QList<QVariant> actualDefaultValues;
    actualDefaultValues.reserve(recordCount);

    for (SqlRecord::size_type i = 0; i < recordCount; ++i) {
        const auto field = record.field(i);
        QVERIFY(field.isValid());
        QVERIFY(field.isNull());
        QCOMPARE(field.tableName(), EmptyWithDefaultValues);

        actualAutoIncrements  << field.isAutoIncrement();
        actualNullColumns     << field.isNullColumn();
        actualMetaTypes       << field.metaType();
        actualSqlTypeNames    << field.sqlTypeName();
        actualLengths         << field.length();
        actualPrecisions      << field.precision();
        actualValues          << field.value();
        actualDefaultValues   << field.defaultValue();
    }

    // Verify all at once
    QCOMPARE(record.fieldNames(),
             QStringList({ID, "user_id", SIZE_, "decimal", "added_on", NOTE}));
    QCOMPARE(actualAutoIncrements, expectedAutoIncrements);
    QCOMPARE(actualNullColumns, expectedNullColumns);
    QCOMPARE(actualMetaTypes, expectedMetaTypes);
    QCOMPARE(actualSqlTypeNames, expectedSqlTypeNames);
    QCOMPARE(actualLengths, expectedLengths);
    QCOMPARE(actualPrecisions, expectedPrecisions);
    QCOMPARE(actualValues, expectedValues);
    QCOMPARE(actualDefaultValues, expectedDefaultValues);
}

void tst_SqlDatabase::table_record_WithoutDefaultValues() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    static const auto EmptyWithDefaultValues = u"empty_with_default_values"_s;

    const auto db = Databases::driversConnection(connection);
    QVERIFY(db.isValid());
    QVERIFY(db.isOpen());
    QVERIFY(!db.isOpenError());

    const auto record = db.record(EmptyWithDefaultValues, false);

    // Verify the record
    QVERIFY(!record.isEmpty());
    const auto recordCount = record.count();
    QCOMPARE(recordCount, 6);

    // Populate values to compare
    // Column definitions related only
    QList<bool> expectedAutoIncrements {true, false, false, false, false, false};
    QList<bool> actualAutoIncrements;
    actualAutoIncrements.reserve(recordCount);

    // NULL in the table definition (not the QVariant value itself)
    QList<bool> expectedNullColumns {false, true, false, true, false, true};
    QList<bool> actualNullColumns;
    actualNullColumns.reserve(recordCount);

    QList<QMetaType> expectedMetaTypes { // clazy:exclude=missing-typeinfo
        QMetaType::fromType<quint64>(), QMetaType::fromType<quint64>(),
        QMetaType::fromType<quint64>(), QMetaType::fromType<double>(),
        QMetaType::fromType<QDateTime>(), QMetaType::fromType<QString>(),
    };
    QList<QMetaType> actualMetaTypes; // clazy:exclude=missing-typeinfo
    actualMetaTypes.reserve(recordCount);

    static const auto BIGINT = u"BIGINT"_s;
    QList<QString> expectedSqlTypeNames {
       BIGINT, BIGINT, BIGINT, u"DECIMAL"_s, u"DATETIME"_s, u"VARCHAR"_s,
    };
    QList<QString> actualSqlTypeNames;
    actualSqlTypeNames.reserve(recordCount);

    QList<qint64> expectedLengths {20, 20, 20, 10, 19, 1020};
    QList<qint64> actualLengths;
    actualLengths.reserve(recordCount);

    QList<qint64> expectedPrecisions {0, 0, 0, 2, 0, 0};
    QList<qint64> actualPrecisions;
    actualPrecisions.reserve(recordCount);

    QList<QVariant> expectedValues {
        NullVariant::ULongLong(), NullVariant::ULongLong(), NullVariant::ULongLong(),
        NullVariant::Double() ,NullVariant::QDateTime(), NullVariant::QString(),
    };
    QList<QVariant> actualValues;
    actualValues.reserve(recordCount);

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
                    Zero.toUtf8(), u"100.12"_s.toUtf8(), u"CURRENT_TIMESTAMP"_s.toUtf8(),
                    NullVariant::QByteArray()};

        if (connection == Databases::MARIADB_DRIVERS)
            return {NullVariant::QString(), NULL_, Zero, u"100.12"_s,
                    u"current_timestamp()"_s, NULL_};

        Q_UNREACHABLE();
    });
    QList<QVariant> actualDefaultValues;
    actualDefaultValues.reserve(recordCount);

    for (SqlRecord::size_type i = 0; i < recordCount; ++i) {
        const auto field = record.field(i);
        QVERIFY(field.isValid());
        QVERIFY(field.isNull());
        QCOMPARE(field.tableName(), EmptyWithDefaultValues);

        actualAutoIncrements << field.isAutoIncrement();
        actualNullColumns    << field.isNullColumn();
        actualMetaTypes      << field.metaType();
        actualSqlTypeNames   << field.sqlTypeName();
        actualLengths        << field.length();
        actualPrecisions     << field.precision();
        actualValues         << field.value();
        QVERIFY(!field.defaultValue().isValid());
    }

    // Verify all at once
    QCOMPARE(record.fieldNames(),
             QStringList({ID, "user_id", SIZE_, "decimal", "added_on", NOTE}));
    QCOMPARE(actualAutoIncrements, expectedAutoIncrements);
    QCOMPARE(actualNullColumns, expectedNullColumns);
    QCOMPARE(actualMetaTypes, expectedMetaTypes);
    QCOMPARE(actualSqlTypeNames, expectedSqlTypeNames);
    QCOMPARE(actualLengths, expectedLengths);
    QCOMPARE(actualPrecisions, expectedPrecisions);
    QCOMPARE(actualValues, expectedValues);

    // Clear before the next loop
    actualAutoIncrements.clear();
    actualNullColumns.clear();
    actualMetaTypes.clear();
    actualSqlTypeNames.clear();
    actualLengths.clear();
    actualPrecisions.clear();
    actualValues.clear();
    actualDefaultValues.clear();

    /* Re-create the SqlRecord with Default Column Values, it of course must be done
       after all the previous tests as the last thing to test it correctly. */
    const auto recordNew = db.record(EmptyWithDefaultValues, true);
    // Non-cached SqlRecord is returned by value
    QVERIFY(std::addressof(recordNew) != std::addressof(record));

    // Verify the record
    QVERIFY(!recordNew.isEmpty());
    const auto recordCountNew = recordNew.count();
    QCOMPARE(recordCountNew, recordCount);

    /* Verify re-populated Default Column Values, OK I reinvoke the same test logic
       again as everything should stay the same, to correctly test it. */

    for (SqlRecord::size_type i = 0; i < recordCountNew; ++i) {
        const auto field = recordNew.field(i);
        QVERIFY(field.isValid());
        QVERIFY(field.isNull());
        QCOMPARE(field.tableName(), EmptyWithDefaultValues);

        actualAutoIncrements << field.isAutoIncrement();
        actualNullColumns    << field.isNullColumn();
        actualMetaTypes      << field.metaType();
        actualSqlTypeNames   << field.sqlTypeName();
        actualLengths        << field.length();
        actualPrecisions     << field.precision();
        actualValues         << field.value();
        actualDefaultValues  << field.defaultValue();
    }

    // Verify all at once
    QCOMPARE(recordNew.fieldNames(),
             QStringList({ID, "user_id", SIZE_, "decimal", "added_on", NOTE}));
    QCOMPARE(actualAutoIncrements, expectedAutoIncrements);
    QCOMPARE(actualNullColumns, expectedNullColumns);
    QCOMPARE(actualMetaTypes, expectedMetaTypes);
    QCOMPARE(actualSqlTypeNames, expectedSqlTypeNames);
    QCOMPARE(actualLengths, expectedLengths);
    QCOMPARE(actualPrecisions, expectedPrecisions);
    QCOMPARE(actualValues, expectedValues);
    QCOMPARE(actualDefaultValues, expectedDefaultValues);
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_SqlDatabase)

#include "tst_sqldatabase.moc"
