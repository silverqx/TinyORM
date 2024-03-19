#pragma once
#ifndef ORM_CONFIGURATIONS_MYSQLCONFIGURATIONPARSER_HPP
#define ORM_CONFIGURATIONS_MYSQLCONFIGURATIONPARSER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/configurations/configurationparser.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Configurations
{

    /*! MySQL configuration parser base class validates and prepares configuration
        options, the parsed configuration will be used in the MySqlConnector. */
    class MySqlConfigurationParser final : public ConfigurationParser
    {
        Q_DISABLE_COPY_MOVE(MySqlConfigurationParser)

    public:
        /*! Default constructor. */
        MySqlConfigurationParser() = default;
        /*! Virtual destructor. */
        ~MySqlConfigurationParser() final = default;

    protected:
        /*! Parse the driver-specific database configuration. */
        void parseDriverSpecificOptions() const final;
        /*! Parse the driver-specific 'options' configuration option. */
        void parseDriverSpecificOptionsOption(QVariantHash &options) const final;

    private:
        /*! Throw if the 'options' hash contains an unsupported option. */
        static void throwIfContainsUnsupportedOption(const QVariantHash &options);

        /*! Add the SSL-related options to the connection 'options' hash. */
        void addSslOptions(QVariantHash &options) const;
    };

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATIONS_MYSQLCONFIGURATIONPARSER_HPP
