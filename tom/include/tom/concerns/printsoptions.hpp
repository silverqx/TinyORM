#pragma once
#ifndef TOM_CONCERNS_PRINTSOPTIONS_HPP
#define TOM_CONCERNS_PRINTSOPTIONS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <orm/macros/commonnamespace.hpp>

#include "tom/types/commandlineoption.hpp"

class QCommandLineOption;

TINYORM_BEGIN_COMMON_NAMESPACE

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
        Q_DISABLE_COPY_MOVE(PrintsOptions)

    public:
        /*! Alias for the QString::size_type. */
        using SizeType = QString::size_type;

        /*! Constructor. */
        PrintsOptions(const QList<CommandLineOption> &options,
                      const Concerns::InteractsWithIO &io);
        /*! Pure virtual destructor. */
        inline virtual ~PrintsOptions() = 0;

        /*! Print options section. */
        SizeType printOptionsSection(bool commonOptions) const;

    private:
        /*! Create an option names list prepared for output. */
        static QStringList createOptionNamesList(const QCommandLineOption &option);

        /*! Get max. option size in all options. */
        SizeType optionsMaxSize() const;
        /*! Print options to the console. */
        void printOptions(SizeType optionsMaxSize) const;

        /*! Print an option's default value part. */
        void printOptionDefaultValue(const QCommandLineOption &option) const;

        /*! Validate command option. */
        static void validateOption(const QCommandLineOption &option);
        /*! Get the command's options list. */
        inline const QList<CommandLineOption> &options() const noexcept;

        /*! Get the reference to the base InteractsWithIO concern. */
        inline const Concerns::InteractsWithIO &io() const noexcept;

        /*! Reference to the command's options list. */
        std::reference_wrapper<const QList<CommandLineOption>> m_options;
        /*! Reference to the IO. */
        std::reference_wrapper<const Concerns::InteractsWithIO> m_io;
    };

    /* public */

    PrintsOptions::~PrintsOptions() = default;

    /* private */

    const QList<CommandLineOption> &PrintsOptions::options() const noexcept
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
