#pragma once
#ifndef POSTGRESCONNECTOR_HPP
#define POSTGRESCONNECTOR_HPP

#include "orm/connectors/connector.hpp"
#include "orm/connectors/connectorinterface.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Connectors
{

    // FEATURE connectors, add some data member of support config options and if not contain throw silverqx
    // FEATURE postgres, add support for sslmode config option silverqx
    class PostgresConnector final : public ConnectorInterface, public Connector
    {
    public:
        /*! Establish a database connection. */
        ConnectionName connect(const QVariantHash &config) const override;

        /*! Get the QSqlDatabase connection options for the current connector. */
        const QVariantHash &getConnectorOptions() const override;
        /*! Parse and validate QSqlDatabase connection options, called from
            the ConfigurationOptionsParser. */
        void parseConfigOptions(QVariantHash &options) const override;

    protected:
        /*! Set the connection character set and collation. */
        void configureEncoding(const QSqlDatabase &connection,
                               const QVariantHash &config) const;
        /*! Set the timezone on the connection. */
        void configureTimezone(const QSqlDatabase &connection,
                               const QVariantHash &config) const;

        /*! Set the schema on the connection. */
        void configureSchema(const QSqlDatabase &connection,
                             const QVariantHash &config) const;
        /*! Format the schema. */
        QString formatSchema(QStringList schema) const;

        /*! Set an application name for the connection. */
        void configureApplicationName(const QSqlDatabase &connection,
                                      const QVariantHash &config) const;
        /*! Configure the synchronous_commit setting. */
        void configureSynchronousCommit(const QSqlDatabase &connection,
                                        const QVariantHash &config) const;

    private:
        /*! The default QSqlDatabase connection options for the SQLiteConnector. */
        inline static const QVariantHash m_options {};
    };

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif


#endif // POSTGRESCONNECTOR_HPP
