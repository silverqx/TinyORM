#pragma once
#ifndef ORM_CONNCECTORS_POSTGRESCONNECTOR_HPP
#define ORM_CONNCECTORS_POSTGRESCONNECTOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/concerns/parsessearchpath.hpp"
#include "orm/connectors/connector.hpp"
#include "orm/connectors/connectorinterface.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{

    // FEATURE connectors, add some data member of support config options and if not contain throw silverqx
    // FEATURE postgres, add support for sslmode, sslcert, sslkey, sslrootcert config. options silverqx
    /*! PostgreSQL connector. */
    class PostgresConnector final : public ConnectorInterface,
                                    public Connector,
                                    protected Concerns::ParsesSearchPath
    {
        Q_DISABLE_COPY_MOVE(PostgresConnector)

    public:
        /*! Default constructor. */
        PostgresConnector() = default;
        /*! Virtual destructor. */
        ~PostgresConnector() final = default;

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
        /*! Set the timezone on the connection. */
        static void configureTimezone(const TSqlDatabase &connection,
                                      const QVariantHash &config);

        /*! Set the 'search_path' on the database connection. */
        static void configureSearchPath(const TSqlDatabase &connection,
                                        const QVariantHash &config);
        /*! Format the 'search_path' for the database query or DSN. */
        static QString quoteSearchPath(const QStringList &searchPath);

        /*! Set an application name for the connection. */
        static void configureApplicationName(const TSqlDatabase &connection,
                                             const QVariantHash &config);
        /*! Configure the synchronous_commit setting. */
        static void configureSynchronousCommit(const TSqlDatabase &connection,
                                               const QVariantHash &config);

    private:
        /*! The default QSqlDatabase connection options for the SQLiteConnector. */
        inline static const QVariantHash m_options;
    };

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_POSTGRESCONNECTOR_HPP
