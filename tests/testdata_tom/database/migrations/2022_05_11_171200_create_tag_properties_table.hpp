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
            Schema::create(u"tag_properties"_s, [](Blueprint &table)
            {
                table.id();

                table.unsignedBigInteger(u"tag_id"_s);

                table.string(u"color"_s);
                table.unsignedInteger(u"position"_s).unique();

                table.timestamps();

                // Indexes
                table.foreign(u"tag_id"_s)
                     .references(ID).on(u"torrent_tags"_s)
                     .cascadeOnDelete().cascadeOnUpdate();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists(u"tag_properties"_s);
        }
    };

} // namespace Migrations
