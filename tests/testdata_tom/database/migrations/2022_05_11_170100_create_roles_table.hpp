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
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("roles");
        }
    };

} // namespace Migrations
