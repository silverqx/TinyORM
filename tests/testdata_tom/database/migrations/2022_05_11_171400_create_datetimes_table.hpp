#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateDatetimeTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create(u"datetimes"_s, [](Blueprint &table)
            {
                table.id();

                table.datetime(u"datetime"_s).nullable();
                table.datetimeTz(u"datetime_tz"_s).nullable();

                table.timestamp(u"timestamp"_s).nullable();
                table.timestampTz(u"timestamp_tz"_s).nullable();

                table.date(u"date"_s).nullable();
                table.time(u"time"_s).nullable();
                table.time(u"time_ms"_s, 3).nullable();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"datetimes"_s);
        }
    };

} // namespace Migrations
