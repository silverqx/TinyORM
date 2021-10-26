#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "orm/db.hpp"

#include "models/massassignmentmodels.hpp"
#include "models/torrent.hpp"
#include "models/torrenteager.hpp"

#include "databases.hpp"

using Orm::Tiny::ConnectionOverride;
using Orm::Constants::CREATED_AT;
using Orm::Tiny::Exceptions::MassAssignmentError;

using TestUtils::Databases;

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

    void with_WithSelectConstraint_QueryWithoutRelatedTable() const;
    void with_BelongsToMany_WithSelectConstraint_QualifiedColumnsForRelatedTable() const;

private:
    /*! Connection name used in this test case. */
    QString m_connection = {};
};

void tst_Model_Connection_Independent::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(QStringLiteral("%1 autotest skipped, environment variables "
                             "for '%2' connection have not been defined.")
              .arg("tst_Model_Connection_Independent",
                   Databases::MYSQL).toUtf8().constData(), );
}

void tst_Model_Connection_Independent::subscriptOperator() const
{
    auto torrent = Torrent::find(2);
    QVERIFY(torrent->exists);

    QCOMPARE((*torrent)[ID], QVariant(2));
    QCOMPARE((*torrent)[NAME], QVariant("test2"));

    QCOMPARE((*torrent)["added_on"],
            QVariant(QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate)));
}

void tst_Model_Connection_Independent::subscriptOperator_OnLhs() const
{
    auto torrent = Torrent::find(2);
    QVERIFY(torrent->exists);

    QCOMPARE(torrent->getAttribute(ID), QVariant(2));
    QCOMPARE(torrent->getAttribute(NAME), QVariant("test2"));
    QCOMPARE(torrent->getAttribute("size"), QVariant(12));

    const auto name = QStringLiteral("test2 operator[]");
    const auto size = 112;
    (*torrent)[NAME] = name;
    (*torrent)["size"] = size;

    QCOMPARE(torrent->getAttribute(NAME), QVariant(name));
    QCOMPARE(torrent->getAttribute("size"), QVariant(size));
}

void tst_Model_Connection_Independent
        ::subscriptOperator_OnLhs_AssignAttributeReference() const
{
    auto torrent2 = Torrent::find(2);
    QVERIFY(torrent2->exists);

    QCOMPARE(torrent2->getAttribute(ID), QVariant(2));
    QCOMPARE(torrent2->getAttribute(NAME), QVariant("test2"));

    auto attributeReference = (*torrent2)[NAME];

    // Fetch fresh torrent to assign an attribute reference to its 'name' attribute
    auto torrent3 = Torrent::find(3);
    QVERIFY(torrent3->exists);

    QCOMPARE(torrent3->getAttribute(ID), QVariant(3));
    QCOMPARE(torrent3->getAttribute(NAME), QVariant("test3"));

    (*torrent3)[NAME] = attributeReference;

    QCOMPARE(torrent3->getAttribute(NAME), torrent2->getAttribute(NAME));

    // Some more testing
    const auto name = QStringLiteral("test2 operator[]");

    attributeReference = name;
    (*torrent3)[NAME] = attributeReference;

    const auto torrent2Name = torrent2->getAttribute(NAME);
    QCOMPARE(torrent2Name, QVariant(name));
    QCOMPARE(torrent3->getAttribute(NAME), torrent2Name);
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
        const auto name = QStringLiteral("test22");
        const auto note = QStringLiteral("Torrent::instance()");

        auto torrent = TorrentEager::instance({
            {NAME, name},
            {"note", note},
        });

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent["size"], QVariant(0));
        QCOMPARE(torrent["progress"], QVariant(0));
        QCOMPARE(torrent["added_on"],
                QVariant(QDateTime::fromString("2021-04-01 15:10:10", Qt::ISODate)));
        QCOMPARE(torrent[NAME], QVariant(name));
        QCOMPARE(torrent["note"], QVariant(note));
        QCOMPARE(torrent.getAttributes().size(), 5);
    }
    {
        const auto name = QStringLiteral("test22");
        const auto note = QStringLiteral("Torrent::instance()");

        TorrentEager torrent {
            {NAME, name},
            {"note", note},
        };

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent["size"], QVariant(0));
        QCOMPARE(torrent["progress"], QVariant(0));
        QCOMPARE(torrent["added_on"],
                QVariant(QDateTime::fromString("2021-04-01 15:10:10", Qt::ISODate)));
        QCOMPARE(torrent[NAME], QVariant(name));
        QCOMPARE(torrent["note"], QVariant(note));
        QCOMPARE(torrent.getAttributes().size(), 5);
    }
}

void tst_Model_Connection_Independent::massAssignment_Fillable() const
{
    Torrent torrent;

    torrent.fill({{NAME, "test150"}, {"size", 10}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[NAME], QVariant("test150"));
    QCOMPARE(torrent["size"], QVariant(10));
    QCOMPARE(torrent.getAttributes().size(), 2);
}

void tst_Model_Connection_Independent::massAssignment_Guarded() const
{
    Torrent_GuardedAttribute torrent;

    torrent.fill({{CREATED_AT, QDateTime::currentDateTime()}});

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
    QVERIFY_EXCEPTION_THROWN(torrent.fill({{NAME, "test150"}}),
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
    torrent.forceFill({{NAME, "foo"}, {"size", 12}, {"progress", 20}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[NAME], QVariant("foo"));
    QCOMPARE(torrent["size"], QVariant(12));
    QCOMPARE(torrent["progress"], QVariant(20));
    QCOMPARE(torrent.getAttributes().size(), 3);
}

void tst_Model_Connection_Independent
     ::massAssignment_forceFill_OnGuardedAttribute() const
{
    Torrent_GuardedAttribute torrent;

    const auto createdAt = QDateTime::fromString("2021-02-02 10:11:12", Qt::ISODate);

    torrent.forceFill({{CREATED_AT, createdAt}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[CREATED_AT], QVariant(createdAt));
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

void tst_Model_Connection_Independent
     ::with_WithSelectConstraint_QueryWithoutRelatedTable() const
{
    DB::flushQueryLog(m_connection);
    DB::enableQueryLog(m_connection);
    auto torrent = Torrent::with({"torrentFiles:id,torrent_id,filepath"})->find(2);
    DB::disableQueryLog(m_connection);

    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto queryLog = DB::getQueryLog(m_connection);
    QCOMPARE(queryLog->size(), 2);
    QCOMPARE(queryLog->at(1).query,
             QString("select `id`, `torrent_id`, `filepath` "
                     "from `torrent_previewable_files` "
                     "where `torrent_previewable_files`.`torrent_id` in (?)"));
}

void tst_Model_Connection_Independent
     ::with_BelongsToMany_WithSelectConstraint_QualifiedColumnsForRelatedTable() const
{
    DB::flushQueryLog(m_connection);
    DB::enableQueryLog(m_connection);
    auto torrent = Torrent::with({"tags:id,name"})->find(3);
    DB::disableQueryLog(m_connection);

    QVERIFY(torrent);
    QVERIFY(torrent->exists);

    const auto queryLog = DB::getQueryLog(m_connection);

    QCOMPARE(queryLog->size(), 3);
    QCOMPARE(queryLog->at(1).query,
             QString(
                 "select `torrent_tags`.`id`, `torrent_tags`.`name`, "
                     "`tag_torrent`.`torrent_id` as `pivot_torrent_id`, "
                     "`tag_torrent`.`tag_id` as `pivot_tag_id`, "
                     "`tag_torrent`.`active` as `pivot_active`, "
                     "`tag_torrent`.`created_at` as `pivot_created_at`, "
                     "`tag_torrent`.`updated_at` as `pivot_updated_at` "
                 "from `torrent_tags` "
                     "inner join `tag_torrent` "
                         "on `torrent_tags`.`id` = `tag_torrent`.`tag_id` "
                 "where `tag_torrent`.`torrent_id` in (?)"));
}

QTEST_MAIN(tst_Model_Connection_Independent)

#include "tst_model_connection_independent.moc"
