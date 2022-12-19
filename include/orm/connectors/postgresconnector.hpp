#pragma once
#ifndef ORM_CONNCECTORS_POSTGRESCONNECTOR_HPP
#define ORM_CONNCECTORS_POSTGRESCONNECTOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/connectors/connector.hpp"
#include "orm/connectors/connectorinterface.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{

    // FEATURE connectors, add some data member of support config options and if not contain throw silverqx
    // FEATURE postgres, add support for sslmode config option silverqx
    /*! PostgreSql connector. */
    class PostgresConnector final : public ConnectorInterface, public Connector
    {
        Q_DISABLE_COPY(PostgresConnector)

    public:
        /*! Default constructor. */
        inline PostgresConnector() = default;
        /*! Virtual destructor. */
        inline ~PostgresConnector() final = default;

        /*! Establish a database connection. */
        ConnectionName connect(const QVariantHash &config) const override;

        /*! Get the QSqlDatabase connection options for the current connector. */
        const QVariantHash &getConnectorOptions() const override;
        /*! Parse and validate QSqlDatabase connection options, called from
            the ConfigurationOptionsParser. */
        void parseConfigOptions(QVariantHash &options) const override;

    protected:
        /*! Set the connection character set and collation. */
        static void configureEncoding(const QSqlDatabase &connection,
                                      const QVariantHash &config);
        /*! Set the timezone on the connection. */
        static void configureTimezone(const QSqlDatabase &connection,
                                      const QVariantHash &config);

        /*! Set the schema on the connection. */
        static void configureSchema(const QSqlDatabase &connection,
                                    const QVariantHash &config);
        /*! Format the schema. */
        static QString formatSchema(QStringList &&schema);

        /*! Set an application name for the connection. */
        static void configureApplicationName(const QSqlDatabase &connection,
                                             const QVariantHash &config);
        /*! Configure the synchronous_commit setting. */
        static void configureSynchronousCommit(const QSqlDatabase &connection,
                                               const QVariantHash &config);

    private:
        /*! The default QSqlDatabase connection options for the SQLiteConnector. */
        inline static const QVariantHash m_options;
    };

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_POSTGRESCONNECTOR_HPP
