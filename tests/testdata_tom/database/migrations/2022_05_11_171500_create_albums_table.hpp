#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateAlbumsTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create(u"albums"_s, [](Blueprint &table)
            {
                table.id();

                table.string(NAME).unique();
                table.string(NOTE).nullable();

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"albums"_s);
        }
    };

} // namespace Migrations
