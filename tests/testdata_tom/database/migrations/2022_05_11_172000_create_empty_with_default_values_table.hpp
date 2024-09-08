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
            Schema::create(u"empty_with_default_values"_s, [](Blueprint &table)
            {
                table.id();

                table.foreignId(u"user_id"_s).nullable().constrained()
                     .cascadeOnDelete().cascadeOnUpdate();

                table.unsignedBigInteger(SIZE_).defaultValue(u"0"_s);
                table.decimal(u"decimal"_s).defaultValue(u"100.12"_s).nullable();
                table.datetime(AddedOn).useCurrent();
                table.string(NOTE).nullable();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"empty_with_default_values"_s);
        }
    };

} // namespace Migrations
