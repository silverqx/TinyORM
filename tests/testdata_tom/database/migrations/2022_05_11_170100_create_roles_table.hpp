#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateRolesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("roles", [](Blueprint &table)
            {
                table.id();

                table.string(NAME).unique();

                // To test Unix timestamps, u_dateFormat = 'U'
                table.bigInteger("added_on").nullable()
                        .comment("To test Unix timestamps, u_dateFormat = 'U'");
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("roles");
        }
    };

} // namespace Migrations
