#include "tom/concerns/usingconnection.hpp"

#include <orm/connectionresolverinterface.hpp>
#include <orm/databaseconnection.hpp>

#include "tom/migrationrepository.hpp"

using Orm::DatabaseConnection;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Concerns
{

/* public */

UsingConnection::UsingConnection(std::shared_ptr<ConnectionResolverInterface> &&resolver)
    : m_resolver(std::move(resolver))
{}

int UsingConnection::usingConnection(
            QString name, const bool debugSql,
            std::optional<std::reference_wrapper<MigrationRepository>> repository,
            std::function<int()> &&callback)
{
    auto previousConnection = m_resolver->getDefaultConnection();
    /* Default connection can also be "" empty string, eg. auto tests are using empty
       string as the default connection. */
    auto previousDebugSql = getConnectionDebugSql(previousConnection);

    /* Case, when no connection name was passed on the command-line (--database argument),
       in this case, set a current connection to the default connection. */
    if (name.isEmpty())
        name = previousConnection;

    // No need to update the same values
    const auto isSameConnection = previousConnection == name &&
                                  previousDebugSql   == debugSql;

    if (!isSameConnection)
        setConnection(std::move(name), debugSql, repository);

    auto exitCode = std::invoke(std::move(callback));

    if (!isSameConnection)
        setConnection(std::move(previousConnection), std::move(previousDebugSql),
                      repository, true);

    return exitCode;
}

int UsingConnection::usingConnection(QString &&name, const bool debugSql,
                                     std::function<int()> &&callback)
{
    return usingConnection(std::move(name), debugSql, std::nullopt, std::move(callback));
}

int UsingConnection::usingConnection(const QString &name, const bool debugSql,
                                     std::function<int()> &&callback)
{
    return usingConnection(name, debugSql, std::nullopt, std::move(callback));
}

DatabaseConnection &UsingConnection::resolveConnection(const QString &name) const
{
    return m_resolver->connection(name.isEmpty() ? m_connection : name);
}

/* private */

void UsingConnection::setConnection(QString &&name, std::optional<bool> &&debugSql,
                                    std::optional<std::reference_wrapper<
                                                    MigrationRepository>> repository,
                                    const bool restore)
{
    // Restore even an empty "" connection, it makes sense
    if (restore || !name.isEmpty())
        m_resolver->setDefaultConnection(name);

    if (repository)
        repository->get().setConnection(name);

    m_connection = std::move(name);

    setConnectionDebugSql(std::move(debugSql));
}

std::optional<bool> UsingConnection::getConnectionDebugSql(const QString &name) const
{
    return name.isEmpty() ? std::nullopt
                          : std::make_optional(m_resolver->connection(name).debugSql());
}

void UsingConnection::setConnectionDebugSql(std::optional<bool> &&debugSql) const
{
    if (m_connection.isEmpty())
        return;

    auto &connection = resolveConnection(m_connection);

    if (*debugSql)
        connection.enableDebugSql();
    else
        connection.disableDebugSql();
}

} // namespace Tom::Concerns

TINYORM_END_COMMON_NAMESPACE
