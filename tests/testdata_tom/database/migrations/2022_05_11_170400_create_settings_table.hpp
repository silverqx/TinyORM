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
            Schema::create(u"settings"_s, [](Blueprint &table)
            {
                table.string(NAME).defaultValue("").unique();
                table.string(u"value"_s).defaultValue("");

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"settings"_s);
        }
    };

} // namespace Migrations
