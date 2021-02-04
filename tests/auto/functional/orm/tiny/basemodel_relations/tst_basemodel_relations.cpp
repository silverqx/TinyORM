#include <QCoreApplication>
#include <QtTest>

#include "models/torrenteager.hpp"
#include "models/torrentpeereager.hpp"

#include "database.hpp"

using namespace Orm;
// TODO tests, namespace silverqx
using namespace Orm::Tiny;

class tst_BaseModel_Relations : public QObject
{
    Q_OBJECT

public:
    tst_BaseModel_Relations();
    ~tst_BaseModel_Relations() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void getRelation_EagerLoad_ManyAndOne() const;
    void getRelation_EagerLoad_BelongsTo() const;

private:
    /*! The database connection instance. */
    ConnectionInterface &m_connection;
};

tst_BaseModel_Relations::tst_BaseModel_Relations()
    : m_connection(TestUtils::Database::createConnection())
{}

void tst_BaseModel_Relations::initTestCase()
{}

void tst_BaseModel_Relations::cleanupTestCase()
{}

void tst_BaseModel_Relations::getRelation_EagerLoad_ManyAndOne() const
{
    auto torrent = TorrentEager::find(2);
    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    // TorrentPreviewableFileEager has many relation
    auto files = torrent->getRelation<TorrentPreviewableFileEager>("torrentFiles");
    QCOMPARE(files.size(), 2);
    QCOMPARE(typeid (QVector<TorrentPreviewableFileEager *>), typeid (files));

    // Expected file IDs
    QVector<quint64> fileIds {2, 3};
    // Expected file property IDs
    QVector<quint64> filePropertyIds {1, 2};
    for (const auto &file : qAsConst(files)) {
        QVERIFY(file);
        QVERIFY(file->exists);
        QCOMPARE(file->getAttribute("torrent_id"), QVariant(2));
        QVERIFY(fileIds.contains(file->getAttribute("id").toULongLong()));

        /* TorrentPreviewableFilePropertyEager has one relation, loaded by
           dot notation in the u_with data member. */
        auto fileProperty =
                file->getRelation<TorrentPreviewableFilePropertyEager, One>("fileProperty");
        QVERIFY(fileProperty);
        QVERIFY(fileProperty->exists);
        QCOMPARE(typeid (TorrentPreviewableFilePropertyEager *), typeid (fileProperty));
    }
}

void tst_BaseModel_Relations::getRelation_EagerLoad_BelongsTo() const
{
    auto torrentPeer = TorrentPeerEager::find(2);
    QVERIFY(torrentPeer);
    QVERIFY(torrentPeer->exists);

    // TorrentEager belongs to relation
    auto torrent = torrentPeer->getRelation<TorrentEager, One>("torrent");
    QVERIFY(torrent);
    QCOMPARE(torrent->getAttribute("id"), QVariant(2));
}

QTEST_MAIN(tst_BaseModel_Relations)

#include "tst_basemodel_relations.moc"
