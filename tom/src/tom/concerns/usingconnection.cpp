#include "tom/concerns/usingconnection.hpp"

#include <orm/databaseconnection.hpp>

#include "tom/concerns/interactswithio.hpp"
#include "tom/migrationrepository.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::DatabaseConnection;

namespace Tom::Concerns
{

/* public */

UsingConnection::UsingConnection(
        std::shared_ptr<ConnectionResolverInterface> &&connectionResolver
)
    : m_connectionResolver(std::move(connectionResolver))
{}

int UsingConnection::usingConnections(
        QStringList &&names, const bool debugSql,
        std::optional<std::reference_wrapper<MigrationRepository>> repository,
        const std::function<int(const QString &)> &callback)
{
    return usingConnectionsInternal(std::move(names), debugSql, repository, callback);
}

int UsingConnection::usingConnections(
        QStringList &&names, const bool debugSql,
        std::optional<std::reference_wrapper<MigrationRepository>> repository,
        const std::function<int()> &callback)
{
    return usingConnectionsInternal(std::move(names), debugSql, repository, callback);
}

int UsingConnection::usingConnections(
        QStringList &&names, const bool debugSql,
        const std::function<int(const QString &)> &callback)
{
    return usingConnections(std::move(names), debugSql, std::nullopt, callback);
}

int UsingConnection::usingConnections(QStringList &&names, const bool debugSql,
                                      const std::function<int()> &callback)
{
    return usingConnections(std::move(names), debugSql, std::nullopt, callback);
}

int UsingConnection::usingConnection(QString &&name, const bool debugSql,
                                     const std::function<int()> &callback)
{
    return usingConnectionInternal(std::move(name), debugSql, std::nullopt, callback);
}

int UsingConnection::usingConnection(const QString &name, const bool debugSql,
                                     const std::function<int()> &callback)
{
    return usingConnectionInternal(name, debugSql, std::nullopt, callback);
}

DatabaseConnection &UsingConnection::resolveConnection(const QString &name) const
{
    return m_connectionResolver->connection(name.isEmpty() ? m_connection : name);
}

/* private */

template<UsingConnectionCallback F>
int UsingConnection::usingConnectionsInternal(
        QStringList &&names, const bool debugSql,
        std::optional<std::reference_wrapper<MigrationRepository>> repository,
        const F &callback)
{
    int exitCode = EXIT_SUCCESS;
    const auto shouldPrintConnection = names.size() > 1;
    auto first = true;

    if (names.isEmpty())
        names << m_connectionResolver->getDefaultConnection();

    for (auto &&name : names) {
        // Visually divide individual connections
        printConnection(name, shouldPrintConnection, first);

        exitCode |= usingConnectionInternal(std::move(name), debugSql, repository,
                                            callback);
    }

    // Return success only, if all executed commands were successful
    return exitCode == EXIT_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}

template<UsingConnectionCallback F>
int UsingConnection::usingConnectionInternal(
        QString name, const bool debugSql,
        std::optional<std::reference_wrapper<MigrationRepository>> repository,
        const F &callback)
{
    const auto previousConnection = m_connectionResolver->getDefaultConnection();
    /* Default connection can also be "" empty string, eg. auto tests are using empty
       string as the default connection. */
    const auto previousDebugSql = getConnectionDebugSql(previousConnection);

    /* Case, when no connection name was passed on the command-line (--database argument),
       in this case, set a default/previous connection as a current connection. */
    if (name.isEmpty())
        name = previousConnection;

    // No need to update the same values
    const auto isSameConnection = previousConnection == name &&
                                  previousDebugSql   == debugSql;

    if (!isSameConnection)
        setConnection(name, debugSql, repository);

    int exitCode = EXIT_FAILURE;

    // Support for both callback types, with and without a connection name parameter
    if constexpr (std::is_same_v<F, std::function<int(const QString &)>>)
        exitCode = std::invoke(callback, name);
    else if constexpr (std::is_same_v<F, std::function<int()>>)
        exitCode = std::invoke(callback);
    else
        Q_UNREACHABLE();

    if (!isSameConnection)
        setConnection(previousConnection, previousDebugSql, repository, true);

    return exitCode;
}

void UsingConnection::setConnection(
        const QString &name, const std::optional<bool> debugSql,
        std::optional<std::reference_wrapper<MigrationRepository>> repository,
        const bool restore)
{
    // Restore even an empty "" connection, it makes sense
    if (restore || !name.isEmpty())
        m_connectionResolver->setDefaultConnection(name);

    if (repository)
        repository->get().setConnection(name);

    m_connection = name;

    if (debugSql)
        setConnectionDebugSql(*debugSql);
}

std::optional<bool> UsingConnection::getConnectionDebugSql(const QString &name) const
{
    return name.isEmpty() ? std::nullopt
                          : std::make_optional(m_connectionResolver->connection(name)
                                               .debugSql());
}

void UsingConnection::setConnectionDebugSql(const bool debugSql) const
{
    // Nothing to restore, a previous/default connection name was empty
    if (m_connection.isEmpty())
        return;

    auto &connection = resolveConnection(m_connection);

    if (debugSql)
        connection.enableDebugSql();
    else
        connection.disableDebugSql();
}

void
UsingConnection::printConnection(const QString &name, const bool shouldPrintConnection,
                                 bool &first) const
{
    // Nothing to print
    if (!shouldPrintConnection)
        return;

    // Newline for second and subsequent connections only
    if (first)
        first = false;
    else
        io().newLine();

    io().note(QStringLiteral("<blue>Connection:</blue> <b-white>%1</b-white>").arg(name),
              true, InteractsWithIO::Quiet);
}

const InteractsWithIO &UsingConnection::io() const
{
    return dynamic_cast<const InteractsWithIO &>(*this);
}

} // namespace Tom::Concerns

TINYORM_END_COMMON_NAMESPACE
