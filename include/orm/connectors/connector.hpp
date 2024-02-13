#pragma once
#ifndef ORM_CONNCECTORS_CONNECTOR_HPP
#define ORM_CONNCECTORS_CONNECTOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlDatabase

#include "orm/concerns/detectslostconnections.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{

    /*! Connectors base class. */
    class Connector : public Concerns::DetectsLostConnections
    {
        Q_DISABLE_COPY_MOVE(Connector)

    public:
        /*! Default constructor. */
        Connector() = default;
        /*! Pure virtual destructor. */
        inline ~Connector() override = 0;

        /*! Create a new QSqlDatabase connection, factory method. */
        static TSqlDatabase
        createConnection(const QString &name, const QVariantHash &config,
                         const QString &options);

        /*! Get the QSqlDatabase connection options based on the configuration. */
        QString getOptions(const QVariantHash &config) const;

        /*! Get the QSqlDatabase connection options for the current connector. */
        virtual const QVariantHash &getConnectorOptions() const = 0;

    protected:
        /*! Create a new QSqlDatabase instance, create a new database connection. */
        static TSqlDatabase
        createQSqlDatabaseConnection(const QString &name, const QVariantHash &config,
                                     const QString &options);

        /*! Add a database to the list of database connections using the driver type. */
        static TSqlDatabase
        addQSqlDatabaseConnection(const QString &name, const QVariantHash &config,
                                  const QString &options);
        /*! Handle an exception that occurred during connect execution. */
        static TSqlDatabase
        tryAgainIfCausedByLostConnection(
                const std::exception_ptr &ePtr, const QString &errorMessage,
                const QString &name, const QVariantHash &config,
                const QString &options);

        /*! Error message used when connection configuration fails. */
        static const QString m_configureErrorMessage;
    };

    Connector::~Connector() = default;

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_CONNECTOR_HPP
