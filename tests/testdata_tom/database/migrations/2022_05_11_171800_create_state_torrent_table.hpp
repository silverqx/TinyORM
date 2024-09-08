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
            Schema::create(u"state_torrent"_s, [](Blueprint &table)
            {
                table.unsignedBigInteger(u"torrent_id"_s);
                table.unsignedBigInteger(u"state_id"_s);

                table.boolean(u"active"_s).defaultValue(false);

                // Indexes
                table.primary({u"torrent_id"_s, u"state_id"_s});

                table.foreign(u"torrent_id"_s)
                     .references(ID).on(u"torrents"_s)
                     .cascadeOnDelete().cascadeOnUpdate();
                table.foreign(u"state_id"_s)
                     .references(ID).on(u"torrent_states"_s)
                     .cascadeOnDelete().cascadeOnUpdate();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"state_torrent"_s);
        }
    };

} // namespace Migrations
