#pragma once
#ifndef ORM_SUPPORT_DATABASECONNECTIONSMAP_HPP
#define ORM_SUPPORT_DATABASECONNECTIONSMAP_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <unordered_map>

#include "orm/databaseconnection.hpp"
#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/threadlocal.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Support
{

    /*! Handles Database connections for the current thread. */
    class DatabaseConnectionsMap
    {
        Q_DISABLE_COPY(DatabaseConnectionsMap)

    public:
        /*! Default constructor. */
        inline DatabaseConnectionsMap() = default;

        /*! Type used to store database connections. */
        using ConnectionsType =
                std::unordered_map<QString, std::unique_ptr<DatabaseConnection>>;

        /*! Return a reference to the database connections map. */
        inline ConnectionsType &operator*();
        /*! Return a reference to the database connections map. */
        inline const ConnectionsType &operator*() const;
        /*! Return a reference to the database connections map. */
        inline ConnectionsType &get();
        /*! Return a reference to the database connections map. */
        inline const ConnectionsType &get() const;

    private:
        /*! Database connections for the current thread. */
        T_THREAD_LOCAL
        inline static ConnectionsType m_connections;
    };

    DatabaseConnectionsMap::ConnectionsType &
    DatabaseConnectionsMap::operator*()
    {
        return m_connections;
    }

    const DatabaseConnectionsMap::ConnectionsType &
    DatabaseConnectionsMap::operator*() const
    {
        return m_connections;
    }

    DatabaseConnectionsMap::ConnectionsType &
    DatabaseConnectionsMap::get()
    {
        return m_connections;
    }

    const DatabaseConnectionsMap::ConnectionsType &
    DatabaseConnectionsMap::get() const
    {
        return m_connections;
    }

} // namespace Orm::Support

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SUPPORT_DATABASECONNECTIONSMAP_HPP
