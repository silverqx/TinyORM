#include "orm/concerns/detectslostconnections.hpp"

#include <QVector>

#include "orm/exceptions/sqlerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

bool DetectsLostConnections::causedByLostConnection(const Exceptions::SqlError &e)
{
    return causedByLostConnection(e.getSqlError());
}

bool DetectsLostConnections::causedByLostConnection(const QSqlError &e)
{
    // TODO verify this will be pain in the ass 😕, but but it looks like few of them for mysql and postgres are completly valid silverqx
    static const QVector<QString> lostMessagesCache {
        QLatin1String("server has gone away"),
        QLatin1String("no connection to the server"),
        QLatin1String("Lost connection"),
        QLatin1String("is dead or not enabled"),
        QLatin1String("Error while sending"),
        QLatin1String("decryption failed or bad record mac"),
        QLatin1String("server closed the connection unexpectedly"),
        QLatin1String("SSL connection has been closed unexpectedly"),
        QLatin1String("Error writing data to the connection"),
        QLatin1String("Resource deadlock avoided"),
        QLatin1String("Transaction() on null"),
        QLatin1String("child connection forced to terminate due to client_idle_limit"),
        QLatin1String("query_wait_timeout"),
        QLatin1String("reset by peer"),
        QLatin1String("Physical connection is not usable"),
        QLatin1String("Packets out of order. Expected"),
        QLatin1String("Adaptive Server connection failed"),
        QLatin1String("Communication link failure"),
        QLatin1String("connection is no longer usable"),
        QLatin1String("Login timeout expired"),
        QLatin1String("running with the --read-only option so it cannot execute this statement"),
        QLatin1String("The connection is broken and recovery is not possible. The connection is marked by the client driver as unrecoverable. No attempt was made to restore the connection."),
        QLatin1String("SSL: Connection timed out"),
        QLatin1String("Temporary failure in name resolution"),
        QLatin1String("SSL: Broken pipe"),
        QLatin1String("The client was disconnected by the server because of inactivity. See wait_timeout and interactive_timeout for configuring this behavior."),
        QLatin1String("SSL: Operation timed out"),
        QLatin1String("SSL: Handshake timed out"),
    };

    return std::ranges::any_of(lostMessagesCache,
                               [databaseError = e.databaseText()]
                               (const auto &lostMessage)
    {
        // found
        return databaseError.indexOf(lostMessage, 0, Qt::CaseInsensitive) >= 0;
    });
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
