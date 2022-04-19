#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct _2014_10_12_000000_create_posts_table : Migration
    {
        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("posts", [](Blueprint &table)
            {
                table.id();

                table.string(NAME);
                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("posts");
        }
    };

} // namespace Migrations
