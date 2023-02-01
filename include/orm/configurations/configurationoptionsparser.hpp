#pragma once
#ifndef ORM_CONFIGURATIONS_CONFIGURATIONOPTIONSPARSER_HPP
#define ORM_CONFIGURATIONS_CONFIGURATIONOPTIONSPARSER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Configurations
{

    /*! TinyORM configuration parser base mixin class validates and prepares
        configuration 'options' option, the parsed configuration will be used
        in the Connector-s by the QSqlDatabase::setConnectOptions() method. */
    class ConfigurationOptionsParser
    {
        Q_DISABLE_COPY(ConfigurationOptionsParser)

    public:
        /*! Default constructor. */
        inline ConfigurationOptionsParser() = default;
        /*! Pure virtual destructor. */
        inline virtual ~ConfigurationOptionsParser() = 0;

        /*! Parse the 'options' configuration option. */
        void parseOptionsOption(QVariantHash &config) const;

        /*! Merge and concatenate a default connector options with a prepared options. */
        static QString mergeAndConcatenateOptions(const QVariantHash &connectortOptions,
                                                  const QVariantHash &config);

    protected:
        /*! Parse the driver-specific 'options' configuration option. */
        virtual void parseDriverSpecificOptionsOption(QVariantHash &options) const = 0;

    private:
        /*! Validate the 'options' configuration type, must be the QString or
            QVariantHash. */
        static void validateConfigOptions(const QVariant &options);
        /*! Prepare the 'options' for the parseDriverSpecificOptionsOption() method,
            convert to the QVariantHash if needed. */
        static QVariantHash prepareConfigOptions(const QVariant &options);

        /*! Merge the TinyORM's default connector 'options' with the user-defined
            options in the configuration. */
        static QVariantHash mergeOptions(const QVariantHash &connectortOptions,
                                         QVariantHash &&preparedConfigOptions);
        /*! Stringify the prepared 'options' option. */
        static QString concatenateOptions(const QVariantHash &options);
    };

    /* public */

    ConfigurationOptionsParser::~ConfigurationOptionsParser() = default;

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATIONS_CONFIGURATIONOPTIONSPARSER_HPP
