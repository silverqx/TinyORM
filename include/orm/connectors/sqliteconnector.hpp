#pragma once
#ifndef ORM_CONNCECTORS_SQLITECONNECTOR_HPP
#define ORM_CONNCECTORS_SQLITECONNECTOR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/connectors/connector.hpp"
#include "orm/connectors/connectorinterface.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{

    /*! SQLite connector. */
    class SQLiteConnector final : public ConnectorInterface,
                                  public Connector
    {
        Q_DISABLE_COPY_MOVE(SQLiteConnector)

    public:
        /*! Default constructor. */
        SQLiteConnector() = default;
        /*! Virtual destructor. */
        ~SQLiteConnector() final = default;

        /*! Establish a database connection. */
        ConnectionName connect(const QVariantHash &config) const override;

        /*! Get the QSqlDatabase connection options for the current connector. */
        const QVariantHash &getConnectorOptions() const override;

    protected:
        /*! Set the connection foreign key constraints. */
        static void configureForeignKeyConstraints(const TSqlDatabase &connection,
                                                   const QVariantHash &config);

    private:
        /*! Check whether the SQLite database file exists. */
        static void checkDatabaseExists(const QVariantHash &config);

        /*! The default QSqlDatabase connection options for the SQLiteConnector. */
        inline static const QVariantHash m_options;
    };

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_SQLITECONNECTOR_HPP
