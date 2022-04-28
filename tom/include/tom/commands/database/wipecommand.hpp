#pragma once
#ifndef TOM_COMMANDS_DATABASE_WIPECOMMAND_HPP
#define TOM_COMMANDS_DATABASE_WIPECOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/concerns/confirmable.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Database
{

    /*! Drop all tables, views, and types. */
    class WipeCommand : public Command,
                        public Concerns::Confirmable
    {
        Q_DISABLE_COPY(WipeCommand)

        /*! Alias for the Command. */
        using Command = Commands::Command;

    public:
        /*! Constructor. */
        WipeCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~WipeCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The signature of the console command. */
        QList<QCommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Drop all of the database tables. */
        void dropAllTables(const QString &database) const;
        /*! Drop all of the database views. */
        void dropAllViews(const QString &database) const;
        /*! Drop all of the database types. */
        void dropAllTypes(const QString &database) const;
    };

    /* public */

    QString WipeCommand::name() const
    {
        return Constants::DbWipe;
    }

    QString WipeCommand::description() const
    {
        return QStringLiteral("Drop all tables, views, and types");
    }

} // namespace Tom::Commands::Database

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_DATABASE_WIPECOMMAND_HPP
