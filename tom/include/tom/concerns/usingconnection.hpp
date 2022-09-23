#pragma once
#ifndef TOM_CONCERNS_USINGCONNECTION_HPP
#define TOM_CONCERNS_USINGCONNECTION_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QStringList>

#include <memory>
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

    class InteractsWithIO;

    /*! Concept for the usingConnection() method callback parameter. */
    template<typename T>
    concept UsingConnectionCallback =
            std::convertible_to<T, std::function<int(const QString &)>> ||
            std::convertible_to<T, std::function<int()>>;

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
        explicit UsingConnection(
                std::shared_ptr<ConnectionResolverInterface> &&connectionResolver);
        /*! Virtual destructor. */
        inline virtual ~UsingConnection() = 0;

        /*! Execute the given callback using the given connections as the default
            connections (in the loop). */
        int usingConnections(
                QStringList &&names, bool debugSql,
                std::optional<std::reference_wrapper<MigrationRepository>> repository,
                const std::function<int(const QString &database)> &callback);
        /*! Execute the given callback using the given connections as the default
            connections (in the loop). */
        int usingConnections(
                QStringList &&names, bool debugSql,
                std::optional<std::reference_wrapper<MigrationRepository>> repository,
                const std::function<int()> &callback);

        /*! Execute the given callback using the given connections as the default
            connections (in the loop). */
        int usingConnections(
                QStringList &&names, bool debugSql,
                const std::function<int(const QString &database)> &callback);
        /*! Execute the given callback using the given connections as the default
            connections (in the loop). */
        int usingConnections(QStringList &&names, bool debugSql,
                            const std::function<int()> &callback);

        /*! Execute the given callback using the given connection as the default
            connection (in the loop). */
        int usingConnection(QString &&name, bool debugSql,
                            const std::function<int()> &callback);
        /*! Execute the given callback using the given connection as the default
            connection (in the loop). */
        int usingConnection(const QString &name, bool debugSql,
                            const std::function<int()> &callback);

        /*! Resolve the database connection instance. */
        DatabaseConnection &resolveConnection(const QString &name = "") const;

        /* Getters */
        /*! Get the currently used connection name. */
        inline const QString &getConnectionName() const noexcept;

    private:
        /*! Execute the given callback using the given connections as the default
            connections (in the loop). */
        template<UsingConnectionCallback F>
        int usingConnectionsInternal(
                QStringList &&names, bool debugSql,
                std::optional<std::reference_wrapper<MigrationRepository>> repository,
                const F &callback);
        /*! Execute the given callback using the given connections as the default
            connections. */
        template<UsingConnectionCallback F>
        int usingConnectionInternal(
                QString name, bool debugSql,
                std::optional<std::reference_wrapper<MigrationRepository>> repository,
                const F &callback);

        /*! Set the default connection name. */
        void setConnection(
                const QString &name, std::optional<bool> debugSql,
                std::optional<std::reference_wrapper<
                                MigrationRepository>> repository = std::nullopt,
                bool restore = false);

        /*! Get the debug sql by the connection name. */
        std::optional<bool> getConnectionDebugSql(const QString &name) const;
        /*! Set the debug sql for the current repository connection. */
        void setConnectionDebugSql(bool debugSql) const;

        /*! Print currently used connection if passed more connections to --database=. */
        void printConnection(const QString &name, bool shouldPrintConnection,
                             bool &first) const;

        /*! Get the reference to the base InteractsWithIO concern. */
        const Concerns::InteractsWithIO &io() const;

        /*! The database connection resolver instance. */
        std::shared_ptr<ConnectionResolverInterface> m_connectionResolver = nullptr;
        /*! The name of the database connection to use. */
        QString m_connection {};
    };

    /* public */

    UsingConnection::~UsingConnection() = default;

    const QString &UsingConnection::getConnectionName() const noexcept
    {
        return m_connection;
    }

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_USINGCONNECTION_HPP
