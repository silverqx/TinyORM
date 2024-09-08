#pragma once

#include <tom/seeder.hpp>

namespace Seeders
{

    /*! Main database seeder. */
    struct DatabaseSeeder : Seeder
    {
        /*! Run the database seeders. */
        void run() override
        {
            DB::table(u"users"_s)->insert({ID, NAME, u"is_banned"_s, NOTE, CREATED_AT, UPDATED_AT, DELETED_AT},
            {
                {1, u"andrej"_s, false, NullVariant::QString(),    u"2022-01-01 14:51:23"_s, u"2022-01-01 17:46:31"_s, NullVariant::QDateTime()},
                {2, u"silver"_s, false, NullVariant::QString(),    u"2022-01-02 14:51:23"_s, u"2022-01-02 17:46:31"_s, NullVariant::QDateTime()},
                {3, u"peter"_s,  true,  u"no torrents no roles"_s, u"2022-01-03 14:51:23"_s, u"2022-01-03 17:46:31"_s, NullVariant::QDateTime()},
                {4, u"jack"_s,   false, u"test SoftDeletes"_s,     u"2022-01-04 14:51:23"_s, u"2022-01-04 17:46:31"_s, u"2022-01-04 20:46:31"_s},
                {5, u"obiwan"_s, true,  u"test SoftDeletes"_s,     u"2022-01-05 14:51:23"_s, u"2022-01-05 17:46:31"_s, u"2022-01-05 20:46:31"_s},
            });

            DB::table(u"roles"_s)->insert({ID, NAME, AddedOn},
            {
                {1, u"role one"_s,   1659361016},
                {2, u"role two"_s,   1659447416},
                {3, u"role three"_s, NullVariant::ULongLong()},
            });

            DB::table(u"role_user"_s)->insert({u"role_id"_s, u"user_id"_s, u"active"_s},
            {
                {1, 1, true},
                {2, 1, false},
                {3, 1, true},
                {2, 2, true},
            });

            DB::table(u"user_phones"_s)->insert({ID, u"user_id"_s, u"number"_s},
            {
                {1, 1, u"914111000"_s},
                {2, 2, u"902555777"_s},
                {3, 3, u"905111999"_s},
            });

            DB::table(u"torrents"_s)->insert({ID, u"user_id"_s, NAME, SIZE_, Progress, AddedOn, HASH_, NOTE, CREATED_AT, UPDATED_AT},
            {
                {1, 1, u"test1"_s, 11, 100, u"2020-08-01 20:11:10"_s, u"1579e3af2768cdf52ec84c1f320333f68401dc6e"_s, NullVariant::QString(),              u"2016-06-01 08:08:23"_s, u"2021-01-01 18:46:31"_s},
                {2, 1, u"test2"_s, 12, 200, u"2020-08-02 20:11:10"_s, u"2579e3af2768cdf52ec84c1f320333f68401dc6e"_s, NullVariant::QString(),              u"2017-07-02 08:09:23"_s, u"2021-01-02 18:46:31"_s},
                {3, 1, u"test3"_s, 13, 300, u"2020-08-03 20:11:10"_s, u"3579e3af2768cdf52ec84c1f320333f68401dc6e"_s, NullVariant::QString(),              u"2018-08-03 08:10:23"_s, u"2021-01-03 18:46:31"_s},
                {4, 1, u"test4"_s, 14, 400, u"2020-08-04 20:11:10"_s, u"4579e3af2768cdf52ec84c1f320333f68401dc6e"_s, u"after update revert updated_at"_s, u"2019-09-04 08:11:23"_s, u"2021-01-04 18:46:31"_s},
                {5, 2, u"test5"_s, 15, 500, u"2020-08-05 20:11:10"_s, u"5579e3af2768cdf52ec84c1f320333f68401dc6e"_s, u"no peers"_s,                       u"2020-10-05 08:12:23"_s, u"2021-01-05 18:46:31"_s},
                {6, 2, u"test6"_s, 16, 600, u"2020-08-06 20:11:10"_s, u"6579e3af2768cdf52ec84c1f320333f68401dc6e"_s, u"no files no peers"_s,              u"2021-11-06 08:13:23"_s, u"2021-01-06 18:46:31"_s},
                {7, 2, u"test7"_s, 17, 700, u"2020-08-07 20:11:10"_s, u"7579e3af2768cdf52ec84c1f320333f68401dc6e"_s, u"for serialization"_s,              u"2021-11-07 08:13:23"_s, u"2021-01-07 18:46:31"_s},
            });

            DB::table(u"torrent_peers"_s)->insert({ID, u"torrent_id"_s, u"seeds"_s, u"total_seeds"_s, u"leechers"_s, u"total_leechers"_s, CREATED_AT, UPDATED_AT},
            {
                {1, 1,                        1,                  1, 1, 1, u"2021-01-01 14:51:23"_s, u"2021-01-01 17:46:31"_s},
                {2, 2,                        2,                  2, 2, 2, u"2021-01-02 14:51:23"_s, u"2021-01-02 17:46:31"_s},
                {3, 3,                        3,                  3, 3, 3, u"2021-01-03 14:51:23"_s, u"2021-01-03 17:46:31"_s},
                {4, 4,                        NullVariant::Int(), 4, 4, 4, u"2021-01-04 14:51:23"_s, u"2021-01-04 17:46:31"_s},
                {5, 7,                        NullVariant::Int(), 7, 7, 7, u"2021-01-07 14:51:23"_s, u"2021-01-07 17:46:31"_s},
                {6, NullVariant::ULongLong(), NullVariant::Int(), 6, 6, 6, u"2021-01-06 14:51:23"_s, u"2021-01-06 17:46:31"_s},
            });

            DB::table(u"torrent_previewable_files"_s)->insert({ID, u"torrent_id"_s, u"file_index"_s, u"filepath"_s, SIZE_, Progress, NOTE, CREATED_AT, UPDATED_AT},
            {
                { 1, 1,                        0, u"test1_file1.mkv"_s, 1024, 200,  u"no file properties"_s,                    u"2021-01-01 14:51:23"_s, u"2021-01-01 17:46:31"_s},
                { 2, 2,                        0, u"test2_file1.mkv"_s, 2048, 870,  NullVariant::QString(),                     u"2021-01-02 14:51:23"_s, u"2021-01-02 17:46:31"_s},
                { 3, 2,                        1, u"test2_file2.mkv"_s, 3072, 1000, NullVariant::QString(),                     u"2021-01-02 14:51:23"_s, u"2021-01-02 17:46:31"_s},
                { 4, 3,                        0, u"test3_file1.mkv"_s, 5568, 870,  NullVariant::QString(),                     u"2021-01-03 14:51:23"_s, u"2021-01-03 17:46:31"_s},
                { 5, 4,                        0, u"test4_file1.mkv"_s, 4096, 0,    NullVariant::QString(),                     u"2021-01-04 14:51:23"_s, u"2021-01-04 17:46:31"_s},
                { 6, 5,                        0, u"test5_file1.mkv"_s, 2048, 999,  NullVariant::QString(),                     u"2021-01-05 14:51:23"_s, u"2021-01-05 17:46:31"_s},
                { 7, 5,                        1, u"test5_file2.mkv"_s, 2560, 890,  u"for tst_BaseModel::remove()/destroy()"_s, u"2021-01-02 14:55:23"_s, u"2021-01-02 17:47:31"_s},
                { 8, 5,                        2, u"test5_file3.mkv"_s, 2570, 896,  u"for tst_BaseModel::destroy()"_s,          u"2021-01-02 14:56:23"_s, u"2021-01-02 17:48:31"_s},
                { 9, NullVariant::ULongLong(), 0, u"test0_file0.mkv"_s, 1440, 420,  u"no torrent parent model"_s,               u"2021-01-06 14:57:23"_s, u"2021-01-06 17:49:31"_s},
                {10, 7,                        0, u"test7_file1.mkv"_s, 4562, 512,  u"for serialization"_s,                     u"2021-01-10 14:51:23"_s, u"2021-01-10 17:46:31"_s},
                {11, 7,                        1, u"test7_file2.mkv"_s, 2567, 256,  u"for serialization"_s,                     u"2021-01-11 14:51:23"_s, u"2021-01-11 17:46:31"_s},
                {12, 7,                        2, u"test7_file3.mkv"_s, 4279, 768,  u"for serialization"_s,                     u"2021-01-12 14:51:23"_s, u"2021-01-12 17:46:31"_s},
            });

            DB::table(u"torrent_previewable_file_properties"_s)->insert({ID, u"previewable_file_id"_s, NAME, SIZE_},
            {
                {1, 2, u"test2_file1"_s, 2},
                {2, 3, u"test2_file2"_s, 2},
                {3, 4, u"test3_file1"_s, 4},
                {4, 5, u"test4_file1"_s, 5},
                {5, 6, u"test5_file1"_s, 6},
            });

            DB::table(u"file_property_properties"_s)->insert({ID, u"file_property_id"_s, NAME, u"value"_s, CREATED_AT, UPDATED_AT},
            {
                {1, 1, u"test2_file1_property1"_s, 1, u"2021-01-01 14:51:23"_s, u"2021-01-01 17:46:31"_s},
                {2, 2, u"test2_file2_property1"_s, 2, u"2021-01-02 14:51:23"_s, u"2021-01-02 17:46:31"_s},
                {3, 3, u"test3_file1_property1"_s, 3, u"2021-01-03 14:51:23"_s, u"2021-01-03 17:46:31"_s},
                {4, 3, u"test3_file1_property2"_s, 4, u"2021-01-04 14:51:23"_s, u"2021-01-04 17:46:31"_s},
                {5, 4, u"test4_file1_property1"_s, 5, u"2021-01-05 14:51:23"_s, u"2021-01-05 17:46:31"_s},
                {6, 5, u"test5_file1_property1"_s, 6, u"2021-01-06 14:51:23"_s, u"2021-01-06 17:46:31"_s},
                {7, 5, u"test5_file1_property2"_s, 7, u"2021-01-07 14:51:23"_s, u"2021-01-07 17:46:31"_s},
                {8, 5, u"test5_file1_property3"_s, 8, u"2021-01-08 14:51:23"_s, u"2021-01-08 17:46:31"_s},
            });

            DB::table(u"torrent_tags"_s)->insert({ID, NAME, NOTE, CREATED_AT, UPDATED_AT},
            {
                {1, u"tag1"_s, NullVariant::QString(), u"2021-01-11 11:51:28"_s, u"2021-01-11 23:47:11"_s},
                {2, u"tag2"_s, NullVariant::QString(), u"2021-01-12 11:51:28"_s, u"2021-01-12 23:47:11"_s},
                {3, u"tag3"_s, NullVariant::QString(), u"2021-01-13 11:51:28"_s, u"2021-01-13 23:47:11"_s},
                {4, u"tag4"_s, NullVariant::QString(), u"2021-01-14 11:51:28"_s, u"2021-01-14 23:47:11"_s},
                {5, u"tag5"_s, NullVariant::QString(), u"2021-01-15 11:51:28"_s, u"2021-01-15 23:47:11"_s},
            });

            DB::table(u"tag_torrent"_s)->insert({u"torrent_id"_s, u"tag_id"_s, u"active"_s, CREATED_AT, UPDATED_AT},
            {
                {2, 1, true,  u"2021-02-21 17:31:58"_s, u"2021-02-21 18:49:22"_s},
                {2, 2, true,  u"2021-02-22 17:31:58"_s, u"2021-02-22 18:49:22"_s},
                {2, 3, false, u"2021-02-23 17:31:58"_s, u"2021-02-23 18:49:22"_s},
                {2, 4, true,  u"2021-02-24 17:31:58"_s, u"2021-02-24 18:49:22"_s},
                {3, 2, true,  u"2021-02-25 17:31:58"_s, u"2021-02-25 18:49:22"_s},
                {3, 4, true,  u"2021-02-26 17:31:58"_s, u"2021-02-26 18:49:22"_s},
                {4, 2, true,  u"2021-02-27 17:31:58"_s, u"2021-02-27 18:49:22"_s},
                {7, 1, true,  u"2021-03-01 17:31:58"_s, u"2021-03-01 18:49:22"_s},
                {7, 2, true,  u"2021-03-02 17:31:58"_s, u"2021-03-02 18:49:22"_s},
                {7, 3, false, u"2021-03-03 17:31:58"_s, u"2021-03-03 18:49:22"_s},
            });

            DB::table(u"tag_properties"_s)->insert({ID, u"tag_id"_s, u"color"_s, u"position"_s, CREATED_AT, UPDATED_AT},
            {
                {1, 1, u"white"_s,  0, u"2021-02-11 12:41:28"_s, u"2021-02-11 22:17:11"_s},
                {2, 2, u"blue"_s,   1, u"2021-02-12 12:41:28"_s, u"2021-02-12 22:17:11"_s},
                {3, 3, u"red"_s,    2, u"2021-02-13 12:41:28"_s, u"2021-02-13 22:17:11"_s},
                {4, 4, u"orange"_s, 3, u"2021-02-14 12:41:28"_s, u"2021-02-14 22:17:11"_s},
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
            DB::table(u"types"_s)->insert({ID, u"bool_true"_s, u"bool_false"_s, u"smallint"_s, u"smallint_u"_s, u"int"_s, u"int_u"_s, u"bigint"_s, u"bigint_u"_s, u"double"_s, u"double_nan"_s, u"double_infinity"_s, u"decimal"_s, u"decimal_nan"_s, u"decimal_infinity"_s, u"decimal_down"_s, u"decimal_up"_s, u"string"_s, u"text"_s, u"medium_text"_s, u"timestamp"_s, u"datetime"_s, u"date"_s, u"time"_s, u"binary"_s, u"medium_binary"_s},
            {
                {1, true, false, static_cast<int16_t>(32760), static_cast<uint16_t>(32761), static_cast<int>(2147483640), static_cast<uint>(2147483641), static_cast<qint64>(9223372036854775800), static_cast<quint64>(9223372036854775801), static_cast<double>(1000000.123), double_nan, double_infinity, static_cast<double>(100000.12), double_nan, double_infinity, static_cast<double>(100.124), static_cast<double>(100.125), u"string text"_s, u"text text"_s, u"mediumtext text"_s, u"2022-09-09 08:41:28"_s, u"2022-09-10 08:41:28"_s, u"2022-09-11"_s, u"17:01:05"_s, "Qt is great!"_ba, "TinyORM is greater!"_ba}, // NOLINT(readability-redundant-casting)
                {2, NullVariant::Bool(), NullVariant::Bool(), static_cast<int16_t>(-32762), NullVariant::UShort(), static_cast<int>(-2147483642), NullVariant::UInt(), static_cast<qint64>(-9223372036854775802), NullVariant::ULongLong(), static_cast<double>(-1000000.123), NullVariant::Double(), NullVariant::Double(), static_cast<double>(-100000.12), NullVariant::Double(), NullVariant::Double(), static_cast<double>(-100.125), static_cast<double>(-100.124), NullVariant::QString(), NullVariant::QString(), NullVariant::QString(), NullVariant::QDateTime(), NullVariant::QDateTime(), NullVariant::QDateTime(), NullVariant::QTime(), NullVariant::QByteArray(), NullVariant::QByteArray()}, // NOLINT(readability-redundant-casting)
                // All types null
                {3, NullVariant::Bool(), NullVariant::Bool(), NullVariant::Short(), NullVariant::UShort(), NullVariant::Int(), NullVariant::UInt(), NullVariant::LongLong(), NullVariant::ULongLong(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::Double(), NullVariant::QString(), NullVariant::QString(), NullVariant::QString(), NullVariant::QDateTime(), NullVariant::QDateTime(), NullVariant::QDateTime(), NullVariant::QTime(), NullVariant::QByteArray(), NullVariant::QByteArray()},
            });

            DB::table(u"albums"_s)->insert({ID, NAME, NOTE, CREATED_AT, UPDATED_AT},
            {
                {1, u"album1"_s, NullVariant::QString(), u"2023-01-01 12:21:14"_s, u"2023-02-01 16:54:28"_s},
                {2, u"album2"_s, NullVariant::QString(), u"2023-01-02 12:21:14"_s, u"2023-02-02 16:54:28"_s},
                {3, u"album3"_s, u"album3 note"_s,       u"2023-01-03 12:21:14"_s, u"2023-02-03 16:54:28"_s},
                {4, u"album4"_s, u"no images"_s,         u"2023-01-04 12:21:14"_s, u"2023-02-04 16:54:28"_s},
            });

            DB::table(u"album_images"_s)->insert({ID, u"album_id"_s, NAME, u"ext"_s, SIZE_, CREATED_AT, UPDATED_AT},
            {
                {1, 1,                        u"album1_image1"_s, u"png"_s, 726, u"2023-03-01 15:24:37"_s, u"2023-04-01 14:35:47"_s},
                {2, 2,                        u"album2_image1"_s, u"png"_s, 424, u"2023-03-02 15:24:37"_s, u"2023-04-02 14:35:47"_s},
                {3, 2,                        u"album2_image2"_s, u"jpg"_s, 512, u"2023-03-03 15:24:37"_s, u"2023-04-03 14:35:47"_s},
                {4, 2,                        u"album2_image3"_s, u"jpg"_s, 324, u"2023-03-04 15:24:37"_s, u"2023-04-04 14:35:47"_s},
                {5, 2,                        u"album2_image4"_s, u"png"_s, 654, u"2023-03-05 15:24:37"_s, u"2023-04-05 14:35:47"_s},
                {6, 2,                        u"album2_image5"_s, u"gif"_s, 294, u"2023-03-06 15:24:37"_s, u"2023-04-06 14:35:47"_s},
                {7, 3,                        u"album3_image1"_s, u"jpg"_s, 718, u"2023-03-07 15:24:37"_s, u"2023-04-07 14:35:47"_s},
                {8, NullVariant::ULongLong(), u"image1"_s,        u"jpg"_s, 498, u"2023-03-08 15:24:37"_s, u"2023-04-08 14:35:47"_s},
                {9, NullVariant::ULongLong(), u"image2"_s,        u"jpg"_s, 568, u"2023-03-09 15:24:37"_s, u"2023-04-09 14:35:47"_s},
            });

            DB::table(u"torrent_states"_s)->insert({ID, NAME},
            {
                {1, u"Active"_s},
                {2, u"Stalled"_s},
                {3, u"Inactive"_s},
                {4, u"Downloading"_s},
                {5, u"Resumed"_s},
            });

            DB::table(u"state_torrent"_s)->insert({u"torrent_id"_s, u"state_id"_s, u"active"_s},
            {
                {7, 1, true},
                {7, 4, false},
            });

            DB::table(u"role_tag"_s)->insert({u"tag_id"_s, u"role_id"_s, u"active"_s},
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
            {u"users_id_seq"_s,                                6},
            {u"roles_id_seq"_s,                                4},
            {u"user_phones_id_seq"_s,                          4},
            {u"torrents_id_seq"_s,                             8},
            {u"torrent_peers_id_seq"_s,                        7},
            {u"torrent_previewable_files_id_seq"_s,           13},
            {u"torrent_previewable_file_properties_id_seq"_s,  6},
            {u"file_property_properties_id_seq"_s,             9},
            {u"torrent_tags_id_seq"_s,                         6},
            {u"tag_properties_id_seq"_s,                       5},
            {u"types_id_seq"_s,                                4},
            {u"albums_id_seq"_s,                               5},
            {u"album_images_id_seq"_s,                         8},
            {u"torrent_states_id_seq"_s,                       6},
        };

        for (const auto &[sequence, id] : sequences)
            DB::connection().unprepared(
                    uR"(alter sequence "%1" restart with %2)"_s
                    .arg(sequence).arg(id));
    }

} // namespace Seeders
