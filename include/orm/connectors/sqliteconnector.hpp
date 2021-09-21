#pragma once
#ifndef SQLITECONNECTOR_HPP
#define SQLITECONNECTOR_HPP

#include "orm/connectors/connector.hpp"
#include "orm/connectors/connectorinterface.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Connectors
{

    /*! SQLite connector. */
    class SQLiteConnector final : public ConnectorInterface, public Connector
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
        /*! Set the connection foreign key constraints. */
        void configureForeignKeyConstraints(const QSqlDatabase &connection,
                                            const QVariantHash &config) const;

    private:
        /*! Check whether the SQLite database file exists. */
        void checkDatabaseExists(const QVariantHash &config) const;

        /*! The default QSqlDatabase connection options for the SQLiteConnector. */
        inline static const QVariantHash m_options = {};
    };

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SQLITECONNECTOR_HPP
