#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct _2014_10_12_200000_create_properties_table : Migration
    {
        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("properties", [](Blueprint &table)
            {
                table.id();

                table.string(NAME);
                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("properties");
        }
    };

} // namespace Migrations
