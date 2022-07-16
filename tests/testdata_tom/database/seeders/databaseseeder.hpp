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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            const auto NullString =    QVariant(QMetaType(QMetaType::QString));
            const auto NullInt =       QVariant(QMetaType(QMetaType::Int));
            const auto NullULLong =    QVariant(QMetaType(QMetaType::ULongLong));
#else
            const auto NullString =    QVariant(QVariant::String);
            const auto NullInt =       QVariant(QVariant::Int);
            const auto NullULLong =    QVariant(QVariant::ULongLong);
#endif

            DB::table("users")->insert({ID, NAME, "note"},
            {
                {1, "andrej", NullString},
                {2, "silver", NullString},
                {3, "peter",  "no torrents no roles"},
            });

            DB::table("roles")->insert({ID, NAME},
            {
                {1, "role one"},
                {2, "role two"},
                {3, "role three"},
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

            DB::table("torrents")->insert({ID, "user_id", NAME, SIZE_, "progress", "added_on", "hash", "note", CREATED_AT, UPDATED_AT},
            {
                {1, 1, "test1", 11, 100, "2020-08-01 20:11:10", "1579e3af2768cdf52ec84c1f320333f68401dc6e", NullString,                       "2021-01-01 14:51:23", "2021-01-01 18:46:31"},
                {2, 1, "test2", 12, 200, "2020-08-02 20:11:10", "2579e3af2768cdf52ec84c1f320333f68401dc6e", NullString,                       "2021-01-02 14:51:23", "2021-01-02 18:46:31"},
                {3, 1, "test3", 13, 300, "2020-08-03 20:11:10", "3579e3af2768cdf52ec84c1f320333f68401dc6e", NullString,                       "2021-01-03 14:51:23", "2021-01-03 18:46:31"},
                {4, 1, "test4", 14, 400, "2020-08-04 20:11:10", "4579e3af2768cdf52ec84c1f320333f68401dc6e", "after update revert updated_at", "2021-01-04 14:51:23", "2021-01-04 18:46:31"},
                {5, 2, "test5", 15, 500, "2020-08-05 20:11:10", "5579e3af2768cdf52ec84c1f320333f68401dc6e", "no peers",                       "2021-01-05 14:51:23", "2021-01-05 18:46:31"},
                {6, 2, "test6", 16, 600, "2020-08-06 20:11:10", "6579e3af2768cdf52ec84c1f320333f68401dc6e", "no files no peers",              "2021-01-06 14:51:23", "2021-01-06 18:46:31"},
            });

            DB::table("torrent_peers")->insert({ID, "torrent_id", "seeds", "total_seeds", "leechers", "total_leechers", CREATED_AT, UPDATED_AT},
            {
                {1, 1, 1,       1, 1, 1, "2021-01-01 14:51:23", "2021-01-01 17:46:31"},
                {2, 2, 2,       2, 2, 2, "2021-01-02 14:51:23", "2021-01-02 17:46:31"},
                {3, 3, 3,       3, 3, 3, "2021-01-03 14:51:23", "2021-01-03 17:46:31"},
                {4, 4, NullInt, 4, 4, 4, "2021-01-04 14:51:23", "2021-01-04 17:46:31"},
            });

            DB::table("torrent_previewable_files")->insert({ID, "torrent_id", "file_index", "filepath", SIZE_, "progress", "note", CREATED_AT, UPDATED_AT},
            {
                {1, 1,          0, "test1_file1.mkv", 1024, 200,  "no file properties",                    "2021-01-01 14:51:23", "2021-01-01 17:46:31"},
                {2, 2,          0, "test2_file1.mkv", 2048, 870,  NullString,                              "2021-01-02 14:51:23", "2021-01-02 17:46:31"},
                {3, 2,          1, "test2_file2.mkv", 3072, 1000, NullString,                              "2021-01-02 14:51:23", "2021-01-02 17:46:31"},
                {4, 3,          0, "test3_file1.mkv", 5568, 870,  NullString,                              "2021-01-03 14:51:23", "2021-01-03 17:46:31"},
                {5, 4,          0, "test4_file1.mkv", 4096, 0,    NullString,                              "2021-01-04 14:51:23", "2021-01-04 17:46:31"},
                {6, 5,          0, "test5_file1.mkv", 2048, 999,  NullString,                              "2021-01-05 14:51:23", "2021-01-05 17:46:31"},
                {7, 5,          1, "test5_file2.mkv", 2560, 890,  "for tst_BaseModel::remove()/destroy()", "2021-01-02 14:55:23", "2021-01-02 17:47:31"},
                {8, 5,          2, "test5_file3.mkv", 2570, 896,  "for tst_BaseModel::destroy()",          "2021-01-02 14:56:23", "2021-01-02 17:48:31"},
                {9, NullULLong, 0, "test0_file0.mkv", 1440, 420,  "no torrent parent model",               "2021-01-02 14:56:23", "2021-01-02 17:48:31"},
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

            DB::table("torrent_tags")->insert({ID, NAME, "note", CREATED_AT, UPDATED_AT},
            {
                {1, "tag1", NullString, "2021-01-11 11:51:28", "2021-01-11 23:47:11"},
                {2, "tag2", NullString, "2021-01-12 11:51:28", "2021-01-12 23:47:11"},
                {3, "tag3", NullString, "2021-01-13 11:51:28", "2021-01-13 23:47:11"},
                {4, "tag4", NullString, "2021-01-14 11:51:28", "2021-01-14 23:47:11"},
                {5, "tag5", NullString, "2021-01-15 11:51:28", "2021-01-15 23:47:11"},
            });

            DB::table("tag_torrent")->insert({"torrent_id", "tag_id", "active", CREATED_AT, UPDATED_AT},
            {
                {2, 1, true,  "2021-02-21 17:31:58", "2021-02-21 18:49:22"},
                {2, 2, true,  "2021-02-22 17:31:58", "2021-02-22 18:49:22"},
                {2, 3, false, "2021-02-23 17:31:58", "2021-02-23 18:49:22"},
                {2, 4, true,  "2021-02-24 17:31:58", "2021-02-24 18:49:22"},
                {3, 2, true,  "2021-02-25 17:31:58", "2021-02-25 18:49:22"},
                {3, 4, true,  "2021-02-26 17:31:58", "2021-02-26 18:49:22"},
            });

            DB::table("tag_properties")->insert({ID, "tag_id", "color", "position", CREATED_AT, UPDATED_AT},
            {
                {1, 1, "white",  0, "2021-02-11 12:41:28", "2021-02-11 22:17:11"},
                {2, 2, "blue",   1, "2021-02-12 12:41:28", "2021-02-12 22:17:11"},
                {3, 3, "red",    2, "2021-02-13 12:41:28", "2021-02-13 22:17:11"},
                {4, 4, "orange", 3, "2021-02-14 12:41:28", "2021-02-14 22:17:11"},
            });

            // Fix sequence numbers for the PostgreSQL
            if (DB::getDefaultConnection() ==
                QLatin1String("tinyorm_testdata_tom_postgres")
            )
                fixPostgresSequences();
        }

    private:
        /*! Fix sequence numbers for the PostgreSQL. */
        inline void fixPostgresSequences() const;
    };

    /* private */

    void DatabaseSeeder::fixPostgresSequences() const
    {
        /* I have to fix sequences in Postgres because I'm inserting IDs manually, and
           it doesn't increment sequences. */

        std::unordered_map<QString, quint64> sequences {
            {QStringLiteral("torrents_id_seq"),                             7},
            {QStringLiteral("torrent_peers_id_seq"),                        5},
            {QStringLiteral("torrent_previewable_files_id_seq"),           10},
            {QStringLiteral("torrent_previewable_file_properties_id_seq"),  6},
            {QStringLiteral("file_property_properties_id_seq"),             7},
            {QStringLiteral("torrent_tags_id_seq"),                         6},
            {QStringLiteral("tag_properties_id_seq"),                       5},
            {QStringLiteral("users_id_seq"),                                4},
            {QStringLiteral("roles_id_seq"),                                4},
            {QStringLiteral("user_phones_id_seq"),                          4},
        };

        for (auto &&[sequence, id] : sequences)
            DB::connection().unprepared(
                    QStringLiteral("ALTER SEQUENCE %1 RESTART WITH %2")
                    .arg(sequence).arg(id));
    }

} // namespace Seeders
