#pragma once

#include <orm/db.hpp>
#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateFilePropertyPropertiesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create(u"file_property_properties"_s, [](Blueprint &table)
            {
                table.id();

                table.unsignedBigInteger(u"file_property_id"_s);

                table.string(NAME).unique();
                table.unsignedBigInteger(u"value"_s);

                table.timestamps();

                // Indexes
                table.foreign(u"file_property_id"_s)
                     .references(ID).on(u"torrent_previewable_file_properties"_s)
                     .cascadeOnDelete().cascadeOnUpdate();
            });

            using Orm::DB;

            // Add the table comment (MySQL only)
            if (DB::getDefaultConnection() == "tinyorm_testdata_tom_mysql"_L1)
                DB::unprepared(u"alter table `file_property_properties` comment = "_s
                                "'used in Builder::chunk() tests, must have exactly 8 "
                                "rows'");
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"file_property_properties"_s);
        }
    };

} // namespace Migrations
