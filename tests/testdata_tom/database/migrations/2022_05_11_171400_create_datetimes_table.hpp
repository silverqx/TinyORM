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
            Schema::create("datetimes", [](Blueprint &table)
            {
                table.id();

                table.datetime("datetime").nullable();
                table.datetimeTz("datetime_tz").nullable();

                table.timestamp("timestamp").nullable();
                table.timestampTz("timestamp_tz").nullable();

                table.date("date").nullable();
                table.time("time").nullable();
                table.time("time_ms", 3).nullable();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("datetimes");
        }
    };

} // namespace Migrations
