#pragma once
#ifndef TOM_CONCERNS_PRINTSOPTIONS_HPP
#define TOM_CONCERNS_PRINTSOPTIONS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
namespace Commands
{
    class Command;
}

namespace Concerns
{

    /*! Print options section. */
    class PrintsOptions
    {
        Q_DISABLE_COPY(PrintsOptions)

    public:
        /*! Constructor (int param. to avoid interpret it as copy ctor). */
        PrintsOptions(const Commands::Command &command, int);
        /*! Default destructor. */
        inline ~PrintsOptions() = default;

        /*! Print options section. */
        int printOptionsSection(bool commonOptions) const;

    private:
        /*! Get max. option size in all options. */
        int optionsMaxSize() const;
        /*! Print options to the console. */
        void printOptions(int optionsMaxSize) const;

        /*! Reference to the command. */
        std::reference_wrapper<const Commands::Command> m_command;
    };

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_PRINTSOPTIONS_HPP
