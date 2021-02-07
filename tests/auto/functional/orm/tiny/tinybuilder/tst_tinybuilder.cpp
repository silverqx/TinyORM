#include <QCoreApplication>
#include <QtTest>

#include "models/torrent.hpp"

#include "database.hpp"

using namespace Orm;
// TODO tests, namespace silverqx
using namespace Orm::Tiny;

class tst_TinyBuilder : public QObject
{
    Q_OBJECT

public:
    tst_TinyBuilder();
    ~tst_TinyBuilder() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void get() const;
    void get_Columns() const;

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

    /*! The database connection instance. */
    ConnectionInterface &m_connection;
};

tst_TinyBuilder::tst_TinyBuilder()
    : m_connection(TestUtils::Database::createConnection())
{}

void tst_TinyBuilder::initTestCase()
{}

void tst_TinyBuilder::cleanupTestCase()
{}

void tst_TinyBuilder::get() const
{
    auto torrents = createQuery<Torrent>()->get();

    QCOMPARE(torrents.size(), 6);
    QCOMPARE(torrents.at(0).getAttribute("id"), QVariant(1));
    QCOMPARE(torrents.at(0).getAttribute("name"), QVariant("test1"));
    QCOMPARE(torrents.at(2).getAttribute("id"), QVariant(3));
    QCOMPARE(torrents.at(2).getAttribute("name"), QVariant("test3"));

    QCOMPARE(torrents.at(1).getAttributes().size(), 9);
}

void tst_TinyBuilder::get_Columns() const
{
    auto torrents = createQuery<Torrent>()->get({"id", "name", "size"});

    const auto &torrent3 = torrents.at(1);
    QCOMPARE(torrent3.getAttributes().size(), 3);
    QCOMPARE(torrent3.getAttributes().at(0).key, QString("id"));
    QCOMPARE(torrent3.getAttributes().at(1).key, QString("name"));
    QCOMPARE(torrent3.getAttributes().at(2).key, QString("size"));
}

void tst_TinyBuilder::firstOrFail_Found() const
{
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
    QVERIFY_EXCEPTION_THROWN(Torrent::whereEq("id", 999999)->firstOrFail(),
                             ModelNotFoundError);
    QVERIFY_EXCEPTION_THROWN(Torrent::whereEq("id", 999999)->firstOrFail({"id", "name"}),
                             ModelNotFoundError);
}

void tst_TinyBuilder::incrementAndDecrement() const
{
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
    QVERIFY_EXCEPTION_THROWN(
                Torrent::whereEq("id", 3)->update({{"progress-NON_EXISTENT", 333}}),
                QueryError);
}

void tst_TinyBuilder::update_SameValue() const
{
    auto timeBeforeUpdate = QDateTime::currentDateTime();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeUpdate.time();
        timeBeforeUpdate.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto torrent = Torrent::find(5);
    QVERIFY(torrent->exists);

    /* Send update query to the database, this is different from
       the BaseModel::update() method. */
    auto [affected, query] = Torrent::whereEq("id", 5)
            ->update({{"progress", torrent->getAttribute("progress")}});

    /* Don't exactly know what cause this, I think some sort of caching can
       occure. */
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
