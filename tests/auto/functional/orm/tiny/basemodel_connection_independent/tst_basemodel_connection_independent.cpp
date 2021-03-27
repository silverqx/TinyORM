#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "orm/db.hpp"

#include "models/torrent.hpp"

#include "database.hpp"

using Orm::Tiny::ConnectionOverride;

class tst_BaseModel_Connection_Independent : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void subscriptOperator() const;
    void subscriptOperator_OnLhs() const;
    void subscriptOperator_OnLhs_AssignAttributeReference() const;

private:
    /*! Connection name used in this test case. */
    QString m_connection;
};

void tst_BaseModel_Connection_Independent::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            TestUtils::Database::createConnection("tinyorm_mysql_tests");
}

void tst_BaseModel_Connection_Independent::subscriptOperator() const
{
    auto torrent = Torrent::find(2);
    QVERIFY(torrent->exists);

    QCOMPARE((*torrent)["id"], QVariant(2));
    QCOMPARE((*torrent)["name"], QVariant("test2"));

    // CUR tests, sqlite datetime silverqx
    if (DB::connection(m_connection).driverName() == "QSQLITE")
        return;

    QCOMPARE((*torrent)["added_on"],
            QVariant(QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate)));
}

void tst_BaseModel_Connection_Independent::subscriptOperator_OnLhs() const
{
    auto torrent = Torrent::find(2);
    QVERIFY(torrent->exists);

    QCOMPARE(torrent->getAttribute("id"), QVariant(2));
    QCOMPARE(torrent->getAttribute("name"), QVariant("test2"));
    QCOMPARE(torrent->getAttribute("size"), QVariant(12));

    const auto name = "test2 operator[]";
    const auto size = 112;
    (*torrent)["name"] = name;
    (*torrent)["size"] = size;

    QCOMPARE(torrent->getAttribute("name"), QVariant(name));
    QCOMPARE(torrent->getAttribute("size"), QVariant(size));
}

void tst_BaseModel_Connection_Independent
        ::subscriptOperator_OnLhs_AssignAttributeReference() const
{
    auto torrent2 = Torrent::find(2);
    QVERIFY(torrent2->exists);

    QCOMPARE(torrent2->getAttribute("id"), QVariant(2));
    QCOMPARE(torrent2->getAttribute("name"), QVariant("test2"));

    auto attributeReference = (*torrent2)["name"];

    // Fetch fresh torrent to assign an attribute reference to its 'name' attribute
    auto torrent3 = Torrent::find(3);
    QVERIFY(torrent3->exists);

    QCOMPARE(torrent3->getAttribute("id"), QVariant(3));
    QCOMPARE(torrent3->getAttribute("name"), QVariant("test3"));

    (*torrent3)["name"] = attributeReference;

    QCOMPARE(torrent3->getAttribute("name"), torrent2->getAttribute("name"));

    // Some more testing
    const auto name = "test2 operator[]";

    attributeReference = name;
    (*torrent3)["name"] = attributeReference;

    const auto torrent2Name = torrent2->getAttribute("name");
    QCOMPARE(torrent2Name, QVariant(name));
    QCOMPARE(torrent3->getAttribute("name"), torrent2Name);
}

QTEST_MAIN(tst_BaseModel_Connection_Independent)

#include "tst_basemodel_connection_independent.moc"
