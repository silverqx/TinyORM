#pragma once
#ifndef TOM_TERMINAL_HPP
#define TOM_TERMINAL_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <iostream>
#include <unordered_map>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    /*! Concept for the ostream and wostream. */
    template<typename T>
    concept OStreamConcept = std::convertible_to<T, const std::ostream &> ||
                             std::convertible_to<T, const std::wostream &>;

    /*! Describes current terminal features. */
    class Terminal
    {
        Q_DISABLE_COPY(Terminal)

    public:
        /*! Default constructor. */
        inline Terminal() = default;
        /*! Default destructor. */
        inline ~Terminal() = default;

        /*! Prepare the console terminal. */
        static void initialize();

        /*! Supports the given output ansi colors? (ansi is disabled for non-tty). */
        bool hasColorSupport(std::ostream &cout = std::cout) const;
        /*! Supports the given output ansi colors? (ansi is disabled for non-tty),
            wide version. */
        bool hasWColorSupport(std::wostream &wcout = std::wcout) const;

        /*! Determines whether a file descriptor is associated with a character device. */
        bool isatty(FILE *stream) const;

        /*! Obtain the current terminal width. */
        int width();
        /*! Obtain the current terminal height. */
        int height();

        /*! Get the cached terminal width. */
        inline int lastWidth() const noexcept;
        /*! Get the cached terminal height. */
        inline int lastHeight() const noexcept;

        /*! Terminal width and height. */
        struct TerminalSize
        {
            /*! Visible columns. */
            int columns;
            /*! Visible lines. */
            int lines;
        };

        /*! Get terminal size of the visible area. */
        TerminalSize terminalSize() const;

    private:
        /*! Supports the given output ansi colors? (common logic). */
        template<OStreamConcept O>
        bool hasColorSupportInternal(O &&cout, FILE *stream) const;

#ifdef _WIN32
        /*! Detect if c++ ostream has enabled virtual terminal processing. */
        bool hasVt100Support(std::ostream &cout) const;
        /*! Detect if c++ wostream has enabled virtual terminal processing,
            wide version. */
        bool hasVt100Support(std::wostream &wcout) const;
#endif

        /* Terminal initialization */
#ifdef _WIN32
        /*! Enable the UTF-8 console input/output character encoding. */
        static void enableUtf8ConsoleEncoding();
#endif
#ifdef __MINGW32__
        /*! Enable the virtual terminal processing on the out/err output streams. */
        static void enableVt100Support();
#endif

        /*! Cache for detected ansi output. */
        mutable std::unordered_map<std::ostream *, bool> m_isAnsiOutput {};
        /*! Cache for detected ansi output, wide version. */
        mutable std::unordered_map<std::wostream *, bool> m_isAnsiWOutput {};

        /*! Current terminal width. */
        int m_lastWidth = 80;
        /*! Current terminal height. */
        int m_lastHeight = 50;
    };

    /* public */

    int Terminal::lastWidth() const noexcept
    {
        return m_lastWidth;
    }

    int Terminal::lastHeight() const noexcept
    {
        return m_lastHeight;
    }

    /* private */

    template<OStreamConcept O>
    bool Terminal::hasColorSupportInternal(O &&cout, FILE *stream) const
    {
#ifndef _WIN32
        Q_UNUSED(cout)
#endif
        // Follow https://no-color.org/
        if (qEnvironmentVariableIsSet("NO_COLOR"))
            return false;

        if (qEnvironmentVariable("TERM_PROGRAM") == QLatin1String("Hyper"))
            return isatty(stream);

#ifdef _WIN32
        return isatty(stream) &&
                (hasVt100Support(std::forward<O>(cout)) ||
                 qEnvironmentVariableIsSet("ANSICON") ||
                 qEnvironmentVariable("ConEmuANSI") == QLatin1String("ON") ||
                 qEnvironmentVariable("TERM") == QLatin1String("xterm"));
#endif

        // Detect character device, in most cases false when the output is redirected
        return isatty(stream);
    }

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TERMINAL_HPP
