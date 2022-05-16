#include "tom/seeder.hpp"

#include <QElapsedTimer>

#include <orm/utils/type.hpp>

#include "tom/concerns/interactswithio.hpp"

using TypeUtils = Orm::Utils::Type;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

/* public */

Seeder &Seeder::setIO(const InteractsWithIO &io)
{
    m_io = io;

    return *this;
}

/* private */

void Seeder::callInternal(const bool silent, std::function<void()> &&callback) const
{
    QElapsedTimer timer;

    const auto shouldLog = !silent && m_io;

    QString seederName;

    if (shouldLog) {
        seederName = TypeUtils::classPureBasename(*this, true);

        m_io->get().comment(QStringLiteral("Seeding: "), false).note(seederName);

        timer.start();
    }

    std::invoke(std::move(callback));

    if (!shouldLog)
        return;

    const auto elapsedTime = timer.elapsed();

    m_io->get().info(QStringLiteral("Seeded:"), false);
    m_io->get().note(QStringLiteral("  %1 (%2ms)").arg(std::move(seederName))
                                                  .arg(elapsedTime));
}

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE
