#pragma once
#ifndef MYSQLCONNECTOR_HPP
#define MYSQLCONNECTOR_HPP

#include "orm/connectors/connector.hpp"
#include "orm/connectors/connectorinterface.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Connectors
{

    /*! MySql connector. */
    class MySqlConnector final : public ConnectorInterface, public Connector
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
        /*! Set the connection transaction isolation level. */
        void configureIsolationLevel(const QSqlDatabase &connection,
                                     const QVariantHash &config) const;
        /*! Set the connection character set and collation. */
        void configureEncoding(const QSqlDatabase &connection,
                               const QVariantHash &config) const;
        /*! Get the collation for the configuration. */
        QString getCollation(const QVariantHash &config) const;
        /*! Set the timezone on the connection. */
        void configureTimezone(const QSqlDatabase &connection,
                               const QVariantHash &config) const;

        /*! Set the modes for the connection. */
        void setModes(const QSqlDatabase &connection,
                      const QVariantHash &config) const;
        /*! Get the query to enable strict mode. */
        QString strictMode(const QSqlDatabase &connection,
                           const QVariantHash &config) const;
        /*! Get the MySql server version. */
        QString getMySqlVersion(const QSqlDatabase &connection,
                                const QVariantHash &config) const;
        /*! Set the custom modes on the connection. */
        void setCustomModes(const QSqlDatabase &connection,
                            const QVariantHash &config) const;

    private:
        /*! The default QSqlDatabase connection options for the MySqlConnector. */
        inline static const QVariantHash m_options {
            {"MYSQL_OPT_RECONNECT", 0},
        };
    };

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MYSQLCONNECTOR_HPP
