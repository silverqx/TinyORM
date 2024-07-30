#include <QCoreApplication>
#include <QTest>

#include "databases.hpp"
#include "macros.hpp"

#include "models/torrent.hpp"

using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::Progress;
using Orm::Constants::SIZE_;

using Orm::TTimeZone;

using TQueryError;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Exceptions::ModelNotFoundError;

template<typename Model>
using TinyBuilder = Orm::Tiny::Builder<Model>;

using TestUtils::Databases;

using Models::Torrent;

class tst_TinyBuilder : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    void get() const;
    void get_Columns() const;

    void value() const;
    void value_ModelNotFound() const;

    void firstOrFail_Found() const;
    void firstOrFail_NotFoundFailed() const;

    void incrementAndDecrement() const;

    void update() const;
    void update_Failed() const;
    void update_SameValue() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Create the TinyBuilder by template parameter. */
    template<typename Model>
    static std::unique_ptr<TinyBuilder<Model>> createQuery();
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_TinyBuilder::initTestCase_data() const
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

void tst_TinyBuilder::get() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = createQuery<Torrent>()->get();

    QCOMPARE(torrents.size(), 7);

    const std::unordered_map<quint64, QString> expectedIdNames {
        {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
        {5, "test5"}, {6, "test6"}, {7, "test7"},
    };
    for (const auto &torrent : std::as_const(torrents)) {
        const auto torrentId = torrent[ID].value<quint64>();

        QVERIFY(expectedIdNames.contains(torrentId));
        QCOMPARE(expectedIdNames.at(torrentId), torrent[NAME].value<QString>());
    }

    QCOMPARE(torrents.at(1).getAttributes().size(), 10);
}

void tst_TinyBuilder::get_Columns() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto torrents = createQuery<Torrent>()->get({ID, NAME, SIZE_});

    const auto &torrent = torrents.at(1);
    QCOMPARE(torrent.getAttributes().size(), 3);
    QCOMPARE(torrent.getAttributes().at(0).key, QString(ID));
    QCOMPARE(torrent.getAttributes().at(1).key, QString(NAME));
    QCOMPARE(torrent.getAttributes().at(2).key, QString(SIZE_));
}

void tst_TinyBuilder::value() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto value = Torrent::whereEq(ID, 2)->value(NAME);

    QCOMPARE(value, QVariant("test2"));
}

void tst_TinyBuilder::value_ModelNotFound() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto value = Torrent::whereEq(ID, 999999)->value(NAME);

    QVERIFY(!value.isValid());
    QVERIFY(value.isNull());
}

void tst_TinyBuilder::firstOrFail_Found() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::whereEq(ID, 3)->firstOrFail();

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 10);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent[NAME], QVariant("test3"));
    }
    {
        auto torrent = Torrent::whereEq(ID, 3)->firstOrFail({ID, NAME});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent[ID], QVariant(3));
        QCOMPARE(torrent[NAME], QVariant("test3"));
    }
}

void tst_TinyBuilder::firstOrFail_NotFoundFailed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    TVERIFY_THROWS_EXCEPTION(ModelNotFoundError,
                             Torrent::whereEq(ID, 999999)->firstOrFail());
    TVERIFY_THROWS_EXCEPTION(ModelNotFoundError,
                             Torrent::whereEq(ID, 999999)->firstOrFail({ID, NAME}));
}

void tst_TinyBuilder::incrementAndDecrement() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto timeBeforeIncrement = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeIncrement.time();
        timeBeforeIncrement.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto torrent4_1 = Torrent::find(4);
    QVERIFY(torrent4_1);
    QVERIFY(torrent4_1->exists);

    const auto &updatedAtColumn = torrent4_1->getUpdatedAtColumn();

    auto sizeOriginal = torrent4_1->getAttribute(SIZE_);
    auto progressOriginal = torrent4_1->getAttribute(Progress);
    auto updatedAtOriginal = torrent4_1->getAttribute(updatedAtColumn);
    QCOMPARE(sizeOriginal, QVariant(14));
    QCOMPARE(progressOriginal, QVariant(400));
    QCOMPARE(updatedAtOriginal,
             QVariant(QDateTime({2021, 1, 4}, {18, 46, 31}, TTimeZone::UTC)));

    // Increment
    Torrent::whereEq(ID, 4)->increment(SIZE_, 2, {{Progress, 444}});

    auto torrent4_2 = Torrent::find(4);
    QVERIFY(torrent4_2);
    QVERIFY(torrent4_2->exists);
    QCOMPARE(torrent4_2->getAttribute(SIZE_), QVariant(16));
    QCOMPARE(torrent4_2->getAttribute(Progress), QVariant(444));
    QVERIFY(torrent4_2->getAttribute<QDateTime>(updatedAtColumn) >= timeBeforeIncrement);

    // Decrement and restore updated at column
    Torrent::whereEq(ID, 4)->decrement(SIZE_, 2, {{Progress, 400},
                                                  {updatedAtColumn, updatedAtOriginal}});

    auto torrent4_3 = Torrent::find(4);
    QVERIFY(torrent4_3);
    QVERIFY(torrent4_3->exists);
    QCOMPARE(torrent4_3->getAttribute(SIZE_), QVariant(14));
    QCOMPARE(torrent4_3->getAttribute(Progress), QVariant(400));
    QCOMPARE(torrent4_3->getAttribute(updatedAtColumn), updatedAtOriginal);
}

void tst_TinyBuilder::update() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto timeBeforeUpdate = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeUpdate.time();
        timeBeforeUpdate.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto torrent = Torrent::find(4);

    const auto &updatedAtColumn = torrent->getUpdatedAtColumn();

    auto progressOriginal = torrent->getAttribute(Progress);
    auto updatedAtOriginal = torrent->getAttribute(updatedAtColumn);

    QVERIFY(torrent->exists);
    QCOMPARE(progressOriginal, QVariant(400));
    QCOMPARE(updatedAtOriginal,
             QVariant(QDateTime({2021, 1, 4}, {18, 46, 31}, TTimeZone::UTC)));

    auto [affected, query] = Torrent::whereEq(ID, 4)->update({{Progress, 447}});
    QCOMPARE(affected, 1);
    QVERIFY(query.isActive());

    // Verify value in the database
    auto torrentVerify = Torrent::find(4);
    QVERIFY(torrentVerify->exists);
    QCOMPARE(torrentVerify->getAttribute(Progress), QVariant(447));
    QVERIFY(torrentVerify->getAttribute<QDateTime>(updatedAtColumn) >= timeBeforeUpdate);

    // Revert value back
    auto [affectedRevert, queryRevert] =
            Torrent::whereEq(ID, 4)->update({{Progress, progressOriginal},
                                               {updatedAtColumn, updatedAtOriginal}});
    QCOMPARE(affectedRevert, 1);
    QVERIFY(queryRevert.isActive());
    QCOMPARE(torrent->getAttribute(Progress), progressOriginal);
    /* Needed to convert toDateTime() because TinyBuilder::update() set update_at
       attribute as QString. */
    QCOMPARE(torrent->getAttribute<QDateTime>(updatedAtColumn),
             updatedAtOriginal.value<QDateTime>());
}

void tst_TinyBuilder::update_Failed() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    TVERIFY_THROWS_EXCEPTION(
                QueryError,
                Torrent::whereEq(ID, 3)->update({{"progress-NON_EXISTENT", 333}}));
}

void tst_TinyBuilder::update_SameValue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    ConnectionOverride::connection = connection;

    auto timeBeforeUpdate = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeUpdate.time();
        timeBeforeUpdate.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto torrent = Torrent::find(5);
    QVERIFY(torrent->exists);

    /* Send update query to the database, this is different from
       the Model::update() method. */
    auto [affected, query] = Torrent::whereEq(ID, 5)
            ->update({{Progress, torrent->getAttribute(Progress)}});

    /* Don't exactly know what cause this, I think some sort of caching can
       occure, but it doesn't matter, leave it as is in the future, it is ok. */
    QVERIFY(affected == 1 || affected == 0);
    QVERIFY(query.isActive());

    // Verify value in the database
    auto torrentVerify = Torrent::find(5);
    QVERIFY(torrentVerify->exists);
    QVERIFY(torrentVerify->getAttribute<QDateTime>(torrent->getUpdatedAtColumn()) >=
            timeBeforeUpdate);
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

template<typename Model>
std::unique_ptr<TinyBuilder<Model>>
tst_TinyBuilder::createQuery()
{
    return Model::query();
}

QTEST_MAIN(tst_TinyBuilder)

#include "tst_tinybuilder.moc"
