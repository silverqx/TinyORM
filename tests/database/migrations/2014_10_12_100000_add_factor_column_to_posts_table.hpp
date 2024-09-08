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
            Schema::table(u"posts"_s, [](Blueprint &table)
            {
                table.integer(u"factor"_s);
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::table(u"posts"_s, [](Blueprint &table)
            {
                table.dropColumn(u"factor"_s);
            });
        }
    };

} // namespace Migrations
