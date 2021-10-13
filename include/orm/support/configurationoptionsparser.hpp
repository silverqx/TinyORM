#pragma once
#ifndef CONFIGURATIONOPTIONSPARSER_HPP
#define CONFIGURATIONOPTIONSPARSER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>
#include <QVariantHash>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

namespace Connectors
{
    class Connector;
}

namespace Support
{

    /*! QSqlDatabase connection configuration parser, validate, prepare, and merge
        QSqlDatabase connection options, these are the settings passed
        to the QSqlDatabase::setConnectOptions(). */
    class ConfigurationOptionsParser
    {
    public:
        /*! Constructor. */
        ConfigurationOptionsParser(const Connectors::Connector &connector);

        /*! Parse the database configuration, validate, prepare, and merge connection
            options. */
        QString parseConfiguration(const QVariantHash &config) const;
    protected:
        /*! Validate the 'options' configuration type, has to be the QString or
            QVariantHash. */
        void validateConfigOptions(const QVariant &options) const;
        /*! Prepare options for parseConfigOptions() function, convert to
            the QVariantHash if needed. */
        QVariantHash prepareConfigOptions(const QVariant &options) const;
        /*! Merge the TinyORM connector options with user's provided connection
            options defined in the config. */
        QVariantHash mergeOptions(const QVariantHash &connectortOptions,
                                  const QVariantHash &preparedConfigOptions) const;
        /*! Stringify merged options. */
        QString joinOptions(const QVariantHash &options) const;

        /*! Connector instance to obtain connection options from. */
        const Connectors::Connector &m_connector;
    };

} // namespace Orm::Support
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONFIGURATIONOPTIONSPARSER_HPP
