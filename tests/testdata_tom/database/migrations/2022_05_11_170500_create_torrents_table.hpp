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
                table.unsignedSmallInteger("progress").defaultValue("0");
                table.dateTime("added_on").useCurrent();
                table.string("hash", 40);
                table.string("note").nullable();

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
