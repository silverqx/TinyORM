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
            Schema::create(u"torrent_tags"_s, [](Blueprint &table)
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
            Schema::dropIfExists(u"torrent_tags"_s);
        }
    };

} // namespace Migrations
