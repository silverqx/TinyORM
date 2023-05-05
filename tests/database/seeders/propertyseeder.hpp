#pragma once

#include <tom/seeder.hpp>

namespace Seeders
{

    struct PropertySeeder : Seeder
    {
        /*! Run the database seeders. */
        void run() override
        {
            DB::table("properties")->insert({
                {{NAME, "1. property"}},
                {{NAME, "2. property"}},
            });
        }
    };

} // namespace Seeders
