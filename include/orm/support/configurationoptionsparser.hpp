#pragma once
#ifndef ORM_SUPPORT_CONFIGURATIONOPTIONSPARSER_HPP
#define ORM_SUPPORT_CONFIGURATIONOPTIONSPARSER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>
#include <QVariantHash>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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
        Q_DISABLE_COPY(ConfigurationOptionsParser)

    public:
        /*! Constructor. */
        explicit ConfigurationOptionsParser(const Connectors::Connector &connector);

        /*! Parse the database configuration, validate, prepare, and merge connection
            options. */
        QString parseConfiguration(const QVariantHash &config) const;

    protected:
        /*! Validate the 'options' configuration type, has to be the QString or
            QVariantHash. */
        static void validateConfigOptions(const QVariant &options);
        /*! Prepare options for parseConfigOptions() function, convert to
            the QVariantHash if needed. */
        static QVariantHash prepareConfigOptions(const QVariant &options);

        /*! Merge the TinyORM connector options with user's provided connection
            options defined in the config. */
        static QVariantHash mergeOptions(const QVariantHash &connectortOptions,
                                         const QVariantHash &preparedConfigOptions);
        /*! Stringify merged options. */
        static QString joinOptions(const QVariantHash &options);

        /*! Connector instance to obtain connection options from. */
        const Connectors::Connector &m_connector;
    };

} // namespace Support
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SUPPORT_CONFIGURATIONOPTIONSPARSER_HPP
