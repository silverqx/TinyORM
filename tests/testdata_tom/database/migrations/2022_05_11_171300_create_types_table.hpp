#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateTypesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("types", [](Blueprint &table)
            {
                table.id();

                table.boolean("bool_true").nullable();
                table.boolean("bool_false").nullable();

                table.smallInteger("smallint").nullable();
                table.unsignedSmallInteger("smallint_u").nullable();
                table.integer("int").nullable();
                table.unsignedInteger("int_u").nullable();
                table.bigInteger("bigint").nullable();
                table.unsignedBigInteger("bigint_u").nullable();

                table.Double("double").nullable();
                table.Double("double_nan").nullable();
                table.Double("double_infinity").nullable();

                table.decimal("decimal").nullable();
                table.decimal("decimal_nan").nullable();
                table.decimal("decimal_infinity", std::nullopt, std::nullopt).nullable();
                table.decimal("decimal_down").nullable();
                table.decimal("decimal_up").nullable();

                table.string("string").nullable();
                table.text("text").nullable();
                table.mediumText("medium_text").nullable();

                table.timestamp("timestamp").nullable();

                table.datetime("datetime").nullable();
                table.date("date").nullable();
                table.time("time").nullable();

                table.binary("binary").nullable();
                table.mediumBinary("medium_binary").nullable();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("types");
        }
    };

} // namespace Migrations
