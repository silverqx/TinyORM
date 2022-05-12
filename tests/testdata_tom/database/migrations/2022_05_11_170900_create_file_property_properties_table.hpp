#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateFilePropertyPropertiesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("file_property_properties", [](Blueprint &table)
            {
                table.id();

                table.unsignedBigInteger("file_property_id");

                table.string(NAME).unique();
                table.unsignedBigInteger("value");

                table.timestamps();

                // Indexes
                table.foreign("file_property_id")
                     .references(ID).on("torrent_previewable_file_properties")
                     .cascadeOnDelete().cascadeOnUpdate();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("file_property_properties");
        }
    };

} // namespace Migrations
