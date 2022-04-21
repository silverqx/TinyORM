#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct _2014_10_12_300000_create_phones_table : Migration
    {
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
