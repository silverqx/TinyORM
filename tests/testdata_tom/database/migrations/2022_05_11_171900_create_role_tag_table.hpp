#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateRoleTagTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("role_tag", [](Blueprint &table)
            {
                table.foreignId("tag_id").constrained("torrent_tags")
                     .cascadeOnDelete().cascadeOnUpdate();
                table.foreignId("role_id").constrained()
                     .cascadeOnDelete().cascadeOnUpdate();

                table.boolean("active").defaultValue(false);

                // Indexes
                table.primary({"tag_id", "role_id"});
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("role_tag");
        }
    };

} // namespace Migrations
