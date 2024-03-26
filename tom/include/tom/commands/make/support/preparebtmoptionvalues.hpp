#pragma once
#ifndef TOM_COMMANDS_MAKE_SUPPORT_PREPAREBTMOPTIONVALUES_HPP
#define TOM_COMMANDS_MAKE_SUPPORT_PREPAREBTMOPTIONVALUES_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <unordered_set>

#include <orm/constants.hpp>

#include "tom/commands/make/modelcommand.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Support
{

    /*! Prepare belongs-to-many's command-line options for the ModelCreator class. */
    template<BtmPreparedValuesConcept P = QStringList, BtmValuesConcept V = QStringList>
    class PrepareBtmOptionValues
    {
        Q_DISABLE_COPY_MOVE(PrepareBtmOptionValues)

    public:
        /*! Constructor. */
        inline PrepareBtmOptionValues(ModelCommand &modelCommand, V &&values);
        /*! Default destructor. */
        ~PrepareBtmOptionValues() = default;

        /*! Prepare belongs-to-many's command-line options for the ModelCreator class. */
        P prepareValuesFor(const QString &optionName);

    private:
        /*! Check if the prepare algorithm can be skipped and return a list with default
            empty values. */
        std::pair<P, bool> checkBtmNothingToPrepare() const;

        /*! Try to start a new btm relation during the given option values search. */
        bool startNewBtmRelation(const QString &option);

        /*! Check whether the given btm option was defined before a btm relation or
            more options given for the single value option. */
        bool isUnusedBtmOption(const QString &option);

        /*! Insert the default value if no option was passed on the command-line. */
        void insertEmptyBtmValue();

        /*! Option found, assign it to the prepared values list. */
        void insertBtmValue();
        /*! Option found, assign it to the prepared values list (bool/switch option). */
        void insertBtmBoolValue();
        /*! Option found, assign it to the prepared values list (multi-value option). */
        void insertBtmMultiValue();

        /* Getters */
        /*! Reference to the ModelCommand. */
        inline ModelCommand &modelCommand();
        /*! Reference to the ModelCommand, const version. */
        inline const ModelCommand &modelCommand() const;

        /*! Reference to the ModelCommand. */
        std::reference_wrapper<ModelCommand> m_modelCommand;
        /*! The given values to prepare. */
        V m_values;

        /*! Is the btm relation currently being processed? */
        std::optional<bool> m_isBtmRelation = std::nullopt;
        /*! Next index value to process in the given values list. */
        typename V::size_type m_valueIndex = 0;
        /*! Next index value of the btm relation. */
        std::size_t m_btmIndex = 0;
        /*! Determine whether a value for currently searched cmd. option was set. */
        bool m_wasValueSet = false;
        /*! Determine whether a value for currently searched cmd. option was set
            (multi-value cmd. option related). */
        bool m_wasValueSetPartial = false;

        /*! Result list of all processed values. */
        P m_preparedValues {};
    };

    /* public */

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    PrepareBtmOptionValues<P, V>::PrepareBtmOptionValues(ModelCommand &modelCommand,
                                                         V &&values)
        : m_modelCommand(modelCommand)
        , m_values(std::move(values))
    {}

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    P PrepareBtmOptionValues<P, V>::prepareValuesFor(const QString &optionName)
    {
        // Nothing to prepare
        if (auto &&[defaults, result] = checkBtmNothingToPrepare(); result)
            return std::move(defaults);

        auto optionsNames = modelCommand().optionNames();

        m_preparedValues.reserve(static_cast<P::size_type>(optionsNames.size()) - 1);

        for (auto &&option : optionsNames) {
            // Try to start a new btm relation during the given option values search
            if (startNewBtmRelation(option))
                continue;

            // Searching for option value/s after the relation option on the command-line
            // Nothing to do
            if (option != optionName)
                continue;

            /* Btm option defined before a btm relation or more options given for
               the single value option. */
            if (isUnusedBtmOption(option))
                continue;

            // Option found, assign it to the prepared values list
            if constexpr (std::is_same_v<P, std::vector<QStringList>>)
                insertBtmMultiValue();
            else if constexpr (std::is_same_v<P, std::vector<bool>>)
                insertBtmBoolValue();
            else
                insertBtmValue();
        }

        // Handle the last btm relation
        // Any option value/s was passed on the command-line so insert the default value
        if (m_isBtmRelation.value_or(false) && !m_wasValueSet && !m_wasValueSetPartial)
            insertEmptyBtmValue();

        return m_preparedValues;
    }

    /* private */

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    std::pair<P, bool> PrepareBtmOptionValues<P, V>::checkBtmNothingToPrepare() const
    {
        const auto allBtmValues = modelCommand().values(Tom::Constants::belongs_to_many);

        // Nothing to prepare, no btm relations on the command-line (return true)
        if (allBtmValues.isEmpty())
            return {{}, true};

        /* Nothing to prepare, btm relation/s defined on the command-line but no values
           (return true with default values) */
        if (m_values.empty()) {
            const auto valuesSize = static_cast<P::size_type>(allBtmValues.size());

            P result;
            result.reserve(valuesSize);

            // Insert default values
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            result.insert(result.cbegin(), valuesSize, {});
#else
            for (typename P::size_type i = 0; i < valuesSize ; ++i)
                result.push_back({});
#endif

            return {std::move(result), true};
        }

        // There are values to process (return false)
        return {{}, false};
    }

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    bool PrepareBtmOptionValues<P, V>::startNewBtmRelation(const QString &option)
    {
        // Nothing to start
        if (!ModelCommand::relationNames().contains(option))
            return false;

        /* Nothing to start, reset and also no need to insert a default values.
           No previous btm and the option is not the btm relation. */
        if (!m_isBtmRelation && option != Tom::Constants::belongs_to_many)
            return false;

        // Relation option passed on the command-line, start (found relation option)

        const auto wasPreviousBtm = m_isBtmRelation.value_or(false);

        /* Any option value/s was passed on the command-line so insert the default value.
           Insert the default value only if a previous relation was the btm relation. */
        if (wasPreviousBtm && !m_wasValueSet && !m_wasValueSetPartial)
            insertEmptyBtmValue();

        m_isBtmRelation = option == Tom::Constants::belongs_to_many;

        /* Also increase the btm index but not for the first btm relation, the first btm
           relation must have 0 index. */
        if (wasPreviousBtm && m_isBtmRelation)
            ++m_btmIndex;

        // Reset to defaults
        m_wasValueSet = false;
        m_wasValueSetPartial = false;

        return true;
    }

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    bool PrepareBtmOptionValues<P, V>::isUnusedBtmOption(const QString &option)
    {
        /* m_isBtmRelation != std::nullopt means that the btm relation was defined
           on the command-line and a value was not set yet. */
        if (m_isBtmRelation && !m_wasValueSet && m_isBtmRelation.value_or(false))
            return false;

        /* Btm option defined before a btm relation or more options given for
           the single value option. */

        // Will be shown in the warning
        /* Currently, the btm related options don't have any long option with a short
           option alternative, this LongOption.arg() will have to be enhanced if
           we will have such an option, in this case, we will have to map a short option
           to a long one. */
        modelCommand().m_unusedBtmOptions.insert(Tom::Constants::LongOption.arg(option));

        /* Skip the value, only the first option's value is used (and
           m_wasValueSetPartial doesn't matter). */
        if (m_wasValueSet)
            ++m_valueIndex;

        return true;
    }

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    void PrepareBtmOptionValues<P, V>::insertEmptyBtmValue()
    {
        m_preparedValues.push_back({});
    }

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    void PrepareBtmOptionValues<P, V>::insertBtmValue()
    {
        m_preparedValues << m_values.at(m_valueIndex++);

        m_wasValueSet = true;
    }

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    void PrepareBtmOptionValues<P, V>::insertBtmBoolValue()
    {
        m_preparedValues.push_back(m_values.at(m_valueIndex++));

        m_wasValueSet = true;
    }

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    void PrepareBtmOptionValues<P, V>::insertBtmMultiValue()
    {
        const auto preparedValuesSize = m_preparedValues.size();
        Q_ASSERT(preparedValuesSize == m_btmIndex ||
                 preparedValuesSize == m_btmIndex + 1);

        // Create a new list if already doesn't exist
        if (preparedValuesSize == m_btmIndex)
            m_preparedValues.template emplace_back<QStringList>({});

        /* Multiple values can be assigned using one --xyz= option, but they must be
           divided by the colon character. */
        // Or they can be assigned using more --xyz= options
        m_preparedValues[m_btmIndex] << m_values.at(m_valueIndex++);

        m_wasValueSetPartial = true;
    }

    /* Getters */

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    ModelCommand &PrepareBtmOptionValues<P, V>::modelCommand()
    {
        return m_modelCommand;
    }

    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    const ModelCommand &PrepareBtmOptionValues<P, V>::modelCommand() const
    {
        return m_modelCommand;
    }

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_SUPPORT_PREPAREBTMOPTIONVALUES_HPP
