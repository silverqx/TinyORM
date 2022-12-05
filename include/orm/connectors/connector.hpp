#pragma once
#ifndef ORM_CONNCECTORS_CONNECTOR_HPP
#define ORM_CONNCECTORS_CONNECTOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>
#include <QtSql/QSqlDatabase>

#include "orm/concerns/detectslostconnections.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{

    /*! Connectors base class. */
    class Connector : public Concerns::DetectsLostConnections
    {
        Q_DISABLE_COPY(Connector)

    public:
        /*! Default constructor. */
        inline Connector() = default;
        /*! Pure virtual destructor. */
        inline ~Connector() override = 0;

        /*! Create a new QSqlDatabase connection, factory method. */
        static QSqlDatabase
        createConnection(const QString &name, const QVariantHash &config,
                         const QString &options);

        /*! Get the QSqlDatabase connection options based on the configuration. */
        QString getOptions(const QVariantHash &config) const;

        /*! Parse and validate QSqlDatabase connection options, called from
            the ConfigurationOptionsParser. */
        virtual void parseConfigOptions(QVariantHash &options) const = 0;
        /*! Get the QSqlDatabase connection options for the current connector. */
        virtual const QVariantHash &getConnectorOptions() const = 0;

    protected:
        /*! Create a new QSqlDatabase instance, create a new database connection. */
        static QSqlDatabase
        createQSqlDatabaseConnection(const QString &name, const QVariantHash &config,
                                     const QString &options);

        /*! Add a database to the list of database connections using the driver type. */
        static QSqlDatabase
        addQSqlDatabaseConnection(const QString &name, const QVariantHash &config,
                                  const QString &options);
        /*! Handle an exception that occurred during connect execution. */
        static QSqlDatabase
        tryAgainIfCausedByLostConnection(
                const std::exception_ptr &ePtr, const Exceptions::SqlError &e,
                const QString &name, const QVariantHash &config,
                const QString &options);

        /*! Error message used when connection configuration fails. */
        static const QString m_configureErrorMessage;
    };

    Connector::~Connector() = default;

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_CONNECTOR_HPP
