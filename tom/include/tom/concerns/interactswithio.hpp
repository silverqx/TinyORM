#pragma once
#ifndef TOM_CONCERNS_INTERACTSWITHIO_HPP
#define TOM_CONCERNS_INTERACTSWITHIO_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QStringList>

#include <iostream>

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

class QCommandLineParser;

namespace tabulate
{
    class Table;
}

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    class Application;
    class Terminal;

namespace Concerns
{

    /*! Set of methods for the console output/input. */
    class TINYORM_EXPORT InteractsWithIO
    {
        Q_DISABLE_COPY_MOVE(InteractsWithIO)

        // To access private constructor and errorWallInternal() (used by logException())
        friend Tom::Application;

        /*! Constructor (used by TomApplication::logException()). */
        explicit InteractsWithIO(bool noAnsi);

    public:
        /*! Constructor. */
        explicit InteractsWithIO(const QCommandLineParser &parser);
        /*! Virtual destructor. */
        virtual ~InteractsWithIO();

        /*! Verbosity levels. */
        enum struct Verbosity : quint8
        {
            /*! Quiet verbosity. */
            Quiet       = 0x0001,
            /*! Normal verbosity (default). */
            Normal      = 0x0002,
            /*! Verbose verbosity. */
            Verbose     = 0x0004,
            /*! Very verbose verbosity. */
            VeryVerbose = 0x0008,
            /*! Debug verbosity. */
            Debug       = 0x0010,
        };
        /*! Expose the Verbosity enum. */
        using enum Verbosity;

        /*! Write a string as standard output. */
        const InteractsWithIO &line(const QString &string, bool newline = true,
                                    Verbosity verbosity = Normal,
                                    const QString &style = {},
                                    std::ostream &cout = std::cout) const;
        /*! Write a string as note output. */
        const InteractsWithIO &note(const QString &string, bool newline = true,
                                    Verbosity verbosity = Normal) const;
        /*! Write a string as information output. */
        const InteractsWithIO &info(const QString &string, bool newline = true,
                                    Verbosity verbosity = Normal) const;
        /*! Write a string as error output. */
        const InteractsWithIO &error(const QString &string, bool newline = true,
                                     Verbosity verbosity = Normal) const;
        /*! Write a string as comment output. */
        const InteractsWithIO &comment(const QString &string, bool newline = true,
                                       Verbosity verbosity = Normal) const;
        /*! Write a string as muted output. */
        const InteractsWithIO &muted(const QString &string, bool newline = true,
                                     Verbosity verbosity = Normal) const;
        /*! Write a string in an alert box. */
        const InteractsWithIO &alert(const QString &string,
                                     Verbosity verbosity = Normal) const;
        /*! Write a string as error output (red box with a white text). */
        const InteractsWithIO &errorWall(const QString &string,
                                         Verbosity verbosity = Normal) const;

        /*! Write a string as standard output, wide version. */
        const InteractsWithIO &wline(const QString &string, bool newline = true,
                                     Verbosity verbosity = Normal,
                                     const QString &style = {},
                                     std::wostream &wcout = std::wcout) const;
        /*! Write a string as note output, wide version. */
        const InteractsWithIO &wnote(const QString &string, bool newline = true,
                                     Verbosity verbosity = Normal) const;
        /*! Write a string as information output, wide version. */
        const InteractsWithIO &winfo(const QString &string, bool newline = true,
                                     Verbosity verbosity = Normal) const;
        /*! Write a string as error output, wide version. */
        const InteractsWithIO &werror(const QString &string, bool newline = true,
                                      Verbosity verbosity = Normal) const;
        /*! Write a string as comment output, wide version. */
        const InteractsWithIO &wcomment(const QString &string, bool newline = true,
                                        Verbosity verbosity = Normal) const;
        /*! Write a string as muted output, wide version. */
        const InteractsWithIO &wmuted(const QString &string, bool newline = true,
                                      Verbosity verbosity = Normal) const;
        /*! Write a string in an alert box, wide version. */
        const InteractsWithIO &walert(const QString &string,
                                      Verbosity verbosity = Normal) const;
        /*! Write a string as error output (red box with a white text). */
        const InteractsWithIO &werrorWall(const QString &string,
                                          Verbosity verbosity = Normal) const;

        /*! Write a blank line. */
        const InteractsWithIO &newLine(quint16 count = 1,
                                       Verbosity verbosity = Normal) const;
        /*! Write a blank line, wide version. */
        const InteractsWithIO &newLineErr(quint16 count = 1,
                                          Verbosity verbosity = Normal) const;

        /*! Alias for the table cell. */
        using TableCell = std::variant<std::string, const char *, std::string_view>;
        /*! Alias for the table row. */
        using TableRow  = std::vector<TableCell>;
        /*! Format the tabulate table callback type. */
        using FormatTableCallback = std::function<void(tabulate::Table &,
                                                       const InteractsWithIO &)>;
        /*! Format input to textual table. */
        const InteractsWithIO &
        table(const TableRow &header, const std::vector<TableRow> &rows,
              const FormatTableCallback &formatCallback = nullptr,
              Verbosity verbosity = Normal) const;

        /*! Confirm a question with the user. */
        bool confirm(const QString &question, bool defaultAnswer = false) const;

        /*! Remove tom ANSI tags from the given string. */
        static QString stripAnsiTags(QString string);

        /* Getters / Setters */
        /*! Should the given output use ANSI? (ANSI is disabled without TTY). */
        bool isAnsiOutput(const std::ostream &cout = std::cout) const;
        /*! Should the given output use ANSI? (ANSI is disabled without TTY),
            wide version. */
        bool isAnsiWOutput(const std::wostream &cout = std::wcout) const;

        /*! Run the given callable with disabled ANSI output support. */
        void withoutAnsi(const std::function<void()> &callback);
        /*! Enable ANSI support. */
        inline InteractsWithIO &enableAnsi();
        /*! Disable ANSI support. */
        inline InteractsWithIO &disableAnsi();
        /*! The ANSI support will be autodetected (set it to std::nullopt). */
        inline InteractsWithIO &autodetectAnsi();
        /*! Get ANSI value. */
        inline const std::optional<bool> &getAnsi() const noexcept;
        /*! Set ANSI value. */
        inline InteractsWithIO &setAnsi(bool value) noexcept;

    protected:
        /*! Get a current verbosity level. */
        inline Verbosity verbosity() const noexcept;
        /*! Is quiet verbosity level? */
        inline bool isQuietVerbosity() const noexcept;
        /*! Is normal verbosity level? */
        inline bool isNormalVerbosity() const noexcept;
        /*! Is verbose verbosity level? */
        inline bool isVerboseVerbosity() const noexcept;
        /*! Is very verbose verbosity level? */
        inline bool isVeryVerboseVerbosity() const noexcept;
        /*! Is debug verbosity level? */
        inline bool isDebugVerbosity() const noexcept;

    private:
        /*! Replace text tags with ANSI sequences. */
        static QString parseOutput(QString string, bool isAnsi = true);

        /*! Initialize verbosity by set options in the command-line parser. */
        static Verbosity initializeVerbosity(const QCommandLineParser &parser);
        /*! Initialize ANSI support by set options in the command-line parser. */
        static std::optional<bool> initializeAnsi(const QCommandLineParser &parser);
        /*! Initialize ANSI support by noAnsi passed to the Application::logException. */
        static std::optional<bool> initializeNoAnsi(bool noAnsi);

        /*! Number of the option name set on the command-line (used by eg. -vvv). */
        static QStringList::size_type
        countSetOption(const QString &optionName, const QCommandLineParser &parser);
        /*! Determine whether discard output by the current and the given verbosity. */
        bool dontOutput(Verbosity verbosity) const;

        /*! Write a string as error output (red box with a white text). */
        QString errorWallInternal(const QString &string) const;
        /*! Compute a reserve value for the QStringList lines. */
        static QStringList::size_type
        computeReserveForErrorWall(const QStringList &splitted, int maxLineWidth);

        /*! Is the input interactive? (don't ask any interactive question if false) */
        bool m_interactive = true;
        /*! Current application verbosity (defined by passed command-line options). */
        Verbosity m_verbosity = Normal;
        /*! Current application ANSI passed by command-line option (nullopt is auto). */
        std::optional<bool> m_ansi;
        /*! Describes features of the current terminal. */
        std::unique_ptr<Terminal> m_terminal;
    };

    /* public */

    /* Getters / Setters */

    InteractsWithIO &InteractsWithIO::enableAnsi()
    {
        m_ansi = true;

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 26815)
#endif
        return *this;
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif
    }

    InteractsWithIO &InteractsWithIO::disableAnsi()
    {
        m_ansi = false;

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 26815)
#endif
        return *this;
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif
    }

    InteractsWithIO &InteractsWithIO::autodetectAnsi()
    {
        m_ansi.reset();

        return *this;
    }

    const std::optional<bool> &InteractsWithIO::getAnsi() const noexcept
    {
        return m_ansi;
    }

    InteractsWithIO &InteractsWithIO::setAnsi(const bool value) noexcept
    {
        m_ansi.emplace(value);

        return *this;
    }

    /* protected */

    InteractsWithIO::Verbosity InteractsWithIO::verbosity() const noexcept
    {
        return m_verbosity;
    }

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 26813)
#endif
    bool InteractsWithIO::isQuietVerbosity() const noexcept
    {
        return m_verbosity == Quiet;
    }

    bool InteractsWithIO::isNormalVerbosity() const noexcept
    {
        return m_verbosity == Normal;
    }

    bool InteractsWithIO::isVerboseVerbosity() const noexcept
    {
        return m_verbosity == Verbose;
    }

    bool InteractsWithIO::isVeryVerboseVerbosity() const noexcept
    {
        return m_verbosity == VeryVerbose;
    }

    bool InteractsWithIO::isDebugVerbosity() const noexcept
    {
        return m_verbosity == Debug;
    }
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_INTERACTSWITHIO_HPP
