#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreatePhonesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create(u"phones"_s, [](Blueprint &table)
            {
                table.id();

                table.string(NAME);
                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"phones"_s);
        }
    };

} // namespace Migrations
