#pragma once
#ifndef TOM_CONCERNS_PRETENDABLE_HPP
#define TOM_CONCERNS_PRETENDABLE_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <functional>
#include <optional>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class DatabaseConnection;
}

namespace Tom
{
namespace Commands
{
    class Command;
}

namespace Concerns
{

    class InteractsWithIO;

    /*! Pretend the callback on the base of a bool value (mainly the --pretend option). */
    class Pretendable
    {
        Q_DISABLE_COPY(Pretendable)

        /*! Alias for the Command. */
        using Command = Commands::Command;
        /*! Alias for the DatabaseConnection. */
        using DatabaseConnection = Orm::DatabaseConnection;

    public:
        /*! Default constructor. */
        inline Pretendable() = default;
        /*! Virtual destructor. */
        inline virtual ~Pretendable() = 0;

        /*! Pretend the callback on the base of a bool value (mainly --pretend option). */
        void optionalPretend(
                bool pretend, const QString &database,
                const std::function<void(DatabaseConnection &)> &callback,
                std::optional<QString> &&title = std::nullopt,
                bool newline = false) const;
        /*! Pretend the callback on the base of a bool value (mainly --pretend option). */
        void optionalPretend(
                bool pretend, DatabaseConnection &connection,
                const std::function<void(DatabaseConnection &)> &callback,
                std::optional<QString> &&title = std::nullopt,
                bool newline = false) const;

    private:
        /*! Dynamic cast *this to the Command & base type, const version. */
        const Commands::Command &command() const;
        /*! Get the reference to the base InteractsWithIO concern. */
        const Concerns::InteractsWithIO &io() const;
    };

    /* public */

    Pretendable::~Pretendable() = default;

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_PRETENDABLE_HPP
