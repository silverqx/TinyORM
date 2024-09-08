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
            Schema::create(u"album_images"_s, [](Blueprint &table)
            {
                table.id();

                table.foreignId(u"album_id"_s).nullable().constrained()
                     .cascadeOnDelete().cascadeOnUpdate();

                table.string(NAME).unique();
                table.string(u"ext"_s);
                table.unsignedBigInteger(SIZE_);

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"album_images"_s);
        }
    };

} // namespace Migrations
