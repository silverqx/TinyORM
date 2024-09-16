#include "tom/concerns/interactswithio.hpp"

#include <QCommandLineParser>

#include <cmath>

#include <orm/constants.hpp>
#include <orm/utils/string.hpp>

#include "tom/terminal.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using tabulate::Table;

using Orm::Constants::ASTERISK;
using Orm::Constants::NEWLINE;
using Orm::Constants::NOSPACE;
using Orm::Constants::SPACE;
using Orm::Constants::TMPL_ONE;

using StringUtils = Orm::Utils::String;

using Tom::Constants::EMPTY;
using Tom::Constants::ansi;
using Tom::Constants::noansi;
using Tom::Constants::nointeraction;
using Tom::Constants::quiet;

namespace Tom::Concerns
{

/* public */

InteractsWithIO::InteractsWithIO(const QCommandLineParser &parser)
    : m_interactive(!parser.isSet(nointeraction))
    , m_verbosity(initializeVerbosity(parser))
    , m_ansi(initializeAnsi(parser))
    , m_terminal(std::make_unique<Terminal>())
{}

// Needed by a unique_ptr()
InteractsWithIO::~InteractsWithIO() = default;

/* protected */

// Needed by a unique_ptr()
InteractsWithIO::InteractsWithIO()
    : m_terminal(nullptr)
{}

void InteractsWithIO::initialize(const QCommandLineParser &parser)
{
    m_interactive = !parser.isSet(nointeraction);
    m_verbosity   = initializeVerbosity(parser);
    m_ansi        = initializeAnsi(parser);
    m_terminal    = std::make_unique<Terminal>();
}

/* private */

InteractsWithIO::InteractsWithIO(const bool noAnsi)
    : m_ansi(initializeNoAnsi(noAnsi))
    , m_terminal(std::make_unique<Terminal>())
{}

/* public */

const InteractsWithIO &
InteractsWithIO::line(const QString &string, const bool newline,
                      const Verbosity verbosity, const QString &style,
                      std::ostream &cout) const
{
    if (dontOutput(verbosity))
        return *this;

    static const auto tmplStyled = u"<%3>%1</%3>%2"_s;

    const auto parsedString = parseOutput(string, isAnsiOutput(cout));

    if (style.isEmpty())
        cout << NOSPACE.arg(parsedString, newline ? NEWLINE : EMPTY).toStdString();
    else
        cout << tmplStyled.arg(parsedString, newline ? NEWLINE : EMPTY, style)
                          .toStdString();

    return *this;
}

const InteractsWithIO &
InteractsWithIO::note(const QString &string, const bool newline,
                      const Verbosity verbosity) const
{
    return line(string, newline, verbosity);
}

const InteractsWithIO &
InteractsWithIO::info(const QString &string, const bool newline,
                      const Verbosity verbosity) const
{
    return line(u"<info>%1</info>"_s.arg(string), newline, verbosity);
}

const InteractsWithIO &
InteractsWithIO::error(const QString &string, const bool newline,
                       const Verbosity verbosity) const
{
    return line(u"<error>%1</error>"_s.arg(string), newline, verbosity,
                {}, std::cerr);
}

const InteractsWithIO &
InteractsWithIO::comment(const QString &string, const bool newline,
                         const Verbosity verbosity) const
{
    return line(u"<comment>%1</comment>"_s.arg(string), newline, verbosity);
}

const InteractsWithIO &
InteractsWithIO::muted(const QString &string, const bool newline,
                       const Verbosity verbosity) const
{
    return line(u"<muted>%1</muted>"_s.arg(string), newline, verbosity);
}

const InteractsWithIO &InteractsWithIO::alert(const QString &string,
                                              const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    const auto asterisks = ASTERISK.repeated(string.size() + 12);

    comment(asterisks);
    comment(u"*     %1     *"_s.arg(string));
    comment(asterisks);

    newLine();

    return *this;
}

const InteractsWithIO &InteractsWithIO::errorWall(const QString &string,
                                                  const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    // Do not print an error wall when ANSI is disabled
    if (!isAnsiOutput())
        return line(string, true, verbosity, {}, std::cerr);

    static const auto tmpl = u"%1%2%1"_s.arg(NEWLINE, TMPL_ONE);

    line(tmpl.arg(errorWallInternal(string)), true, verbosity, {}, std::cerr);

    return *this;
}

const InteractsWithIO &
InteractsWithIO::wline(const QString &string, const bool newline,
                       const Verbosity verbosity, const QString &style,
                       std::wostream &wcout) const
{
    if (dontOutput(verbosity))
        return *this;

    static const auto tmplStyled = u"<%3>%1</%3>%2"_s;

    const auto parsedString = parseOutput(string, isAnsiWOutput(wcout));

    if (style.isEmpty())
        wcout << NOSPACE.arg(parsedString, newline ? NEWLINE : EMPTY).toStdWString();
    else
        wcout << tmplStyled.arg(parsedString, newline ? NEWLINE : EMPTY, style)
                           .toStdWString();

    return *this;
}

const InteractsWithIO &
InteractsWithIO::wnote(const QString &string, const bool newline,
                       const Verbosity verbosity) const
{
    return wline(string, newline, verbosity);
}

const InteractsWithIO &
InteractsWithIO::winfo(const QString &string, const bool newline,
                       const Verbosity verbosity) const
{
    return wline(u"<info>%1</info>"_s.arg(string), newline, verbosity);
}

const InteractsWithIO &
InteractsWithIO::werror(const QString &string, const bool newline,
                        const Verbosity verbosity) const
{
    return wline(u"<error>%1</error>"_s.arg(string), newline, verbosity, {}, std::wcerr);
}

const InteractsWithIO &
InteractsWithIO::wcomment(const QString &string, const bool newline,
                          const Verbosity verbosity) const
{
    return wline(u"<comment>%1</comment>"_s.arg(string), newline, verbosity);
}

const InteractsWithIO &
InteractsWithIO::wmuted(const QString &string, const bool newline,
                        const Verbosity verbosity) const
{
    return wline(u"<muted>%1</muted>"_s.arg(string), newline, verbosity);
}

const InteractsWithIO &InteractsWithIO::walert(const QString &string,
                                               const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    const auto asterisks = ASTERISK.repeated(string.size() + 12);

    wcomment(asterisks);
    wcomment(u"*     %1     *"_s.arg(string));
    wcomment(asterisks);

    newLine();

    return *this;
}

const InteractsWithIO &InteractsWithIO::werrorWall(const QString &string,
                                                   const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    // Do not print an error wall when ANSI is disabled
    if (!isAnsiWOutput())
        return wline(string, true, verbosity, {}, std::wcerr);

    static const auto tmpl = u"%1%2%1"_s.arg(NEWLINE, TMPL_ONE);

    wline(tmpl.arg(errorWallInternal(string)), true, verbosity, {}, std::wcerr);

    return *this;
}

const InteractsWithIO &
InteractsWithIO::newLine(const quint16 count, const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    std::cout << std::string(count, '\n');

    return *this;
}

const InteractsWithIO &
InteractsWithIO::newLineErr(const quint16 count, const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    std::cerr << std::string(count, '\n');

    return *this;
}

const InteractsWithIO &
InteractsWithIO::table(const TableRow &header, const std::vector<TableRow> &rows,
                       const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    Table table;

    // TODO study tom, "locale::facet::_S_create_c_locale name not valid" for all locales, how the hell it works? silverqx
    /* Set locale to C until I discover how the heck this works on MSYS2 because every
       other locale throws "locale::facet::_S_create_c_locale name not valid",
       ok it looks like MSYS2 only supports "C" and "POSIX" locale, nothing else. */
#ifdef __MINGW32__
    table.format().locale("C");
#endif

    table.add_row(header);

    for (const auto &row : rows)
        table.add_row(row);

    // Initialize tabulate table colors by supported ANSI
    const auto [green, red] = initializeTableColors();

    // Format table
    // Green text in header
    table.row(0).format().font_color(green);

    for (std::size_t i = 1; i <= rows.size() ; ++i) {
        auto &row = table.row(i);

        // Remove line between rows in the tbody
        if (i > 1)
            row.format().hide_border_top();

        // Align the Batch column to the right (must be after the hide_border_top())
        row.cell(2).format().font_align(tabulate::FontAlign::right);

        // Ran? column : Yes - green, No - red
        {
            auto &cell0 = row.cell(0);
            auto &format = cell0.format();

            if (cell0.get_text() == "Yes")
                format.font_color(green);
            else
                format.font_color(red);
        }
    }

    std::cout << table << std::endl; // NOLINT(performance-avoid-endl)

    return *this;
}

bool InteractsWithIO::confirm(const QString &question, const bool defaultAnswer) const
{
    if (!m_interactive)
        return defaultAnswer;

    info(u"%1 (yes/no) "_s.arg(question), false);

    comment(u"[%1]"_s.arg(defaultAnswer ? u"yes"_s : u"no"_s));
    note(u"> "_s, false);

    /* MSVC contains a bug, it doesn't work with std::cin, it throws assert,
       the std::wcin works well, even emoji-s work.

       For MSYS2 the Unicode support for non-Unicode programs must be enabled/checked
       in Control Panel - Region - Administrative - Language for non-Unicode programs:
       ✔️ Beta: Use Unicode UTF-8 for worldwide language support
       It doesn't work without this, I've tried all possible combinations, but I think
       I would need to manually translate the current ANSI code page to UTF-8 using
       the WideCharToMultiByte() or even std::codecvt() functions, and that is a waste
       of time because we don't need it, currently, we only read 'yes' and latin1 is
       enough for this.

       std::noskipws - if contains whitespaces at the beginning, treat it as an empty
                       string, it returns the false answer. */
#if defined(_MSC_VER) || defined(__MINGW32__)
    std::wstring answerRaw;
    std::wcin >> std::noskipws >> answerRaw;

    const auto answer = QString::fromStdWString(answerRaw).toLower();
#else
    std::string answerRaw;
    std::cin >> std::noskipws >> answerRaw;

    const auto answer = QString::fromStdString(answerRaw).toLower();
#endif

    return answer == u'y' || answer == "ye"_L1 || answer == "yes"_L1;
}

QString InteractsWithIO::stripAnsiTags(QString string)
{
    return string
            .replace(u"<note>"_s,     EMPTY)
            .replace(u"</note>"_s,    EMPTY)
            .replace(u"<info>"_s,     EMPTY)
            .replace(u"<error>"_s,    EMPTY)
            .replace(u"<comment>"_s,  EMPTY)
            .replace(u"<muted>"_s,    EMPTY)
            .replace(u"<blue>"_s,     EMPTY)
            .replace(u"<gray>"_s,     EMPTY)
            .replace(u"<b-blue>"_s,   EMPTY)
            .replace(u"<b-white>"_s,  EMPTY)
            .replace(u"</info>"_s,    EMPTY)
            .replace(u"</error>"_s,   EMPTY)
            .replace(u"</comment>"_s, EMPTY)
            .replace(u"</muted>"_s,   EMPTY)
            .replace(u"</blue>"_s,    EMPTY)
            .replace(u"</gray>"_s,    EMPTY)
            .replace(u"</b-blue>"_s,  EMPTY)
            .replace(u"</b-white>"_s, EMPTY);
}

/* Getters / Setters */

void InteractsWithIO::withoutAnsi(const std::function<void()> &callback)
{
    // Nothing to do, ANSI is already disabled
    if (m_ansi && !m_ansi.value())
        return std::invoke(callback); // NOLINT(readability-avoid-return-with-void-value) clazy:exclude=returning-void-expression

    // Backup the current m_ansi value
    auto previousAnsi = m_ansi;

    disableAnsi();

    try {
        std::invoke(callback);

        m_ansi = previousAnsi;

    } catch (...) {

        m_ansi = previousAnsi;
        // Re-throw
        throw;
    }
}

/* private */

QString InteractsWithIO::parseOutput(QString string, const bool isAnsi)
{
    // FUTURE ansi, keep track and restore previous styles, don't use ESC[0m, practically recursive parser with nested tags needed silverqx
    // ANSI output
    if (isAnsi)
        return string
                .replace(u"<note>"_s,     EMPTY)
                .replace(u"</note>"_s,    EMPTY)
                .replace(u"<info>"_s,     u"\033[32m"_s)
                .replace(u"<error>"_s,    u"\033[37;41m"_s)
                .replace(u"<comment>"_s,  u"\033[33m"_s)
                .replace(u"<muted>"_s,    u"\033[90m"_s) // Bright black
                .replace(u"<blue>"_s,     u"\033[34m"_s)
                .replace(u"<gray>"_s,     u"\033[90m"_s) // Bright black
                .replace(u"<b-blue>"_s,   u"\033[94m"_s)
                .replace(u"<b-white>"_s,  u"\033[97m"_s)
                .replace(u"</info>"_s,    u"\033[0m"_s)
                .replace(u"</error>"_s,   u"\033[0m"_s)
                .replace(u"</comment>"_s, u"\033[0m"_s)
                .replace(u"</muted>"_s,   u"\033[0m"_s)
                .replace(u"</blue>"_s,    u"\033[0m"_s)
                .replace(u"</gray>"_s,    u"\033[0m"_s)
                .replace(u"</b-blue>"_s,  u"\033[0m"_s)
                .replace(u"</b-white>"_s, u"\033[0m"_s);

    // --no-ansi output
    return stripAnsiTags(std::move(string));
}

InteractsWithIO::Verbosity
InteractsWithIO::initializeVerbosity(const QCommandLineParser &parser)
{
    if (parser.isSet(quiet))
        return Quiet;

    const auto verboseCount = countSetOption(u"v"_s, parser);

    if (verboseCount == 1)
        return Verbose;

    if (verboseCount == 2)
        return VeryVerbose;

    if (verboseCount >= 3)
        return Debug;

    return Normal;
}

std::optional<bool>
InteractsWithIO::initializeAnsi(const QCommandLineParser &parser)
{
    // --ansi option has higher priority
    if (parser.isSet(ansi))
        return true;

    if (parser.isSet(noansi))
        return false;

    return std::nullopt;
}

std::optional<bool>
InteractsWithIO::initializeNoAnsi(const bool noAnsi)
{
    if (noAnsi)
        return false;

    return std::nullopt;
}

QStringList::size_type
InteractsWithIO::countSetOption(const QString &optionName,
                                const QCommandLineParser &parser)
{
    /* This should be in the CommandLineParser, but I will not create a wrapper class
       because of one line, I don't even create a future todo task for this. */
    return std::ranges::count(parser.optionNames(), optionName);
}

bool InteractsWithIO::dontOutput(const Verbosity verbosity) const
{
    return verbosity > m_verbosity;
}

bool InteractsWithIO::isAnsiOutput(const std::ostream &cout) const
{
    // ANSI was explicitly set on the command-line, respect it
    if (m_ansi)
        return *m_ansi;

    // Instead autodetect
    return m_terminal->hasColorSupport(cout);
}

bool InteractsWithIO::isAnsiWOutput(const std::wostream &wcout) const
{
    // ANSI was explicitly set on the command-line, respect it
    if (m_ansi)
        return *m_ansi;

    // Instead autodetect
    return m_terminal->hasWColorSupport(wcout);
}

namespace
{
    /*! Get max. line size after the split with the newline in all rendered lines. */
    QString::size_type getMaxLineWidth(const QStringList &lines)
    {
        const auto it = std::ranges::max_element(lines, std::less(),
                                                 [](const QString &line)
        {
            return line.size();
        });

        return it->size();
    }

} // namespace

QString InteractsWithIO::errorWallInternal(const QString &string) const
{
    // Nothing to print
    if (QStringView(string).trimmed().isEmpty())
        return string;

    QStringList lines;

    {
        const auto splitted = string.split(NEWLINE, Qt::SkipEmptyParts);

        /* Compute the max. box width */
        // Get max. line width after the split with the newline in all rendered lines
        const auto maxLineWidth = std::min(m_terminal->width() - 4,
                                           static_cast<int>(getMaxLineWidth(splitted)));

        lines.reserve(computeReserveForErrorWall(splitted, maxLineWidth));

        // Split lines by the given width
        for (const auto &lineNl : splitted)
            for (auto &&line : StringUtils::splitStringByWidth(lineNl, maxLineWidth))
                lines << std::move(line);
    }

    QString output;

    {
        // ANSI template
        static const auto tmpl = u"\033[37;41m%1\033[0m"_s;
        // Get final max. line width in all rendered lines (after splitted by the width)
        const auto maxLineWidth = getMaxLineWidth(lines);
        // Full line width (with spaces at the beginning and end)
        const auto fullLineWidth = maxLineWidth + 4;
        // Above/below empty line
        const auto emptyLine = QString(fullLineWidth, SPACE);

        /* Length of line  - 'tmpl.size() - 2' : -2 to exclude %1; '+ 1' : NEWLINE;
           Number of lines - '* (2 +' : empty line above/below
           Final +32 as a reserve. */
        output.reserve(((fullLineWidth + (tmpl.size() - 2) + 1) *
                        (2 + lines.size())) + 32);

        // Empty line above
        output += tmpl.arg(emptyLine).append(NEWLINE);

        for (const auto &line : std::as_const(lines)) {
            // Prepend/append spaces
            auto lineSpaced = u"  %1  "_s.arg(line);
            // Fill a line to the end with spaces
            lineSpaced += QString(fullLineWidth - lineSpaced.size(), SPACE);
            // ANSI wrap
            output += tmpl.arg(lineSpaced).append(NEWLINE);
        }

        // Empty line below
        output += tmpl.arg(emptyLine);
    }

    return output;
}

QStringList::size_type
InteractsWithIO::computeReserveForErrorWall(const QStringList &splitted,
                                            const int maxLineWidth)
{
    QStringList::size_type size = 0;

    for (const auto &line : splitted)
        /* +2 serves as a reserve because the splitting algorithm can decided
           to start a new line if there is <30% free space, +2 is enough. */
        size += std::llround(static_cast<double>(line.size()) / maxLineWidth) + 2;

    return size;
}

InteractsWithIO::TableColors InteractsWithIO::initializeTableColors() const
{
    /* Even is I detect ANSI support as true, tabulate has it's own detection logic,
       it only check isatty(), it has some consequences, eg. no colors when output
       is redirected and --ansi was passed to the tom application, practically all the
       logic in the isAnsiOutput() will be skipped because of this tabulate internal
       logic, not a big deal though. */
    if (isAnsiOutput())
        return {};

    // Disable coloring if no-ansi
    return {Color::none, Color::none};
}

} // namespace Tom::Concerns

TINYORM_END_COMMON_NAMESPACE
