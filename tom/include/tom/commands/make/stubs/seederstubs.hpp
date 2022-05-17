#pragma once
#ifndef TOM_COMMANDS_MAKE_STUBS_SEEDERSTUBS_HPP
#define TOM_COMMANDS_MAKE_STUBS_SEEDERSTUBS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Stubs
{

/*! Seeder stub. */
inline const auto *const SeederStub = R"(#pragma once

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
)";

} // namespace Tom::Commands::Make::Stubs

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_STUBS_SEEDERSTUBS_HPP
