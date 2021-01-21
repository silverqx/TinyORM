#ifndef DATABASE_H
#define DATABASE_H

#include "orm/connectioninterface.hpp"

#include "utils_global.hpp"

namespace TestUtils
{

    class UTILS_EXPORT Database
    {
    public:
        Database();

        static Orm::ConnectionInterface &createConnection();
    };

}
#endif // DATABASE_H
