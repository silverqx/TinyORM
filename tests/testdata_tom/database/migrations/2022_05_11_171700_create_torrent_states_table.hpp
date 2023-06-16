#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateTorrentStatesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("torrent_states", [](Blueprint &table)
            {
                table.id();

                table.string(NAME).unique();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("torrent_states");
        }
    };

} // namespace Migrations
