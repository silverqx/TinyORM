#pragma once
#ifndef ORM_CONFIGURATIONS_SQLITECONFIGURATIONPARSER_HPP
#define ORM_CONFIGURATIONS_SQLITECONFIGURATIONPARSER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/configurations/configurationparser.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Configurations
{

    /*! SQLite configuration parser validates and prepares configuration
        options, the parsed configuration will be used in the SQLiteConnector. */
    class SQLiteConfigurationParser final : public ConfigurationParser
    {
        Q_DISABLE_COPY_MOVE(SQLiteConfigurationParser)

    public:
        /*! Default constructor. */
        SQLiteConfigurationParser() = default;
        /*! Virtual destructor. */
        ~SQLiteConfigurationParser() final = default;

    protected:
        /*! Parse the driver-specific database configuration. */
        void parseDriverSpecificOptions() const final;
        /*! Parse the driver-specific 'options' configuration option. */
        void parseDriverSpecificOptionsOption(QVariantHash &options) const final;
    };

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATIONS_SQLITECONFIGURATIONPARSER_HPP
