#include "orm/concerns/detectslostconnections.hpp"

#include <QString>

#include <array>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

bool DetectsLostConnections::causedByLostConnection(const QString &errorMessage)
{
    using Qt::StringLiterals::operator""_L1;

    /* The _L1 allows constinit and is faster then the const char * during the contains()
       call, but all case-insensitive comparisons are very slow anyway. */

    // TODO verify this will be pain in the ass 😕, but but it looks like few of them for mysql and postgres are completly valid silverqx
    constinit static const std::array lostMessagesCache = std::to_array({
        "server has gone away"_L1,
        "no connection to the server"_L1,
        "Lost connection"_L1,
        "is dead or not enabled"_L1,
        "Error while sending"_L1,
        "decryption failed or bad record mac"_L1,
        "server closed the connection unexpectedly"_L1,
        "SSL connection has been closed unexpectedly"_L1,
        "Error writing data to the connection"_L1,
        "Resource deadlock avoided"_L1,
        "Transaction() on null"_L1,
        "child connection forced to terminate due to client_idle_limit"_L1,
        "query_wait_timeout"_L1,
        "reset by peer"_L1,
        "Physical connection is not usable"_L1,
        "Packets out of order. Expected"_L1,
        "Adaptive Server connection failed"_L1,
        "Communication link failure"_L1,
        "connection is no longer usable"_L1,
        "Login timeout expired"_L1,
        "running with the --read-only option so it cannot execute this statement"_L1,
        "The connection is broken and recovery is not possible. The connection is marked by the client driver as unrecoverable. No attempt was made to restore the connection."_L1,
        "SSL: Connection timed out"_L1,
        "Temporary failure in name resolution"_L1,
        "SSL: Broken pipe"_L1,
        "The client was disconnected by the server because of inactivity. See wait_timeout and interactive_timeout for configuring this behavior."_L1,
        "SSL: Operation timed out"_L1,
        "SSL: Handshake timed out"_L1,
    });

    return std::ranges::any_of(lostMessagesCache,
                               [&errorMessage](const QLatin1StringView lostMessage)
    {
        // found
        return errorMessage.contains(lostMessage, Qt::CaseInsensitive);
    });
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
