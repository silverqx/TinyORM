#include "tom/concerns/interactswithio.hpp"

#include <QCommandLineParser>

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
                      const Verbosity verbosity, QString &&style,
                      std::ostream &cout) const
{
    if (dontOutput(verbosity))
        return *this;

    static const auto tmplStyled = QStringLiteral("<%3>%1</%3>%2");

    auto parsedString = parseOutput(string, isAnsiOutput(cout));

    if (style.isEmpty())
        cout << NOSPACE
                .arg(std::move(parsedString), newline ? NEWLINE : "").toStdString();
    else
        cout << tmplStyled
                .arg(std::move(parsedString), newline ? NEWLINE : "", std::move(style))
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
    return line(QStringLiteral("<info>%1</info>").arg(string), newline, verbosity);
}

const InteractsWithIO &
InteractsWithIO::error(const QString &string, const bool newline,
                       const Verbosity verbosity) const
{
    return line(QStringLiteral("<error>%1</error>").arg(string), newline, verbosity,
                "", std::cerr);
}

const InteractsWithIO &
InteractsWithIO::comment(const QString &string, const bool newline,
                         const Verbosity verbosity) const
{
    return line(QStringLiteral("<comment>%1</comment>").arg(string), newline, verbosity);
}

const InteractsWithIO &InteractsWithIO::alert(const QString &string,
                                              const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    const auto asterisks = ASTERISK.repeated(string.count() + 12);

    comment(asterisks);
    comment(QStringLiteral("*     %1     *").arg(string));
    comment(asterisks);

    newLine();

    return *this;
}

const InteractsWithIO &InteractsWithIO::errorWall(const QString &string,
                                                  const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    // Do not print an error wall when ansi is disabled
    if (!isAnsiOutput())
        return line(string, true, verbosity, {}, std::cerr);

    static const auto tmpl = QStringLiteral("%1%2%1").arg(NEWLINE, TMPL_ONE);

    line(tmpl.arg(errorWallInternal(string)), true, verbosity, {}, std::cerr);

    return *this;
}

const InteractsWithIO &
InteractsWithIO::wline(const QString &string, const bool newline,
                       const Verbosity verbosity, QString &&style,
                       std::wostream &wcout) const
{
    if (dontOutput(verbosity))
        return *this;

    static const auto tmplStyled = QStringLiteral("<%3>%1</%3>%2");

    auto parsedString = parseOutput(string, isAnsiWOutput(wcout));

    if (style.isEmpty())
        wcout << NOSPACE
                 .arg(std::move(parsedString), newline ? NEWLINE : "").toStdWString();
    else
        wcout << tmplStyled
                 .arg(std::move(parsedString), newline ? NEWLINE : "", std::move(style))
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
    return wline(QStringLiteral("<info>%1</info>").arg(string), newline, verbosity);
}

const InteractsWithIO &
InteractsWithIO::werror(const QString &string, const bool newline,
                        const Verbosity verbosity) const
{
    return wline(QStringLiteral("<error>%1</error>").arg(string), newline, verbosity,
                 "", std::wcerr);
}

const InteractsWithIO &
InteractsWithIO::wcomment(const QString &string, const bool newline,
                          const Verbosity verbosity) const
{
    return wline(QStringLiteral("<comment>%1</comment>").arg(string), newline, verbosity);
}

const InteractsWithIO &InteractsWithIO::walert(const QString &string,
                                               const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    const auto asterisks = ASTERISK.repeated(string.count() + 12);

    wcomment(asterisks);
    wcomment(QStringLiteral("*     %1     *").arg(string));
    wcomment(asterisks);

    newLine();

    return *this;
}

const InteractsWithIO &InteractsWithIO::werrorWall(const QString &string,
                                                   const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    // Do not print an error wall when ansi is disabled
    if (!isAnsiWOutput())
        return wline(string, true, verbosity, {}, std::wcerr);

    static const auto tmpl = QStringLiteral("%1%2%1").arg(NEWLINE, TMPL_ONE);

    wline(tmpl.arg(errorWallInternal(string)), true, verbosity, {}, std::wcerr);

    return *this;
}

const InteractsWithIO &
InteractsWithIO::newLine(const int count, const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    std::cout << std::string(static_cast<std::size_t>(count), '\n');

    return *this;
}

const InteractsWithIO &
InteractsWithIO::newLineErr(const int count, const Verbosity verbosity) const
{
    if (dontOutput(verbosity))
        return *this;

    std::cerr << std::string(static_cast<std::size_t>(count), '\n');

    return *this;
}

const InteractsWithIO &
InteractsWithIO::table(const TableRow &headers, const std::vector<TableRow> &rows,
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

    table.add_row(headers);

    for (const auto &row : rows)
        table.add_row(row);

    // Initialize tabulate table colors by supported ansi
    const auto [green, red] = initializeTableColors();

    // Format table
    // Green text in header
    table.row(0).format().font_color(green);

    for (std::size_t i = 1; i <= rows.size() ; ++i) {
        // Remove line between rows in the tbody
        if (i > 1)
            table.row(i).format().hide_border_top();

        // Ran? column : Yes - green, No - red
        const auto &ran = rows.at(i - 1).front();

        if (!std::holds_alternative<std::string>(ran))
            continue;

        if (auto &format = table.row(i).cell(0).format();
            std::get<std::string>(ran) == "Yes"
        )
            format.font_color(green);
        else
            format.font_color(red);
    }

    std::cout << table << std::endl;

    return *this;
}

bool InteractsWithIO::confirm(const QString &question, const bool defaultAnswer) const
{
    if (!m_interactive)
        return defaultAnswer;

    info(QStringLiteral("%1 (yes/no) ").arg(question), false);

    comment(QStringLiteral("[%1]").arg(defaultAnswer ? QStringLiteral("yes")
                                                     : QStringLiteral("no")));
    note(QStringLiteral("> "), false);

    std::wstring answerRaw;
    std::wcin >> std::noskipws >> answerRaw;

    const auto answer = QString::fromStdWString(answerRaw).toLower();

    return answer == QLatin1String("y") || answer == QLatin1String("ye") ||
           answer == QLatin1String("yes");
}

/* private */

QString InteractsWithIO::parseOutput(QString string, const bool isAnsi)
{
    // FUTURE ansi, keep track and restore previous styles, don't use ESC[0m, practically recursive parser with nested tags needed silverqx
    // ansi output
    if (isAnsi)
        return string
                .replace(QStringLiteral("<note>"),     "")
                .replace(QStringLiteral("</note>"),    "")
                .replace(QStringLiteral("<info>"),     QStringLiteral("\033[32m"))
                .replace(QStringLiteral("<error>"),    QStringLiteral("\033[37;41m"))
                .replace(QStringLiteral("<comment>"),  QStringLiteral("\033[33m"))
                .replace(QStringLiteral("<blue>"),     QStringLiteral("\033[34m"))
                .replace(QStringLiteral("<gray>"),     QStringLiteral("\033[90m")) // Bright black
                .replace(QStringLiteral("<b-blue>"),   QStringLiteral("\033[94m"))
                .replace(QStringLiteral("<b-white>"),  QStringLiteral("\033[97m"))
                .replace(QStringLiteral("</info>"),    QStringLiteral("\033[0m"))
                .replace(QStringLiteral("</error>"),   QStringLiteral("\033[0m"))
                .replace(QStringLiteral("</comment>"), QStringLiteral("\033[0m"))
                .replace(QStringLiteral("</blue>"),    QStringLiteral("\033[0m"))
                .replace(QStringLiteral("</gray>"),    QStringLiteral("\033[0m"))
                .replace(QStringLiteral("</b-blue>"),  QStringLiteral("\033[0m"))
                .replace(QStringLiteral("</b-white>"), QStringLiteral("\033[0m"));

    // no-ansi output
    return stripTags(std::move(string));
}

QString InteractsWithIO::stripTags(QString string)
{
    return string
            .replace(QStringLiteral("<note>"),     "")
            .replace(QStringLiteral("</note>"),    "")
            .replace(QStringLiteral("<info>"),     "")
            .replace(QStringLiteral("<error>"),    "")
            .replace(QStringLiteral("<comment>"),  "")
            .replace(QStringLiteral("<blue>"),     "")
            .replace(QStringLiteral("<gray>"),     "")
            .replace(QStringLiteral("<b-blue>"),   "")
            .replace(QStringLiteral("<b-white>"),  "")
            .replace(QStringLiteral("</info>"),    "")
            .replace(QStringLiteral("</error>"),   "")
            .replace(QStringLiteral("</comment>"), "")
            .replace(QStringLiteral("</blue>"),    "")
            .replace(QStringLiteral("</gray>"),    "")
            .replace(QStringLiteral("</b-blue>"),  "")
            .replace(QStringLiteral("</b-white>"), "");
}

InteractsWithIO::Verbosity
InteractsWithIO::initializeVerbosity(const QCommandLineParser &parser)
{
    if (parser.isSet(quiet))
        return Quiet;

    const auto verboseCount = countSetOption(QStringLiteral("v"), parser);

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
    // Ansi option has higher priority
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
       because of one line, I don't event create a future todo task for this. */
    return static_cast<QStringList::size_type>(
                std::ranges::count(parser.optionNames(), optionName));
}

bool InteractsWithIO::dontOutput(const Verbosity verbosity) const
{
    return verbosity > m_verbosity;
}

bool InteractsWithIO::isAnsiOutput(const std::ostream &cout) const
{
    // ansi was set explicitly on the command-line, respect it
    if (m_ansi)
        return *m_ansi;

    // Instead autodetect
    return m_terminal->hasColorSupport(cout);
}

bool InteractsWithIO::isAnsiWOutput(const std::wostream &wcout) const
{
    // ansi was set explicitly on the command-line, respect it
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
        const auto it = std::ranges::max_element(lines, std::less {},
                                                 [](const auto &line)
        {
            return line.size();
        });

        return (*it).size();
    }

} // namespace

QString InteractsWithIO::errorWallInternal(const QString &string) const
{
    QStringList lines;

    {
        const auto splitted = string.split(NEWLINE, Qt::SkipEmptyParts);

        /* Compute the max. box width */
        // Get max. line width after the split with the newline in all rendered lines
        const auto maxLineWidth = std::min(m_terminal->width() - 4,
                                           static_cast<int>(getMaxLineWidth(splitted)));

        // Split lines by the given width
        for (const auto &lineNl : splitted)
            for (auto &&line : StringUtils::splitStringByWidth(lineNl, maxLineWidth))
                lines << std::move(line);
    }

    QString output;

    {
        // Ansi template
        static const auto tmpl = QStringLiteral("\033[37;41m%1\033[0m");
        // Get final max. line width in all rendered lines (after split by the width)
        const auto maxLineWidth = getMaxLineWidth(lines);
        // Above/below empty line
        auto emptyLine = QString(maxLineWidth + 4, SPACE);

        // Empty line above
        output += tmpl.arg(emptyLine).append(NEWLINE);

        for (auto &&line : lines) {
            // Prepend/append spaces
            auto lineSpaced = QStringLiteral("  %1  ").arg(std::move(line));
            // Fill a line to the end with spaces
            lineSpaced.append(QString(maxLineWidth - lineSpaced.size() + 4, SPACE));
            // Ansi wrap
            output += tmpl.arg(std::move(lineSpaced)).append(NEWLINE);
        }

        // Empty line below
        output += tmpl.arg(std::move(emptyLine));
    }

    return output;
}

InteractsWithIO::TableColors InteractsWithIO::initializeTableColors() const
{
    /* Even is I detect ansi support as true, tabulate has it's own detection logic,
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
