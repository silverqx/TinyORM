#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateSettingsTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("settings", [](Blueprint &table)
            {
                table.string(NAME).defaultValue("").unique();
                table.string("value").defaultValue("");

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("settings");
        }
    };

} // namespace Migrations
