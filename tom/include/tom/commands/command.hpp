#pragma once
#ifndef TOM_COMMANDS_COMMAND_HPP
#define TOM_COMMANDS_COMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QJsonDocument>

#include "tom/concerns/callscommands.hpp"
#include "tom/concerns/interactswithio.hpp"
#include "tom/types/commandlineoption.hpp"

class QCommandLineOption;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class ConnectionResolverInterface;
    class DatabaseConnection;
}

namespace Tom
{
namespace Concerns
{
    class Confirmable;
    class Pretendable;
}

namespace Commands
{

    /*! Positional argument item for a console command. */
    struct PositionalArgument
    {
        /*! Argument name. */
        QString name;
        /*! Argument description. */
        QString description;
        /*! Appended to the Usage line, if empty the name is used. */
        QString syntax;
        /*! Is argument optional? */
        bool optional = false;
        /*! Argument's default value (optional argument only). */
        QString defaultValue;
    };

    /*! Abstract base class for the console command. */
    class Command : public Concerns::CallsCommands,
                    public Concerns::InteractsWithIO
    {
        Q_DISABLE_COPY_MOVE(Command)

        // To access parser()
        friend Concerns::Confirmable;
        // To access connection()
        friend Concerns::Pretendable;

        /*! Alias for the ConnectionResolverInterface. */
        using ConnectionResolverInterface = Orm::ConnectionResolverInterface;

    public:
        /*! Constructor. */
        Command(Application &application, QCommandLineParser &parser);
        /*! Pure virtual destructor. */
        inline ~Command() override = 0;

        /*! The console command name. */
        virtual QString name() const = 0;
        /*! The console command description. */
        virtual QString description() const = 0;

        /*! The console command positional arguments signature. */
        inline virtual const std::vector<PositionalArgument> &
        positionalArguments() const;
        /*! The signature of the console command. */
        virtual QList<CommandLineOption> optionsSignature() const;

        /*! The console command help. */
        inline virtual QString help() const;

        /*! Execute the console command. */
        virtual int run();
        /*! Execute the console command with the given arguments. */
        int runWithArguments(QStringList &&arguments);

        /* Getters */
        /*! Get the tom application. */
        inline Application &application() const noexcept;
        /*! Determine whether a command has own positional arguments. */
        bool hasPositionalArguments() const;
        /*! Determine whether a command has own options. */
        bool hasOptions() const;

    protected:
        /* Getters */
        /*! Obtain passed arguments to parse (can come from three sources). */
        QStringList passedArguments() const;

        /* Parser helpers */
        /*! Returns a list of option names found by the parser (addes , support). */
        QStringList optionNames() const;
        /*! Check whether the option name was set in the parser. */
        bool isSet(const QString &name) const;
        /*! Check whether the option name was set in the parser (multiple values). */
        std::vector<bool> isSetAll(const QString &name) const;
        /*! Returns a option value found for the given option name or empty string. */
        QString value(const QString &name) const;
        /*! Returns a option values found for the given option name (addes , support). */
        QStringList values(const QString &name,
                           Qt::SplitBehavior behavior = Qt::KeepEmptyParts) const;
        /*! Get a full command-line value option if value is set in the parser. */
        QString valueCmd(const QString &name, const QString &key = "") const;
        /*! Get a full command-line boolean option if it's set in the parser. */
        QString boolCmd(const QString &name, const QString &key = "") const;
        /*! Get a long option name by the given name (prepend -- before the name). */
        static QString longOption(const QString &name);
        /*! Create the long option with value (--database=xyz). */
        static QString longOption(const QString &name, const QString &value);

        /*! Alias for the QList command-line option size type. */
        using ArgumentsSizeType = QStringList::size_type;

        /*! Check whether a positional argument at the given index was set. */
        bool hasArgument(ArgumentsSizeType index) const;
        /*! Check whether a positional argument by the given name was set. */
        bool hasArgument(const QString &name) const;
        /*! Get a list of positional arguments. */
        QStringList arguments() const;
        /*! Get a positional argument at the given index position. */
        QString argument(ArgumentsSizeType index, bool useDefault = true) const;
        /*! Get a positional argument by the given name. */
        QString argument(const QString &name, bool useDefault = true) const;

        /*! Get the JsonFormat based on the --pretty option. */
        QJsonDocument::JsonFormat jsonFormat() const;

        /* Getters */
        /*! Get a database connection. */
        Orm::DatabaseConnection &connection(const QString &name) const;
        /*! Get a command-line parser. */
        QCommandLineParser &parser() const noexcept;
        /*! Get database connection resolver. */
        std::shared_ptr<ConnectionResolverInterface> connectionResolver() const noexcept;

        /* Auto tests helpers */
#ifdef TINYTOM_TESTS_CODE
        /*! Is enabled logic for unit testing? */
        static bool inUnitTests() noexcept;
#endif

        /* Data members */
        /*! Reference to the tom application. */
        std::reference_wrapper<Application> m_application;
        /*! Command-line parser. */
        std::reference_wrapper<QCommandLineParser> m_parser;

        /*! Passed command's arguments. */
        QStringList m_arguments;

        /*! Map positional argument names to the index for obtaining values. */
        std::unordered_map<QString, ArgumentsSizeType> m_positionalArguments;

    private:
        /*! Initialize positional arguments map. */
        void initializePositionalArguments();
        /*! Show help if --help argument was passed. */
        void checkHelpArgument() const;
        /*! Show the error wall and exit the application if the parser fails. */
        void showParserError(const QCommandLineParser &parser) const;
        /*! Validate if all required positional arguments were passed on the cmd. line. */
        void validateRequiredArguments() const;

        /*! Count the number of commas in the values list for reserve. */
        static QStringList::size_type
        commasCount(const QStringList &values);
    };

    /* public */

    Command::~Command() = default;

    const std::vector<PositionalArgument> &Command::positionalArguments() const
    {
        static const std::vector<PositionalArgument> cached;

        return cached;
    }

    QString Command::help() const
    {
        return {};
    }

    Application &Command::application() const noexcept
    {
        return m_application;
    }

} // namespace Commands
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_COMMAND_HPP
