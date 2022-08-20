#include <QCoreApplication>
#include <QtSql/QSqlDriver>
#include <QtTest>

#include "orm/db.hpp"

#include "models/filepropertyproperty.hpp"
#include "models/massassignmentmodels.hpp"
#include "models/torrent.hpp"
#include "models/torrenteager.hpp"

#include "databases.hpp"

using Orm::Constants::ASTERISK;
using Orm::Constants::CREATED_AT;
using Orm::Constants::EMPTY;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::SIZE;

using Orm::DB;
using Orm::Exceptions::MultipleRecordsFoundError;
using Orm::Exceptions::RecordsNotFoundError;

using Orm::Tiny::ConnectionOverride;
using Orm::Tiny::Exceptions::MassAssignmentError;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

using Models::FilePropertyProperty;
using Models::Torrent;
using Models::TorrentPreviewableFile;
using Models::Torrent_AllowedMassAssignment;
using Models::Torrent_GuardedAttribute;
using Models::Torrent_TotallyGuarded;
using Models::TorrentEager;

class tst_Model_Connection_Independent : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private slots:
    void initTestCase();

    void subscriptOperator() const;
    void subscriptOperator_OnLhs() const;
    void subscriptOperator_OnLhs_AssignAttributeReference() const;

    void isCleanAndIsDirty() const;

    void is() const;
    void isNot() const;

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

    void pluck() const;
    void pluck_EmptyResult() const;
    void pluck_QualifiedColumnOrKey() const;

    void pluck_With_u_dates() const;
    void pluck_EmptyResult_With_u_dates() const;
    void pluck_QualifiedColumnOrKey_With_u_dates() const;

    void whereRowValues() const;

    /* Builds Queries */
    void chunk() const;
    void chunk_ReturnFalse() const;
    void chunk_EnforceOrderBy() const;
    void chunk_EmptyResult() const;

    void each() const;
    void each_ReturnFalse() const;
    void each_EnforceOrderBy() const;
    void each_EmptyResult() const;

    void chunkMap() const;
    void chunkMap_EnforceOrderBy() const;
    void chunkMap_EmptyResult() const;

    void chunkMap_TemplatedReturnValue() const;
    void chunkMap_EnforceOrderBy_TemplatedReturnValue() const;
    void chunkMap_EmptyResult_TemplatedReturnValue() const;

    void chunkById() const;
    void chunkById_ReturnFalse() const;
    void chunkById_EmptyResult() const;

    void chunkById_WithAlias() const;
    void chunkById_ReturnFalse_WithAlias() const;
    void chunkById_EmptyResult_WithAlias() const;

    void eachById() const;
    void eachById_ReturnFalse() const;
    void eachById_EmptyResult() const;

    void eachById_WithAlias() const;
    void eachById_ReturnFalse_WithAlias() const;
    void eachById_EmptyResult_WithAlias() const;

    void tap() const;

    void sole() const;
    void sole_RecordsNotFoundError() const;
    void sole_MultipleRecordsFoundError() const;
    void sole_Pretending() const;

    void soleValue() const;
    void soleValue_RecordsNotFoundError() const;
    void soleValue_MultipleRecordsFoundError() const;
    void soleValue_Pretending() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection {};
};

void tst_Model_Connection_Independent::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
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
    QCOMPARE(torrent->getAttribute(SIZE), QVariant(12));

    const auto name = QStringLiteral("test2 operator[]");
    const auto size = 112;
    (*torrent)[NAME] = name;
    (*torrent)[SIZE] = size;

    QCOMPARE(torrent->getAttribute(NAME), QVariant(name));
    QCOMPARE(torrent->getAttribute(SIZE), QVariant(size));
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

void tst_Model_Connection_Independent::isCleanAndIsDirty() const
{
    auto torrent = Torrent::find(3);

    QVERIFY(torrent->isClean());
    QVERIFY(!torrent->isDirty());
    QVERIFY(torrent->isClean(NAME));
    QVERIFY(!torrent->isDirty(NAME));

    torrent->setAttribute(NAME, "test3 dirty");

    QVERIFY(!torrent->isClean());
    QVERIFY(torrent->isDirty());
    QVERIFY(!torrent->isClean(NAME));
    QVERIFY(torrent->isDirty(NAME));
    QVERIFY(torrent->isClean(SIZE));
    QVERIFY(!torrent->isDirty(SIZE));

    torrent->save();

    QVERIFY(torrent->isClean());
    QVERIFY(!torrent->isDirty());
    QVERIFY(torrent->isClean(NAME));
    QVERIFY(!torrent->isDirty(NAME));
    QVERIFY(torrent->isClean(SIZE));
    QVERIFY(!torrent->isDirty(SIZE));

    // Restore the name
    torrent->setAttribute(NAME, "test3");
    torrent->save();
}

void tst_Model_Connection_Independent::is() const
{
    auto torrent2_1 = Torrent::find(2);
    auto torrent2_2 = Torrent::find(2);

    // The same primary key, table name and connection name
    QVERIFY(torrent2_1->is(torrent2_2));
}

void tst_Model_Connection_Independent::isNot() const
{
    auto torrent2_1 = Torrent::find(2);

    // Different primary key
    {
        auto torrent3 = Torrent::find(3);

        QVERIFY(torrent2_1->isNot(torrent3));
    }
    // Different table name (also different type)
    {
        auto file4 = TorrentPreviewableFile::find(4);

        QVERIFY(torrent2_1->isNot(file4));
    }
    // Different connection name
    {
        auto torrent2_2 = Torrent::find(2);

        torrent2_2->setConnection("dummy_connection");
        /* Disable connection override, so the isNot() can pickup a connection from
           the model itself (don't pickup an overridden connection). */
        ConnectionOverride::connection = EMPTY;

        QVERIFY(torrent2_1->isNot(torrent2_2));
    }

    // Restore
    ConnectionOverride::connection = m_connection;
}

void tst_Model_Connection_Independent::defaultAttributeValues() const
{
    {
        TorrentEager torrent;

        QVERIFY(!torrent.exists);
        QCOMPARE(torrent[SIZE], QVariant(0));
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
        QCOMPARE(torrent[SIZE], QVariant(0));
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
        QCOMPARE(torrent[SIZE], QVariant(0));
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

    torrent.fill({{NAME, "test150"}, {SIZE, 10}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[NAME], QVariant("test150"));
    QCOMPARE(torrent[SIZE], QVariant(10));
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
    torrent.forceFill({{NAME, "foo"}, {SIZE, 12}, {"progress", 20}});

    QVERIFY(!torrent.exists);
    QCOMPARE(torrent[NAME], QVariant("foo"));
    QCOMPARE(torrent[SIZE], QVariant(12));
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

void tst_Model_Connection_Independent::pluck() const
{
    // Simple pluck without keying
    {
        auto result = Torrent::orderBy(NAME)->pluck(NAME);

        QVector<QVariant> expected {
            "test1", "test2", "test3", "test4", "test5", "test6",
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by id<quint64>
    {
        auto result = Torrent::pluck<quint64>(NAME, ID);

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"},
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by name<QString>
    {
        auto result = Torrent::pluck<QString>(SIZE, NAME);

        std::map<QString, QVariant> expected {
            {"test1", 11}, {"test2", 12}, {"test3", 13}, {"test4", 14},
            {"test5", 15}, {"test6", 16},
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by file_index<bool>, bool type is used intentionally 😎
    {
        auto result = TorrentPreviewableFile::orderBy(ID)
                      ->pluck<bool>("filepath", "file_index");

        std::map<bool, QVariant> expected {
            {false, "test1_file1.mkv"}, {true, "test2_file2.mkv"},
        };
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::pluck_EmptyResult() const
{
    {
        auto result = Torrent::whereEq(NAME, "dummy-NON_EXISTENT")->pluck(NAME);

        QCOMPARE(result, QVector<QVariant>());
    }
    {
        auto result = Torrent::whereEq(NAME, "dummy-NON_EXISTENT")
                      ->pluck<quint64>(NAME, ID);

        std::map<quint64, QVariant> expected;
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::pluck_QualifiedColumnOrKey() const
{
    // Strip table name
    {
        auto result = Torrent::orderBy(NAME)->pluck("torrents.name");

        QVector<QVariant> expected {
            "test1", "test2", "test3", "test4", "test5", "test6",
        };
        QCOMPARE(result, expected);
    }
    // Strip table name
    {
        auto result = Torrent::pluck<quint64>("torrents.name", "torrents.id");

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"},
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias
    {
        auto result = Torrent::orderBy("name_alt")->pluck("name as name_alt");

        QVector<QVariant> expected {
            "test1", "test2", "test3", "test4", "test5", "test6",
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias
    {
        auto result = Torrent::pluck<quint64>("name as name_alt", "id as id_alt");

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"},
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias and table name
    {
        auto result = Torrent::pluck<quint64>("torrents.name", "id as id_alt");

        std::map<quint64, QVariant> expected {
            {1, "test1"}, {2, "test2"}, {3, "test3"}, {4, "test4"},
            {5, "test5"}, {6, "test6"},
        };
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::pluck_With_u_dates() const
{
    // Simple pluck without keying
    {
        auto result = Torrent::pluck("added_on");

        QVector<QVariant> expected {
            QDateTime::fromString("2020-08-01 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-03 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-04 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-05 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-06 20:11:10", Qt::ISODate),
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by id<quint64> and QVariant<QDateTime> as the value
    {
        auto result = Torrent::pluck<quint64>("added_on", ID);

        std::map<quint64, QVariant> expected {
            {1, QDateTime::fromString("2020-08-01 20:11:10", Qt::ISODate)},
            {2, QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate)},
            {3, QDateTime::fromString("2020-08-03 20:11:10", Qt::ISODate)},
            {4, QDateTime::fromString("2020-08-04 20:11:10", Qt::ISODate)},
            {5, QDateTime::fromString("2020-08-05 20:11:10", Qt::ISODate)},
            {6, QDateTime::fromString("2020-08-06 20:11:10", Qt::ISODate)},
        };
        QCOMPARE(result, expected);
    }
    // Templated pluck keyed by added_on<QDateTime>
    {
        auto result = Torrent::pluck<QDateTime>(ID, "added_on");

        std::map<QDateTime, QVariant> expected {
            {QDateTime::fromString("2020-08-01 20:11:10", Qt::ISODate), 1},
            {QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate), 2},
            {QDateTime::fromString("2020-08-03 20:11:10", Qt::ISODate), 3},
            {QDateTime::fromString("2020-08-04 20:11:10", Qt::ISODate), 4},
            {QDateTime::fromString("2020-08-05 20:11:10", Qt::ISODate), 5},
            {QDateTime::fromString("2020-08-06 20:11:10", Qt::ISODate), 6},
        };
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::pluck_EmptyResult_With_u_dates() const
{
    {
        auto result = Torrent::whereEq(NAME, "dummy-NON_EXISTENT")->pluck("added_on");

        QCOMPARE(result, QVector<QVariant>());
    }
    {
        auto result = Torrent::whereEq(NAME, "dummy-NON_EXISTENT")
                      ->pluck<QDateTime>(ID, "added_on");

        std::map<QDateTime, QVariant> expected;
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::pluck_QualifiedColumnOrKey_With_u_dates() const
{
    // Strip table name
    {
        auto result = Torrent::pluck("torrents.added_on");

        QVector<QVariant> expected {
            QDateTime::fromString("2020-08-01 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-03 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-04 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-05 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-06 20:11:10", Qt::ISODate),
        };
        QCOMPARE(result, expected);
    }
    // Strip table name
    {
        auto result = Torrent::pluck<quint64>("torrents.added_on", ID);

        std::map<quint64, QVariant> expected {
            {1, QDateTime::fromString("2020-08-01 20:11:10", Qt::ISODate)},
            {2, QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate)},
            {3, QDateTime::fromString("2020-08-03 20:11:10", Qt::ISODate)},
            {4, QDateTime::fromString("2020-08-04 20:11:10", Qt::ISODate)},
            {5, QDateTime::fromString("2020-08-05 20:11:10", Qt::ISODate)},
            {6, QDateTime::fromString("2020-08-06 20:11:10", Qt::ISODate)},
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias
    {
        auto result = Torrent::orderBy(ID)->pluck("added_on as added_on_alt");

        QVector<QVariant> expected {
            QDateTime::fromString("2020-08-01 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-03 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-04 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-05 20:11:10", Qt::ISODate),
            QDateTime::fromString("2020-08-06 20:11:10", Qt::ISODate),
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias
    {
        auto result = Torrent::pluck<quint64>("added_on as added_on_alt", "id as id_alt");

        std::map<quint64, QVariant> expected {
            {1, QDateTime::fromString("2020-08-01 20:11:10", Qt::ISODate)},
            {2, QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate)},
            {3, QDateTime::fromString("2020-08-03 20:11:10", Qt::ISODate)},
            {4, QDateTime::fromString("2020-08-04 20:11:10", Qt::ISODate)},
            {5, QDateTime::fromString("2020-08-05 20:11:10", Qt::ISODate)},
            {6, QDateTime::fromString("2020-08-06 20:11:10", Qt::ISODate)},
        };
        QCOMPARE(result, expected);
    }
    // Strip column alias and table name
    {
        auto result = Torrent::pluck<quint64>("torrents.added_on", "id as id_alt");

        std::map<quint64, QVariant> expected {
            {1, QDateTime::fromString("2020-08-01 20:11:10", Qt::ISODate)},
            {2, QDateTime::fromString("2020-08-02 20:11:10", Qt::ISODate)},
            {3, QDateTime::fromString("2020-08-03 20:11:10", Qt::ISODate)},
            {4, QDateTime::fromString("2020-08-04 20:11:10", Qt::ISODate)},
            {5, QDateTime::fromString("2020-08-05 20:11:10", Qt::ISODate)},
            {6, QDateTime::fromString("2020-08-06 20:11:10", Qt::ISODate)},
        };
        QCOMPARE(result, expected);
    }
}

void tst_Model_Connection_Independent::whereRowValues() const
{
    auto tag = Torrent::whereRowValuesEq({ID, NAME}, {3, "test3"})->first();

    QVERIFY(tag);
    QCOMPARE(tag->getAttribute(ID), QVariant(3));
    QCOMPARE(tag->getAttribute(NAME), QVariant("test3"));
}

/* Builds Queries */

void tst_Model_Connection_Independent::chunk() const
{
    using SizeType = QVector<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<int, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](const SizeType size, const int page)
    {
        QCOMPARE(size, expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunk(3, [&compareResultSize, &ids]
                             (QVector<FilePropertyProperty> &&models, const int page)
    {
        compareResultSize(models.size(), page);

        for (auto &&fileProperty : models)
            ids.emplace_back(fileProperty[ID]->template value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunk_ReturnFalse() const
{
    using SizeType = QVector<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount> (I leave it here also in this test, doesn't matter much)
    const std::unordered_map<int, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](const SizeType size, const int page)
    {
        QCOMPARE(size, expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunk(3, [&compareResultSize, &ids]
                             (QVector<FilePropertyProperty> &&models, const int page)
    {
        compareResultSize(models.size(), page);

        for (auto &&fileProperty : models) {
            auto id = fileProperty[ID]->template value<quint64>();
            ids.emplace_back(id);

            // Interrupt chunk-ing
            if (id == 5)
                return false;
        }

        return true;
    });

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunk_EnforceOrderBy() const
{
    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    using SizeType = QVector<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<int, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](const SizeType size, const int page)
    {
        QCOMPARE(size, expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::chunk(
                      3, [&compareResultSize, &ids]
                         (QVector<FilePropertyProperty> &&models, const int page)
    {
        compareResultSize(models.size(), page);

        for (auto &&fileProperty : models)
            ids.emplace_back(fileProperty[ID]->template value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunk_EmptyResult() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->orderBy(ID)
                  .chunk(3, [&callbackInvoked]
                            (QVector<FilePropertyProperty> &&/*unused*/,
                             const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Connection_Independent::each() const
{
    std::vector<int> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->each([&indexes, &ids]
                         (FilePropertyProperty &&model, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getAttribute(ID).template value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::each_ReturnFalse() const
{
    std::vector<int> indexes;
    indexes.reserve(5);
    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->each([&indexes, &ids](FilePropertyProperty &&model, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getAttribute(ID).template value<quint64>());

        return index != 4; // false/interrupt on 4
    });

    QVERIFY(!result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::each_EnforceOrderBy() const
{
    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    std::vector<int> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::each(
                      [&indexes, &ids](FilePropertyProperty &&model, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getAttribute(ID).template value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::each_EmptyResult() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->orderBy(ID)
                  .each([&callbackInvoked]
                        (FilePropertyProperty &&/*unused*/, const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

namespace
{
    /*! Used to compare results from the TinyBuilder::chunkMap() method for
        the FilePropertyProperty model. */
    struct IdAndName
    {
        /*! FilePropertyProperty ID. */
        quint64 id;
        /*! FilePropertyProperty name. */
        QString name;

        /*! Comparison operator for the IdAndName. */
        inline bool operator==(const IdAndName &right) const noexcept
        {
            return id == right.id && name == right.name;
        }
    };
} // namespace

void tst_Model_Connection_Independent::chunkMap() const
{
    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunkMap([](FilePropertyProperty &&model)
    {
        auto nameRef = model[NAME];

        // Modify the name attribute
        nameRef = QStringLiteral("%1_mapped").arg(nameRef->template value<QString>());

        return std::move(model);
    });

    QVector<IdAndName> expectedResult {
        {1, "test2_file1_property1_mapped"},
        {2, "test2_file2_property1_mapped"},
        {3, "test3_file1_property1_mapped"},
        {4, "test3_file1_property2_mapped"},
        {5, "test4_file1_property1_mapped"},
        {6, "test5_file1_property1_mapped"},
        {7, "test5_file1_property2_mapped"},
        {8, "test5_file1_property3_mapped"},
    };

    // Transform the result so we can compare it
    auto resultTransformed = result
            | ranges::views::transform([](const FilePropertyProperty &model)
                                       -> IdAndName
    {
        return {model.getAttribute(ID).value<quint64>(),
                model.getAttribute(NAME).value<QString>()};
    })
            | ranges::to<QVector<IdAndName>>();

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(resultTransformed, expectedResult);
}

void tst_Model_Connection_Independent::chunkMap_EnforceOrderBy() const
{
    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    auto result = FilePropertyProperty::chunkMap([](FilePropertyProperty &&model)
    {
        auto nameRef = model[NAME];

        // Modify the name attribute
        nameRef = QStringLiteral("%1_mapped").arg(nameRef->template value<QString>());

        return std::move(model);
    });

    QVector<IdAndName> expectedResult {
        {1, "test2_file1_property1_mapped"},
        {2, "test2_file2_property1_mapped"},
        {3, "test3_file1_property1_mapped"},
        {4, "test3_file1_property2_mapped"},
        {5, "test4_file1_property1_mapped"},
        {6, "test5_file1_property1_mapped"},
        {7, "test5_file1_property2_mapped"},
        {8, "test5_file1_property3_mapped"},
    };

    // Transform the result so I can compare it
    auto resultTransformed = result
            | ranges::views::transform([](const FilePropertyProperty &model)
                                       -> IdAndName
    {
        return {model.getAttribute(ID).value<quint64>(),
                model.getAttribute(NAME).value<QString>()};
    })
            | ranges::to<QVector<IdAndName>>();

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(resultTransformed, expectedResult);
}

void tst_Model_Connection_Independent::chunkMap_EmptyResult() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->chunkMap([&callbackInvoked]
                             (FilePropertyProperty &&model)
    {
        callbackInvoked = true;

        return std::move(model);
    });

    QVERIFY(!callbackInvoked);
    QVERIFY((std::is_same_v<decltype (result), QVector<FilePropertyProperty>>));
    QVERIFY(result.isEmpty());
}

void tst_Model_Connection_Independent::chunkMap_TemplatedReturnValue() const
{
    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunkMap<QString>([](FilePropertyProperty &&model)
    {
        // Return the modify name directly
        return QStringLiteral("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QVector<QString> expectedResult {
        {"test2_file1_property1_mapped"},
        {"test2_file2_property1_mapped"},
        {"test3_file1_property1_mapped"},
        {"test3_file1_property2_mapped"},
        {"test4_file1_property1_mapped"},
        {"test5_file1_property1_mapped"},
        {"test5_file1_property2_mapped"},
        {"test5_file1_property3_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void
tst_Model_Connection_Independent::chunkMap_EnforceOrderBy_TemplatedReturnValue() const
{
    /* The TinBuilder version doesn't throws exception if the 'order by' clause is not
       specified, instead it adds a generic 'order by' clause
       on the Model::getQualifiedKeyName() (it sorts by the primary key by default). */
    auto result = FilePropertyProperty::chunkMap<QString>(
                      [](FilePropertyProperty &&model)
    {
        // Return the modify name directly
        return QStringLiteral("%1_mapped").arg(model[NAME]->template value<QString>());
    });

    QVector<QString> expectedResult {
        {"test2_file1_property1_mapped"},
        {"test2_file2_property1_mapped"},
        {"test3_file1_property1_mapped"},
        {"test3_file1_property2_mapped"},
        {"test4_file1_property1_mapped"},
        {"test5_file1_property1_mapped"},
        {"test5_file1_property2_mapped"},
        {"test5_file1_property3_mapped"},
    };

    QVERIFY(expectedResult.size() == result.size());
    QCOMPARE(result, expectedResult);
}

void tst_Model_Connection_Independent::chunkMap_EmptyResult_TemplatedReturnValue() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->chunkMap<QString>([&callbackInvoked]
                                      (FilePropertyProperty &&/*unused*/)
                                      -> QString
    {
        callbackInvoked = true;

        return {};
    });

    QVERIFY(!callbackInvoked);
    QVERIFY((std::is_same_v<decltype (result), QVector<QString>>));
    QVERIFY(result.isEmpty());
}

void tst_Model_Connection_Independent::chunkById() const
{
    using SizeType = QVector<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<int, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](const SizeType size, const int page)
    {
        QCOMPARE(size, expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunkById(3, [&compareResultSize, &ids]
                                 (QVector<FilePropertyProperty> &&models, const int page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models)
            ids.emplace_back(tag.getAttribute(ID).value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunkById_ReturnFalse() const
{
    using SizeType = QVector<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount> (I leave it here also in this test, doesn't matter much
    const std::unordered_map<int, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](const SizeType size, const int page)
    {
        QCOMPARE(size, expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->chunkById(3, [&compareResultSize, &ids]
                                (QVector<FilePropertyProperty> &&models, const int page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            auto id = tag.getAttribute(ID).value<quint64>();
            ids.emplace_back(id);

            // Interrupt chunk-ing
            if (id == 5)
                return false;
        }

        return true;
    });

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunkById_EmptyResult() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->orderBy(ID)
                  .chunkById(3, [&callbackInvoked]
                                (QVector<FilePropertyProperty> &&/*unused*/,
                                 const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Connection_Independent::chunkById_WithAlias() const
{
    using SizeType = QVector<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount>
    const std::unordered_map<int, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](const SizeType size, const int page)
    {
        QCOMPARE(size, expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->orderBy(ID)
                  .chunkById(3, [&compareResultSize, &ids]
                                (QVector<FilePropertyProperty> &&models, const int page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models)
            ids.emplace_back(tag.getAttribute(ID).value<quint64>());

        return true;
    },
            ID, "id_as");

    QVERIFY(result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunkById_ReturnFalse_WithAlias() const
{
    using SizeType = QVector<FilePropertyProperty>::size_type;

    // <page, chunk_modelsCount> (I leave it here also in this test, doesn't matter much
    const std::unordered_map<int, SizeType> expectedRows {{1, 3}, {2, 3}, {3, 2}};

    /* Can't be inside the chunk's callback because QCOMPARE internally calls 'return;'
       and it causes compile error. */
    const auto compareResultSize = [&expectedRows](const SizeType size, const int page)
    {
        QCOMPARE(size, expectedRows.at(page));
    };

    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->orderBy(ID)
                  .chunkById(3, [&compareResultSize, &ids]
                                (QVector<FilePropertyProperty> &&models, const int page)
    {
        compareResultSize(models.size(), page);

        for (auto &&tag : models) {
            auto id = tag.getAttribute(ID).value<quint64>();
            ids.emplace_back(id);

            // Interrupt chunk-ing
            if (id == 5)
                return false;
        }

        return true;
    },
            ID, "id_as");

    QVERIFY(!result);

    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::chunkById_EmptyResult_WithAlias() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->whereEq(NAME, QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .chunkById(3, [&callbackInvoked]
                                (QVector<FilePropertyProperty> &&/*unused*/,
                                 const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    },
            ID, "id_as");

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Connection_Independent::eachById() const
{
    std::vector<int> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->eachById([&indexes, &ids]
                             (FilePropertyProperty &&model, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getAttribute(ID).value<quint64>());

        return true;
    });

    QVERIFY(result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::eachById_ReturnFalse() const
{
    std::vector<int> indexes;
    indexes.reserve(5);
    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::orderBy(ID)
                  ->eachById([&indexes, &ids]
                             (FilePropertyProperty &&model, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getAttribute(ID).value<quint64>());

        return index != 4; // false/interrupt on 4
    });

    QVERIFY(!result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::eachById_EmptyResult() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::whereEq(NAME,
                                                QStringLiteral("dummy-NON_EXISTENT"))
                  ->orderBy(ID)
                  .eachById([&callbackInvoked]
                            (FilePropertyProperty &&/*unused*/, const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    });

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Connection_Independent::eachById_WithAlias() const
{
    std::vector<int> indexes;
    indexes.reserve(8);
    std::vector<quint64> ids;
    ids.reserve(8);

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->orderBy(ID)
                  .eachById([&indexes, &ids]
                            (FilePropertyProperty &&model, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getAttribute(ID).value<quint64>());

        return true;
    },
            1000, ID, "id_as");

    QVERIFY(result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5, 6, 7, 8};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::eachById_ReturnFalse_WithAlias() const
{
    std::vector<int> indexes;
    indexes.reserve(5);
    std::vector<quint64> ids;
    ids.reserve(5);

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->orderBy(ID)
                  .eachById([&indexes, &ids]
                            (FilePropertyProperty &&model, const int index)
    {
        indexes.emplace_back(index);
        ids.emplace_back(model.getAttribute(ID).value<quint64>());

        return index != 4; // false/interrupt on 4
    },
            1000, ID, "id_as");

    QVERIFY(!result);

    std::vector<int> expectedIndexes {0, 1, 2, 3, 4};
    std::vector<quint64> expectedIds {1, 2, 3, 4, 5};

    QVERIFY(indexes.size() == expectedIndexes.size());
    QCOMPARE(indexes, expectedIndexes);
    QVERIFY(ids.size() == expectedIds.size());
    QCOMPARE(ids, expectedIds);
}

void tst_Model_Connection_Independent::eachById_EmptyResult_WithAlias() const
{
    auto callbackInvoked = false;

    auto result = FilePropertyProperty::select({ASTERISK, "id as id_as"})
                  ->whereEq(NAME, QStringLiteral("dummy-NON_EXISTENT"))
                  .orderBy(ID)
                  .eachById([&callbackInvoked]
                            (FilePropertyProperty &&/*unused*/, const int /*unused*/)
    {
        callbackInvoked = true;

        return true;
    },
            1000, ID, "id_as");

    QVERIFY(!callbackInvoked);
    QVERIFY(result);
}

void tst_Model_Connection_Independent::tap() const
{
    auto builder = FilePropertyProperty::query();

    auto callbackInvoked = false;
    auto &tappedBuilder = builder->tap([&callbackInvoked]
                                       (Orm::Tiny::Builder<FilePropertyProperty> &query)
    {
        callbackInvoked = true;

        return query;
    });

    QVERIFY((std::is_same_v<decltype (tappedBuilder), decltype (*builder)>));
    QVERIFY(callbackInvoked);
    // It must be the same TinyBuilder (the same memory address)
    QVERIFY(reinterpret_cast<uintptr_t>(&*builder)
            == reinterpret_cast<uintptr_t>(&tappedBuilder));
}

void tst_Model_Connection_Independent::sole() const
{
    auto model = FilePropertyProperty::whereEq(ID, 1)->sole();

    QCOMPARE(model[ID]->value<quint64>(), static_cast<quint64>(1));
    QCOMPARE(model[NAME]->value<QString>(), QString("test2_file1_property1"));
}

void tst_Model_Connection_Independent::sole_RecordsNotFoundError() const
{
    QVERIFY_EXCEPTION_THROWN(
            FilePropertyProperty::whereEq("name", "dummy-NON_EXISTENT")->sole(),
            RecordsNotFoundError);
}

void tst_Model_Connection_Independent::sole_MultipleRecordsFoundError() const
{
    QVERIFY_EXCEPTION_THROWN(
            FilePropertyProperty::whereEq("file_property_id", 5)->sole(),
            MultipleRecordsFoundError);
}

void tst_Model_Connection_Independent::sole_Pretending() const
{
    auto log = DB::connection(m_connection).pretend([]()
    {
        FilePropertyProperty::whereEq("name", "dummy-NON_EXISTENT")->sole();
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select * from `file_property_properties` where `name` = ? limit 2");
    QCOMPARE(firstLog.boundValues,
             QVector<QVariant>({QVariant(QString("dummy-NON_EXISTENT"))}));
}

void tst_Model_Connection_Independent::soleValue() const
{
    auto value = FilePropertyProperty::whereEq(ID, 1)->soleValue(NAME);

    QVERIFY((std::is_same_v<decltype (value), QVariant>));
    QVERIFY(value.isValid() && !value.isNull());
    QCOMPARE(value, QVariant(QString("test2_file1_property1")));
}

void tst_Model_Connection_Independent::soleValue_RecordsNotFoundError() const
{
    QVERIFY_EXCEPTION_THROWN(
            FilePropertyProperty::whereEq("name", "dummy-NON_EXISTENT")->soleValue(NAME),
            RecordsNotFoundError);
}

void tst_Model_Connection_Independent::soleValue_MultipleRecordsFoundError() const
{
    QVERIFY_EXCEPTION_THROWN(
            FilePropertyProperty::soleValue(NAME),
            MultipleRecordsFoundError);
}

void tst_Model_Connection_Independent::soleValue_Pretending() const
{
    auto log = DB::connection(m_connection).pretend([]()
    {
        FilePropertyProperty::whereEq("name", "dummy-NON_EXISTENT")->soleValue(NAME);
    });

    QVERIFY(!log.isEmpty());
    const auto &firstLog = log.first();

    QCOMPARE(log.size(), 1);
    QCOMPARE(firstLog.query,
             "select `name` from `file_property_properties` where `name` = ? limit 2");
    QCOMPARE(firstLog.boundValues,
             QVector<QVariant>({QVariant(QString("dummy-NON_EXISTENT"))}));
}

QTEST_MAIN(tst_Model_Connection_Independent)

#include "tst_model_connection_independent.moc"
