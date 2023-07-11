#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreateTorrentPreviewableFilesTable : Migration
    {
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("torrent_previewable_files", [](Blueprint &table)
            {
                table.id();

                table.foreignId("torrent_id").nullable()
                     .constrained().cascadeOnDelete().cascadeOnUpdate();

                table.integer("file_index");
                table.string("filepath").unique();

                table.unsignedBigInteger(SIZE_);
                table.unsignedSmallInteger(Progress);

                table.string(NOTE).nullable();

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("torrent_previewable_files");
        }
    };

} // namespace Migrations
