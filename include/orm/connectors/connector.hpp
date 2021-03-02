#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include <QtSql/QSqlDatabase>
#include <QVariantHash>

#include "orm/concerns/detectslostconnections.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Connectors
{

    class Connector : public Concerns::DetectsLostConnections
    {
    public:
        inline virtual ~Connector() = default;

        /*! Create a new QSqlDatabase connection, factory method. */
        QSqlDatabase
        createConnection(const QString &name, const QVariantHash &config,
                         const QString &options) const;
        /*! Create a new QSqlDatabase instance, create a new database connection. */
        QSqlDatabase
        createQSqlDatabaseConnection(
                const QString &name, const QVariantHash &config,
                const QString &options) const;

        /*! Get the QSqlDatabase connection options based on the configuration. */
        QString getOptions(const QVariantHash &config) const;

        /*! Parse connection options. */
        virtual void parseConfigOptions(QVariantHash &options) const = 0;
        /*! Get the QSqlDatabase connection options for the current connector. */
        virtual const QVariantHash &getConnectorOptions() const = 0;

    protected:
        /*! Add a database to the list of database connections using the driver type. */
        QSqlDatabase
        addQSqlDatabaseConnection(
                const QString &name, const QVariantHash &config,
                const QString &options) const;
        /*! Handle an exception that occurred during connect execution. */
        QSqlDatabase
        tryAgainIfCausedByLostConnection(
                const SqlError &e, const QString &name, const QVariantHash &config,
                const QString &options) const;
    };

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONNECTOR_HPP
