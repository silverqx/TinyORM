#pragma once
#ifndef TOM_COMMANDS_MAKE_STUBS_MIGRATIONSTUBS_HPP
#define TOM_COMMANDS_MAKE_STUBS_MIGRATIONSTUBS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Stubs
{

using Qt::StringLiterals::operator""_s;

/*! Empty migration stub. */
inline const auto MigrationStub = uR"(#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct {{ class }} : Migration
    {
        /*! Filename of the migration file. */
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            //
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            //
        }
    };

} // namespace Migrations
)"_s;

/*! Migration stub for creating a new table. */
inline const auto MigrationCreateStub = uR"(#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct {{ class }} : Migration
    {
        /*! Filename of the migration file. */
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("{{ table }}", [](Blueprint &table)
            {
                table.id();
                table.timestamps();
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::dropIfExists("{{ table }}");
        }
    };

} // namespace Migrations
)"_s;

/*! Migration stub for updating an existing table. */
inline const auto MigrationUpdateStub = uR"(#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct {{ class }} : Migration
    {
        /*! Filename of the migration file. */
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::table("{{ table }}", [](Blueprint &table)
            {
                //
            });
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            Schema::table("{{ table }}", [](Blueprint &table)
            {
                //
            });
        }
    };

} // namespace Migrations
)"_s;

} // namespace Tom::Commands::Make::Stubs

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_STUBS_MIGRATIONSTUBS_HPP
