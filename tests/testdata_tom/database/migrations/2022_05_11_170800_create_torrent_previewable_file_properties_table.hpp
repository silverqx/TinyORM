#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateTorrentPreviewableFilePropertiesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("torrent_previewable_file_properties", [](Blueprint &table)
            {
                table.id();

                table.unsignedBigInteger("previewable_file_id");

                table.string(NAME).unique();
                table.unsignedBigInteger(SIZE_);

                table.timestamps();

                // Indexes
                table.foreign("previewable_file_id")
                     .references(ID).on("torrent_previewable_files")
                     .cascadeOnDelete().cascadeOnUpdate();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("torrent_previewable_file_properties");
        }
    };

} // namespace Migrations
