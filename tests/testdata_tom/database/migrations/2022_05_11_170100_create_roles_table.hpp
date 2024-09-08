#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateRolesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create(u"roles"_s, [](Blueprint &table)
            {
                table.id();

                table.string(NAME).unique();

                // To test Unix timestamps, u_dateFormat = 'U'
                table.bigInteger(AddedOn).nullable()
                        .comment(u"To test Unix timestamps, u_dateFormat = 'U'"_s);
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"roles"_s);
        }
    };

} // namespace Migrations
