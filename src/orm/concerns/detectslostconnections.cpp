#include "orm/concerns/detectslostconnections.hpp"

#include "orm/sqlerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Concerns
{

bool DetectsLostConnections::causedByLostConnection(const SqlError &e) const
{
    // TODO verify this will be pain in the ass 😕 silverqx
    static const QVector<QString> lostMessagesCache {
        "server has gone away",
        "no connection to the server",
        "Lost connection",
        "is dead or not enabled",
        "Error while sending",
        "decryption failed or bad record mac",
        "server closed the connection unexpectedly",
        "SSL connection has been closed unexpectedly",
        "Error writing data to the connection",
        "Resource deadlock avoided",
        "Transaction() on null",
        "child connection forced to terminate due to client_idle_limit",
        "query_wait_timeout",
        "reset by peer",
        "Physical connection is not usable",
        "TCP Provider: Error code 0x68",
        "ORA-03114",
        "Packets out of order. Expected",
        "Adaptive Server connection failed",
        "Communication link failure",
        "connection is no longer usable",
        "Login timeout expired",
        "SQLSTATE[HY000] [2002] Connection refused",
        "running with the --read-only option so it cannot execute this statement",
        "The connection is broken and recovery is not possible. The connection is marked by the client driver as unrecoverable. No attempt was made to restore the connection.",
        "SQLSTATE[HY000] [2002] php_network_getaddresses: getaddrinfo failed: Try again",
        "SQLSTATE[HY000]: General error: 7 SSL SYSCALL error: EOF detected",
        "SQLSTATE[HY000] [2002] Connection timed out",
        "SSL: Connection timed out",
    };

    for (const auto databaseError = e.getSqlError().databaseText();
         const auto &lostMessage : lostMessagesCache
    )
        // found
        if (databaseError.indexOf(lostMessage, 0, Qt::CaseInsensitive) >= 0)
            return true;

    return false;
}

} // namespace Orm::Concerns
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
