#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreatePhonesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("phones", [](Blueprint &table)
            {
                table.id();

                table.string(NAME);
                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("phones");
        }
    };

} // namespace Migrations
