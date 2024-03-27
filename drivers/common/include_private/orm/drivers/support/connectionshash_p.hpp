#pragma once
#ifndef ORM_DRIVERS_SUPPORT_CONNECTIONSHASH_P_HPP
#define ORM_DRIVERS_SUPPORT_CONNECTIONSHASH_P_HPP

#include <QStringList>

#include <shared_mutex>
#include <unordered_map>

#include "orm/drivers/sqldatabase.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Support
{

    /*! Database connections hash (thread-safe). */
    class ConnectionsHashPrivate
    {
        Q_DISABLE_COPY_MOVE(ConnectionsHashPrivate)

    public:
        /* Container related */
        using ContainerType   = typename std::unordered_map<QString, SqlDatabase>;
        using node_type       = typename ContainerType::node_type;
        using key_type        = typename ContainerType::key_type;
        using mapped_type     = typename ContainerType::mapped_type;
        using value_type      = typename ContainerType::value_type;
        using reference       = value_type &;
        using const_reference = const value_type &;
        /* Iterators related */
        using iterator        = typename ContainerType::iterator;
        using const_iterator  = typename ContainerType::const_iterator;
        using difference_type = typename ContainerType::difference_type;
        using size_type       = typename ContainerType::size_type;

        /*! Default constructor. */
        ConnectionsHashPrivate() = default;
        /*! Default destructor. */
        ~ConnectionsHashPrivate() = default;

        /* Lookup */
        /*! Get a connection by the given connection name. */
        inline mapped_type &at_ts(const key_type &key);
        /*! Get a connection by the given connection name, const version. */
        inline const mapped_type &at_ts(const key_type &key) const;

        /*! Determine whether the hash contains the given connection. */
        inline bool contains(const key_type &key) const;
        /*! Determine whether the hash contains the given connection. */
        inline bool contains_ts(const key_type &key) const;

        /*! Get all keys/connection names. */
        inline QStringList keys_ts() const;

        /* Modifiers */
        /*! Extract the node by the given connection name. */
        inline node_type extract(const key_type &key);

        /*! Construct a connection in-place if doesn't exist. */
        template<typename ...Args>
        std::pair<iterator, bool> try_emplace(const key_type &key, Args &&...args);

        /* Connections specific */
        /*! Get a list of opened connections. */
        inline QStringList openedConnectionNames_ts() const;

        /* Multi-threading */
        /*! Get a reference to the shared mutex. */
        inline std::shared_mutex &mutex() const noexcept;

    private:
        /*! Database connections hash. */
        ContainerType m_data;
        /*! Shared mutex. */
        mutable std::shared_mutex m_mutex;
    };

    /* public */

    /* Lookup */

    ConnectionsHashPrivate::mapped_type &
    ConnectionsHashPrivate::at_ts(const key_type &key)
    {
        // Shared/read lock
        const std::shared_lock lock(m_mutex);

        return m_data.at(key);
    }

    const ConnectionsHashPrivate::mapped_type &
    ConnectionsHashPrivate::at_ts(const key_type &key) const
    {
        // Shared/read lock
        const std::shared_lock lock(m_mutex);

        return m_data.at(key);
    }

    bool ConnectionsHashPrivate::contains(const key_type &key) const
    {
        return m_data.contains(key);
    }

    bool ConnectionsHashPrivate::contains_ts(const key_type &key) const
    {
        // Shared/read lock
        const std::shared_lock lock(m_mutex);

        return m_data.contains(key);
    }

    QStringList ConnectionsHashPrivate::keys_ts() const
    {
        // Shared/read lock
        const std::shared_lock lock(m_mutex);

        QStringList result;
        result.reserve(static_cast<decltype (result)::size_type>(m_data.size()));

        for (const auto &[key, _] : m_data)
            result << key;

        return result;
    }

    /* Modifiers */

    ConnectionsHashPrivate::node_type
    ConnectionsHashPrivate::extract(const key_type &key)
    {
        return m_data.extract(key);
    }

    template<typename ...Args>
    std::pair<ConnectionsHashPrivate::iterator, bool>
    ConnectionsHashPrivate::try_emplace(const key_type &key, Args &&...args)
    {
        return m_data.try_emplace(key, std::forward<Args>(args)...);
    }

    /* Connections specific */

    QStringList ConnectionsHashPrivate::openedConnectionNames_ts() const
    {
        // Shared/read lock
        const std::shared_lock lock(m_mutex);

        QStringList result;
        result.reserve(static_cast<decltype (result)::size_type>(m_data.size()));

        for (const auto &[connectionName, connection] : m_data)
            if (connection.isOpen())
                result << connectionName;

        return result;
    }

    /* Multi-threading */

    std::shared_mutex &ConnectionsHashPrivate::mutex() const noexcept
    {
        return m_mutex;
    }

} // namespace Orm::Drivers::Support

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SUPPORT_CONNECTIONSHASH_P_HPP
