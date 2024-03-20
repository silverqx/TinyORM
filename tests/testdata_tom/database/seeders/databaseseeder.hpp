#pragma once

#include <tom/seeder.hpp>

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

namespace Seeders
{

    /*! Main database seeder. */
    struct DatabaseSeeder : Seeder
    {
        /*! Run the database seeders. */
        void run() override
        {
            DB::table("users")->insert({ID, NAME, "is_banned", NOTE, CREATED_AT, UPDATED_AT, DELETED_AT},
            {
                {1, "andrej", false, NullVariant::QString(), "2022-01-01 14:51:23", "2022-01-01 17:46:31", NullVariant::QDateTime()},
                {2, "silver", false, NullVariant::QString(), "2022-01-02 14:51:23", "2022-01-02 17:46:31", NullVariant::QDateTime()},
                {3, "peter",  true,  "no torrents no roles", "2022-01-03 14:51:23", "2022-01-03 17:46:31", NullVariant::QDateTime()},
                {4, "jack",   false, "test SoftDeletes",     "2022-01-04 14:51:23", "2022-01-04 17:46:31", "2022-01-04 20:46:31"},
                {5, "obiwan", true,  "test SoftDeletes",     "2022-01-05 14:51:23", "2022-01-05 17:46:31", "2022-01-05 20:46:31"},
            });

            DB::table("roles")->insert({ID, NAME, "added_on"},
            {
                {1, "role one",   1659361016},
                {2, "role two",   1659447416},
                {3, "role three", NullVariant::ULongLong()},
            });

            DB::table("role_user")->insert({"role_id", "user_id", "active"},
            {
                {1, 1, true},
                {2, 1, false},
                {3, 1, true},
                {2, 2, true},
            });

            DB::table("user_phones")->insert({ID, "user_id", "number"},
            {
                {1, 1, "914111000"},
                {2, 2, "902555777"},
                {3, 3, "905111999"},
            });

            DB::table("torrents")->insert({ID, "user_id", NAME, SIZE_, Progress, "added_on", HASH_, NOTE, CREATED_AT, UPDATED_AT},
            {
                {1, 1, "test1", 11, 100, "2020-08-01 20:11:10", "1579e3af2768cdf52ec84c1f320333f68401dc6e", NullVariant::QString(),           "2016-06-01 08:08:23", "2021-01-01 18:46:31"},
                {2, 1, "test2", 12, 200, "2020-08-02 20:11:10", "2579e3af2768cdf52ec84c1f320333f68401dc6e", NullVariant::QString(),           "2017-07-02 08:09:23", "2021-01-02 18:46:31"},
                {3, 1, "test3", 13, 300, "2020-08-03 20:11:10", "3579e3af2768cdf52ec84c1f320333f68401dc6e", NullVariant::QString(),           "2018-08-03 08:10:23", "2021-01-03 18:46:31"},
                {4, 1, "test4", 14, 400, "2020-08-04 20:11:10", "4579e3af2768cdf52ec84c1f320333f68401dc6e", "after update revert updated_at", "2019-09-04 08:11:23", "2021-01-04 18:46:31"},
                {5, 2, "test5", 15, 500, "2020-08-05 20:11:10", "5579e3af2768cdf52ec84c1f320333f68401dc6e", "no peers",                       "2020-10-05 08:12:23", "2021-01-05 18:46:31"},
                {6, 2, "test6", 16, 600, "2020-08-06 20:11:10", "6579e3af2768cdf52ec84c1f320333f68401dc6e", "no files no peers",              "2021-11-06 08:13:23", "2021-01-06 18:46:31"},
                {7, 2, "test7", 17, 700, "2020-08-07 20:11:10", "7579e3af2768cdf52ec84c1f320333f68401dc6e", "for serialization",              "2021-11-07 08:13:23", "2021-01-07 18:46:31"},
            });

            DB::table("torrent_peers")->insert({ID, "torrent_id", "seeds", "total_seeds", "leechers", "total_leechers", CREATED_AT, UPDATED_AT},
            {
                {1, 1,                        1,                  1, 1, 1, "2021-01-01 14:51:23", "2021-01-01 17:46:31"},
                {2, 2,                        2,                  2, 2, 2, "2021-01-02 14:51:23", "2021-01-02 17:46:31"},
                {3, 3,                        3,                  3, 3, 3, "2021-01-03 14:51:23", "2021-01-03 17:46:31"},
                {4, 4,                        NullVariant::Int(), 4, 4, 4, "2021-01-04 14:51:23", "2021-01-04 17:46:31"},
                {5, 7,                        NullVariant::Int(), 7, 7, 7, "2021-01-07 14:51:23", "2021-01-07 17:46:31"},
                {6, NullVariant::ULongLong(), NullVariant::Int(), 6, 6, 6, "2021-01-06 14:51:23", "2021-01-06 17:46:31"},
            });

            DB::table("torrent_previewable_files")->insert({ID, "torrent_id", "file_index", "filepath", SIZE_, Progress, NOTE, CREATED_AT, UPDATED_AT},
            {
                { 1, 1,                        0, "test1_file1.mkv", 1024, 200,  "no file properties",                    "2021-01-01 14:51:23", "2021-01-01 17:46:31"},
                { 2, 2,                        0, "test2_file1.mkv", 2048, 870,  NullVariant::QString(),                  "2021-01-02 14:51:23", "2021-01-02 17:46:31"},
                { 3, 2,                        1, "test2_file2.mkv", 3072, 1000, NullVariant::QString(),                  "2021-01-02 14:51:23", "2021-01-02 17:46:31"},
                { 4, 3,                        0, "test3_file1.mkv", 5568, 870,  NullVariant::QString(),                  "2021-01-03 14:51:23", "2021-01-03 17:46:31"},
                { 5, 4,                        0, "test4_file1.mkv", 4096, 0,    NullVariant::QString(),                  "2021-01-04 14:51:23", "2021-01-04 17:46:31"},
                { 6, 5,                        0, "test5_file1.mkv", 2048, 999,  NullVariant::QString(),                  "2021-01-05 14:51:23", "2021-01-05 17:46:31"},
                { 7, 5,                        1, "test5_file2.mkv", 2560, 890,  "for tst_BaseModel::remove()/destroy()", "2021-01-02 14:55:23", "2021-01-02 17:47:31"},
                { 8, 5,                        2, "test5_file3.mkv", 2570, 896,  "for tst_BaseModel::destroy()",          "2021-01-02 14:56:23", "2021-01-02 17:48:31"},
                { 9, NullVariant::ULongLong(), 0, "test0_file0.mkv", 1440, 420,  "no torrent parent model",               "2021-01-06 14:57:23", "2021-01-06 17:49:31"},
                {10, 7,                        0, "test7_file1.mkv", 4562, 512,  "for serialization",                     "2021-01-10 14:51:23", "2021-01-10 17:46:31"},
                {11, 7,                        1, "test7_file2.mkv", 2567, 256,  "for serialization",                     "2021-01-11 14:51:23", "2021-01-11 17:46:31"},
                {12, 7,                        2, "test7_file3.mkv", 4279, 768,  "for serialization",                     "2021-01-12 14:51:23", "2021-01-12 17:46:31"},
            });

            DB::table("torrent_previewable_file_properties")->insert({ID, "previewable_file_id", NAME, SIZE_},
            {
                {1, 2, "test2_file1", 2},
                {2, 3, "test2_file2", 2},
                {3, 4, "test3_file1", 4},
                {4, 5, "test4_file1", 5},
                {5, 6, "test5_file1", 6},
            });

            DB::table("file_property_properties")->insert({ID, "file_property_id", NAME, "value", CREATED_AT, UPDATED_AT},
            {
                {1, 1, "test2_file1_property1", 1, "2021-01-01 14:51:23", "2021-01-01 17:46:31"},
                {2, 2, "test2_file2_property1", 2, "2021-01-02 14:51:23", "2021-01-02 17:46:31"},
                {3, 3, "test3_file1_property1", 3, "2021-01-03 14:51:23", "2021-01-03 17:46:31"},
                {4, 3, "test3_file1_property2", 4, "2021-01-04 14:51:23", "2021-01-04 17:46:31"},
                {5, 4, "test4_file1_property1", 5, "2021-01-05 14:51:23", "2021-01-05 17:46:31"},
                {6, 5, "test5_file1_property1", 6, "2021-01-06 14:51:23", "2021-01-06 17:46:31"},
                {7, 5, "test5_file1_property2", 7, "2021-01-07 14:51:23", "2021-01-07 17:46:31"},
                {8, 5, "test5_file1_property3", 8, "2021-01-08 14:51:23", "2021-01-08 17:46:31"},
            });

            DB::table("torrent_tags")->insert({ID, NAME, NOTE, CREATED_AT, UPDATED_AT},
            {
                {1, "tag1", NullVariant::QString(), "2021-01-11 11:51:28", "2021-01-11 23:47:11"},
                {2, "tag2", NullVariant::QString(), "2021-01-12 11:51:28", "2021-01-12 23:47:11"},
                {3, "tag3", NullVariant::QString(), "2021-01-13 11:51:28", "2021-01-13 23:47:11"},
                {4, "tag4", NullVariant::QString(), "2021-01-14 11:51:28", "2021-01-14 23:47:11"},
                {5, "tag5", NullVariant::QString(), "2021-01-15 11:51:28", "2021-01-15 23:47:11"},
            });

            DB::table("tag_torrent")->insert({"torrent_id", "tag_id", "active", CREATED_AT, UPDATED_AT},
            {
                {2, 1, true,  "2021-02-21 17:31:58", "2021-02-21 18:49:22"},
                {2, 2, true,  "2021-02-22 17:31:58", "2021-02-22 18:49:22"},
                {2, 3, false, "2021-02-23 17:31:58", "2021-02-23 18:49:22"},
                {2, 4, true,  "2021-02-24 17:31:58", "2021-02-24 18:49:22"},
                {3, 2, true,  "2021-02-25 17:31:58", "2021-02-25 18:49:22"},
                {3, 4, true,  "2021-02-26 17:31:58", "2021-02-26 18:49:22"},
                {4, 2, true,  "2021-02-27 17:31:58", "2021-02-27 18:49:22"},
                {7, 1, true,  "2021-03-01 17:31:58", "2021-03-01 18:49:22"},
                {7, 2, true,  "2021-03-02 17:31:58", "2021-03-02 18:49:22"},
                {7, 3, false, "2021-03-03 17:31:58", "2021-03-03 18:49:22"},
            });

            DB::table("tag_properties")->insert({ID, "tag_id", "color", "position", CREATED_AT, UPDATED_AT},
            {
                {1, 1, "white",  0, "2021-02-11 12:41:28", "2021-02-11 22:17:11"},
                {2, 2, "blue",   1, "2021-02-12 12:41:28", "2021-02-12 22:17:11"},
                {3, 3, "red",    2, "2021-02-13 12:41:28", "2021-02-13 22:17:11"},
                {4, 4, "orange", 3, "2021-02-14 12:41:28", "2021-02-14 22:17:11"},
            });

            // Table - types
            const auto isPostgreSql = DB::driverName() == Orm::QPSQL;

            QVariant double_nan = isPostgreSql
                                  ? std::numeric_limits<double>::quiet_NaN()
                                  : NullVariant::Double();
            QVariant double_infinity = isPostgreSql
                                       ? std::numeric_limits<double>::infinity()
                                       : NullVariant::Double();

            // Insert
            DB::table("types")->insert({ID, "bool_true", "bool_false", "smallint", "smallint_u", "int", "int_u", "bigint", "bigint_u", "double", "double_nan", "double_infinity", "decimal", "decimal_nan", "decimal_infinity", "decimal_down", "decimal_up", "string", "text", "medium_text", "timestamp", "datetime", "date", "time", "binary", "medium_binary"},
            {
                {1, true, false, static_cast<int16_t>(32760), static_cast<uint16_t>(32761), static_cast<int>(2147483640), static_cast<uint>(2147483641), static_cast<qint64>(9223372036854775800), static_cast<quint64>(9223372036854775801), static_cast<double>(1000000.123), double_nan, double_infinity, static_cast<double>(100000.12), double_nan, double_infinity, static_cast<double>(100.124), static_cast<double>(100.125), "string text", "text text", "mediumtext text", "2022-09-09 08:41:28", "2022-09-10 08:41:28", "2022-09-11", "17:01:05", QByteArray::fromHex("517420697320677265617421"), QByteArray::fromHex("54696e794f524d206973206772656174657221")}, // NOLINT(readability-redundant-casting)
                {2, NullVariant::Bool(), NullVariant::Bool(), static_cast<int16_t>(-32762), NullVariant::UShort(), static_cast<int>(-2147483642), NullVariant::UInt(), static_cast<qint64>(-9223372036854775802), NullVariant::ULongLong(), static_cast<double>(-1000000.123), NullVariant::Double(), NullVariant::Double(), static_cast<double>(-100000.12), NullVariant::Double(), NullVariant::Double(), static_cast<double>(-100.125), static_cast<double>(-100.124), NullVariant::QString(), NullVariant::QString(), NullVariant::QString(), NullVariant::QDateTime(), NullVariant::QDateTime(), NullVariant::QDateTime(), NullVariant::QTime(), NullVariant::QByteArray(), NullVariant::QByteArray()}, // NOLINT(readability-redundant-casting)
                // All types null
                {3, NullVariant::Bool(), NullVariant::Bool(), NullVariant::Short(), NullVariant::UShort(), NullVariant::Int(), NullVariant::UInt(), NullVariant::LongLong(), NullVariant::ULongLong(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::QString(), NullVariant::QString(), NullVariant::QString(), NullVariant::QDateTime(), NullVariant::QDateTime(), NullVariant::QDateTime(), NullVariant::QTime(), NullVariant::QByteArray(), NullVariant::QByteArray()},
            });

            DB::table("albums")->insert({ID, NAME, NOTE, CREATED_AT, UPDATED_AT},
            {
                {1, "album1", NullVariant::QString(), "2023-01-01 12:21:14", "2023-02-01 16:54:28"},
                {2, "album2", NullVariant::QString(), "2023-01-02 12:21:14", "2023-02-02 16:54:28"},
                {3, "album3", "album3 note",          "2023-01-03 12:21:14", "2023-02-03 16:54:28"},
                {4, "album4", "no images",            "2023-01-04 12:21:14", "2023-02-04 16:54:28"},
            });

            DB::table("album_images")->insert({ID, "album_id", NAME, "ext", SIZE_, CREATED_AT, UPDATED_AT},
            {
                {1, 1,                        "album1_image1", "png", 726, "2023-03-01 15:24:37", "2023-04-01 14:35:47"},
                {2, 2,                        "album2_image1", "png", 424, "2023-03-02 15:24:37", "2023-04-02 14:35:47"},
                {3, 2,                        "album2_image2", "jpg", 512, "2023-03-03 15:24:37", "2023-04-03 14:35:47"},
                {4, 2,                        "album2_image3", "jpg", 324, "2023-03-04 15:24:37", "2023-04-04 14:35:47"},
                {5, 2,                        "album2_image4", "png", 654, "2023-03-05 15:24:37", "2023-04-05 14:35:47"},
                {6, 2,                        "album2_image5", "gif", 294, "2023-03-06 15:24:37", "2023-04-06 14:35:47"},
                {7, 3,                        "album3_image1", "jpg", 718, "2023-03-07 15:24:37", "2023-04-07 14:35:47"},
                {8, NullVariant::ULongLong(), "image1",        "jpg", 498, "2023-03-08 15:24:37", "2023-04-08 14:35:47"},
                {9, NullVariant::ULongLong(), "image2",        "jpg", 568, "2023-03-09 15:24:37", "2023-04-09 14:35:47"},
            });

            DB::table("torrent_states")->insert({ID, NAME},
            {
                {1, "Active"},
                {2, "Stalled"},
                {3, "Inactive"},
                {4, "Downloading"},
                {5, "Resumed"},
            });

            DB::table("state_torrent")->insert({"torrent_id", "state_id", "active"},
            {
                {7, 1, true},
                {7, 4, false},
            });

            DB::table("role_tag")->insert({"tag_id", "role_id", "active"},
            {
                {2, 1, true},
                {2, 3, false},
            });

            // Fix sequence numbers for the PostgreSQL
            if (isPostgreSql)
                fixPostgresSequences();
        }

    private:
        /*! Fix sequence numbers for the PostgreSQL. */
        inline static void fixPostgresSequences();
    };

    /* private */

    void DatabaseSeeder::fixPostgresSequences()
    {
        /* I have to fix sequences in Postgres because I'm inserting IDs manually, and
           it doesn't increment sequences. */

        const std::unordered_map<QString, quint64> sequences {
            {sl("users_id_seq"),                                6},
            {sl("roles_id_seq"),                                4},
            {sl("user_phones_id_seq"),                          4},
            {sl("torrents_id_seq"),                             8},
            {sl("torrent_peers_id_seq"),                        7},
            {sl("torrent_previewable_files_id_seq"),           13},
            {sl("torrent_previewable_file_properties_id_seq"),  6},
            {sl("file_property_properties_id_seq"),             9},
            {sl("torrent_tags_id_seq"),                         6},
            {sl("tag_properties_id_seq"),                       5},
            {sl("types_id_seq"),                                4},
            {sl("albums_id_seq"),                               5},
            {sl("album_images_id_seq"),                         8},
            {sl("torrent_states_id_seq"),                       6},
        };

        for (const auto &[sequence, id] : sequences)
            DB::connection().unprepared(
                    sl(R"(alter sequence "%1" restart with %2)")
                    .arg(sequence).arg(id));
    }

} // namespace Seeders
