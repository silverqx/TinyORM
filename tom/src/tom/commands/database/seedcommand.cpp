#include "tom/commands/database/seedcommand.hpp"

#include <orm/utils/type.hpp>

#ifndef TINYORM_DISABLE_ORM
#  include <orm/tiny/model.hpp>
#endif

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/algorithm/find.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidtemplateargumenterror.hpp"
#include "tom/seeder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Constants::database_;

#ifndef TINYORM_DISABLE_ORM
using Orm::Tiny::GuardedModel;
#endif

using Tom::Constants::class_;
using Tom::Constants::class_up;
using Tom::Constants::database_up;
using Tom::Constants::force;
using Tom::Constants::pretend;

using TypeUtils = Orm::Utils::Type;

namespace Tom::Commands::Database
{

const QString SeedCommand::DatabaseSeeder = u"Seeders::DatabaseSeeder"_s;

/* public */

SeedCommand::SeedCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
    , Concerns::UsingConnection(connectionResolver())
{}

const std::vector<PositionalArgument> &SeedCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {class_, u"The class name of the root seeder"_s, {}, true},
    };

    return cached;
}

QList<CommandLineOption> SeedCommand::optionsSignature() const
{
    return {
        {class_,       u"The class name of the root seeder"_s, class_up, DatabaseSeeder}, // Value
        {database_,    u"The database connection to use <comment>(multiple values "
                        "allowed)</comment>"_s, database_up}, // Value
        {{QChar('f'),
         force},       u"Force the operation to run when in production"_s},
        {pretend,      u"Dump the SQL queries that would be run"_s},
    };
}

int SeedCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    // Database connection to use (multiple connections supported)
    return usingConnections(values(database_), isDebugVerbosity(), [this]
    {
        const auto seederResult = getSeeder();

        comment(u"Seeding: "_s, false);
        note(u"%1 (root)"_s.arg(seederResult.name));

        QElapsedTimer timer; // NOLINT(misc-const-correctness)
        timer.start();

        // Fire it up 🔥
        if (!isSet(pretend))
            runRootSeeder(seederResult.seeder);

        const auto elapsedTime = timer.elapsed();

        info(u"Seeded:"_s, false);
        note(u"  %1 (%2ms total)"_s.arg(seederResult.name).arg(elapsedTime));

        info(u"Database seeding completed successfully."_s);

        return EXIT_SUCCESS;
    });
}

/* protected */

SeedCommand::GetSeederResult Tom::Commands::Database::SeedCommand::getSeeder() const
{
    // The --class option also provides a default value 'Seeders::DatabaseSeeder'
    auto seederClass = hasArgument(class_) ? argument(class_) : value(class_);

    // Prepend Seeders:: namespace, it's requirement
    if (!seederClass.contains(u"::"_s))
        seederClass.prepend(u"Seeders::"_s);

    // Throw if the root seeder is not defined
    throwIfDoesntContainSeeder(seederClass);

    // Find a reference to the root seeder, doesn't need to check the std::end()
    auto &rootSeeder = **ranges::find(seeders(), seederClass,
                                      [](const auto &seeder)
    {
        return TypeUtils::classPureBasename(*seeder, true);
    });

    /* This command is used as the InteractsWithIO interface inside the seeder and passed
       down to other seeders. */
    rootSeeder.setIO(*this);

    return {std::move(seederClass), rootSeeder};
}

void SeedCommand::runRootSeeder(Seeder &seeder)
{
#ifdef TINYORM_DISABLE_ORM
    seeder.run();
#else
    GuardedModel::unguarded([&seeder]
    {
        seeder.run();
    });
#endif
}

/* private */

void SeedCommand::throwIfDoesntContainSeeder(const QString &seederClass) const
{
    const auto containsSeeder = ranges::contains(seeders(), seederClass,
                                                 [](const auto &seeder)
    {
        return TypeUtils::classPureBasename(*seeder, true);
    });

    if (containsSeeder)
        return;

    throw Exceptions::InvalidTemplateArgumentError(
                u"The root seeder '%1' is not defined."_s.arg(seederClass));
}

const std::vector<std::shared_ptr<Seeder>> &SeedCommand::seeders() const noexcept
{
    return application().getSeeders();
}

} // namespace Tom::Commands::Database

TINYORM_END_COMMON_NAMESPACE
