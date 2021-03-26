#ifndef DATABASE_H
#define DATABASE_H

#include "orm/connectioninterface.hpp"

#include "utils_global.hpp"

namespace TestUtils
{

    class UTILS_EXPORT Database
    {
        Q_DISABLE_COPY(Database)

    public:
        /*! Create all database connections which will be tested. */
        static const QStringList &createConnections();
    };

}
#endif // DATABASE_H
