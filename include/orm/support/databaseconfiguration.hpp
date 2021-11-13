#pragma once
#ifndef ORM_CONFIGURATION_HPP
#define ORM_CONFIGURATION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/threadlocal.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Support
{

    /*! Database configuration class. */
    class DatabaseConfiguration
    {
        Q_DISABLE_COPY(DatabaseConfiguration)

    public:
        /*! Type used for Database Connections map. */
        using ConfigurationsType = QHash<QString, QVariantHash>;

        /*! Default constructor. */
        inline DatabaseConfiguration() = default;

        /*! Default Database Connection Name, used as default value in method declarations
            only. */
        T_THREAD_LOCAL
        inline static QString defaultConnectionName = QStringLiteral("tinyorm_default");

        /*! Currently set Default Database Connection Name in a current thread. */
        T_THREAD_LOCAL
        inline static QString defaultConnection;

        /*! Default namespace prefix for MySQL savepoints in a current thread. */
        T_THREAD_LOCAL
        inline static
        QString defaultSavepointNamespace = QStringLiteral("tinyorm_savepoint");

        /*! Return a reference to the database connection configurations map. */
        inline ConfigurationsType &operator*();
        /*! Return a reference to the database connection configurations map. */
        inline const ConfigurationsType &operator*() const;
        /*! Return a reference to the database connection configurations map. */
        inline ConfigurationsType &get();
        /*! Return a reference to the database connection configurations map. */
        inline const ConfigurationsType &get() const;

    private:
        /*! Database connection configurations. */
        T_THREAD_LOCAL
        inline static ConfigurationsType m_configurations;
    };

    DatabaseConfiguration::ConfigurationsType &
    DatabaseConfiguration::operator*()
    {
        return m_configurations;
    }

    const DatabaseConfiguration::ConfigurationsType &
    DatabaseConfiguration::operator*() const
    {
        return m_configurations;
    }

    DatabaseConfiguration::ConfigurationsType &
    DatabaseConfiguration::get()
    {
        return m_configurations;
    }

    const DatabaseConfiguration::ConfigurationsType &
    DatabaseConfiguration::get() const
    {
        return m_configurations;
    }

} // namespace Orm::Support

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATION_HPP
