#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateUserPhonesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("user_phones", [](Blueprint &table)
            {
                table.id();

                table.foreignId("user_id").constrained().cascadeOnDelete().cascadeOnUpdate();

                table.string("number").unique();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("user_phones");
        }
    };

} // namespace Migrations
