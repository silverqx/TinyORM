#pragma once
#ifndef ORM_CONNECTOR_HPP
#define ORM_CONNECTOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtSql/QSqlDatabase>
#include <QVariantHash>

#include "orm/concerns/detectslostconnections.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{

    /*! Connectors base class. */
    class Connector : public Concerns::DetectsLostConnections
    {
    public:
        /*! Pure virtual destructor. */
        virtual ~Connector() = 0;

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

        /*! Parse and validate QSqlDatabase connection options, called from
            the ConfigurationOptionsParser. */
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
                const std::exception_ptr &ePtr, const Exceptions::SqlError &e,
                const QString &name, const QVariantHash &config,
                const QString &options) const;

        /*! Error message used when connection configuration fails. */
        static const QString m_configureErrorMessage;
    };

    inline Connector::~Connector() = default;

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNECTOR_HPP
