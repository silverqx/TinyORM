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
    /* std::w/ostream don't have a copy constructor so need to check
       for const std::ostream &. */
    /*! Concept for the ostream and wostream. */
    template<typename T>
    concept OStreamConcept = std::convertible_to<T, const std::ostream &> ||
                             std::convertible_to<T, const std::wostream &>;

    /*! Describes current terminal features. */
    class Terminal
    {
        Q_DISABLE_COPY_MOVE(Terminal)

    public:
        /*! Default constructor. */
        Terminal() = default;
        /*! Default destructor. */
        ~Terminal() = default;

        /* Static operations on the Terminal class */
        /*! Prepare the console terminal. */
        static void initialize();
        /*! Determines if a file descriptor is associated with a character device. */
        static bool isatty(FILE *stream) noexcept;

        /*! Terminal width and height. */
        struct TerminalSize
        {
            /*! Visible columns. */
            int columns;
            /*! Visible lines. */
            int lines;
        };

        /*! Get terminal size of the visible area. */
        static TerminalSize terminalSize() noexcept;

        /* Operations on a Terminal instance */
        /*! Supports the given output ANSI colors? (ANSI is disabled without TTY). */
        bool hasColorSupport(const std::ostream &cout = std::cout) const;
        /*! Supports the given output ANSI colors? (ANSI is disabled without TTY),
            wide version. */
        bool hasWColorSupport(const std::wostream &wcout = std::wcout) const;

        /*! Obtain the current terminal width. */
        int width();
        /*! Obtain the current terminal height. */
        int height();

        /*! Get the cached terminal width. */
        inline int lastWidth() const noexcept;
        /*! Get the cached terminal height. */
        inline int lastHeight() const noexcept;

    private:
        /*! Supports the given output ANSI colors? (common logic). */
        template<OStreamConcept O>
        bool hasColorSupportInternal(const O &cout, FILE *stream) const;

#ifdef _WIN32
        /*! Detect if c++ ostream has enabled virtual terminal processing. */
        static bool hasVt100Support(const std::ostream &cout);
        /*! Detect if c++ wostream has enabled virtual terminal processing,
            wide version. */
        static bool hasVt100Support(const std::wostream &wcout);
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

        /*! Cache for detected ANSI output. */
        mutable std::unordered_map<const std::ostream *, bool> m_isAnsiOutput;
        /*! Cache for detected ANSI output, wide version. */
        mutable std::unordered_map<const std::wostream *, bool> m_isAnsiWOutput;

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
    bool Terminal::hasColorSupportInternal(const O &cout, FILE *stream) const
    {
#ifndef _WIN32
        Q_UNUSED(cout)
#endif
        // Follow https://no-color.org/
        if (qEnvironmentVariableIsSet("NO_COLOR"))
            return false;

        // BUG msvc the using Qt::StringLiterals::operator""_L1; doesn't work here, compilation fails silverqx
        using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

        if (qEnvironmentVariable("TERM_PROGRAM") == "Hyper"_L1)
            return isatty(stream);

#ifdef _WIN32
        return isatty(stream) &&
                (hasVt100Support(cout) ||
                 qEnvironmentVariableIsSet("ANSICON") ||
                 qEnvironmentVariable("ConEmuANSI") == "ON"_L1 ||
                 qEnvironmentVariable("TERM") == "xterm"_L1);
#else
        // Detect character device, in most cases false when the output is redirected
        return isatty(stream);
#endif
    }

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TERMINAL_HPP
