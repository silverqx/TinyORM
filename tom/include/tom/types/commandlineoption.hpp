#pragma once
#ifndef TOM_TYPES_COMMANDLINEOPTION_HPP
#define TOM_TYPES_COMMANDLINEOPTION_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QCommandLineOption>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
namespace Types
{

    /*! Command line option with a hidden option support (wrapper around
        the QCommandLineOption). */
    class CommandLineOption : public QCommandLineOption
    {
    public:
        /*! Inherit constructors. */
        using QCommandLineOption::QCommandLineOption;

        /*! Constructor with a hidden option support. */
        inline CommandLineOption(
                const QString &name, const QString &description, bool hidden,
                const QString &valueName = {}, const QString &defaultValue = {});
        /*! Constructor with a hidden option support. */
        inline CommandLineOption(
                const QStringList &names, const QString &description, bool hidden,
                const QString &valueName = {}, const QString &defaultValue = {});

        /*! Determine whether this option is a hidden option. */
        inline bool hidden() const noexcept;
        /*! Setter for the hidden option. */
        inline CommandLineOption &setHidden(bool value = true) noexcept;

    private:
        /*! Is this option a hidden option? */
        bool m_hidden = false;
    };

    /* public */

    CommandLineOption::CommandLineOption(
            const QString &name, const QString &description, const bool hidden,
            const QString &valueName, const QString &defaultValue
    )
        : QCommandLineOption(name, description, valueName, defaultValue)
        , m_hidden(hidden)
    {}

    CommandLineOption::CommandLineOption(
            const QStringList &names, const QString &description, const bool hidden,
            const QString &valueName, const QString &defaultValue
    )
        : QCommandLineOption(names, description, valueName, defaultValue)
        , m_hidden(hidden)
    {}

    bool CommandLineOption::hidden() const noexcept
    {
        return m_hidden;
    }

    CommandLineOption &CommandLineOption::setHidden(const bool value) noexcept
    {
        m_hidden = value;

        return *this;
    }

} // namespace Types

    using CommandLineOption = Tom::Types::CommandLineOption;

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TYPES_COMMANDLINEOPTION_HPP
