#pragma once
#ifndef TOM_CONCERNS_USINGCONNECTION_HPP
#define TOM_CONCERNS_USINGCONNECTION_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QStringList>

#include <optional>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class ConnectionResolverInterface;
    class DatabaseConnection;
}

namespace Tom
{
    class MigrationRepository;

namespace Concerns
{

    /*! Invoke the given callback inside the defined connection and restore connection
        to the previous state after finish. */
    class UsingConnection
    {
        Q_DISABLE_COPY(UsingConnection)

        /*! Alias for the ConnectionResolverInterface. */
        using ConnectionResolverInterface = Orm::ConnectionResolverInterface;
        /*! Alias for the DatabaseConnection. */
        using DatabaseConnection = Orm::DatabaseConnection;

    public:
        /*! Constructor. */
        explicit UsingConnection(std::shared_ptr<ConnectionResolverInterface> &&resolver);
        /*! Default destructor. */
        inline ~UsingConnection() = default;

        /*! Execute the given callback using the given connection as the default
            connection. */
        int usingConnection(QString name, bool debugSql,
                            std::optional<std::reference_wrapper<
                                            MigrationRepository>> repository,
                            std::function<int()> &&callback);
        /*! Execute the given callback using the given connection as the default
            connection. */
        int usingConnection(QString &&name, bool debugSql,
                            std::function<int()> &&callback);
        /*! Execute the given callback using the given connection as the default
            connection. */
        int usingConnection(const QString &name, bool debugSql,
                            std::function<int()> &&callback);

        /*! Resolve the database connection instance. */
        DatabaseConnection &resolveConnection(const QString &name = "") const;

        /* Getters / Setters */
        /*! Get the currently used connection name. */
        inline const QString &getConnectionName() const noexcept;

    private:
        /*! Set the default connection name. */
        void setConnection(
                QString &&name, std::optional<bool> &&debugSql,
                std::optional<std::reference_wrapper<
                                MigrationRepository>> repository = std::nullopt,
                bool restore = false);

        /*! Get the debug sql by the connection name. */
        std::optional<bool> getConnectionDebugSql(const QString &name) const;
        /*! Set the debug sql for the current repository connection. */
        void setConnectionDebugSql(std::optional<bool> &&debugSql) const;

        /*! The database connection resolver instance. */
        std::shared_ptr<ConnectionResolverInterface> m_resolver = nullptr;
        /*! The name of the database connection to use. */
        QString m_connection {};
    };

    /* public */

    const QString &UsingConnection::getConnectionName() const noexcept
    {
        return m_connection;
    }

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_USINGCONNECTION_HPP
