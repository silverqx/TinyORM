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

    class ConfigurationParser;

    /*! TinyORM configuration parser base mixin class validates and prepares
        configuration 'options' option, the parsed configuration will be used
        in the Connector-s by the QSqlDatabase::setConnectOptions() method. */
    class ConfigurationOptionsParser
    {
        Q_DISABLE_COPY_MOVE(ConfigurationOptionsParser)

    public:
        /*! Default constructor. */
        ConfigurationOptionsParser() = default;
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

        /*! Copy options from the top-level configuration to the 'options' option hash. */
        void copyOptionsFromTopLevel(QVariantHash &options,
                                     std::vector<QString> &&optionNames,
                                     bool checkLowerCase = false) const;

    private:
        /*! Validate the 'options' configuration type, must be the QString or
            QVariantHash. */
        static void validateConfigOptions(const QVariant &options);
        /*! Prepare the 'options' for the parseDriverSpecificOptionsOption() method,
            convert to the QVariantHash if needed. */
        static QVariantHash prepareConfigOptions(const QVariant &options);
        /*! Split the 'options' string by the ; and also ,. */
        static QList<QStringView> splitConfigOptions(const QString &optionsString);

        /*! Merge the TinyORM's default connector 'options' with the user-defined
            options in the configuration. */
        static QVariantHash mergeOptions(const QVariantHash &connectortOptions,
                                         QVariantHash &&preparedConfigOptions);
        /*! Stringify the prepared 'options' option. */
        static QString concatenateOptions(const QVariantHash &options);

        /*! Get top-level option name (support top-level option names in lowercase). */
        QString getTopLevelOptionName(const QString &option, bool checkLowerCase) const;

        /*! Get a cached configuration reference. */
        QVariantHash &config() const;

        /*! Dynamic cast *this to the ConfigurationParser & derived type. */
        const ConfigurationParser &parser() const;
    };

    /* public */

    ConfigurationOptionsParser::~ConfigurationOptionsParser() = default;

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATIONS_CONFIGURATIONOPTIONSPARSER_HPP
