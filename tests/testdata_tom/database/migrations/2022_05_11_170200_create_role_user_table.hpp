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
            Schema::create(u"role_user"_s, [](Blueprint &table)
            {
                table.foreignId(u"role_id"_s).constrained()
                     .cascadeOnDelete().cascadeOnUpdate();
                table.foreignId(u"user_id"_s).constrained()
                     .cascadeOnDelete().cascadeOnUpdate();

                table.boolean(u"active"_s).defaultValue(true);

                // Indexes
                table.primary({u"role_id"_s, u"user_id"_s});
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"role_user"_s);
        }
    };

} // namespace Migrations
