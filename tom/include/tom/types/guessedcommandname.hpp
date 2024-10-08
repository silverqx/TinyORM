#pragma once
#ifndef TOM_TYPES_GUESSEDCOMMANDNAME_HPP
#define TOM_TYPES_GUESSEDCOMMANDNAME_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
namespace Types
{

    /*! Enum result for guessing the Tom command name (for the complete command). */
    enum struct GuessedCommandNameResult : quint8
    {
        /*! Tom command name was found. */
        kFound,
        /*! Tom command name was not found. */
        kNotFound,
        /*! More Tom command name were found (ambiguous result). */
        kAmbiguous,
    };

    /*! Result for guessing the Tom command name (for the complete command). */
    struct GuessedCommandName
    {
        /*! Guessing result for the Tom command name. */
        GuessedCommandNameResult result = GuessedCommandNameResult::kNotFound;
        /*! Guessed Tom command name. */
        std::optional<QString> commandName = std::nullopt;

        /*! Alias for the commandName type. */
        using ResultType      = decltype (result);
        /*! Alias for the result type. */
        using CommandNameType = decltype (commandName);
        /*! Alias for the std::optional<>::value_type. */
        using value_type      = CommandNameType::value_type;

        /*! Equality comparison operator for the std::optional<QString> (commandName). */
        template<typename T>
        requires (std::convertible_to<T, const GuessedCommandName::value_type &>)
        [[nodiscard]] constexpr bool operator==(const T &other) const
        noexcept(noexcept(commandName == other));

        /*! Equality comparison operator for the GuessedCommandNameResult (result). */
        template<typename T>
        requires (std::convertible_to<T, GuessedCommandName::ResultType>)
        [[nodiscard]] constexpr bool operator==(T other) const noexcept;

        /*! Determine whether the std::optional<> contains a Tom command name. */
        constexpr explicit operator bool() const noexcept;

        /*! Return a reference to the command name. */
        [[nodiscard]] constexpr value_type &operator*() & noexcept;
        /*! Return a reference to the command name. */
        [[nodiscard]] constexpr const value_type &operator*() const & noexcept;
        /*! Return a reference to the command name. */
        [[nodiscard]] constexpr value_type &&operator*() && noexcept;
        /*! Return a reference to the command name. */
        [[nodiscard]] constexpr const value_type &&operator*() const && noexcept;
    };

    /* public */

    template<typename T>
    requires (std::convertible_to<T, const GuessedCommandName::value_type &>)
    [[nodiscard]] constexpr bool GuessedCommandName::operator==(const T &other) const
    noexcept(noexcept(commandName == other))
    {
        return commandName == other;
    }

    template<typename T>
    requires (std::convertible_to<T, GuessedCommandName::ResultType>)
    constexpr bool GuessedCommandName::operator==(const T other) const noexcept
    {
        return result == other;
    }

    constexpr GuessedCommandName::operator bool() const noexcept
    {
        return static_cast<bool>(commandName);
    }

    constexpr GuessedCommandName::value_type &
    GuessedCommandName::operator*() & noexcept
    {
        return *commandName; // NOLINT(bugprone-unchecked-optional-access)
    }

    constexpr const GuessedCommandName::value_type &
    GuessedCommandName::operator*() const & noexcept
    {
        return *commandName; // NOLINT(bugprone-unchecked-optional-access)
    }

    constexpr GuessedCommandName::value_type &&
    GuessedCommandName::operator*() && noexcept
    {
        return *std::move(commandName); // NOLINT(bugprone-unchecked-optional-access)
    }

    constexpr const GuessedCommandName::value_type &&
    GuessedCommandName::operator*() const && noexcept
    {
        return *std::move(commandName); // NOLINT(bugprone-unchecked-optional-access, performance-move-const-arg)
    }

} // namespace Types

    using GuessedCommandNameResult = Tom::Types::GuessedCommandNameResult;
    using GuessedCommandName       = Tom::Types::GuessedCommandName;

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TYPES_GUESSEDCOMMANDNAME_HPP
