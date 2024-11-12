#include "tom/commands/command.hpp"

#include <QCommandLineParser>
#include <QRegularExpression>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/zip_with.hpp>

#include <orm/databasemanager.hpp>

#include "tom/application.hpp"
#include "tom/tomutils.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::ConnectionResolverInterface;

using Tom::Constants::COMMA_C;
using Tom::Constants::Help;
using Tom::Constants::LongOption;
using Tom::Constants::pretty;

using TomUtils = Tom::Utils;

namespace Tom::Commands
{

/* public */

Command::Command(Application &application, QCommandLineParser &parser)
    : Concerns::InteractsWithIO(parser)
    , m_application(application)
    , m_parser(parser)
{}

QList<CommandLineOption> Command::optionsSignature() const
{
    return {};
}

int Command::run()
{
    initializePositionalArgumentsHash();

    auto &parser = this->parser();

    parser.clearPositionalArguments();

    parser.addOptions(TomUtils::convertToQCommandLineOptionList(optionsSignature()));

    if (!parser.parse(passedArguments()))
        showParserError(parser);

    // Show help if --help argument was passed, if it was, the code below will not run
    checkHelpArgument();

    /* Validate if all required positional arguments were passed on the command-line,
       shows an error wall and exit application if validation failed. */
    validateRequiredArguments();

    /* This value can be dropped in descendants as it always returns an EXIT_SUCCESS
       exit code. If something bad happens above then it throws an exception instead.
       So there is no need for any advanced logic for exit codes in descendants. */
    return EXIT_SUCCESS;
}

int Command::runWithArguments(QStringList &&arguments)
{
    m_arguments = std::move(arguments);

    return run();
}

/* Getters */

bool Command::hasPositionalArguments() const
{
    return !positionalArguments().empty();
}

bool Command::hasOptions() const
{
    return !optionsSignature().isEmpty();
}

/* protected */

/* Signature helpers */

const CommandLineOption &Command::getOptionFromSignature(const QString &name) const
{
    /* Also cache the options signature as it is not necessary to keep them up to date as
       they are already parsed and options are not added/updated after parsing. Also,
       this method is called in a loop. */
    static const auto OptionsSignature = optionsSignature();
    static const auto OptionNamesHash  = getOptionsSignatureHash(OptionsSignature);

    return OptionsSignature.at(OptionNamesHash.at(name));
}

/* Parser helpers */

namespace
{
    /*! Find the nth option with the given name in the option names list. */
    auto findNthOption(const QString &optionName, const QString::size_type nthOptionIdx,
                       const QStringList &optionNames)
    {
        QString::size_type nthOptionLooping = 0;

        return std::ranges::find_if(optionNames,
                                    [nthOptionIdx, &nthOptionLooping, &optionName]
                                    (const auto &option)
        {
            if (option != optionName)
                return false;

            // Got it
            if (nthOptionIdx == nthOptionLooping)
                return true;

            // Option name matches but we are at earlier index so increase and continue
            return ++nthOptionLooping, false;
        });
    }
} // namespace

QStringList Command::optionNames() const
{
    /* This algorithm, inserts the same amount of option names as is a count of commas
       in the option's value. Eg. xyz option at position 3 has value with 2 commas,
       --xyz=aa,bb,cc so insert two more xyz option names before 3. position.
       This method is counterpart for our values() method and it's needed because our
       values() method have added support for more values per one option --xyz=aa,bb,cc
       using the , character 🤯. */

    // No caching needed, already cached inside the QCommandLineParser
    auto optionNames = parser().optionNames();

    const auto optionNamesSize = optionNames.size();
    // Number of a new option names can be estimated by counting the , characters
    optionNames.reserve(optionNamesSize + countCommas(passedArguments()) + 8); // +8 as reserve

    /* Allows to loop through all values for every (unique) option name defined
       on the command-line. */
    auto optionNamesUnique = optionNames;
    optionNamesUnique.removeDuplicates();

    for (const auto &optionName : std::as_const(optionNamesUnique)) {
        /* Check if the option has a value name, if the option has a value name then
           it has/expects a value. This will avoid the warning added from Qt v6.7. */
        if (!optionHasValueName(optionName))
            continue;

        // Obtain raw parser().values() so we can count the , char.
        const auto values = parser().values(optionName);

        /* Index of one option name eg. --with-pivot= in all arguments
           (1. option = 0, 2. = 1). */
        QString::size_type nthOptionIdx = 0;

        for (const auto &value : values) {
            // Supports escaped , character using \,
            const auto commasCount = countCommas(value);

            // Nothing to do
            if (commasCount == 0) {
                ++nthOptionIdx;
                continue;
            }

            // Iterator to the option name
            optionNames.insert(findNthOption(optionName, nthOptionIdx, optionNames),
                               commasCount, optionName);

            // Take into account also a newly inserted option names
            ++nthOptionIdx += commasCount;
        }
    }

    return optionNames;
}

bool Command::isSet(const QString &name) const
{
    return parser().isSet(name);
}

std::vector<bool> Command::isSetAll(const QString &name) const
{
    std::vector<bool> result;

    for (const auto &option : parser().optionNames()) // clazy:exclude=range-loop-detach
        if (option == name)
            result.push_back(true);

    return result;
}

QString Command::value(const QString &name) const
{
    return parser().value(name);
}

QStringList Command::values(const QString &name,
                            const Qt::SplitBehavior splitBehavior) const
{
    // No caching needed, already cached inside the QCommandLineParser
    auto values = parser().values(name);

    QStringList valuesSplit;
    valuesSplit.reserve(values.size() + countCommas(values) + 8); // +8 as reserve

    /* Allow to escape , character using \,
       This is a simple implementation that is sufficient for all currently implemented
       options. Once we have a multi-valued option with file/folder paths, this will
       need to be refactored. In this case, it may happen that the folder path may end
       with \ and , is right after it. The correct solution is to do the same as is
       described in NOTES.txt[Parsing C command-line arguments]. */
    static const QRegularExpression RegEx(uR"((?<!\\),)"_s);

    // Support passing more values delimited by comma
    for (auto &value : values) {
        // Nothing to do, no unescaped comma in the value
        if (!containsComma(value)) {
            valuesSplit << std::move(value);
            continue;
        }

        valuesSplit << value.split(RegEx, splitBehavior);
    }

    return valuesSplit;
}

QString Command::valueCmd(const QString &name, const QString &key) const
{
    if (const auto value = parser().value(name);
        !value.isEmpty()
    )
        return u"--%1=%2"_s.arg(key.isEmpty() ? name : key, value);

    return {};
}

QString Command::boolCmd(const QString &name, const QString &key) const
{
    if (!parser().isSet(name))
        return {};

    return LongOption.arg(key.isEmpty() ? name : key);
}

QString Command::longOption(const QString &name)
{
    return LongOption.arg(name);
}

QString Command::longOption(const QString &name, const QString &value)
{
    return u"--%1=%2"_s.arg(name, value);
}

bool Command::hasArgument(const SizeType index) const
{
    /* Has to be isNull(), an argument passed on the command-line still can be an empty
       value, like "", in this case it has to return a true value. */
    return !argument(index, false).isNull();
}

bool Command::hasArgument(const QString &name) const
{
    return m_positionalArguments.contains(name) &&
            !argument(m_positionalArguments.at(name), false).isNull();
}

QStringList Command::arguments() const
{
    // No caching needed, already cached inside the QCommandLineParser
    return parser().positionalArguments();
}

QString Command::argument(const SizeType index, const bool useDefault) const
{
    /* Below is confusing so look at the example for eg. tom help about:
       parser().positionalArguments() will contain {"help", "about"}
       this->positionalArguments() will contain whatever command defines in its
       std::vector<PositionalArgument> HelpCommand::positionalArguments() method.
       Because of this we need -1 for index because HelpCommand::positionalArguments()
       defines positional arguments only without the command name of course. */

    const auto positionalArguments = parser().positionalArguments();

    /* If we don't need the default value then we can return immediately using the basic
       implementation w/o Default value support (QCommandLineParser). */
    if (!useDefault)
        return positionalArguments.value(index);

    return argumentInternal(positionalArguments, index);
}

QString Command::argument(const QString &name, const bool useDefault) const
{
    // Default value supported
    return argument(m_positionalArguments.at(name), useDefault);
}

QJsonDocument::JsonFormat Command::jsonFormat() const
{
    return isSet(pretty) ? QJsonDocument::Indented : QJsonDocument::Compact;
}

/* Getters */

QStringList Command::passedArguments() const
{
    if (!m_arguments.isEmpty())
        return m_arguments;

    /* Never obtain arguments from the QCoreApplication instance in unit tests because
       they are passed using the runWithArguments() method. */
    return application().arguments();
}

Orm::DatabaseConnection &Command::connection(const QString &name) const
{
    return application().db().connection(name);
}

QCommandLineParser &Command::parser() const noexcept
{
    return m_parser;
}

std::shared_ptr<ConnectionResolverInterface>
Command::connectionResolver() const noexcept
{
    return application().connectionResolver();
}

/* Auto tests helpers */

#ifdef TINYTOM_TESTS_CODE
bool Command::inUnitTests() noexcept
{
    return Tom::Application::inUnitTests();
}
#endif

/* private */

/* Lookup hashes */

void Command::initializePositionalArgumentsHash()
{
    const auto &arguments = positionalArguments();

    if (arguments.empty())
        return;

    m_positionalArguments =
            ranges::views::zip_with([](const PositionalArgument &argument,
                                       const SizeType index)
                                    -> std::pair<QString, SizeType>
    {
        return {argument.name, index};
    },
        arguments,
        ranges::views::closed_iota(static_cast<SizeType>(1),
                                   static_cast<SizeType>(arguments.size()))
    )
        | ranges::to<decltype (m_positionalArguments)>();

    // The same as above, I leave above as I want to have one example with zip_with()
//    for (SizeType index = 0; const auto &argument : positionalArguments())
//        m_positionalArguments.emplace(argument.name, ++index);
}

std::unordered_map<QString, Command::SizeType>
Command::getOptionsSignatureHash(const QList<CommandLineOption> &optionsSignature)
{
    std::unordered_map<QString, SizeType> result;
    /* I have no option with more than 2 option names, +8 as reserve.
       Don't use the ranges::accumulate() as the option.names() return by value. */
    result.reserve((optionsSignature.size() * MaxOptionNamesCount) + 8);

    SizeType index = 0;

    for (const auto &optionSignature : optionsSignature) {
        // Hash all option names, not just long ones
        for (auto &optionName : optionSignature.names())
            result.try_emplace(std::move(optionName), index);

        index++;
    }

    return result;
}

/* run() support */

void Command::checkHelpArgument() const
{
    if (!isSet(Constants::help))
        return;

    call(Help, {name()});

    Application::exitApplication(EXIT_SUCCESS);
}

void Command::showParserError(const QCommandLineParser &parser) const
{
    errorWall(parser.errorText());

    Application::exitApplication(EXIT_FAILURE);
}

void Command::validateRequiredArguments() const
{
    const auto &arguments = positionalArguments();

    using StdSizeType = std::remove_cvref_t<decltype (arguments)>::size_type;

    // Count required arguments
    StdSizeType requiredArgsSize = 0;
    for (const auto &argument : arguments)
        // Required arguments can not be after optional arguments
        if (argument.optional)
            break;
        else
            ++requiredArgsSize;

    /* -1 to exclude the command name, our this->positionalArguments() doesn't contain
       command name of course.
       It can be also understood as the index to the missing argument. */
    const auto passedArgsSize = this->arguments().size() - 1;

    // All required positional arguments were passed
    if (static_cast<StdSizeType>(passedArgsSize) >= requiredArgsSize)
        return;

    errorWall(uR"(Not enough arguments (missing: "%1").)"_s
              .arg(arguments.at(static_cast<StdSizeType>(passedArgsSize)).name));

    Application::exitApplication(EXIT_FAILURE);
}

/* Parser helpers */

QString Command::argumentInternal(const QStringList &positionalArguments,
                                  const SizeType index) const
{
    // This method can't be called with index 0 (main/tom command) if useDefault == true
    Q_ASSERT_X(index > 0, "Command::argument()",
               "The this->positionalArguments() doesn't provide data for 0-index "
               "(command name).");

    const auto &positionalArgumentsRef = this->positionalArguments();

    using StdSizeType = std::remove_cvref_t<decltype (positionalArgumentsRef)>::size_type;

    // Default value support
    const auto defaultValue = positionalArgumentsRef.at(
                                  // -1 to exclude the command name
                                  static_cast<StdSizeType>(index) - 1)
                              .defaultValue;

    return positionalArguments.value(index, defaultValue);
}

// The following 2 methods exist for performance reasons, to avoid RegEx

bool Command::containsComma(const QStringView value)
{
    // Nothing to do
    if (value.isEmpty())
        return false;

    const auto valueSize = value.size();
    SizeType position = 0; // 0-based

    do {
        position = value.indexOf(COMMA_C, position);

        // Comma not found, reached the end of the value string
        if (position == -1)
            return false;

        // Comma found
        if (position == 0 || value.at(position - 1) != u'\\')
            return true;

    } while (++position < valueSize);

    // Edge case, can happen if a comma is the last character
    return false;
}

Command::SizeType Command::countCommas(const QStringView value)
{
    /* Paradoxically, this is much faster than the count() 800ms versus 3000ms
       per 1,000,000 loop, it also counts commas correctly as a bonus. */

    // Nothing to do
    if (value.isEmpty())
        return 0;

    const auto valueSize = value.size();
    SizeType commasCount = 0;
    SizeType position = 0; // 0-based

    do {
        position = value.indexOf(COMMA_C, position);

        // Comma not found, reached the end of the value string
        if (position == -1)
            return commasCount;

        // Comma found
        if (position == 0 || value.at(position - 1) != u'\\')
            ++commasCount;

    } while (++position < valueSize);

    // Edge case, can happen if a comma is the last character
    return commasCount;
}

Command::SizeType Command::countCommas(const QStringList &values)
{
    // Nothing to do
    if (values.isEmpty())
        return 0;

    SizeType result = 0;

    for (const auto &value : values)
        result += countCommas(value);

    return result;
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
