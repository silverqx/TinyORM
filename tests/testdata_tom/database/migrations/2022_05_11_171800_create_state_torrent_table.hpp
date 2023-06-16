#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateStateTorrentTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("state_torrent", [](Blueprint &table)
            {
                table.unsignedBigInteger("torrent_id");
                table.unsignedBigInteger("state_id");

                table.boolean("active").defaultValue(false);

                // Indexes
                table.primary({"torrent_id", "state_id"});

                table.foreign("torrent_id")
                     .references(ID).on("torrents")
                     .cascadeOnDelete().cascadeOnUpdate();
                table.foreign("state_id")
                     .references(ID).on("torrent_states")
                     .cascadeOnDelete().cascadeOnUpdate();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("state_torrent");
        }
    };

} // namespace Migrations
