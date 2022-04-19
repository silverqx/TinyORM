#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct _2014_10_12_100000_add_factor_column_to_posts_table : Migration
    {
        /*! Run the migrations. */
        void up() const override
        {
            Schema::table("posts", [](Blueprint &table)
            {
                table.integer("factor");
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::table("posts", [](Blueprint &table)
            {
                table.dropColumn("factor");
            });
        }
    };

} // namespace Migrations
