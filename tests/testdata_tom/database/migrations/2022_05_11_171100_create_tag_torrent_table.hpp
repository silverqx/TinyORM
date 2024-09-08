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
            Schema::create(u"tag_torrent"_s, [](Blueprint &table)
            {
                // Don't use the terser methods syntax here, leave it this way as example
                table.unsignedBigInteger(u"torrent_id"_s);
                table.unsignedBigInteger(u"tag_id"_s);

                table.boolean(u"active"_s).defaultValue(true);

                table.timestamps();

                // Indexes
                table.primary({u"torrent_id"_s, u"tag_id"_s});

                table.foreign(u"torrent_id"_s)
                     .references(ID).on(u"torrents"_s)
                     .cascadeOnDelete().cascadeOnUpdate();
                table.foreign(u"tag_id"_s)
                     .references(ID).on(u"torrent_tags"_s)
                     .cascadeOnDelete().cascadeOnUpdate();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"tag_torrent"_s);
        }
    };

} // namespace Migrations
