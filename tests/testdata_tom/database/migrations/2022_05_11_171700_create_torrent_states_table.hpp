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
            Schema::create(u"torrent_states"_s, [](Blueprint &table)
            {
                table.id();

                table.string(NAME).unique();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"torrent_states"_s);
        }
    };

} // namespace Migrations
