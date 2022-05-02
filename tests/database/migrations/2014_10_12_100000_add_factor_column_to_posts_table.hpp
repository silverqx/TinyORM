#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct AddFactorColumnToPostsTable : Migration
    {
        T_MIGRATION

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
