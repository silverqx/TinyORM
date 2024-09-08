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
            Schema::create(u"torrent_peers"_s, [](Blueprint &table)
            {
                table.id();

                table.foreignId(u"torrent_id"_s).nullable().constrained()
                     .cascadeOnDelete().cascadeOnUpdate();

                table.integer(u"seeds"_s).nullable();
                table.integer(u"total_seeds"_s).nullable();

                table.integer(u"leechers"_s);
                table.integer(u"total_leechers"_s);

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"torrent_peers"_s);
        }
    };

} // namespace Migrations
