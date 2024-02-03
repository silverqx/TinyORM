#pragma once
#ifndef TOM_CONCERNS_INTERACTSWITHIO_HPP
#define TOM_CONCERNS_INTERACTSWITHIO_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QStringList>

#include <tabulate/table.hpp>
/* This header exists from the tabulate v1.4.0, it has been added in the middle of v1.3.0
   and didn't exist at the v1.3.0 release. */
#if __has_include(<tabulate/tabulate.hpp>)
#  include <tabulate/tabulate.hpp>
#endif

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

class QCommandLineParser;

TINYORM_BEGIN_COMMON_NAMESPACE

// Defines to detect the tabulate version
#ifndef TABULATE_VERSION_MAJOR
#  define TABULATE_VERSION_MAJOR 0
#  define TABULATE_VERSION_MINOR 0
#  define TABULATE_VERSION_PATCH 0
#endif

#define TINY_TABULATE_VERSION QT_VERSION_CHECK(TABULATE_VERSION_MAJOR, \
                                               TABULATE_VERSION_MINOR, \
                                               TABULATE_VERSION_PATCH)
namespace Tom
{
    class Application;
    class Terminal;

namespace Concerns
{

    /*! Set of methods for the console output/input. */
    class SHAREDLIB_EXPORT InteractsWithIO
    {
        Q_DISABLE_COPY(InteractsWithIO)

        // To access private ctor and errorWallInternal() (used by logException())
        friend Tom::Application;

    public:
#if TINY_TABULATE_VERSION >= QT_VERSION_CHECK(1, 5, 0)
        /*! Alias for the tabulate cell. */
        using TableCell = tabulate::Table::Row_t::value_type;
#elif TINY_TABULATE_VERSION == QT_VERSION_CHECK(1, 4, 0)
        /*! Alias for the tabulate cell. */
        using TableCell = std::variant<std::string, const char *, tabulate::Table>;
#else
        /*! Alias for the tabulate cell. */
        using TableCell = std::variant<std::string, tabulate::Table>;
#endif

#if TINY_TABULATE_VERSION >= QT_VERSION_CHECK(1, 5, 0)
        /*! Alias for the tabulate row. */
        using TableRow = tabulate::Table::Row_t;
        // Check the type because we have no control over this type
        static_assert (std::is_same_v<TableRow, std::vector<TableCell>>,
                "The InteractsWithIO::TableRow must be the std::vector<TableCell> type.");
#else
        /*! Alias for the tabulate row. */
        using TableRow = std::vector<TableCell>;
#endif

        /*! Constructor. */
        explicit InteractsWithIO(const QCommandLineParser &parser);
        /*! Virtual destructor. */
        virtual ~InteractsWithIO();

        /*! Verbosity levels. */
        enum struct Verbosity {
            Quiet       = 0x0001,
            Normal      = 0x0002,
            Verbose     = 0x0004,
            VeryVerbose = 0x0008,
            Debug       = 0x0010,
        };
        /*! Quiet verbosity. */
        constexpr static Verbosity Quiet       = Verbosity::Quiet;
        /*! Normal verbosity (default). */
        constexpr static Verbosity Normal      = Verbosity::Normal;
        /*! Verbose verbosity. */
        constexpr static Verbosity Verbose     = Verbosity::Verbose;
        /*! Very verbose verbosity. */
        constexpr static Verbosity VeryVerbose = Verbosity::VeryVerbose;
        /*! Debug verbosity. */
        constexpr static Verbosity Debug       = Verbosity::Debug;

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
        const InteractsWithIO &newLine(int count = 1,
                                       Verbosity verbosity = Normal) const;
        /*! Write a blank line, wide version. */
        const InteractsWithIO &newLineErr(int count = 1,
                                          Verbosity verbosity = Normal) const;

        /*! Format input to textual table. */
        const InteractsWithIO &
        table(const TableRow &headers, const std::vector<TableRow> &rows,
              Verbosity verbosity = Normal) const;

        /*! Confirm a question with the user. */
        bool confirm(const QString &question, bool defaultAnswer = false) const;

        /*! Remove tom ansi tags from the given string. */
        static QString stripAnsiTags(QString string);

        /* Getters / Setters */
        /*! Run the given callable with disabled ansi output support. */
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
        /*! Default constructor (used by the TomApplication, instance is initialized
            later in the TomApplication::parseCommandLine()). */
        InteractsWithIO();
        /*! Initialize instance like the second constructor do, allows to create
            an instance in two steps. */
        void initialize(const QCommandLineParser &parser);

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
        /*! Constructor (used by TomApplication::logException()). */
        explicit InteractsWithIO(bool noAnsi);

        /*! Repalce text tags with ANSI sequences. */
        static QString parseOutput(QString string, bool isAnsi = true);

        /*! Initialize verbosity by set options in the command-line parser. */
        static Verbosity initializeVerbosity(const QCommandLineParser &parser);
        /*! Initialize ansi support by set options in the command-line parser. */
        static std::optional<bool> initializeAnsi(const QCommandLineParser &parser);
        /*! Initialize ansi support by noAnsi passed to the Application::logException. */
        static std::optional<bool> initializeNoAnsi(bool noAnsi);

        /*! Number of the option name set on the command-line (used by eg. -vvv). */
        static QStringList::size_type
        countSetOption(const QString &optionName, const QCommandLineParser &parser);
        /*! Determine whether discard output by the current and the given verbosity. */
        bool dontOutput(Verbosity verbosity) const;

        /*! Should the given output use ansi? (ansi is disabled for non-tty). */
        bool isAnsiOutput(const std::ostream &cout = std::cout) const;
        /*! Should the given output use ansi? (ansi is disabled for non-tty),
            wide version. */
        bool isAnsiWOutput(const std::wostream &cout = std::wcout) const;

        /*! Write a string as error output (red box with a white text). */
        QString errorWallInternal(const QString &string) const;
        /*! Compute a reserve value for the QStringList lines. */
        static QStringList::size_type
        computeReserveForErrorWall(const QStringList &splitted, int maxLineWidth);

        /*! Alias for the tabulate color. */
        using Color = tabulate::Color;
        /*! Default tabulate table colors. */
        struct TableColors
        {
            Color green = Color::green;
            Color red   = Color::red;
        };
        /*! Initialize tabulate table colors by supported ansi. */
        TableColors initializeTableColors() const;

        /*! Is this input means interactive? */
        bool m_interactive = true;
        /*! Current application verbosity (defined by passed command-line options). */
        Verbosity m_verbosity = Normal;
        /*! Current application ansi passed by command-line option. */
        std::optional<bool> m_ansi = std::nullopt;
        /*! Describes current terminal features. */
        std::unique_ptr<Terminal> m_terminal;
    };

    /* public */

    /* Getters / Setters */

    InteractsWithIO &InteractsWithIO::enableAnsi()
    {
        m_ansi = true;

        return *this;
    }

    InteractsWithIO &InteractsWithIO::disableAnsi()
    {
        m_ansi = false;

        return *this;
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
