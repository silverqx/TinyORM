#pragma once
#ifndef ORM_CONFIGURATIONS_POSTGRESCONFIGURATIONPARSER_HPP
#define ORM_CONFIGURATIONS_POSTGRESCONFIGURATIONPARSER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/configurations/configurationparser.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Configurations
{

    /*! PostgreSQL configuration parser base class validates and prepares configuration
        options, the parsed configuration will be used in the PostgresConnector. */
    class PostgresConfigurationParser final : public ConfigurationParser
    {
        Q_DISABLE_COPY(PostgresConfigurationParser)

    public:
        /*! Default constructor. */
        inline PostgresConfigurationParser() = default;
        /*! Virtual destructor. */
        inline ~PostgresConfigurationParser() final = default;

    protected:
        /*! Parse the driver-specific database configuration. */
        void parseDriverSpecificOptions() const final;
        /*! Parse the driver-specific 'options' configuration option. */
        void parseDriverSpecificOptionsOption(QVariantHash &options) const final;
    };

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATIONS_POSTGRESCONFIGURATIONPARSER_HPP
