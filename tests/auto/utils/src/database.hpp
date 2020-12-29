#ifndef DATABASE_H
#define DATABASE_H

#include "utils_global.hpp"

namespace Orm
{
    class DatabaseConnection;
}

namespace Utils
{

    class UTILS_EXPORT Database
    {
    public:
        Database();

        static Orm::DatabaseConnection &createConnection();
    };

}
#endif // DATABASE_H
