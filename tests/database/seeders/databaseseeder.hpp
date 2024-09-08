#pragma once

#include <tom/seeder.hpp>

#include "seeders/phoneseeder.hpp"
#include "seeders/propertyseeder.hpp"

/* This class serves as a showcase, so all possible features are defined / used. */

namespace Seeders
{

    /*! Main database seeder. */
    struct DatabaseSeeder : Seeder
    {
        /*! Run the database seeders. */
        void run() override
        {
            DB::table(u"posts"_s)->insert({
                {{NAME, u"1. post"_s}, {u"factor"_s, 10}},
                {{NAME, u"2. post"_s}, {u"factor"_s, 20}},
            });

            call<PhoneSeeder, PropertySeeder>();

            // You can also pass arguments to the call() related methods
//            callWith<UsersSeeder>(shouldSeedPassword);
        }
    };

} // namespace Seeders
