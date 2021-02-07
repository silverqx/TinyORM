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
    auto beforeIncrement = QDateTime::currentDateTime();
    // Reset milliseconds to 0
    {
        auto time = beforeIncrement.time();
        beforeIncrement.setTime(QTime(time.hour(), time.minute(), time.second()));
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
             QVariant(QDateTime::fromString("2021-01-03 18:46:31", Qt::ISODate)));

    // Incremented
    Torrent::whereEq("id", 4)->increment("size", 2, {{"progress", 444}});

    auto torrent4_2 = Torrent::find(4);
    QVERIFY(torrent4_2);
    QVERIFY(torrent4_2->exists);
    QCOMPARE(torrent4_2->getAttribute("size"), QVariant(16));
    QCOMPARE(torrent4_2->getAttribute("progress"), QVariant(444));
    QVERIFY(torrent4_2->getAttribute(updatedAtColumn).toDateTime() >= beforeIncrement);

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

QTEST_MAIN(tst_TinyBuilder)

#include "tst_tinybuilder.moc"
