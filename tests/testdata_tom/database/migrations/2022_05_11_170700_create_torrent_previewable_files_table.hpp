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
            Schema::create(u"torrent_previewable_files"_s, [](Blueprint &table)
            {
                table.id();

                table.foreignId(u"torrent_id"_s).nullable()
                     .constrained().cascadeOnDelete().cascadeOnUpdate();

                table.integer(u"file_index"_s);
                table.string(u"filepath"_s).unique();

                table.unsignedBigInteger(SIZE_);
                table.unsignedSmallInteger(Progress);

                table.string(NOTE).nullable();

                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"torrent_previewable_files"_s);
        }
    };

} // namespace Migrations
