#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateRoleUserTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("role_user", [](Blueprint &table)
            {
                table.foreignId("role_id").constrained().cascadeOnDelete().cascadeOnUpdate();
                table.foreignId("user_id").constrained().cascadeOnDelete().cascadeOnUpdate();

                table.boolean("active").defaultValue(true);

                // Indexes
                table.primary({"role_id", "user_id"});
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("role_user");
        }
    };

} // namespace Migrations
