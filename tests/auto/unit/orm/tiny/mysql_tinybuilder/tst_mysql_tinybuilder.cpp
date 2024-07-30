#include <QCoreApplication>
#include <QTest>

#include "orm/db.hpp"
#include "orm/tiny/tinybuilder.hpp"
#include "orm/utils/nullvariant.hpp"

#include "databases.hpp"
#include "macros.hpp"

#include "models/torrent.hpp"

using Orm::Constants::AND;
using Orm::Constants::LIKE;
using Orm::Constants::NAME;
using Orm::Constants::OR;
using Orm::Constants::SIZE_;

using Orm::DB;
using Orm::Exceptions::InvalidArgumentError;
using Orm::QueryBuilder;
using Orm::TTimeZone;
using Orm::Utils::Helpers;
using Orm::Utils::NullVariant;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::Model;
using Orm::Tiny::TinyBuilder;

using TestUtils::Databases;

using Models::FilePropertyProperty;
using Models::Phone;
using Models::Role;
using Models::Tag;
using Models::TagProperty;
using Models::Torrent;
using Models::TorrentPeer;
using Models::TorrentPreviewableFile;
using Models::TorrentPreviewableFileProperty;
using Models::User;

class tst_MySql_TinyBuilder : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    /* Touching timestamps */
    void touch() const;
    void touch_CustomColumn() const;
    void touch_NotUsesTimestamps() const;

    /* Attributes - Unix timestamps - u_dateFormat = 'U' */
    void setAttribute_UnixTimestamp_With_UDates_UDateFormat() const;
    void setAttribute_UnixTimestamp_With_UDates_Without_UDateFormat() const;
    void setAttribute_UnixTimestamp_WithOut_UDates() const;

    void setAttribute_UnixTimestamp_With_UDates_UDateFormat_Null() const;
    void setAttribute_UnixTimestamp_With_UDates_Without_UDateFormat_Null() const;
    void setAttribute_UnixTimestamp_WithOut_UDates_Null() const;

    /* Querying Relationship Existence/Absence on HasMany */
    void has_Basic_OnHasMany() const;
    void has_Count_OnHasMany() const;

    void has_Basic_OnBelongsTo_WithSoftDeletes() const;
    void has_Count_OnBelongsToMany_WithSoftDeletes() const;

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

    /* SoftDeletes */
    void deletedAt_Column_WithoutJoins() const;
    void deletedAt_Column_WithJoins() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Create TinyBuilder instance for the given connection. */
    template<typename Model>
    std::unique_ptr<TinyBuilder<Model>> createTinyQuery() const;

    /*! Connection name used in this test case. */
    QString m_connection;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_MySql_TinyBuilder::initTestCase()
{
    /* No need to test with the MariaDB (MARIADB connection) because it will produce
       the same output in 99% cases. */
    m_connection = Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

/* Touching timestamps */

void tst_MySql_TinyBuilder::touch() const
{
    auto timeBeforeTouch = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeTouch.time();
        timeBeforeTouch.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto log = DB::connection(m_connection).pretend([this]
    {
        User::on(m_connection)
                ->whereEq("status", "new")
                .touch();
    });

    QCOMPARE(log.size(), 1);
    const auto &firstLog = log.first();

    QCOMPARE(firstLog.query,
             "update `users` "
             "set `updated_at` = ? "
             "where `status` = ? and `users`.`deleted_at` is null");
    QCOMPARE(firstLog.boundValues.size(), 2);
    QVERIFY(Helpers::setTimeZone(firstLog.boundValues.at(0).value<QDateTime>(),
                                 m_connection) >=
            timeBeforeTouch);
    QCOMPARE(firstLog.boundValues.at(1), QVariant(QString("new")));
}

void tst_MySql_TinyBuilder::touch_CustomColumn() const
{
    auto timeBeforeTouch = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeTouch.time();
        timeBeforeTouch.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    auto log = DB::connection(m_connection).pretend([this]
    {
        User::on(m_connection)
                ->whereEq("status", "new")
                .touch("updated_on");
    });

    QCOMPARE(log.size(), 1);
    const auto &firstLog = log.first();

    QCOMPARE(firstLog.query,
             "update `users` "
             "set `updated_on` = ? "
             "where `status` = ? and `users`.`deleted_at` is null");
    QCOMPARE(firstLog.boundValues.size(), 2);
    QVERIFY(Helpers::setTimeZone(firstLog.boundValues.at(0).value<QDateTime>(),
                                 m_connection) >=
            timeBeforeTouch);
    QCOMPARE(firstLog.boundValues.at(1), QVariant(QString("new")));
}

void tst_MySql_TinyBuilder::touch_NotUsesTimestamps() const
{
    int affected = -1;
    std::optional<TSqlQuery> query = std::nullopt;

    auto log = DB::connection(m_connection).pretend([this, &affected, &query]
    {
        std::tie(affected, query) = Phone::on(m_connection)
                                    ->whereEq("status", "new")
                                    .touch();
    });

    QVERIFY(log.isEmpty());
    QCOMPARE(affected, 0);
    QCOMPARE(query, std::nullopt);
}

/* Attributes - Unix timestamps - u_dateFormat = 'U' */

void tst_MySql_TinyBuilder::setAttribute_UnixTimestamp_With_UDates_UDateFormat() const
{
    // 2022-08-03 13:36:56 UTC
    qint64 timestamp = 1659533816;

    // QDateTime
    {
        Role role;
        role.setConnection(m_connection);
        role.setAttribute("added_on",
                          QDateTime::fromSecsSinceEpoch(timestamp, TTimeZone::UTC));

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        QCOMPARE(addedOn, QVariant(timestamp));
    }
    // qint64 - QVariant
    {
        Role role;
        role.setConnection(m_connection);
        role.setAttribute("added_on", timestamp);

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        QCOMPARE(addedOn, QVariant(timestamp));
    }
}

namespace
{
    /*! Role class to test without u_dates and u_dateFormat. */
    class Role_CustomUDate final : public Model<Role_CustomUDate> // NOLINT(misc-no-recursion)
    {
        friend Model;
        using Model::Model;

        /*! The table associated with the model. */
        QString u_table {"roles"};

        /*! Indicates whether the model should be timestamped. */
        bool u_timestamps = false;

    public:
        /*! The attributes that should be mutated to dates. */
        inline static QStringList u_dates {"added_on"}; // clazy:exclude=non-pod-global-static
    };
} // namespace

void tst_MySql_TinyBuilder::
     setAttribute_UnixTimestamp_With_UDates_Without_UDateFormat() const
{
    // 2022-08-03 13:36:56 UTC
    qint64 timestamp = 1659533816;
    // Prepare without u_dateFormat but with u_dates
    Role_CustomUDate::u_dates = QStringList {"added_on"};
    /* Expected format without u_dateFormat is - 2022-08-03 15:36:56 UTC, even if
       the input is the Unix timestamp. */
    auto expectedTimestamp = QDateTime::fromSecsSinceEpoch(timestamp, TTimeZone::UTC)
                             .toString(Role_CustomUDate()
                                       .setConnection(m_connection)
                                       .getDateFormat());

    // QDateTime
    {
        Role_CustomUDate role;
        role.setConnection(m_connection);
        role.setAttribute("added_on",
                          QDateTime::fromSecsSinceEpoch(timestamp, TTimeZone::UTC));

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        /* Here comes into the play the variant.typeId() == QDateTime in setAttribute()
           and special handling for pure number string in asDateTime(), so the QDateTime
           is autodetected. */
        QCOMPARE(addedOn, QVariant(expectedTimestamp));
    }
    // qint64 - QVariant
    {
        Role_CustomUDate role;
        role.setConnection(m_connection);
        role.setAttribute("added_on", timestamp);

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        /* Here comes into the play the variant.typeId() == QDateTime in setAttribute()
           and special handling for pure number string in asDateTime(), so the QDateTime
           is autodetected. */
        QCOMPARE(addedOn, QVariant(expectedTimestamp));
    }
}

void tst_MySql_TinyBuilder::setAttribute_UnixTimestamp_WithOut_UDates() const
{
    // 2022-08-03 13:36:56 UTC
    qint64 timestamp = 1659533816;
    // Prepare without u_dates and also without u_dateFormat
    Role_CustomUDate::u_dates.clear();

    // QDateTime
    {
        /* Expected format without u_dateFormat is - 2022-08-03 15:36:56 UTC, even if
           the input is the Unix timestamp. */
        auto expectedTimestamp = QDateTime::fromSecsSinceEpoch(timestamp, TTimeZone::UTC)
                                 .toString(Role_CustomUDate()
                                           .setConnection(m_connection)
                                           .getDateFormat());

        Role_CustomUDate role;
        role.setConnection(m_connection);
        role.setAttribute("added_on",
                          QDateTime::fromSecsSinceEpoch(timestamp, TTimeZone::UTC));

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        /* Here comes into the play the variant.typeId() == QDateTime in setAttribute()
           and special handling for pure number string in asDateTime(), so the QDateTime
           is autodetected. */
        QCOMPARE(addedOn, QVariant(expectedTimestamp));
    }
    // qint64 - QVariant
    {
        Role_CustomUDate role;
        role.setConnection(m_connection);
        role.setAttribute("added_on", timestamp);

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        QCOMPARE(addedOn, QVariant(timestamp));
    }
}

void tst_MySql_TinyBuilder::
     setAttribute_UnixTimestamp_With_UDates_UDateFormat_Null() const
{
    // Because the u_dateFormat = 'U' the result type will be qint64

    // QDateTime
    {
        Role role;
        role.setConnection(m_connection);
        role.setAttribute("added_on", NullVariant::QDateTime());

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        QCOMPARE(addedOn, NullVariant::LongLong());
    }
    // qint64 - QVariant
    {
        Role role;
        role.setConnection(m_connection);
        role.setAttribute("added_on", NullVariant::LongLong());

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        QCOMPARE(addedOn, NullVariant::LongLong());
    }
}

void tst_MySql_TinyBuilder::
     setAttribute_UnixTimestamp_With_UDates_Without_UDateFormat_Null() const
{
    // Prepare without u_dateFormat but with u_dates
    Role_CustomUDate::u_dates = QStringList {"added_on"};

    /* As the added_on is in u_dates it autodetects QDateTime values even if the string
       will be with all chars as numbers. */

    // QDateTime
    {
        Role_CustomUDate role;
        role.setConnection(m_connection);
        role.setAttribute("added_on", NullVariant::QDateTime());

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        QCOMPARE(addedOn, NullVariant::QDateTime());
    }
    // qint64 - QVariant
    {
        Role_CustomUDate role;
        role.setConnection(m_connection);
        role.setAttribute("added_on", NullVariant::LongLong());

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        /* Here comes into the play the variant.typeId() == QDateTime in setAttribute()
           and special handling for pure number string in asDateTime(), so the QDateTime
           is autodetected. */
        QCOMPARE(addedOn, NullVariant::QDateTime());
    }
}

void tst_MySql_TinyBuilder::setAttribute_UnixTimestamp_WithOut_UDates_Null() const
{
    // Prepare without u_dates and also without u_dateFormat
    Role_CustomUDate::u_dates.clear();

    // Whatever is passed to the setAttribute() will be saved into the storage

    // QDateTime
    {
        Role_CustomUDate role;
        role.setConnection(m_connection);
        role.setAttribute("added_on", NullVariant::QDateTime());

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        QCOMPARE(addedOn, NullVariant::QDateTime());
    }
    // qint64 - QVariant
    {
        Role_CustomUDate role;
        role.setConnection(m_connection);
        role.setAttribute("added_on", NullVariant::LongLong());

        const auto &attributes = role.getAttributes();

        QCOMPARE(attributes.size(), 1);

        const auto &[_, addedOn] = attributes.constFirst();
        QCOMPARE(addedOn, NullVariant::LongLong());
    }
}

/* Querying Relationship Existence/Absence on HasMany */

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

void tst_MySql_TinyBuilder::has_Basic_OnBelongsTo_WithSoftDeletes() const
{
    auto builder = createTinyQuery<Phone>();

    builder->has("user");

    QCOMPARE(builder->toSql(),
             "select * from `user_phones` where exists "
               "(select * from `users` "
               "where `user_phones`.`user_id` = `users`.`id` and "
                 "`users`.`deleted_at` is null)");
    QVERIFY(builder->getBindings().isEmpty());
}

void tst_MySql_TinyBuilder::has_Count_OnBelongsToMany_WithSoftDeletes() const
{
    auto builder = createTinyQuery<Role>();

    builder->has("users", ">", 3);

    QCOMPARE(builder->toSql(),
             "select * from `roles` where "
               "(select count(*) from `users` "
                 "inner join `role_user` on `users`.`id` = `role_user`.`user_id` "
                 "where `roles`.`id` = `role_user`.`role_id` and "
                   "`users`.`deleted_at` is null) > 3");
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
             QList<QVariant>({QVariant("%_file2.mkv")}));
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
             QList<QVariant>({QVariant("%_file2.mkv")}));
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
             QList<QVariant>({QVariant("%_file2.mkv")}));
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
             QList<QVariant>({QVariant("%_file2.mkv")}));
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
             QList<QVariant>({QVariant("%_file2.mkv")}));
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
             QList<QVariant>({QVariant("%_file2.mkv")}));
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
             QList<QVariant>({QVariant("%_file2.mkv")}));
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
                 QList<QVariant>({QVariant(6)}));
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
                 QList<QVariant>({QVariant(6)}));
    }
}

void tst_MySql_TinyBuilder::
     whereHas_hasNested_Basic_TinyBuilder_WhereHasInCallback_OnHasMany() const
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

            query1.where(SIZE_, ">", 1);
        });
    });

    auto builder2 = createTinyQuery<Torrent>();

    builder2->whereHas<TorrentPreviewableFileProperty>("torrentFiles.fileProperty",
                                                       [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query),
                 TinyBuilder<TorrentPreviewableFileProperty> &>));

        query.where(SIZE_, ">", 1);
    });

    QCOMPARE(builder1->toSql(), builder2->toSql());
    QCOMPARE(builder1->getBindings(),
             QList<QVariant>({QVariant(1)}));
    QCOMPARE(builder2->getBindings(),
             QList<QVariant>({QVariant(1)}));
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

    TVERIFY_THROWS_EXCEPTION(InvalidArgumentError,
                             builder->has("torrentFiles.fileProperty"));
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

/* Querying Relationship Absence on HasMany */

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
             QList<QVariant>({QVariant(5)}));
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

/* Querying Relationship Existence/Absence on BelongsTo */

void tst_MySql_TinyBuilder::has_QueryBuilder_OnBelongsTo() const
{
    auto builder = createTinyQuery<TorrentPreviewableFile>();

    builder->has("torrent", ">=", 1, AND, [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where(SIZE_, ">", 5);
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrent_previewable_files` where exists "
               "(select * from `torrents` "
               "where `torrent_previewable_files`.`torrent_id` = `torrents`.`id` "
                 "and `size` > ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant(5)}));
}

void tst_MySql_TinyBuilder::hasNested_Count_TinyBuilder_OnBelongsTo_NestedAsLast() const
{
    /* Also following tests can be counted as part of BelongsTo tests:
      'hasNested_Count_OnBelongsToMany_NestedAsLast' has BelongsTo type relation as
      first relation in hasNested() call - 'torrent.tags' ( torrent
      belongs-to torrentPeer ).
      'hasNested_Count_OnBelongsToMany_NestedInMiddle' has BelongsTo type relation as
      first relation in hasNested() call - 'torrent.tags.tagProperty' ( torrent
      belongs-to torrentPeer ).
      This note has nothing to do with the this/current test method. */

    auto builder = createTinyQuery<TorrentPreviewableFileProperty>();

    builder->has<Torrent>("torrentFile.torrent", ">=", 2, AND,
                          [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), TinyBuilder<Torrent> &>));

        query.where(SIZE_, ">", 6);
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
             QList<QVariant>({QVariant(6)}));
}

/* Querying Relationship Existence/Absence on BelongsToMany */

void tst_MySql_TinyBuilder::has_Basic_TinyBuilder_OnBelongsToMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has<Tag>("tags", ">=", 1, AND, [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), TinyBuilder<Tag> &>));

        query.where(NAME, LIKE, "tag%");
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where exists "
               "(select * from `torrent_tags` "
               "inner join `tag_torrent` "
                 "on `torrent_tags`.`id` = `tag_torrent`.`tag_id` "
               "where `torrents`.`id` = `tag_torrent`.`torrent_id` "
                 "and `name` like ?)");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant("tag%")}));
}

void tst_MySql_TinyBuilder::has_Count_QueryBuilder_OnBelongsToMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has("tags", ">=", 2, AND, [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), QueryBuilder &>));

        query.where(NAME, LIKE, "tag%");
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where "
               "(select count(*) from `torrent_tags` "
               "inner join `tag_torrent` "
                 "on `torrent_tags`.`id` = `tag_torrent`.`tag_id` "
               "where `torrents`.`id` = `tag_torrent`.`torrent_id` "
                 "and `name` like ?) >= 2");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant("tag%")}));
}

void tst_MySql_TinyBuilder::has_Count_TinyBuilder_OnBelongsToMany() const
{
    auto builder = createTinyQuery<Torrent>();

    builder->has<Tag>("tags", ">=", 2, AND, [](auto &query)
    {
        QVERIFY((std::is_same_v<decltype (query), TinyBuilder<Tag> &>));

        query.where(NAME, LIKE, "tag%");
    });

    QCOMPARE(builder->toSql(),
             "select * from `torrents` where "
               "(select count(*) from `torrent_tags` "
               "inner join `tag_torrent` "
                 "on `torrent_tags`.`id` = `tag_torrent`.`tag_id` "
               "where `torrents`.`id` = `tag_torrent`.`torrent_id` "
                 "and `name` like ?) >= 2");
    QCOMPARE(builder->getBindings(),
             QList<QVariant>({QVariant("tag%")}));
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
             QList<QVariant>({QVariant(6)}));
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
             QList<QVariant>({QVariant(1)}));
}

/* SoftDeletes */

void tst_MySql_TinyBuilder::deletedAt_Column_WithoutJoins() const
{
    // deleted_at column will be unqualified
    auto log = DB::connection(m_connection).pretend([this]
    {
        User::on(m_connection)->whereKey(3).remove();
    });

    QCOMPARE(log.size(), 1);
    const auto &firstLog = log.first();

    QCOMPARE(firstLog.query,
             "update `users` set `deleted_at` = ?, `users`.`updated_at` = ? "
             "where `users`.`id` = ? and `users`.`deleted_at` is null");
    QCOMPARE(firstLog.boundValues.size(), 3);
}

void tst_MySql_TinyBuilder::deletedAt_Column_WithJoins() const
{
    // deleted_at column will be qualified if joins are in the game
    auto log = DB::connection(m_connection).pretend([this]
    {
        User::on(m_connection)
                ->join("contacts", "users.id", "=", "contacts.user_id")
                .whereKey(3).remove();
    });

    QCOMPARE(log.size(), 1);
    const auto &firstLog = log.first();

    QCOMPARE(firstLog.query,
             "update `users` "
             "inner join `contacts` on `users`.`id` = `contacts`.`user_id` "
             "set `users`.`deleted_at` = ?, `users`.`updated_at` = ? "
             "where `users`.`id` = ? and `users`.`deleted_at` is null");
    QCOMPARE(firstLog.boundValues.size(), 3);
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

template<typename Model>
std::unique_ptr<TinyBuilder<Model>>
tst_MySql_TinyBuilder::createTinyQuery() const
{
    return Model().on(m_connection);
}

QTEST_MAIN(tst_MySql_TinyBuilder)

#include "tst_mysql_tinybuilder.moc"
