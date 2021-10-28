#include <QCoreApplication>
#include <QtTest>

#include "orm/tiny/tinybuilder.hpp"

#include "models/torrent.hpp"

#include "databases.hpp"

using Orm::Constants::AND;
using Orm::Constants::LIKE;
using Orm::Constants::OR;
using Orm::Exceptions::InvalidArgumentError;
using Orm::QueryBuilder;
using Orm::Tiny::TinyBuilder;

using TestUtils::Databases;

class tst_MySql_TinyBuilder : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    /* Querying Relationship Existence/Absence on HasMany */
    void has_Basic_OnHasMany() const;
    void has_Count_OnHasMany() const;

    void whereHas_Basic_QueryBuilder_OnHasMany() const;
    void whereHas_Basic_TinyBuilder_OnHasMany() const;
    void whereHas_Count_QueryBuilder_OnHasMany() const;
    void orWhereHas_Count_QueryBuilder_OnHasMany() const;
    void orWhereHas_Count_TinyBuilder_OnHasMany() const;
    void orWhereDoesntHave_Count_QueryBuilder_OnHasMany() const;
    void orWhereDoesntHave_Count_TinyBuilder_OnHasMany() const;

    void hasNested_Basic_OnHasMany() const;
    void hasNested_Count_OnHasMany() const;
    void hasNested_Count_TinyBuilder_OnHasMany() const;

    void whereHas_hasNested_Basic_TinyBuilder_WhereHasInCallback_OnHasMany() const;
    void whereHas_hasNested_Basic_HasInCallback_OnHasMany() const;
    void whereHas_orHasNested_Basic_HasInCallbacks_OnHasMany() const;

    void has_UnsupportedHasNested_Failed() const;

    void orHas_Basic_OnHasMany() const;
    void orHas_Count_OnHasMany() const;

    /* Querying Relationship Absence on HasMany */
    void doesntHave_Basic_OnHasMany() const;
    void doesntHave_Callback_OnHasMany() const;

    void orDoesntHave_OnHasMany() const;

    /* Querying Relationship Existence/Absence on BelongsTo */
    void has_QueryBuilder_OnBelongsTo() const;

    void hasNested_Count_TinyBuilder_OnBelongsTo_NestedAsLast() const;

    /* Querying Relationship Existence/Absence on BelongsToMany */
    void has_Basic_TinyBuilder_OnBelongsToMany() const;

    void has_Count_QueryBuilder_OnBelongsToMany() const;
    void has_Count_TinyBuilder_OnBelongsToMany() const;

    void hasNested_Count_TinyBuilder_OnBelongsToMany_NestedAsLast() const;
    void hasNested_Count_TinyBuilder_OnBelongsToMany_NestedInMiddle() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Create TinyBuilder instance for the given connection. */
    template<typename Model>
    std::unique_ptr<TinyBuilder<Model>> createTinyQuery() const;

    /*! Connection name used in this test case. */
    QString m_connection = {};
};

void tst_MySql_TinyBuilder::initTestCase()
{
    m_connection = Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(QStringLiteral("%1 autotest skipped, environment variables "
                             "for '%2' connection have not been defined.")
              .arg("tst_MySql_TinyBuilder", Databases::MYSQL).toUtf8().constData(), );
}

void tst_MySql_TinyBuilder::has_Basic_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has("torrentFiles");

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id`)");
    QVERIFY(builder->getBindings().isEmpty());
}

void tst_MySql_TinyBuilder::has_Count_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has("torrentFiles", ">", 3);

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where "
               "(select count(*) from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id`) > 3");
    QVERIFY(builder->getBindings().isEmpty());
}

void tst_MySql_TinyBuilder::whereHas_Basic_QueryBuilder_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->whereHas("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                 "and `filepath` like ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant("%_file2.mkv")}));
}

void tst_MySql_TinyBuilder::whereHas_Basic_TinyBuilder_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->whereHas<TorrentPreviewableFile>("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<TorrentPreviewableFile> &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                 "and `filepath` like ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant("%_file2.mkv")}));
}

void tst_MySql_TinyBuilder::whereHas_Count_QueryBuilder_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->whereHas("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    }, ">", 5);

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where "
               "(select count(*) from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                 "and `filepath` like ?) > 5");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant("%_file2.mkv")}));
}

void tst_MySql_TinyBuilder::orWhereHas_Count_QueryBuilder_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->whereHas("torrentPeer")
            .orWhereHas("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    }, ">", 5);

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_peers` "
               "where `torrents`.`id` = `torrent_peers`.`torrent_id`) "
             "or "
               "(select count(*) from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                 "and `filepath` like ?) > 5");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant("%_file2.mkv")}));
}

void tst_MySql_TinyBuilder::orWhereHas_Count_TinyBuilder_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->whereHas("torrentPeer")
            .orWhereHas<TorrentPreviewableFile>("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<TorrentPreviewableFile> &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    }, ">", 5);

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_peers` "
               "where `torrents`.`id` = `torrent_peers`.`torrent_id`) "
             "or "
               "(select count(*) from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                 "and `filepath` like ?) > 5");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant("%_file2.mkv")}));
}

void tst_MySql_TinyBuilder::orWhereDoesntHave_Count_QueryBuilder_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->whereHas("torrentPeer")
            .orWhereDoesntHave("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_peers` "
               "where `torrents`.`id` = `torrent_peers`.`torrent_id`) "
             "or not exists "
               "(select * from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                 "and `filepath` like ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant("%_file2.mkv")}));
}

void tst_MySql_TinyBuilder::orWhereDoesntHave_Count_TinyBuilder_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->whereHas("torrentPeer")
            .orWhereDoesntHave<TorrentPreviewableFile>("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                                TinyBuilder<TorrentPreviewableFile> &>));

        query.where("filepath", LIKE, "%_file2.mkv");
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_peers` "
               "where `torrents`.`id` = `torrent_peers`.`torrent_id`) "
             "or not exists "
               "(select * from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                 "and `filepath` like ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant("%_file2.mkv")}));
}

void tst_MySql_TinyBuilder::hasNested_Basic_OnHasMany() const
{
    // Single nesting
    {
        auto builder = createTinyQuery<Torrent>();

        builder->has<TorrentPreviewableFileProperty>("torrentFiles.fileProperty");

        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where exists "
                   "(select * from `torrent_previewable_files` "
                   "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                     "and exists "
                     "(select * from `torrent_previewable_file_properties` "
                     "where `torrent_previewable_files`.`id` = "
                       "`torrent_previewable_file_properties`.`previewable_file_id`))");
        QVERIFY(builder->getBindings().isEmpty());
    }

    // Double nesting
    {
        auto builder = createTinyQuery<Torrent>();

        builder->has<FilePropertyProperty>(
                    "torrentFiles.fileProperty.filePropertyProperty");

        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where exists "
                   "(select * from `torrent_previewable_files` "
                   "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                     "and exists "
                       "(select * from `torrent_previewable_file_properties` "
                       "where `torrent_previewable_files`.`id` = "
                         "`torrent_previewable_file_properties`.`previewable_file_id` "
                         "and exists "
                           "(select * from `file_property_properties` "
                           "where `torrent_previewable_file_properties`.`id` = "
                             "`file_property_properties`.`file_property_id`)))");
        QVERIFY(builder->getBindings().isEmpty());
    }
}

void tst_MySql_TinyBuilder::hasNested_Count_OnHasMany() const
{
    // Single nesting
    {
        auto builder = createTinyQuery<Torrent>();

        builder->has<TorrentPreviewableFileProperty>("torrentFiles.fileProperty",
                                                     ">", 2);

        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where exists "
                   "(select * from `torrent_previewable_files` "
                   "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                     "and "
                       "(select count(*) from `torrent_previewable_file_properties` "
                       "where `torrent_previewable_files`.`id` = "
                         "`torrent_previewable_file_properties`.`previewable_file_id`) "
                           "> 2)");
        QVERIFY(builder->getBindings().isEmpty());
    }

    // Double nesting
    {
        auto builder = createTinyQuery<Torrent>();

        builder->has<FilePropertyProperty>(
                    "torrentFiles.fileProperty.filePropertyProperty", ">=", 5);

        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where exists "
                   "(select * from `torrent_previewable_files` "
                   "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                     "and exists "
                       "(select * from `torrent_previewable_file_properties` "
                       "where `torrent_previewable_files`.`id` = "
                         "`torrent_previewable_file_properties`.`previewable_file_id` "
                         "and "
                           "(select count(*) from `file_property_properties` "
                           "where `torrent_previewable_file_properties`.`id` = "
                             "`file_property_properties`.`file_property_id`) >= 5))");
        QVERIFY(builder->getBindings().isEmpty());
    }
}

void tst_MySql_TinyBuilder::hasNested_Count_TinyBuilder_OnHasMany() const
{
    // Single nesting
    {
        auto builder = createTinyQuery<Torrent>();

        builder->has<TorrentPreviewableFileProperty>("torrentFiles.fileProperty",
                                                     ">", 2, AND,
                                                     [](auto &query)
        {
            QVERIFY((std::is_same_v<decltype (query),
                     TinyBuilder<TorrentPreviewableFileProperty> &>));

            query.where("value", ">=", 6);
        });

        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where exists "
                   "(select * from `torrent_previewable_files` "
                   "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                     "and "
                       "(select count(*) from `torrent_previewable_file_properties` "
                       "where `torrent_previewable_files`.`id` = "
                         "`torrent_previewable_file_properties`.`previewable_file_id` "
                         "and `value` >= ?) > 2)");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(6)}));
    }

    // Double nesting
    {
        auto builder = createTinyQuery<Torrent>();

        builder->has<FilePropertyProperty>(
                    "torrentFiles.fileProperty.filePropertyProperty", ">=", 5, AND,
                    [](auto &query)
        {
            QVERIFY((std::is_same_v<decltype (query),
                     TinyBuilder<FilePropertyProperty> &>));

            query.where("value", ">=", 6);
        });

        QCOMPARE(builder->toSql(),
                 "select * from `torrents` where exists "
                   "(select * from `torrent_previewable_files` "
                   "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id` "
                     "and exists "
                       "(select * from `torrent_previewable_file_properties` "
                       "where `torrent_previewable_files`.`id` = "
                         "`torrent_previewable_file_properties`.`previewable_file_id` "
                         "and "
                           "(select count(*) from `file_property_properties` "
                           "where `torrent_previewable_file_properties`.`id` = "
                             "`file_property_properties`.`file_property_id` "
                             "and `value` >= ?) >= 5))");
        QCOMPARE(builder->getBindings(),
                 QVector<QVariant>({QVariant(6)}));
    }
}

void tst_MySql_TinyBuilder
     ::whereHas_hasNested_Basic_TinyBuilder_WhereHasInCallback_OnHasMany() const
{
    auto builder1 = createTinyQuery<Torrent>();

    builder1->whereHas<TorrentPreviewableFile>("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                 TinyBuilder<TorrentPreviewableFile> &>));

        query.template whereHas<TorrentPreviewableFileProperty>(
                    "fileProperty", [](auto &query1)
        {
            QVERIFY((std::is_same_v<decltype (query1),
                     TinyBuilder<TorrentPreviewableFileProperty> &>));

            query1.where("size", ">", 1);
        });
    });

    auto builder2 = createTinyQuery<Torrent>();

    builder2->whereHas<TorrentPreviewableFileProperty>("torrentFiles.fileProperty",
                                                       [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                 TinyBuilder<TorrentPreviewableFileProperty> &>));

        query.where("size", ">", 1);
    });

    QCOMPARE(builder1->toSql(), builder2->toSql());
    QCOMPARE(builder1->getBindings(),
             QVector<QVariant>({QVariant(1)}));
    QCOMPARE(builder2->getBindings(),
             QVector<QVariant>({QVariant(1)}));
}

void tst_MySql_TinyBuilder::whereHas_hasNested_Basic_HasInCallback_OnHasMany() const
{
    auto builder1 = createTinyQuery<Torrent>();

    builder1->whereHas<TorrentPreviewableFile>("torrentFiles", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                 TinyBuilder<TorrentPreviewableFile> &>));

        query.has("fileProperty");
    });

    auto builder2 = createTinyQuery<Torrent>();

    builder2->has<TorrentPreviewableFileProperty>("torrentFiles.fileProperty");

    QCOMPARE(builder1->toSql(), builder2->toSql());
    QVERIFY(builder1->getBindings().isEmpty());
    QVERIFY(builder2->getBindings().isEmpty());
}

void tst_MySql_TinyBuilder::whereHas_orHasNested_Basic_HasInCallbacks_OnHasMany() const
{
    auto builder1 = createTinyQuery<TorrentPeer>();

    builder1->whereHas<Torrent>("torrent", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), TinyBuilder<Torrent> &>));

        query.has("torrentFiles");
    })
            .orWhereHas<Torrent>("torrent", [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), TinyBuilder<Torrent> &>));

        query.has("tags");
    });

    auto builder2 = createTinyQuery<TorrentPeer>();

    builder2->has<TorrentPreviewableFile>("torrent.torrentFiles")
            .orHas<Tag>("torrent.tags");

    QCOMPARE(builder1->toSql(), builder2->toSql());
    QVERIFY(builder1->getBindings().isEmpty());
    QVERIFY(builder2->getBindings().isEmpty());
}

void tst_MySql_TinyBuilder::has_UnsupportedHasNested_Failed() const
{
    auto builder = createTinyQuery<Torrent>();

    QVERIFY_EXCEPTION_THROWN(builder->has("torrentFiles.fileProperty"),
                             InvalidArgumentError);
}

void tst_MySql_TinyBuilder::orHas_Basic_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has("torrentPeer")
            .orHas("torrentFiles");

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_peers` "
               "where `torrents`.`id` = `torrent_peers`.`torrent_id`) "
                 "or exists "
                   "(select * from `torrent_previewable_files` "
                   "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id`)");
    QVERIFY(builder->getBindings().isEmpty());
}

void tst_MySql_TinyBuilder::orHas_Count_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has("torrentPeer")
            .orHas("torrentFiles", ">", 2);

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_peers` "
               "where `torrents`.`id` = `torrent_peers`.`torrent_id`) or "
                 "(select count(*) from `torrent_previewable_files` "
                 "where `torrents`.`id` = "
                   "`torrent_previewable_files`.`torrent_id`) > 2");
    QVERIFY(builder->getBindings().isEmpty());
}

void tst_MySql_TinyBuilder::doesntHave_Basic_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->doesntHave("torrentFiles");

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where not exists "
               "(select * from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id`)");
    QVERIFY(builder->getBindings().isEmpty());
}

void tst_MySql_TinyBuilder::doesntHave_Callback_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has("torrentFiles")
            .doesntHave("torrentPeer", OR, [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where("total_seeds", ">", 5);
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id`) "
                 "or not exists "
                   "(select * from `torrent_peers` "
                   "where `torrents`.`id` = `torrent_peers`.`torrent_id` "
                     "and `total_seeds` > ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(5)}));
}

void tst_MySql_TinyBuilder::orDoesntHave_OnHasMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has("torrentFiles")
            .orDoesntHave("torrentPeer");

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_previewable_files` "
               "where `torrents`.`id` = `torrent_previewable_files`.`torrent_id`) "
                 "or not exists "
                   "(select * from `torrent_peers` "
                   "where `torrents`.`id` = `torrent_peers`.`torrent_id`)");
    QVERIFY(builder->getBindings().isEmpty());
}

void tst_MySql_TinyBuilder::has_QueryBuilder_OnBelongsTo() const
{
    auto builder = createTinyQuery<TorrentPreviewableFile>();

    builder->has("torrent", ">=", 1, AND, [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where("size", ">", 5);
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_files` where exists "
               "(select * from `torrents` "
               "where `torrent_previewable_files`.`torrent_id` = `torrents`.`id` "
                 "and `size` > ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(5)}));
}

void tst_MySql_TinyBuilder::hasNested_Count_TinyBuilder_OnBelongsTo_NestedAsLast() const
{
    /* Also following tests can be counted as part of BelongsTo tests:
      'hasNested_Count_OnBelongsToMany_NestedAsLast' has BelongsTo type relation as
      first relation in hasNested() call - 'torrent.tags' ( torrent
      belongs to torrentPeer ).
      'hasNested_Count_OnBelongsToMany_NestedInMiddle' has BelongsTo type relation as
      first relation in hasNested() call - 'torrent.tags.tagProperty' ( torrent
      belongs to torrentPeer ).
      This note has nothing to do with the this/current test method. */

    auto builder = createTinyQuery<TorrentPreviewableFileProperty>();

    builder->has<Torrent>("torrentFile.torrent", ">=", 2, AND,
                          [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), TinyBuilder<Torrent> &>));

        query.where("size", ">", 6);
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_file_properties` where exists "
               "(select * from `torrent_previewable_files` "
               "where `torrent_previewable_file_properties`.`previewable_file_id` "
                 "= `torrent_previewable_files`.`id` "
                 "and "
                   "(select count(*) from `torrents` "
                   "where `torrent_previewable_files`.`torrent_id` = `torrents`.`id` "
                     "and `size` > ?) >= 2)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(6)}));
}

void tst_MySql_TinyBuilder::has_Basic_TinyBuilder_OnBelongsToMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has<Tag>("tags", ">=", 1, AND, [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), TinyBuilder<Tag> &>));

        query.where("name", LIKE, "tag%");
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_tags` "
               "inner join `tag_torrent` "
                 "on `torrent_tags`.`id` = `tag_torrent`.`tag_id` "
               "where `torrents`.`id` = `tag_torrent`.`torrent_id` "
                 "and `name` like ?)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant("tag%")}));
}

void tst_MySql_TinyBuilder::has_Count_QueryBuilder_OnBelongsToMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has("tags", ">=", 2, AND, [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where("name", LIKE, "tag%");
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where "
               "(select count(*) from `torrent_tags` "
               "inner join `tag_torrent` "
                 "on `torrent_tags`.`id` = `tag_torrent`.`tag_id` "
               "where `torrents`.`id` = `tag_torrent`.`torrent_id` "
                 "and `name` like ?) >= 2");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant("tag%")}));
}

void tst_MySql_TinyBuilder::has_Count_TinyBuilder_OnBelongsToMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has<Tag>("tags", ">=", 2, AND, [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), TinyBuilder<Tag> &>));

        query.where("name", LIKE, "tag%");
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where "
               "(select count(*) from `torrent_tags` "
               "inner join `tag_torrent` "
                 "on `torrent_tags`.`id` = `tag_torrent`.`tag_id` "
               "where `torrents`.`id` = `tag_torrent`.`torrent_id` "
                 "and `name` like ?) >= 2");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant("tag%")}));
}

void
tst_MySql_TinyBuilder::hasNested_Count_TinyBuilder_OnBelongsToMany_NestedAsLast() const
{
    auto builder = createTinyQuery<TorrentPeer>();

    builder->has<Tag>("torrent.tags", ">=", 2, AND,
                      [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), TinyBuilder<Tag> &>));

        query.where("value", ">=", 6);
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrent_peers` where exists "
               "(select * from `torrents` "
               "where `torrent_peers`.`torrent_id` = `torrents`.`id` "
                 "and "
                   "(select count(*) from `torrent_tags` "
                   "inner join `tag_torrent` "
                     "on `torrent_tags`.`id` = `tag_torrent`.`tag_id` "
                   "where `torrents`.`id` = `tag_torrent`.`torrent_id` "
                     "and `value` >= ?) >= 2)");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(6)}));
}

void tst_MySql_TinyBuilder::hasNested_Count_TinyBuilder_OnBelongsToMany_NestedInMiddle() const
{
    auto builder = createTinyQuery<TorrentPeer>();

    builder->has<TagProperty>("torrent.tags.tagProperty", ">=", 2, AND,
                              [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), TinyBuilder<TagProperty> &>));

        query.where("position", ">=", 1);
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrent_peers` where exists "
               "(select * from `torrents` "
               "where `torrent_peers`.`torrent_id` = `torrents`.`id` "
                 "and exists "
                   "(select * from `torrent_tags` "
                   "inner join `tag_torrent` "
                     "on `torrent_tags`.`id` = `tag_torrent`.`tag_id` "
                   "where `torrents`.`id` = `tag_torrent`.`torrent_id` "
                     "and "
                       "(select count(*) from `tag_properties` "
                       "where `torrent_tags`.`id` = `tag_properties`.`tag_id` "
                         "and `position` >= ?) >= 2))");
    QCOMPARE(builder->getBindings(),
             QVector<QVariant>({QVariant(1)}));
}

template<typename Model>
std::unique_ptr<TinyBuilder<Model>>
tst_MySql_TinyBuilder::createTinyQuery() const
{
    return Model().on(m_connection);
}

QTEST_MAIN(tst_MySql_TinyBuilder)

#include "tst_mysql_tinybuilder.moc"
