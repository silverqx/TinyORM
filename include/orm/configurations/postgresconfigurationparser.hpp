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
        Q_DISABLE_COPY_MOVE(PostgresConfigurationParser)

    public:
        /*! Default constructor. */
        PostgresConfigurationParser() = default;
        /*! Virtual destructor. */
        ~PostgresConfigurationParser() final = default;

    protected:
        /*! Parse the driver-specific database configuration. */
        void parseDriverSpecificOptions() const final;
        /*! Parse the driver-specific 'options' configuration option. */
        void parseDriverSpecificOptionsOption(QVariantHash &options) const final;

    private:
        /*! Add the SSL-related options to the connection 'options' hash. */
        void addSslOptions(QVariantHash &options) const;

        /*! Throw an exception if configuration contains the 'schema' option. */
        void throwIfContainsSchemaOption() const;
        /*! Throw exception if the 'search_path' option isn't QString or QStringList. */
        void throwIfSearchPathHasWrongType() const;
        /*! Throw an exception if the 'dont_drop' option isn't QString or QStringList. */
        void throwIfDontDropHasWrongType() const;
    };

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATIONS_POSTGRESCONFIGURATIONPARSER_HPP
