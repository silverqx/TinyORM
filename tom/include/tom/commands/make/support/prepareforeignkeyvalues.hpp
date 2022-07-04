#pragma once
#ifndef TOM_COMMANDS_MAKE_SUPPORT_PREPAREFOREIGNKEYVALUES_HPP
#define TOM_COMMANDS_MAKE_SUPPORT_PREPAREFOREIGNKEYVALUES_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QStringList>

#include <optional>
#include <unordered_set>

#include <orm/constants.hpp>

#include "tom/commands/make/modelcommandtypes.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{
    class ModelCommand;

namespace Support
{

    /*! Divide foreign key names by relation types for the ModelCreator class. */
    class PrepareForeignKeyValues
    {
        Q_DISABLE_COPY(PrepareForeignKeyValues)

    public:
        /*! Default constructor. */
        PrepareForeignKeyValues(ModelCommand &modelCommand, QStringList &&values);
        /*! Default destructor. */
        inline ~PrepareForeignKeyValues() = default;

        /*! Divide foreign key names by relation types for the ModelCreator class. */
        ForeignKeys prepareValues();

    private:
        /*! Try to start a new relation during foreign key names search. */
        bool startNewRelation(QString &option);

        /*! Insert the default value if no foreign key was passed on the command-line. */
        void insertEmptyForeignKeyValue();
        /*! Foreign key name found, assign it to the correct relation type. */
        void insertForeignKeyValue();
        /*! Foreign key name found, assign it to the correct relation type (for btm). */
        void insertForeignKeyValueBtm();

        /*! Show unused foreign key option warning. */
        void showUnusedForeignKeyWarning();

        /* Getters */
        /*! Reference to the ModelCommand. */
        inline ModelCommand &modelCommand();
        /*! Reference to the ModelCommand, const version. */
        inline const ModelCommand &modelCommand() const;
        /*! Set of all cmd. option relation names. */
        const std::unordered_set<QString> &relationNames();

        /*! Reference to the ModelCommand. */
        std::reference_wrapper<ModelCommand> m_modelCommand;
        /*! The given values to prepare. */
        QStringList m_values;

        /*! Currently processed relation name. */
        QString m_currentRelation {};
        /*! Next index value to process in the given values list. */
        QStringList::size_type m_valueIndex = 0;
        /*! Determine whether a value for currently processed relation was set. */
        bool m_wasForeignKeySet = false;
        /*! Determine whether a value for currently processed relation was set
            (multi-value foreign key for btm relation). */
        bool m_wasForeignKeySetPartial = false;

        /*! Result of all prepared foreign keys. */
        ForeignKeys m_preparedValues {};
    };

    /* private */

    /* Getters */

    ModelCommand &PrepareForeignKeyValues::modelCommand()
    {
        return m_modelCommand;
    }

    const ModelCommand &PrepareForeignKeyValues::modelCommand() const
    {
        return m_modelCommand;
    }

} // namespace Support
} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_SUPPORT_PREPAREFOREIGNKEYVALUES_HPP
