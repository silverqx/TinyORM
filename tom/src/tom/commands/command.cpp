#include "tom/commands/command.hpp"

#include <QCommandLineParser>
#include <QRegularExpression>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/zip_with.hpp>

#include <orm/databasemanager.hpp>

#include "tom/application.hpp"
#include "tom/tomconstants.hpp"
#include "tom/tomutils.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::ConnectionResolverInterface;

using Orm::Constants::COMMA_C;
using Orm::Constants::SPACE;

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
    initializePositionalArguments();

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

/* Getters */

QStringList Command::passedArguments() const
{
    if (!m_arguments.isEmpty())
        return m_arguments;

    /* Never obtain arguments from the QCoreApplication instance in unit tests because
       they are passed using the runWithArguments() method. */
    return application().arguments();
}

/* Parser helpers */

namespace
{
    /*! Find n-th option with the given name in option names list. */
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
       --xyz=aa,bb,cc; so insert two more xyz option names before 3. position.
       This method is counterpart for our values() method and it's needed because our
       values() method added support for more values per one option --xyz=aa,bb,cc
       using the , character 🤯. */

    // Allow to escape , char using \,
    static const QRegularExpression regex(R"((?<!\\),)");

    auto optionNames = parser().optionNames();

    const auto optionNamesSize = optionNames.size();
    // Number of a new option names can be estimated by counting the , characters
    optionNames.reserve(
                std::max<decltype (optionNames)::size_type>(
                    optionNamesSize * 2,
                    optionNamesSize + application().arguments().join(SPACE)
                                      .count(regex)) + 1); // +1 for sure

    /* Allows to loop through all values for every (unique) option name defined
       on the command-line. */
    auto optionNamesUnique = optionNames;
    optionNamesUnique.removeDuplicates();

    for (const auto &optionName : std::as_const(optionNamesUnique)) {
        // Obtain raw parser().values() so we can count the , char.
        const auto values = parser().values(optionName);

        /* Index of one option name eg. --with-pivot= in all arguments
           (1. option = 0, 2. = 1). */
        QString::size_type nthOptionIdx = 0;

        for (const auto &value : values) {
            const auto commasCount = value.count(regex);

            // Nothing to do
            if (commasCount == 0) {
                ++nthOptionIdx;
                continue;
            }

            // Iterator to the option name
            const auto nthOption = findNthOption(optionName, nthOptionIdx, optionNames);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            optionNames.insert(nthOption, commasCount, optionName);
#else
            /* We need to save the index position because the nthOption iterator will be
               invalidated after an insertion. */
            const auto optionIdx = static_cast<decltype (optionNames)::size_type>(
                                       std::distance(optionNames.cbegin(), nthOption));

            for (auto i = 0; i < commasCount ; ++i)
                optionNames.insert(optionIdx, optionName);
#endif
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

    for (auto &&option : parser().optionNames())
        if (option == name)
            result.push_back(true);

    return result;
}

QString Command::value(const QString &name) const
{
    return parser().value(name);
}

QStringList Command::values(const QString &name, const Qt::SplitBehavior behavior) const
{
    auto values = parser().values(name);

    QStringList valuesSplitted;
    valuesSplitted.reserve(values.size() + commasCount(values));

    // Allow to escape , char using \,
    static const QRegularExpression regex(R"((?<!\\),)");

    // Support passing more values delimited by comma
    for (auto &&value : values) {
        if (!value.contains(regex)) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            valuesSplitted << value;
#else
            valuesSplitted << std::move(value);
#endif

            continue;
        }

        valuesSplitted << value.split(regex, behavior);
    }

    return valuesSplitted;
}

QString Command::valueCmd(const QString &name, const QString &key) const
{
    if (const auto value = parser().value(name);
        !value.isEmpty()
    )
        return QStringLiteral("--%1=%2").arg(key.isEmpty() ? name : key,
                                             value);

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
    return QStringLiteral("--%1=%2").arg(name, value);
}

bool Command::hasArgument(const ArgumentsSizeType index) const
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
    return parser().positionalArguments();
}

QString Command::argument(const ArgumentsSizeType index, const bool useDefault) const
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

    const auto &positionalArgumentsRef = this->positionalArguments();

    using ArgumentsStdSizeType = std::remove_cvref_t<decltype (positionalArgumentsRef)>
                                    ::size_type;

    // Default value support
    const auto defaultValue = positionalArgumentsRef.at(
                                  // -1 to exclude the command name
                                  static_cast<ArgumentsStdSizeType>(index) - 1)
                              .defaultValue;

    return positionalArguments.value(index, defaultValue);
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

void Command::initializePositionalArguments()
{
    const auto &arguments = positionalArguments();

    if (arguments.empty())
        return;

    m_positionalArguments =
            ranges::views::zip_with([](const PositionalArgument &argument,
                                       const ArgumentsSizeType index)
                                    -> std::pair<QString, ArgumentsSizeType>
    {
        return {argument.name, index};
    },
        arguments,
        ranges::views::closed_iota(static_cast<ArgumentsSizeType>(1),
                                   static_cast<ArgumentsSizeType>(arguments.size()))
    )
        | ranges::to<decltype (m_positionalArguments)>();

    // The same as above, I leave above as I want to have one example with zip_with()
//    for (ArgumentsSizeType index = 0; const auto &argument : positionalArguments())
//        m_positionalArguments.emplace(argument.name, ++index);
}

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

    using RequiredStdSizeType = std::remove_cvref_t<decltype (arguments)>::size_type;

    // Count required arguments
    RequiredStdSizeType requiredArgsSize = 0;
    for (const auto &argument : arguments)
        // Required arguments can not be after optional arguments
        if (argument.optional)
            break;
        else
            ++requiredArgsSize;

    /* -1 to exclude the command name.
       It can be also understand as the index to the missing argument. */
    const auto passedArgsSize = this->arguments().size() - 1;

    // All required positional arguments were passed
    if (static_cast<RequiredStdSizeType>(passedArgsSize) >= requiredArgsSize)
        return;

    errorWall(QStringLiteral(R"(Not enough arguments (missing: "%1").)")
              .arg(arguments.at(static_cast<RequiredStdSizeType>(passedArgsSize)).name));

    Application::exitApplication(EXIT_FAILURE);
}

QStringList::size_type Command::commasCount(const QStringList &values)
{
    QStringList::size_type result = 0;

    for (const auto &value : values)
        result += value.count(COMMA_C);

    return result;
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
