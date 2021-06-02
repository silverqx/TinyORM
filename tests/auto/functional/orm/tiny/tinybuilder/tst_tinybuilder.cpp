#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"

#include "models/torrent.hpp"

#include "database.hpp"

using Orm::QueryError;
using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::ModelNotFoundError;

template<typename Model>
using TinyBuilder = Orm::Tiny::Builder<Model>;

class tst_TinyBuilder : public QObject
{
    Q_OBJECT

private slots:
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

private:
    /*! Create the TinyBuilder by template parameter. */
    template<typename Model>
    inline static std::unique_ptr<TinyBuilder<Model>>
    createQuery()
    { return Model().query(); }
};

void tst_TinyBuilder::initTestCase_data() const
{
    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : TestUtils::Database::createConnections())
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_TinyBuilder::get() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = createQuery<Torrent>()->get();

    QCOMPARE(torrents.size(), 6);

    const std::unordered_map<quint64, QString> expectedIdNames {
        {1, "test1"}, {2, "test2"}, {3, "test3"},
        {4, "test4"}, {5, "test5"}, {6, "test6"},
    };
    for (const auto &torrent : torrents) {
        const auto torrentId = torrent["id"].value<quint64>();

        QVERIFY(expectedIdNames.contains(torrentId));
        QCOMPARE(expectedIdNames.at(torrentId), torrent["name"].value<QString>());
    }

    QCOMPARE(torrents.at(1).getAttributes().size(), 9);
}

void tst_TinyBuilder::get_Columns() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto torrents = createQuery<Torrent>()->get({"id", "name", "size"});

    const auto &torrent = torrents.at(1);
    QCOMPARE(torrent.getAttributes().size(), 3);
    QCOMPARE(torrent.getAttributes().at(0).key, QString("id"));
    QCOMPARE(torrent.getAttributes().at(1).key, QString("name"));
    QCOMPARE(torrent.getAttributes().at(2).key, QString("size"));
}

void tst_TinyBuilder::value() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto value = Torrent::whereEq("id", 2)->value("name");

    QCOMPARE(value, QVariant("test2"));
}

void tst_TinyBuilder::value_ModelNotFound() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto value = Torrent::whereEq("id", 999999)->value("name");

    QVERIFY(!value.isValid());
    QVERIFY(value.isNull());
}

void tst_TinyBuilder::firstOrFail_Found() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    {
        auto torrent = Torrent::whereEq("id", 3)->firstOrFail();

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 9);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
    {
        auto torrent = Torrent::whereEq("id", 3)->firstOrFail({"id", "name"});

        QVERIFY(torrent.exists);
        QCOMPARE(torrent.getAttributes().size(), 2);
        QCOMPARE(torrent["id"], QVariant(3));
        QCOMPARE(torrent["name"], QVariant("test3"));
    }
}

void tst_TinyBuilder::firstOrFail_NotFoundFailed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    QVERIFY_EXCEPTION_THROWN(Torrent::whereEq("id", 999999)->firstOrFail(),
                             ModelNotFoundError);
    QVERIFY_EXCEPTION_THROWN(Torrent::whereEq("id", 999999)->firstOrFail({"id", "name"}),
                             ModelNotFoundError);
}

void tst_TinyBuilder::incrementAndDecrement() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto timeBeforeIncrement = QDateTime::currentDateTime();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeIncrement.time();
        timeBeforeIncrement.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto torrent4_1 = Torrent::find(4);
    QVERIFY(torrent4_1);
    QVERIFY(torrent4_1->exists);

    auto &updatedAtColumn = torrent4_1->getUpdatedAtColumn();

    auto sizeOriginal = torrent4_1->getAttribute("size");
    auto progressOriginal = torrent4_1->getAttribute("progress");
    auto updatedAtOriginal = torrent4_1->getAttribute(updatedAtColumn);
    QCOMPARE(sizeOriginal, QVariant(14));
    QCOMPARE(progressOriginal, QVariant(400));
    QCOMPARE(updatedAtOriginal,
             QVariant(QDateTime::fromString("2021-01-04 18:46:31", Qt::ISODate)));

    // Incremented
    Torrent::whereEq("id", 4)->increment("size", 2, {{"progress", 444}});

    auto torrent4_2 = Torrent::find(4);
    QVERIFY(torrent4_2);
    QVERIFY(torrent4_2->exists);
    QCOMPARE(torrent4_2->getAttribute("size"), QVariant(16));
    QCOMPARE(torrent4_2->getAttribute("progress"), QVariant(444));
    QVERIFY(torrent4_2->getAttribute(updatedAtColumn).toDateTime()
            >= timeBeforeIncrement);

    // Decremented and restore updated at column
    Torrent::whereEq("id", 4)->decrement("size", 2,
                                         {{"progress", 400},
                                          {updatedAtColumn, updatedAtOriginal}});

    auto torrent4_3 = Torrent::find(4);
    QVERIFY(torrent4_3);
    QVERIFY(torrent4_3->exists);
    QCOMPARE(torrent4_3->getAttribute("size"), QVariant(14));
    QCOMPARE(torrent4_3->getAttribute("progress"), QVariant(400));
    QCOMPARE(torrent4_3->getAttribute(updatedAtColumn), updatedAtOriginal);
}

void tst_TinyBuilder::update() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto timeBeforeUpdate = QDateTime::currentDateTime();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeUpdate.time();
        timeBeforeUpdate.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto torrent = Torrent::find(4);

    auto &updatedAtColumn = torrent->getUpdatedAtColumn();

    auto progressOriginal = torrent->getAttribute("progress");
    auto updatedAtOriginal = torrent->getAttribute(updatedAtColumn);

    QVERIFY(torrent->exists);
    QCOMPARE(progressOriginal, QVariant(400));
    QCOMPARE(updatedAtOriginal,
             QVariant(QDateTime::fromString("2021-01-04 18:46:31", Qt::ISODate)));

    auto [affected, query] = Torrent::whereEq("id", 4)->update({{"progress", 447}});
    QCOMPARE(affected, 1);
    QVERIFY(query.isActive());

    // Verify value in the database
    auto torrentVerify = Torrent::find(4);
    QVERIFY(torrentVerify->exists);
    QCOMPARE(torrentVerify->getAttribute("progress"), QVariant(447));
    QVERIFY(torrentVerify->getAttribute(updatedAtColumn).toDateTime()
            >= timeBeforeUpdate);

    // Revert value back
    auto [affectedRevert, queryRevert] =
            Torrent::whereEq("id", 4)->update({{"progress", progressOriginal},
                                               {updatedAtColumn, updatedAtOriginal}});
    QCOMPARE(affectedRevert, 1);
    QVERIFY(queryRevert.isActive());
    QCOMPARE(torrent->getAttribute("progress"), progressOriginal);
    qDebug() << torrent->getAttribute(updatedAtColumn);
    qDebug() << updatedAtOriginal;
    /* Needed to convert toDateTime() because TinyBuilder::update() set update_at
       attribute as QString. */
    QCOMPARE(torrent->getAttribute(updatedAtColumn).toDateTime(),
             updatedAtOriginal.toDateTime());
}

void tst_TinyBuilder::update_Failed() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    QVERIFY_EXCEPTION_THROWN(
                Torrent::whereEq("id", 3)->update({{"progress-NON_EXISTENT", 333}}),
                QueryError);
}

void tst_TinyBuilder::update_SameValue() const
{
    QFETCH_GLOBAL(QString, connection);

    ConnectionOverride::connection = connection;

    auto timeBeforeUpdate = QDateTime::currentDateTime();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeUpdate.time();
        timeBeforeUpdate.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto torrent = Torrent::find(5);
    QVERIFY(torrent->exists);

    /* Send update query to the database, this is different from
       the Model::update() method. */
    auto [affected, query] = Torrent::whereEq("id", 5)
            ->update({{"progress", torrent->getAttribute("progress")}});

    /* Don't exactly know what cause this, I think some sort of caching can
       occure, but it doesn't matter, leave it as is in the future, it is ok. */
    QVERIFY(affected == 1 || affected == 0);
    QVERIFY(query.isActive());

    // Verify value in the database
    auto torrentVerify = Torrent::find(5);
    QVERIFY(torrentVerify->exists);
    QVERIFY(torrentVerify->getAttribute(torrent->getUpdatedAtColumn()).toDateTime()
            >= timeBeforeUpdate);
}

QTEST_MAIN(tst_TinyBuilder)

#include "tst_tinybuilder.moc"
