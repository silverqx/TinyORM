#include "tom/seeder.hpp"

#include <QElapsedTimer>

#include <orm/utils/type.hpp>

#include "tom/concerns/interactswithio.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Qt::StringLiterals::operator""_s;

using TypeUtils = Orm::Utils::Type;

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

        m_io->get().comment(u"Seeding: "_s, false).note(seederName);

        timer.start();
    }

    std::invoke(std::move(callback));

    if (!shouldLog)
        return;

    const auto elapsedTime = timer.elapsed();

    m_io->get().info(u"Seeded:"_s, false);
    m_io->get().note(u"  %1 (%2ms)"_s.arg(seederName).arg(elapsedTime));
}

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE
