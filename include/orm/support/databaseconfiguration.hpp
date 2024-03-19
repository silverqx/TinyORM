#pragma once
#ifndef ORM_SUPPORT_DATABASECONFIGURATION_HPP
#define ORM_SUPPORT_DATABASECONFIGURATION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>

#include <unordered_map>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/threadlocal.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Support
{

    /*! Database configuration class. */
    class DatabaseConfiguration
    {
        Q_DISABLE_COPY_MOVE(DatabaseConfiguration)

    public:
        /*! Type used for Database Connections map. */
        using ConfigurationsType = std::unordered_map<QString, QVariantHash>;

        /*! Default constructor. */
        DatabaseConfiguration() = default;
        /*! Default destructor. */
        ~DatabaseConfiguration() = default;

        /*! Default Database Connection Name, used as default value in method declarations
            only. */
        T_THREAD_LOCAL
        inline static QString defaultConnectionName = QStringLiteral("tinyorm_default");

        /*! Currently set Default Database Connection Name for a current thread. */
        T_THREAD_LOCAL
        inline static QString defaultConnection;

        /*! Default namespace prefix for MySQL savepoints in a current thread. */
        T_THREAD_LOCAL
        inline static
        QString defaultSavepointNamespace = QStringLiteral("tinyorm_savepoint");

        /*! Return a pointer to the database connection configurations map. */
        inline ConfigurationsType *operator->() noexcept;
        /*! Return a pointer to the database connection configurations map. */
        inline const ConfigurationsType *operator->() const noexcept;
        /*! Return a reference to the database connection configurations map. */
        inline ConfigurationsType &operator*() noexcept;
        /*! Return a reference to the database connection configurations map. */
        inline const ConfigurationsType &operator*() const noexcept;
        /*! Return a reference to the database connection configurations map. */
        inline ConfigurationsType &get() noexcept;
        /*! Return a reference to the database connection configurations map. */
        inline const ConfigurationsType &get() const noexcept;

    private:
        /*! Database connection configurations. */
        T_THREAD_LOCAL
        inline static ConfigurationsType m_configurations;
    };

    /* public */

    DatabaseConfiguration::ConfigurationsType *
    DatabaseConfiguration::operator->() noexcept
    {
        return std::addressof(m_configurations);
    }

    const DatabaseConfiguration::ConfigurationsType *
    DatabaseConfiguration::operator->() const noexcept
    {
        return std::addressof(m_configurations);
    }

    DatabaseConfiguration::ConfigurationsType &
    DatabaseConfiguration::operator*() noexcept
    {
        return m_configurations;
    }

    const DatabaseConfiguration::ConfigurationsType &
    DatabaseConfiguration::operator*() const noexcept
    {
        return m_configurations;
    }

    DatabaseConfiguration::ConfigurationsType &
    DatabaseConfiguration::get() noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return m_configurations;
    }

    const DatabaseConfiguration::ConfigurationsType &
    DatabaseConfiguration::get() const noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return m_configurations;
    }

} // namespace Orm::Support

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SUPPORT_DATABASECONFIGURATION_HPP
