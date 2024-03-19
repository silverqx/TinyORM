#pragma once
#ifndef TOM_COMMANDS_MIGRATIONS_STATUSCOMMAND_HPP
#define TOM_COMMANDS_MIGRATIONS_STATUSCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QVariant>

#include <tabulate/table.hpp>

#ifdef TINYTOM_TESTS_CODE
#  include <orm/macros/threadlocal.hpp>
#endif

#include "tom/commands/command.hpp"
#include "tom/concerns/usingconnection.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    class Migrator;

namespace Commands::Migrations
{

    /*! Show the status of each migration. */
    class StatusCommand : public Command,
                          public Concerns::UsingConnection
    {
        Q_DISABLE_COPY_MOVE(StatusCommand)

        /*! Alias for the tabulate cell. */
        using TableCell = InteractsWithIO::TableCell;
        /*! Alias for the tabulate row. */
        using TableRow  = InteractsWithIO::TableRow;

    public:
        /*! Constructor. */
        StatusCommand(Application &application, QCommandLineParser &parser,
                      std::shared_ptr<Migrator> migrator);
        /*! Virtual destructor. */
        ~StatusCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

#ifdef TINYTOM_TESTS_CODE
        /*! Alias for the test output row. */
        using StatusRow = std::vector<std::string>;

        /*! Get result of the status command (used in auto tests). */
        inline static std::vector<StatusRow> status();
#endif

    protected:
        /*! Get the status for the given ran migrations. */
        std::vector<TableRow>
        getStatusFor(const QVector<QVariant> &ran,
                     const std::map<QString, QVariant> &batches) const;
#ifdef TINYTOM_TESTS_CODE
        /*! Transform migrations status for comparing in auto tests. */
        static std::vector<StatusRow>
        statusForUnitTest(std::vector<TableRow> &&migrations);
#endif

        /*! The migrator service instance. */
        std::shared_ptr<Migrator> m_migrator;

#ifdef TINYTOM_TESTS_CODE
        /*! Result of the status command (used in auto tests). */
        T_THREAD_LOCAL
        inline static std::vector<StatusRow> m_status;
#endif
    };

    /* public */

    QString StatusCommand::name() const
    {
        return Constants::MigrateStatus;
    }

    QString StatusCommand::description() const
    {
        return QStringLiteral("Show the status of each migration");
    }

#ifdef TINYTOM_TESTS_CODE
    std::vector<StatusCommand::StatusRow> StatusCommand::status()
    {
        return m_status;
    }
#endif

} // namespace Commands::Migrations
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MIGRATIONS_STATUSCOMMAND_HPP
