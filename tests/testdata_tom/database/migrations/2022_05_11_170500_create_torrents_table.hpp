#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateTorrentsTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("torrents", [](Blueprint &table)
            {
                table.id();

                table.foreignId("user_id").nullable()
                     .constrained().cascadeOnDelete().cascadeOnUpdate();

                table.string(NAME).unique().comment("Torrent name");
                table.unsignedBigInteger(SIZE_).defaultValue("0");
                table.unsignedSmallInteger(Progress).defaultValue("0");
                table.datetime("added_on").useCurrent();
                table.string(HASH_, 40);
                table.string(NOTE).nullable();

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("torrents");
        }
    };

} // namespace Migrations
