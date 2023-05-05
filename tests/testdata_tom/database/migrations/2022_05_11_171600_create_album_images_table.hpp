#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateAlbumImagesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("album_images", [](Blueprint &table)
            {
                table.id();

                table.foreignId("album_id").nullable()
                     .constrained().cascadeOnDelete().cascadeOnUpdate();

                table.string(NAME).unique();
                table.string("ext");
                table.unsignedBigInteger(SIZE_);

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("album_images");
        }
    };

} // namespace Migrations
