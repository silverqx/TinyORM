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
            Schema::create(u"role_tag"_s, [](Blueprint &table)
            {
                table.foreignId(u"tag_id"_s).constrained(u"torrent_tags"_s)
                     .cascadeOnDelete().cascadeOnUpdate();
                table.foreignId(u"role_id"_s).constrained()
                     .cascadeOnDelete().cascadeOnUpdate();

                table.boolean(u"active"_s).defaultValue(false);

                // Indexes
                table.primary({u"tag_id"_s, u"role_id"_s});
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"role_tag"_s);
        }
    };

} // namespace Migrations
