#include "tom/concerns/pretendable.hpp"

#include <orm/databaseconnection.hpp>
#include <orm/utils/query.hpp>

#include "tom/commands/command.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using QueryUtils = Orm::Utils::Query;

namespace Tom::Concerns
{

/* public */

void Pretendable::optionalPretend(
        const bool pretend, const QString &database,
        const std::function<void(DatabaseConnection &)> &callback,
        const std::optional<QString> &title, const bool newline) const
{
    optionalPretend(pretend, command().connection(database), callback, title, newline);
}

void Pretendable::optionalPretend(
        const bool pretend, DatabaseConnection &connection,
        const std::function<void(DatabaseConnection &)> &callback,
        const std::optional<QString> &title, const bool newline) const
{
    // Execute the callback without pretending
    if (!pretend)
        return std::invoke(callback, connection); // NOLINT(readability-avoid-return-with-void-value) clazy:exclude=returning-void-expression

    // Gather executed queries
    auto queriesLog = connection.pretend(callback);

    // Log gathered queries to the console
    optionalPretendInternal(std::move(queriesLog), title, newline);
}

void Pretendable::optionalPretend(
        const bool pretend, const QString &database,
        const std::function<void()> &callback,
        const std::optional<QString> &title, const bool newline) const
{
    optionalPretend(pretend, command().connection(database), callback, title, newline);
}

void Pretendable::optionalPretend(
        const bool pretend, DatabaseConnection &connection,
        const std::function<void()> &callback,
        const std::optional<QString> &title, const bool newline) const
{
    // Execute the callback without pretending
    if (!pretend)
        return std::invoke(callback); // NOLINT(readability-avoid-return-with-void-value) clazy:exclude=returning-void-expression

    // Gather executed queries
    auto queriesLog = connection.pretend(callback);

    // Log gathered queries to the console
    optionalPretendInternal(std::move(queriesLog), title, newline);
}

/* private */

void Pretendable::optionalPretendInternal(
        QVector<Orm::Log> &&queriesLog, const std::optional<QString> &title, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        const bool newline) const
{
    // Log gathered queries to the console
    for (auto &&query : queriesLog) {
        if (title && !title->isEmpty())
            io().info(QStringLiteral("%1: ").arg(*title), newline);

        io().note(QueryUtils::parseExecutedQueryForPretend(std::move(query.query),
                                                           query.boundValues));
    }
}

const Commands::Command &Pretendable::command() const
{
    return dynamic_cast<const Commands::Command &>(*this);
}

const InteractsWithIO &Pretendable::io() const
{
    return dynamic_cast<const InteractsWithIO &>(*this);
}

} // namespace Tom::Concerns

TINYORM_END_COMMON_NAMESPACE
