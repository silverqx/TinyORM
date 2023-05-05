#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateTorrentTagsTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("torrent_tags", [](Blueprint &table)
            {
                table.id();

                table.string(NAME).unique();
                table.string(NOTE).nullable();

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("torrent_tags");
        }
    };

} // namespace Migrations
