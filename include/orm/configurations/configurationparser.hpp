#pragma once
#ifndef ORM_CONFIGURATIONS_CONFIGURATIONPARSER_HPP
#define ORM_CONFIGURATIONS_CONFIGURATIONPARSER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <optional>

#include "orm/configurations/configurationoptionsparser.hpp"
#include "orm/configurations/configurationparserinterface.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Configurations
{

    /*! TinyORM configuration parser base class validates and prepares configuration
        options, the parsed configuration will be used in the Connector-s. */
    class ConfigurationParser : public ConfigurationParserInterface,
                                public ConfigurationOptionsParser
    {
        Q_DISABLE_COPY_MOVE(ConfigurationParser)

        // To access config()
        friend ConfigurationOptionsParser;

    public:
        /*! Default constructor. */
        ConfigurationParser() = default;
        /*! Pure virtual destructor. */
        inline ~ConfigurationParser() override = 0;

        /*! Parse and prepare the database configuration. */
        QVariantHash parseConfiguration(QVariantHash &config,
                                        const ConnectionName &connection) const final;

    protected:
        /*! Parse the driver-specific database configuration. */
        virtual void parseDriverSpecificOptions() const = 0;

        /* Getters */
        /*! Get a cached configuration reference. */
        inline QVariantHash &config() const noexcept;
        /*! Get a cached connection name reference. */
        inline const ConnectionName &connection() const noexcept;

        /* Data members */
        /*! Configuration reference cache. */
        mutable std::optional<
                std::reference_wrapper<QVariantHash>> m_config = std::nullopt;
        /*! Connection name reference cache. */
        mutable std::optional<
                std::reference_wrapper<const ConnectionName>> m_connection = std::nullopt;

    private:
        /*! Parse common configuration options for all supported databases. */
        void parseCommonOptions() const;
        /*! Modify the driver name to uppercase. */
        void normalizeDriverName() const;

        /*! Parse the qt_timezone configuration option. */
        void parseQtTimeZone(QVariantHash &config) const;
    };

    /* public */

    ConfigurationParser::~ConfigurationParser() = default;

    /* protected */

    /* Getters */

    QVariantHash &ConfigurationParser::config() const noexcept
    {
        return m_config.value(); // NOLINT(bugprone-unchecked-optional-access)
    }

    const ConnectionName &ConfigurationParser::connection() const noexcept
    {
        return m_connection.value(); // NOLINT(bugprone-unchecked-optional-access)
    }

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATIONS_CONFIGURATIONPARSER_HPP
