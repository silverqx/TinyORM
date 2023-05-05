#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateUsersTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("users", [](Blueprint &table)
            {
                table.id();

                table.string(NAME).unique();
                table.boolean("is_banned").defaultValue(false);
                table.string(NOTE).nullable();

                table.timestamps();
                table.softDeletes();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("users");
        }
    };

} // namespace Migrations
