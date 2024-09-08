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
            Schema::create(u"users"_s, [](Blueprint &table)
            {
                table.id();

                table.string(NAME).unique();
                table.boolean(u"is_banned"_s).defaultValue(false);
                table.string(NOTE).nullable();

                table.timestamps();
                table.softDeletes();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"users"_s);
        }
    };

} // namespace Migrations
