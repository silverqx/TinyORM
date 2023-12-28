#pragma once
#ifndef ORM_CONNCECTORS_MYSQLCONNECTOR_HPP
#define ORM_CONNCECTORS_MYSQLCONNECTOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/connectors/connector.hpp"
#include "orm/connectors/connectorinterface.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{

    /*! MySQL connector. */
    class MySqlConnector final : public ConnectorInterface,
                                 public Connector
    {
        Q_DISABLE_COPY_MOVE(MySqlConnector)

    public:
        /*! Default constructor. */
        MySqlConnector() = default;
        /*! Virtual destructor. */
        ~MySqlConnector() final = default;

        /*! Establish a database connection. */
        ConnectionName connect(const QVariantHash &config) const override;

        /*! Get the QSqlDatabase connection options for the current connector. */
        const QVariantHash &getConnectorOptions() const override;

    protected:
        /*! Set the connection transaction isolation level. */
        static void configureIsolationLevel(const TSqlDatabase &connection,
                                            const QVariantHash &config);
        /*! Set the connection character set and collation. */
        static void configureEncoding(const TSqlDatabase &connection,
                                      const QVariantHash &config);
        /*! Get the collation for the configuration. */
        static QString getCollation(const QVariantHash &config);
        /*! Set the timezone on the connection. */
        static void configureTimezone(const TSqlDatabase &connection,
                                      const QVariantHash &config);

        /*! Set the modes for the connection. */
        static void setModes(const TSqlDatabase &connection,
                             const QVariantHash &config);
        /*! Get the query to enable strict mode. */
        static QString strictMode(const TSqlDatabase &connection,
                                  const QVariantHash &config);
        /*! Get the MySQL server version. */
        static QString getMySqlVersion(const TSqlDatabase &connection,
                                       const QVariantHash &config);
        /*! Set the custom modes on the connection. */
        static void setCustomModes(const TSqlDatabase &connection,
                                   const QVariantHash &config);

    private:
        /*! Get the MySQL server version querying the database server. */
        static QString getMySqlVersionFromDatabase(const TSqlDatabase &connection);

        /*! The default QSqlDatabase connection options for the MySqlConnector. */
        inline static const QVariantHash m_options;
    };

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_MYSQLCONNECTOR_HPP
