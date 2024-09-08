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
            Schema::create(u"torrents"_s, [](Blueprint &table)
            {
                table.id();

                table.foreignId(u"user_id"_s).nullable()
                     .constrained().cascadeOnDelete().cascadeOnUpdate();

                table.string(NAME).unique().comment(u"Torrent name"_s);
                table.unsignedBigInteger(SIZE_).defaultValue(u"0"_s);
                table.unsignedSmallInteger(Progress).defaultValue(u"0"_s);
                table.datetime(AddedOn).useCurrent();
                table.string(HASH_, 40);
                table.string(NOTE).nullable();

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"torrents"_s);
        }
    };

} // namespace Migrations
