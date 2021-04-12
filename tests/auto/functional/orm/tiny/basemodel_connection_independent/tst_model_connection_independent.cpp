#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "orm/db.hpp"

#include "models/massassignmentmodels.hpp"
#include "models/torrent.hpp"
#include "models/torrenteager.hpp"

#include "database.hpp"

using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::MassAssignmentError;

class tst_Model_Connection_Independent : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void subscriptOperator() const;
    void subscriptOperator_OnLhs() const;
    void subscriptOperator_OnLhs_AssignAttributeReference() const;

    void defaultAttributeValues() const;

    void massAssignment_Fillable() const;
    void massAssignment_Guarded() const;
    void massAssignment_GuardedAll_NonExistentAttribute() const;
    void massAssignment_GuardedDisabled_ExistentAttribute() const;
    void massAssignment_GuardedDisabled_NonExistentAttribute() const;
    void massAssignment_TotallyGuarded_Exception() const;
    void massAssignment_CantMassFillAttributesWithTableNamesWhenUsingGuarded() const;

    void massAssignment_forceFill_OnTotallyGuardedModel() const;
    void massAssignment_forceFill_OnGuardedAttribute() const;
    void massAssignment_forceFill_NonExistentAttribute() const;

private:
    /*! Connection name used in this test case. */
    QString m_connection;
};

void tst_Model_Connection_Independent::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            TestUtils::Database::createConnection("tinyorm_mysql_tests");
}

void tst_Model_Connection_Independent::subscriptOperator() const
{
    auto torrent = Torrent::find(2);
    QVERIFY(torrent->exists);

    QCOMPARE((*torrent)["id"], QVariant(2));
    QCOMPARE((*torrent)["name"], QVariant("test2"));

    QCOMPARE((*torrent)["added_on"],
            QVariant(QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate)));
}

void tst_Model_Connection_Independent::subscriptOperator_OnLhs() const
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

void tst_Model_Connection_Independent
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

void tst_Model_Connection_Independent::defaultAttributeValues() const
{
    {
        TorrentEager torrent;

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent["size"], QVariant(0));
        QCOMPARE(torrent["progress"], QVariant(0));
        QCOMPARE(torrent["added_on"],
                QVariant(QDateTime::fromString("2021-04-01 15:10:10", Qt::ISODate)));
        QCOMPARE(torrent.getAttributes().size(), 3);
    }
    {
        const auto name = "test22";
        const auto note = "Torrent::instance()";

        auto torrent = TorrentEager::instance({
            {"name", name},
            {"note", note},
        });

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent["size"], QVariant(0));
        QCOMPARE(torrent["progress"], QVariant(0));
        QCOMPARE(torrent["added_on"],
                QVariant(QDateTime::fromString("2021-04-01 15:10:10", Qt::ISODate)));
        QCOMPARE(torrent["name"], QVariant(name));
        QCOMPARE(torrent["note"], QVariant(note));
        QCOMPARE(torrent.getAttributes().size(), 5);
    }
    {
        const auto name = "test22";
        const auto note = "Torrent::instance()";

        TorrentEager torrent {
            {"name", name},
            {"note", note},
        };

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent["size"], QVariant(0));
        QCOMPARE(torrent["progress"], QVariant(0));
        QCOMPARE(torrent["added_on"],
                QVariant(QDateTime::fromString("2021-04-01 15:10:10", Qt::ISODate)));
        QCOMPARE(torrent["name"], QVariant(name));
        QCOMPARE(torrent["note"], QVariant(note));
        QCOMPARE(torrent.getAttributes().size(), 5);
    }
}

void tst_Model_Connection_Independent::massAssignment_Fillable() const
{
    Torrent torrent;

    torrent.fill({{"name", "test150"}, {"size", 10}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent["name"], QVariant("test150"));
    QCOMPARE(torrent["size"], QVariant(10));
    QCOMPARE(torrent.getAttributes().size(), 2);
}

void tst_Model_Connection_Independent::massAssignment_Guarded() const
{
    Torrent_GuardedAttribute torrent;

    torrent.fill({{"created_at", QDateTime::currentDateTime()}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 0);
}

void tst_Model_Connection_Independent
     ::massAssignment_GuardedAll_NonExistentAttribute() const
{
    Torrent torrent;

    torrent.fill({{"dummy-NON_EXISTENT", "foo"}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 0);
}

void tst_Model_Connection_Independent
     ::massAssignment_GuardedDisabled_ExistentAttribute() const
{
    Torrent_AllowedMassAssignment torrent;

    torrent.fill({{"dummy-NON_EXISTENT", "foo"}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent["dummy-NON_EXISTENT"], QVariant("foo"));
    QCOMPARE(torrent.getAttributes().size(), 1);
}

void tst_Model_Connection_Independent
     ::massAssignment_GuardedDisabled_NonExistentAttribute() const
{
    Torrent_AllowedMassAssignment torrent;

    torrent.fill({{"dummy-NON_EXISTENT", "foo"}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent["dummy-NON_EXISTENT"], QVariant("foo"));
    QCOMPARE(torrent.getAttributes().size(), 1);
}

void
tst_Model_Connection_Independent::massAssignment_TotallyGuarded_Exception() const
{
    Torrent_TotallyGuarded torrent;

    QVERIFY(!torrent.exists);
    QVERIFY_EXCEPTION_THROWN(torrent.fill({{"name", "test150"}}),
                             MassAssignmentError);
}

void tst_Model_Connection_Independent
     ::massAssignment_CantMassFillAttributesWithTableNamesWhenUsingGuarded() const
{
    Torrent torrent;

    torrent.fill({{"foo.bar", 123}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent.getAttributes().size(), 0);
}

void tst_Model_Connection_Independent
     ::massAssignment_forceFill_OnTotallyGuardedModel() const
{
    Torrent_TotallyGuarded torrent;
    torrent.forceFill({{"name", "foo"}, {"size", 12}, {"progress", 20}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent["name"], QVariant("foo"));
    QCOMPARE(torrent["size"], QVariant(12));
    QCOMPARE(torrent["progress"], QVariant(20));
    QCOMPARE(torrent.getAttributes().size(), 3);
}

void tst_Model_Connection_Independent
     ::massAssignment_forceFill_OnGuardedAttribute() const
{
    Torrent_GuardedAttribute torrent;

    const auto createdAt = QDateTime::fromString("2021-02-02 10:11:12", Qt::ISODate);

    torrent.forceFill({{"created_at", createdAt}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent["created_at"], QVariant(createdAt));
    QCOMPARE(torrent.getAttributes().size(), 1);
}

void tst_Model_Connection_Independent
     ::massAssignment_forceFill_NonExistentAttribute() const
{
    Torrent_TotallyGuarded torrent;
    torrent.forceFill({{"dummy-NON_EXISTENT", "foo"}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent["dummy-NON_EXISTENT"], QVariant("foo"));
    QCOMPARE(torrent.getAttributes().size(), 1);
}

QTEST_MAIN(tst_Model_Connection_Independent)

#include "tst_model_connection_independent.moc"
