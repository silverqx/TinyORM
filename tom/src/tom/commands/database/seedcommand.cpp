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

using Orm::Constants::database_;

#ifndef TINYORM_DISABLE_ORM
using Orm::Tiny::GuardedModel;
#endif

using Tom::Constants::class_;
using Tom::Constants::class_up;
using Tom::Constants::database_up;
using Tom::Constants::force;

using TypeUtils = Orm::Utils::Type;

namespace Tom::Commands::Database
{

const QString SeedCommand::DatabaseSeeder = QStringLiteral("Seeders::DatabaseSeeder");

/* public */

SeedCommand::SeedCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
    , Concerns::Confirmable(*this, 0)
    , Concerns::UsingConnection(resolver())
{}

const std::vector<PositionalArgument> &SeedCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {class_, QStringLiteral("The class name of the root seeder"), {}, true},
    };

    return cached;
}

QList<QCommandLineOption> SeedCommand::optionsSignature() const
{
    return {
        {class_,       QStringLiteral("The class name of the root seeder"), class_up,
                       DatabaseSeeder}, // Value
        {database_,    QStringLiteral("The database connection to use "
                                      "<comment>(multiple values allowed)</comment>"),
                       database_up}, // Value
        {{QChar('f'),
         force},       QStringLiteral("Force the operation to run when in production")},
    };
}

int SeedCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    auto databases = values(database_);

    auto result = EXIT_SUCCESS;
    const auto shouldPrintConnection = databases.size() > 1;
    auto first = true;

    // Database connection to use (multiple connections supported)
    for (auto &&database : databases) {
        // Visually divide individual connections
        printConnection(database, shouldPrintConnection, first);

        result &= usingConnection(std::move(database), isDebugVerbosity(), [this]
        {
            auto seederResult = getSeeder();

            comment(QStringLiteral("Seeding: "), false);
            note(QStringLiteral("%1 (root)").arg(seederResult.name));

            QElapsedTimer timer;
            timer.start();

            // Fire it up 🔥
#ifdef TINYORM_DISABLE_ORM
            seederResult.seeder.get().run();
#else
            GuardedModel::unguarded([&seederResult]
            {
                seederResult.seeder.get().run();
            });
#endif

            const auto elapsedTime = timer.elapsed();

            info(QStringLiteral("Seeded:"), false);
            note(QStringLiteral("  %1 (%2ms total)").arg(std::move(seederResult.name))
                                                    .arg(elapsedTime));

            info(QStringLiteral("Database seeding completed successfully."));

            return EXIT_SUCCESS;
        });
    }

    return result;
}

/* protected */

SeedCommand::GetSeederResult Tom::Commands::Database::SeedCommand::getSeeder() const
{
    // The --class option also provides a default value 'Seeders::DatabaseSeeder'
    auto seederClass = hasArgument(class_) ? argument(class_) : value(class_);

    // Prepend Seeders:: namespace, it's requirement
    if (!seederClass.contains(QStringLiteral("::")))
        seederClass.prepend(QStringLiteral("Seeders::"));

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

const std::vector<std::shared_ptr<Seeder>> &SeedCommand::seeders() const noexcept
{
    return application().getSeeders();
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
                QStringLiteral("The root seeder '%1' is not defined.")
                .arg(seederClass));
}

} // namespace Tom::Commands::Database

TINYORM_END_COMMON_NAMESPACE
