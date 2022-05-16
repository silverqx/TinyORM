#pragma once

#include <tom/seeder.hpp>

#include "seeders/phonesseeder.hpp"
#include "seeders/propertiesseeder.hpp"

/* This class serves as a showcase, so all possible features are defined / used. */

namespace Seeders
{

    /*! Main database seeder. */
    struct DatabaseSeeder : Seeder
    {
        /*! Run the database seeders. */
        void run() override
        {
            DB::table("posts")->insert({
                {{"name", "1. post"}, {"factor", 10}},
                {{"name", "2. post"}, {"factor", 20}},
            });

            call<PhonesSeeder, PropertiesSeeder>();

            // You can also pass arguments to the call() related methods
//            callWith<UsersSeeder>(shouldSeedPasswd);
        }
    };

} // namespace Seeders
