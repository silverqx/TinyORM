#pragma once
#ifndef ORM_SUPPORT_DATABASECONNECTIONSMAP_HPP
#define ORM_SUPPORT_DATABASECONNECTIONSMAP_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <unordered_map>

#include "orm/databaseconnection.hpp"
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

        /*! Return a pointer to the database connections map. */
        constexpr ConnectionsType *operator->() noexcept;
        /*! Return a pointer to the database connections map. */
        constexpr const ConnectionsType *operator->() const noexcept;
        /*! Return a reference to the database connections map. */
        constexpr ConnectionsType &operator*() noexcept;
        /*! Return a reference to the database connections map. */
        constexpr const ConnectionsType &operator*() const noexcept;
        /*! Return a reference to the database connections map. */
        constexpr ConnectionsType &get() noexcept;
        /*! Return a reference to the database connections map. */
        constexpr const ConnectionsType &get() const noexcept;

    private:
        /*! Database connections for the current thread. */
        T_THREAD_LOCAL
        inline static ConnectionsType m_connections;
    };

    /* public */

    constexpr DatabaseConnectionsMap::ConnectionsType *
    DatabaseConnectionsMap::operator->() noexcept
    {
        return std::addressof(m_connections);
    }

    constexpr const DatabaseConnectionsMap::ConnectionsType *
    DatabaseConnectionsMap::operator->() const noexcept
    {
        return std::addressof(m_connections);
    }

    constexpr DatabaseConnectionsMap::ConnectionsType &
    DatabaseConnectionsMap::operator*() noexcept
    {
        return m_connections;
    }

    constexpr const DatabaseConnectionsMap::ConnectionsType &
    DatabaseConnectionsMap::operator*() const noexcept
    {
        return m_connections;
    }

    constexpr DatabaseConnectionsMap::ConnectionsType &
    DatabaseConnectionsMap::get() noexcept
    {
        return m_connections;
    }

    constexpr const DatabaseConnectionsMap::ConnectionsType &
    DatabaseConnectionsMap::get() const noexcept
    {
        return m_connections;
    }

} // namespace Orm::Support

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SUPPORT_DATABASECONNECTIONSMAP_HPP
