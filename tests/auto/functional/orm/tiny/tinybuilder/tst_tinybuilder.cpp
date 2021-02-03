#include <QCoreApplication>
#include <QtTest>

#include "models/torrent.hpp"

#include "database.hpp"

using namespace Orm;
// TODO namespace silverqx
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

    QCOMPARE(torrents.at(1).getAttributes().size(), 8);
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
        QCOMPARE(torrent.getAttributes().size(), 8);
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

QTEST_MAIN(tst_TinyBuilder)

#include "tst_tinybuilder.moc"
