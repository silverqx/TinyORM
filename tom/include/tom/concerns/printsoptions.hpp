#pragma once
#ifndef TOM_CONCERNS_PRINTSOPTIONS_HPP
#define TOM_CONCERNS_PRINTSOPTIONS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

class QCommandLineOption;

namespace Tom
{
namespace Commands
{
    class Command;
}

namespace Concerns
{
    class InteractsWithIO;

    /*! Print options section. */
    class PrintsOptions
    {
        Q_DISABLE_COPY(PrintsOptions)

    public:
        /*! Constructor (int param. to avoid interpret it as copy ctor). */
        PrintsOptions(const QList<QCommandLineOption> &options,
                      const Concerns::InteractsWithIO &io);
        /*! Virtual destructor. */
        inline virtual ~PrintsOptions() = default;

        /*! Print options section. */
        int printOptionsSection(bool commonOptions) const;

    protected:
        /*! Get the default value text (quotes the string type). */
        static QString defaultValueText(const QString &value);

    private:
        /*! Create an option names list prepared for output. */
        static QStringList createOptionNamesList(const QCommandLineOption &option);

        /*! Get max. option size in all options. */
        int optionsMaxSize() const;
        /*! Print options to the console. */
        void printOptions(int optionsMaxSize) const;

        /*! Print an option's default value part. */
        void printOptionDefaultValue(const QCommandLineOption &option) const;

        /*! Validate command option. */
        void validateOption(const QCommandLineOption &option) const;
        /*! Get the command's options list. */
        inline const QList<QCommandLineOption> &options() const noexcept;

        /*! Get the reference to the base InteractsWithIO concern. */
        inline const Concerns::InteractsWithIO &io() const noexcept;

        /*! Reference to the command's options list. */
        std::reference_wrapper<const QList<QCommandLineOption>> m_options;
        /*! Reference to the IO. */
        std::reference_wrapper<const Concerns::InteractsWithIO> m_io;
    };

    /* private */

    const QList<QCommandLineOption> &PrintsOptions::options() const noexcept
    {
        return m_options;
    }

    const InteractsWithIO &PrintsOptions::io() const noexcept
    {
        return m_io;
    }

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_PRINTSOPTIONS_HPP
