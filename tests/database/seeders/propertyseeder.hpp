#pragma once

#include <tom/seeder.hpp>

namespace Seeders
{

    struct PropertySeeder : Seeder
    {
        /*! Run the database seeders. */
        void run() override
        {
            DB::table(u"properties"_s)->insert({
                {{NAME, u"1. property"_s}},
                {{NAME, u"2. property"_s}},
            });
        }
    };

} // namespace Seeders
