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
            Schema::create(u"torrent_previewable_file_properties"_s, [](Blueprint &table)
            {
                table.id();

                table.unsignedBigInteger(u"previewable_file_id"_s);

                table.string(NAME).unique();
                table.unsignedBigInteger(SIZE_);

                table.timestamps();

                // Indexes
                table.foreign(u"previewable_file_id"_s)
                     .references(ID).on(u"torrent_previewable_files"_s)
                     .cascadeOnDelete().cascadeOnUpdate();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"torrent_previewable_file_properties"_s);
        }
    };

} // namespace Migrations
