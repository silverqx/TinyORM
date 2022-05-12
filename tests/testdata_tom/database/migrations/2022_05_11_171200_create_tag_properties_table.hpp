#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateTagPropertiesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("tag_properties", [](Blueprint &table)
            {
                table.id();

                table.unsignedBigInteger("tag_id");

                table.string("color");
                table.unsignedInteger("position").unique();

                table.timestamps();

                // Indexes
                table.foreign("tag_id")
                     .references(ID).on("torrent_tags")
                     .cascadeOnDelete().cascadeOnUpdate();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("tag_properties");
        }
    };

} // namespace Migrations
