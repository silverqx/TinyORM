#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateEmptyWithDefaultValuesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("empty_with_default_values", [](Blueprint &table)
            {
                table.id();

                table.foreignId("user_id").nullable()
                     .constrained().cascadeOnDelete().cascadeOnUpdate();

                table.unsignedBigInteger(SIZE_).defaultValue("0");
                table.decimal("decimal").defaultValue("100.12").nullable();
                table.datetime("added_on").useCurrent();
                table.string(NOTE).nullable();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("empty_with_default_values");
        }
    };

} // namespace Migrations
