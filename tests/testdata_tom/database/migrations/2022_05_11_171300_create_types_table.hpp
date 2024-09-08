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
            Schema::create(u"types"_s, [](Blueprint &table)
            {
                table.id();

                table.boolean(u"bool_true"_s).nullable();
                table.boolean(u"bool_false"_s).nullable();

                table.smallInteger(u"smallint"_s).nullable();
                table.unsignedSmallInteger(u"smallint_u"_s).nullable();
                table.integer(u"int"_s).nullable();
                table.unsignedInteger(u"int_u"_s).nullable();
                table.bigInteger(u"bigint"_s).nullable();
                table.unsignedBigInteger(u"bigint_u"_s).nullable();

                table.Double(u"double"_s).nullable();
                table.Double(u"double_nan"_s).nullable();
                table.Double(u"double_infinity"_s).nullable();

                table.decimal(u"decimal"_s).nullable();
                table.decimal(u"decimal_nan"_s).nullable();
                table.decimal(u"decimal_infinity"_s, std::nullopt,
                                                     std::nullopt).nullable();
                table.decimal(u"decimal_down"_s).nullable();
                table.decimal(u"decimal_up"_s).nullable();

                table.string(u"string"_s).nullable();
                table.text(u"text"_s).nullable();
                table.mediumText(u"medium_text"_s).nullable();

                table.timestamp(u"timestamp"_s).nullable();

                table.datetime(u"datetime"_s).nullable();
                table.date(u"date"_s).nullable();
                table.time(u"time"_s).nullable();

                table.binary(u"binary"_s).nullable();
                table.mediumBinary(u"medium_binary"_s).nullable();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"types"_s);
        }
    };

} // namespace Migrations
