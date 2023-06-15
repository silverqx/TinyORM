#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateTorrentPeersTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("torrent_peers", [](Blueprint &table)
            {
                table.id();

                table.foreignId("torrent_id").nullable().constrained()
                     .cascadeOnDelete().cascadeOnUpdate();

                table.integer("seeds").nullable();
                table.integer("total_seeds").nullable();

                table.integer("leechers");
                table.integer("total_leechers");

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("torrent_peers");
        }
    };

} // namespace Migrations
