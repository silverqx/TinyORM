#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateTagTorrentTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("tag_torrent", [](Blueprint &table)
            {
                // Don't use the terser methods syntax here, leave it this way as example
                table.unsignedBigInteger("torrent_id");
                table.unsignedBigInteger("tag_id");

                table.boolean("active").defaultValue(true);

                table.timestamps();

                // Indexes
                table.primary({"torrent_id", "tag_id"});

                table.foreign("torrent_id")
                     .references(ID).on("torrents")
                     .cascadeOnDelete().cascadeOnUpdate();
                table.foreign("tag_id")
                     .references(ID).on("torrent_tags")
                     .cascadeOnDelete().cascadeOnUpdate();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("tag_torrent");
        }
    };

} // namespace Migrations
