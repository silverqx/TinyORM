#pragma once
#ifndef TOM_COMMANDS_MAKE_STUBS_SEEDERSTUBS_HPP
#define TOM_COMMANDS_MAKE_STUBS_SEEDERSTUBS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Stubs
{

using Qt::StringLiterals::operator""_s;

/*! Seeder stub. */
inline const auto SeederStub = uR"(#pragma once

#include <tom/seeder.hpp>

namespace Seeders
{

    struct {{ class }} : Seeder
    {
        /*! Run the database seeders. */
        void run() override
        {
            // DB::table("{{ table }}")->insert();
        }
    };

} // namespace Seeders
)"_s;

} // namespace Tom::Commands::Make::Stubs

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_STUBS_SEEDERSTUBS_HPP
