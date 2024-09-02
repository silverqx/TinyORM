#include "orm/concerns/detectslostconnections.hpp"

#include <QList>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

bool DetectsLostConnections::causedByLostConnection(const QString &errorMessage)
{
    using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

    // TODO verify this will be pain in the ass 😕, but but it looks like few of them for mysql and postgres are completly valid silverqx
    static const QList<QString> lostMessagesCache {
        u"server has gone away"_s,
        u"no connection to the server"_s,
        u"Lost connection"_s,
        u"is dead or not enabled"_s,
        u"Error while sending"_s,
        u"decryption failed or bad record mac"_s,
        u"server closed the connection unexpectedly"_s,
        u"SSL connection has been closed unexpectedly"_s,
        u"Error writing data to the connection"_s,
        u"Resource deadlock avoided"_s,
        u"Transaction() on null"_s,
        u"child connection forced to terminate due to client_idle_limit"_s,
        u"query_wait_timeout"_s,
        u"reset by peer"_s,
        u"Physical connection is not usable"_s,
        u"Packets out of order. Expected"_s,
        u"Adaptive Server connection failed"_s,
        u"Communication link failure"_s,
        u"connection is no longer usable"_s,
        u"Login timeout expired"_s,
        u"running with the --read-only option so it cannot execute this statement"_s,
        u"The connection is broken and recovery is not possible. The connection is marked by the client driver as unrecoverable. No attempt was made to restore the connection."_s,
        u"SSL: Connection timed out"_s,
        u"Temporary failure in name resolution"_s,
        u"SSL: Broken pipe"_s,
        u"The client was disconnected by the server because of inactivity. See wait_timeout and interactive_timeout for configuring this behavior."_s,
        u"SSL: Operation timed out"_s,
        u"SSL: Handshake timed out"_s,
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
