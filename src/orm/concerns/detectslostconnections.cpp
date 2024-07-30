#include "orm/concerns/detectslostconnections.hpp"

#include <QList>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

bool DetectsLostConnections::causedByLostConnection(const QString &errorMessage)
{
    // TODO verify this will be pain in the ass 😕, but but it looks like few of them for mysql and postgres are completly valid silverqx
    static const QList<QString> lostMessagesCache {
        QStringLiteral("server has gone away"),
        QStringLiteral("no connection to the server"),
        QStringLiteral("Lost connection"),
        QStringLiteral("is dead or not enabled"),
        QStringLiteral("Error while sending"),
        QStringLiteral("decryption failed or bad record mac"),
        QStringLiteral("server closed the connection unexpectedly"),
        QStringLiteral("SSL connection has been closed unexpectedly"),
        QStringLiteral("Error writing data to the connection"),
        QStringLiteral("Resource deadlock avoided"),
        QStringLiteral("Transaction() on null"),
        QStringLiteral("child connection forced to terminate due to client_idle_limit"),
        QStringLiteral("query_wait_timeout"),
        QStringLiteral("reset by peer"),
        QStringLiteral("Physical connection is not usable"),
        QStringLiteral("Packets out of order. Expected"),
        QStringLiteral("Adaptive Server connection failed"),
        QStringLiteral("Communication link failure"),
        QStringLiteral("connection is no longer usable"),
        QStringLiteral("Login timeout expired"),
        QStringLiteral("running with the --read-only option so it cannot execute this statement"),
        QStringLiteral("The connection is broken and recovery is not possible. The connection is marked by the client driver as unrecoverable. No attempt was made to restore the connection."),
        QStringLiteral("SSL: Connection timed out"),
        QStringLiteral("Temporary failure in name resolution"),
        QStringLiteral("SSL: Broken pipe"),
        QStringLiteral("The client was disconnected by the server because of inactivity. See wait_timeout and interactive_timeout for configuring this behavior."),
        QStringLiteral("SSL: Operation timed out"),
        QStringLiteral("SSL: Handshake timed out"),
    };

    return std::ranges::any_of(lostMessagesCache,
                               [&errorMessage](const auto &lostMessage)
    {
        // found
        return errorMessage.indexOf(lostMessage, 0, Qt::CaseInsensitive) >= 0;
    });
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
