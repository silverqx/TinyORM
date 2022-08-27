#pragma once
#ifndef TOM_COMMANDS_MAKE_CONCERNS_PREPAREOPTIONVALUES_HPP
#define TOM_COMMANDS_MAKE_CONCERNS_PREPAREOPTIONVALUES_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QStringList>

#include <orm/macros/commonnamespace.hpp>

#include "tom/commands/make/modelcommandtypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{
    class ModelCommand;

namespace Concerns
{

    /*! Prepare command-line option values for the ModelCreator class. */
    class PrepareOptionValues
    {
        Q_DISABLE_COPY(PrepareOptionValues)

    public:
        /*! Default constructor. */
        inline PrepareOptionValues() = default;
        /*! Virtual destructor. */
        inline virtual ~PrepareOptionValues() = 0;

    protected:
        /*! Prepare foreign key option values for the ModelCreator. */
        ForeignKeys foreignKeyValues();

        /*! Prepare btm option values for the ModelCreator (multi-value option). */
        QStringList btmValues(const QString &optionName);
        /*! Prepare btm option values for the ModelCreator (multi-value option). */
        std::vector<QStringList> btmMultiValues(const QString &optionName);
        /*! Prepare btm option values for the ModelCreator (bool/switch option). */
        std::vector<bool> btmBoolValues(const QString &optionName);

    private:
        /*! Static cast *this to the ModelCommand & derived type. */
        ModelCommand &modelCommand();
    };

    /* public */

    PrepareOptionValues::~PrepareOptionValues() = default;

} // namespace Concerns
} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_CONCERNS_PREPAREOPTIONVALUES_HPP
