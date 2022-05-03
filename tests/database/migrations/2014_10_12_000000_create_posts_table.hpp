#pragma once

#include <tom/migration.hpp>

/* This class serves as a showcase, so all possible features are defined / used.
   Only PostgreSQL and Microsoft SQL Server support the 'withinTransaction' option,
   as TinyORM Schema builder currently only supports MySQL server, this option is
   unused. */

namespace Migrations
{

    struct CreatePostsTable : Migration
//    struct _2014_10_12_000000_create_posts_table : Migration
    {
        /*! Filename of the migration file. */
        T_MIGRATION

        /*! The name of the database connection to use. */
//        QString connection = QStringLiteral("tinyorm_tom");
        /*! Wrapping the migration within a transaction, if supported. */
        bool withinTransaction = false;

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("posts", [](Blueprint &table)
            {
                table.id();

                table.string(NAME);
                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("posts");
        }
    };

} // namespace Migrations
