#include <QCoreApplication>
#include <QTest>

#include "orm/db.hpp"
#include "orm/utils/query.hpp"
#include "orm/utils/string.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"
#include "macros.hpp"

using Orm::Constants::QMYSQL;
using Orm::Constants::text_;

using Orm::DB;
using Orm::Query::Builder;

using QueryBuilder = Orm::Query::Builder;
using QueryUtils = Orm::Utils::Query;
using StringUtils = Orm::Utils::String;
using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_Blobs : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;
    void cleanup() const;

    void text() const;
    void mediumText() const;

    void binary() const;
    void mediumBinary() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Create QueryBuilder instance for the given connection. */
    [[nodiscard]] static std::shared_ptr<QueryBuilder>
    createQuery(const QString &connection);

    /* MySQL max_allowed_packet */
    /*! Check and set the max_allowed_packet GLOBAL variable to the given value. */
    void prepareMaxAllowedPacketForMySql(std::size_t minimumValue,
                                         const QString &connection) const;
    /*! Restore the max_allowed_packet GLOBAL variable to the initial value. */
    void restoreMaxAllowedPacketForMySql(const QString &connection) const;
    /*! Obtain the max_allowed_packet SESSION variable value in bytes. */
    static std::size_t getMaxAllowedPacketForMySql(const QString &connection);

    /*! Lorem ipsum paragraph size (number of characters including a newline). */
    constexpr static auto m_paragraphSize = 512;
    /*! An initial max_allowed_packet value cache (to be able to restore it). */
    mutable std::optional<std::size_t> m_initialMaxAllowedPacket = std::nullopt;
};

/*! QString constant for the "types" table. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, TypesTable, ("types")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! QString constant for the "medium_text" column name. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, medium_text, ("medium_text")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! QString constant for the "binary" column name. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, medium_binary, ("medium_binary")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Blobs::initTestCase_data() const
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

void tst_Blobs::cleanup() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    restoreMaxAllowedPacketForMySql(connection);
}

void tst_Blobs::text() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Prepare the max_allowed_packet MySQL variable (requires 16MB)
    prepareMaxAllowedPacketForMySql(16'777'216, connection);

    constexpr static auto paragraphsCount = 128;
    /* 65'535 bytes (65kB without 1 byte).
       It's the max. size of the MySQL TEXT column type. */
    constexpr auto expectedLoremIpsumSize = m_paragraphSize * paragraphsCount - 1;

    static const auto loremIpsum = StringUtils::loremIpsum512Paragraph(paragraphsCount);
    /* 128 paragraphs each has 511 characters + a newline;
       The last paragraph has 511 characters without a newline. */
    QCOMPARE(loremIpsum.size(), expectedLoremIpsumSize);

    QVariant lastId;
    quint64 lastIdInt = 0;

    // Insert to the TEXT column
    {
        auto query = createQuery(connection)
                     ->from(*TypesTable).insert({text_}, {{loremIpsum}});
        QVERIFY(query->isActive());
        QVERIFY(!query->isSelect());
        QVERIFY(!query->isValid());

        lastId = query->lastInsertId();
        lastIdInt = lastId.value<quint64>();

        QVERIFY(lastId.isValid() && !lastId.isNull());
        QVERIFY(lastIdInt > 3);
    }

    // Verify an insert
    {
        auto query = createQuery(connection)->from(*TypesTable).find(lastId);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(query.isValid());
        QCOMPARE(QueryUtils::queryResultSize(query), 1);

        QVERIFY(query.first());
        QVERIFY(query.isValid());

        QCOMPARE(query.value(text_), QVariant(loremIpsum));
    }

    // Restore
    {
        auto &&[affected, query] = createQuery(connection)
                                   ->from(*TypesTable).deleteRow(lastIdInt);
        QVERIFY(query.isActive());
        QVERIFY(!query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(affected, 1);
    }

    // Verify Restore
    {
        auto query = createQuery(connection)->from(*TypesTable).find(lastId);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(QueryUtils::queryResultSize(query), 0);
    }
}

void tst_Blobs::mediumText() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Prepare the max_allowed_packet MySQL variable (requires 64MB)
    prepareMaxAllowedPacketForMySql(67'108'864, connection);

    constexpr static auto paragraphsCount = 32'768;
    /* 16'777'215 bytes (16MB without 1 byte).
       It's the max. size of the MySQL MEDIUMTEXT column type. */
    constexpr auto expectedLoremIpsumSize = m_paragraphSize * paragraphsCount - 1;

    static const auto loremIpsum = StringUtils::loremIpsum512Paragraph(paragraphsCount);
    /* 32768 paragraphs each has 511 characters + a newline;
       The last paragraph has 511 characters without a newline. */
    QCOMPARE(loremIpsum.size(), expectedLoremIpsumSize);

    QVariant lastId;
    quint64 lastIdInt = 0;

    // Insert to the TEXT column
    {
        auto query = createQuery(connection)
                     ->from(*TypesTable).insert({*medium_text}, {{loremIpsum}});
        QVERIFY(query->isActive());
        QVERIFY(!query->isSelect());
        QVERIFY(!query->isValid());

        lastId = query->lastInsertId();
        lastIdInt = lastId.value<quint64>();

        QVERIFY(lastId.isValid() && !lastId.isNull());
        QVERIFY(lastIdInt > 3);
    }

    // Verify an insert
    {
        auto query = createQuery(connection)->from(*TypesTable).find(lastId);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(query.isValid());
        QCOMPARE(QueryUtils::queryResultSize(query), 1);

        QVERIFY(query.first());
        QVERIFY(query.isValid());

        QCOMPARE(query.value(*medium_text), QVariant(loremIpsum));
    }

    // Restore the types table
    {
        auto &&[affected, query] = createQuery(connection)
                                   ->from(*TypesTable).deleteRow(lastIdInt);
        QVERIFY(query.isActive());
        QVERIFY(!query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(affected, 1);
    }

    // Verify Restore the types table
    {
        auto query = createQuery(connection)->from(*TypesTable).find(lastId);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(QueryUtils::queryResultSize(query), 0);
    }
}

void tst_Blobs::binary() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Prepare the max_allowed_packet MySQL variable (requires 16MB)
    prepareMaxAllowedPacketForMySql(16'777'216, connection);

    constexpr static auto paragraphsCount = 128;
    /* 65'535 bytes (65kB without 1 byte).
       It's the max. size of the MySQL BLOB column type. */
    constexpr auto expectedLoremIpsumSize = m_paragraphSize * paragraphsCount - 1;

    static const auto loremIpsumString = StringUtils::loremIpsum512Paragraph(
                                             paragraphsCount);
    static const auto loremIpsum = QByteArray(loremIpsumString.toUtf8().constData());

    /* 128 paragraphs each has 511 characters + a newline;
       The last paragraph has 511 characters without a newline. */
    QCOMPARE(loremIpsum.size(), expectedLoremIpsumSize);

    QVariant lastId;
    quint64 lastIdInt = 0;

    // Insert to the BLOB column
    {
        auto query = createQuery(connection)
                     ->from(*TypesTable).insert({*medium_binary}, {{loremIpsum}});
        QVERIFY(query->isActive());
        QVERIFY(!query->isSelect());
        QVERIFY(!query->isValid());

        lastId = query->lastInsertId();
        lastIdInt = lastId.value<quint64>();

        QVERIFY(lastId.isValid() && !lastId.isNull());
        QVERIFY(lastIdInt > 3);
    }

    // Verify an insert
    {
        auto query = createQuery(connection)->from(*TypesTable).find(lastId);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(query.isValid());
        QCOMPARE(QueryUtils::queryResultSize(query), 1);

        QVERIFY(query.first());
        QVERIFY(query.isValid());

        auto val = query.value(*medium_binary);
        QCOMPARE(val, QVariant(loremIpsum));
    }

    // Restore the types table
    {
        auto &&[affected, query] = createQuery(connection)
                                   ->from(*TypesTable).deleteRow(lastIdInt);
        QVERIFY(query.isActive());
        QVERIFY(!query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(affected, 1);
    }

    // Verify Restore the types table
    {
        auto query = createQuery(connection)->from(*TypesTable).find(lastId);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(QueryUtils::queryResultSize(query), 0);
    }
}

void tst_Blobs::mediumBinary() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Prepare the max_allowed_packet MySQL variable (requires 64MB)
    prepareMaxAllowedPacketForMySql(67'108'864, connection);

    constexpr static auto paragraphsCount = 32'768;
    /* 16'777'215 bytes (16MB without 1 byte).
       It's the max. size of the MySQL MEDIUMBLOB column type. */
    constexpr auto expectedLoremIpsumSize = m_paragraphSize * paragraphsCount - 1;

    static const auto loremIpsumString = StringUtils::loremIpsum512Paragraph(
                                             paragraphsCount);
    static const auto loremIpsum = QByteArray(loremIpsumString.toUtf8().constData());

    /* 32768 paragraphs each has 511 characters + a newline;
       The last paragraph has 511 characters without a newline. */
    QCOMPARE(loremIpsum.size(), expectedLoremIpsumSize);

    QVariant lastId;
    quint64 lastIdInt = 0;

    // Insert to the BLOB column
    {
        auto query = createQuery(connection)
                     ->from(*TypesTable).insert({*medium_binary}, {{loremIpsum}});
        QVERIFY(query->isActive());
        QVERIFY(!query->isSelect());
        QVERIFY(!query->isValid());

        lastId = query->lastInsertId();
        lastIdInt = lastId.value<quint64>();

        QVERIFY(lastId.isValid() && !lastId.isNull());
        QVERIFY(lastIdInt > 3);
    }

    // Verify an insert
    {
        auto query = createQuery(connection)->from(*TypesTable).find(lastId);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(query.isValid());
        QCOMPARE(QueryUtils::queryResultSize(query), 1);

        QVERIFY(query.first());
        QVERIFY(query.isValid());

        auto val = query.value(*medium_binary);
        QCOMPARE(val, QVariant(loremIpsum));
    }

    // Restore the types table
    {
        auto &&[affected, query] = createQuery(connection)
                                   ->from(*TypesTable).deleteRow(lastIdInt);
        QVERIFY(query.isActive());
        QVERIFY(!query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(affected, 1);
    }

    // Verify Restore the types table
    {
        auto query = createQuery(connection)->from(*TypesTable).find(lastId);
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(QueryUtils::queryResultSize(query), 0);
    }
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

std::shared_ptr<QueryBuilder>
tst_Blobs::createQuery(const QString &connection)
{
    return DB::query(connection);
}

/* MySQL max_allowed_packet */

void tst_Blobs::prepareMaxAllowedPacketForMySql(const std::size_t minimumValue,
                                                const QString &connection) const
{
    // Nothing to do, this is only needed for MySQL/MariaDB databases
    if (DB::driverName(connection) != QMYSQL)
        return;

    // Obtain the max_allowed_packet SESSION variable value in bytes
    const auto initialMaxAllowedPacket = getMaxAllowedPacketForMySql(connection);

    // Nothing to do, the max_allowed_packet value is OK
    if (initialMaxAllowedPacket >= minimumValue)
        return;

    // Set the max_allowed_packet GLOBAL variable
    DB::unprepared(sl("set global `max_allowed_packet` = %1").arg(minimumValue),
                   connection);

    // Re-connect is needed so a newly set max_allowed_packet can be applied
    DB::disconnect(connection);

    // Verify the max_allowed_packet SESSION variable value
    if (getMaxAllowedPacketForMySql(connection) == minimumValue) {
        // Cache the current/initial value
        m_initialMaxAllowedPacket = initialMaxAllowedPacket;
        return;
    }

    throw std::runtime_error(
                sl("Setting the max_allowed_packet to '%1' value for '%2' connection "
                   "failed in %3().")
                .arg(minimumValue)
                .arg(connection, __tiny_func__).toUtf8().constData());
}

void tst_Blobs::restoreMaxAllowedPacketForMySql(const QString &connection) const
{
    /* Nothing to do, this is only needed for MySQL/MariaDB databases or
       the max_allowed_packet GLOBAL variable wasn't set/changed. */
    if (DB::driverName(connection) != QMYSQL || !m_initialMaxAllowedPacket)
        return;

    // Restore the max_allowed_packet GLOBAL variable to the initial value
    auto query = DB::unprepared(sl("set global `max_allowed_packet` = %1")
                                .arg(*m_initialMaxAllowedPacket),
                                connection);

    // Re-connect is needed so a newly set max_allowed_packet can be applied
    DB::disconnect(connection);

    // Verify the max_allowed_packet SESSION variable value
    if (getMaxAllowedPacketForMySql(connection) == m_initialMaxAllowedPacket)
        return m_initialMaxAllowedPacket.reset(); // NOLINT(readability-avoid-return-with-void-value) clazy:exclude=returning-void-expression

    throw std::runtime_error(
                sl("Restoring the max_allowed_packet to '%1' value for '%2' connection "
                   "failed in %3().")
                .arg(*m_initialMaxAllowedPacket)
                .arg(connection, __tiny_func__).toUtf8().constData());
}

std::size_t tst_Blobs::getMaxAllowedPacketForMySql(const QString &connection)
{
    // Obtain the max_allowed_packet SESSION variable value in bytes
    auto query = DB::unprepared(sl("select @@session.`max_allowed_packet` as `value`"),
                                connection);

    if (query.first())
        return query.value(sl("value")).value<std::size_t>();

    throw std::runtime_error(
                sl("Obtaining the @@session.max_allowed_packet variable "
                   "for '%1' connection failed in %2().")
                .arg(connection, __tiny_func__).toUtf8().constData());
}

QTEST_MAIN(tst_Blobs)

#include "tst_blobs.moc"
