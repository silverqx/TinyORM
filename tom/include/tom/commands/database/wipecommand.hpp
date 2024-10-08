#pragma once
#ifndef TOM_COMMANDS_DATABASE_WIPECOMMAND_HPP
#define TOM_COMMANDS_DATABASE_WIPECOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/concerns/confirmable.hpp"
#include "tom/concerns/pretendable.hpp"
#include "tom/concerns/usingconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Database
{

    /*! Drop all tables, views, and types. */
    class WipeCommand : public Command,
                        public Concerns::Confirmable,
                        public Concerns::Pretendable,
                        public Concerns::UsingConnection
    {
        Q_DISABLE_COPY_MOVE(WipeCommand)

        /*! Alias for the DatabaseConnection. */
        using DatabaseConnection = Orm::DatabaseConnection;
        /*! Alias for the Command. */
        using Command = Commands::Command;

    public:
        /*! Constructor. */
        WipeCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~WipeCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! Options signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Drop all of the database tables. */
        void dropAllTables(DatabaseConnection &connection) const;
        /*! Drop all of the database views. */
        void dropAllViews(DatabaseConnection &connection) const;
        /*! Drop all of the database types. */
        void dropAllTypes(DatabaseConnection &connection) const;
    };

    /* public */

    QString WipeCommand::name() const
    {
        return Constants::DbWipe;
    }

    QString WipeCommand::description() const
    {
        return u"Drop all tables, views, and types"_s;
    }

} // namespace Tom::Commands::Database

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_DATABASE_WIPECOMMAND_HPP
