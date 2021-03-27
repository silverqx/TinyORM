#ifndef DATABASE_H
#define DATABASE_H

#include "orm/configuration.hpp"
#include "orm/connectioninterface.hpp"

#include "utils_global.hpp"

namespace TestUtils
{

    class UTILS_EXPORT Database
    {
        Q_DISABLE_COPY(Database)

    public:
        /*! Create all database connections which will be tested. */
        static const QStringList &
        createConnections(const QStringList &connections = {});
        /*! Create database connection. */
        inline static QString createConnection(const QString &connection)
        { return createConnections({connection}).first(); }

    private:
        /*! Obtain configurations for the given connection names. */
        static const Orm::ConfigurationsType &
        getConfigurations(const QStringList &connections = {});
    };

}
#endif // DATABASE_H
