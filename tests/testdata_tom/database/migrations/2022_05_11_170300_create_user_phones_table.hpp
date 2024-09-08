#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateUserPhonesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create(u"user_phones"_s, [](Blueprint &table)
            {
                table.id();

                table.foreignId(u"user_id"_s).constrained().cascadeOnDelete().cascadeOnUpdate();

                table.string(u"number"_s).unique();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"user_phones"_s);
        }
    };

} // namespace Migrations
