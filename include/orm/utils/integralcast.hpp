#pragma once
#ifndef ORM_UTILS_INTEGRALCAST_HPP
#define ORM_UTILS_INTEGRALCAST_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/constants.hpp"
#include "orm/exceptions/outofrangeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Private
{
    /*! Concept for integral type excluding the bool and char types. */
    template<typename T>
    concept IntegralNoBoolChar = std::integral<T> &&
                                !std::same_as<T, bool> && !std::same_as<T, char>;

    /*! Get integral type name as a string. */
    template<std::integral T> requires (!std::is_reference_v<T>)
    consteval const char *IntegralTypeName() noexcept
    {
        // Sorted by most used types
        if constexpr (std::is_same_v<T, quint64>)
            return "uint64";
        else if constexpr (std::is_same_v<T, qint64>)
            return "int64";

        else if constexpr (std::is_same_v<T, int>)
            return "int32";
        else if constexpr (std::is_same_v<T, uint>)
            return "uint32";

        else if constexpr (std::is_same_v<T, bool>)
            return "bool";

        // Platform dependent long int type (LP64, LLP64, ILP32, and LP32)
        else if constexpr (std::is_same_v<T, long int>) { // NOLINT(google-runtime-int)

            if constexpr (sizeof (T) == 4)
                return "int32 (long)";
            else if constexpr (sizeof (T) == 8)
                return "int64 (long)";
            else if constexpr (sizeof (T) == 2)
                return "int16 (long)";

#if defined(__GNUG__) && defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE < 13
            else {
                Q_UNREACHABLE(); // Don't throw here, this must be handled/solved during development
                return "<unknown> (long)";
            }
#else
            else
                // This should never happen :/
                static_assert (false,
                        "Unhandled code branch in the Private::IntegralTypeName<T>() "
                        "(long).");
#endif
        }
        else if constexpr (std::is_same_v<T, unsigned long int>) { // NOLINT(google-runtime-int)

            if constexpr (sizeof (T) == 4)
                return "uint32 (ulong)";
            else if constexpr (sizeof (T) == 8)
                return "uint64 (ulong)";
            else if constexpr (sizeof (T) == 2)
                return "uint16 (ulong)";

#if defined(__GNUG__) && defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE < 13
            else {
                Q_UNREACHABLE(); // Don't throw here, this must be handled/solved during development
                return "<unknown> (ulong)";
            }
#else
            else
                // This should never happen :/
                static_assert (false,
                        "Unhandled code branch in the Private::IntegralTypeName<T>() "
                        "(ulong).");
#endif
        }

        else if constexpr (std::is_same_v<T, qint16>)
            return "int16";
        else if constexpr (std::is_same_v<T, quint16>)
            return "uint16";
        else if constexpr (std::is_same_v<T, quint8>) // unsigned char
            return "uint8";
        else if constexpr (std::is_same_v<T, qint8>) // signed char
            return "int8";

        /* Has the same representation and alignment as either signed char or
           unsigned char, but is always a distinct type. */
        else if constexpr (std::is_same_v<T, char>)
            return "char";

#if defined(__GNUG__) && defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE < 13
        else {
            Q_UNREACHABLE(); // Don't throw here, this must be handled/solved during development
            return "<unknown>";
        }
#else
        else
            // This should never happen :/
            static_assert (false,
                    "Unhandled code branch in the Private::IntegralTypeName<T>().");
#endif
    }

    /*! Compute number group separators count for reserve for formatNumber() function. */
    inline QString::size_type
    computeGroupSeparatorsCount(const QString::size_type numberSize,
                                const QString::size_type groupWidth) noexcept
    {
        /* The std::div returns both, integer after division operation (quot) and also
           the remainder (rem). */
        const auto groupSeparatorDiv = std::lldiv(numberSize, groupWidth);

        /* -1 is for pure mathematic reasons, try to call lldiv with different numberSize
           and 3 for groupWith to see how it works. */
        return groupSeparatorDiv.rem == 0 ? groupSeparatorDiv.quot - 1
                                          : groupSeparatorDiv.quot;
    }

    /*! Format the given integer number (insert group separator before every 3th
        number). */
    template<IntegralNoBoolChar T> requires (!std::is_reference_v<T>)
    QString formatNumber(const T integer, QChar groupSeparator = Constants::SQUOTE)
    {
        constexpr static QString::size_type groupWidth = 3;

        auto number = QString::number(integer);
        // For reserve()
        const auto numberSizeWithSign = number.size();
        // Qt doesn't prepend + character but test is anyway to be sure
        const auto hasSign = number.startsWith(Constants::PLUS) ||
                             number.startsWith(Constants::MINUS);
        // To compute group separators count for reserve()
        const auto numberSize = hasSign ? numberSizeWithSign - 1 : numberSizeWithSign;

        // No number groups to process so we can return right away
        if (numberSize <= groupWidth)
            return number;

        QString result;
        result.reserve(numberSizeWithSign + std::invoke(computeGroupSeparatorsCount,
                                                        numberSize, groupWidth));
        // To find every 3th number
        quint8 groupIndex = 0;
        const auto itNumberPenultimate = number.crend() - 1;

        for (auto itNumber = number.crbegin(); itNumber != number.crend();
             ++itNumber, ++groupIndex
        ) {
            /* Prepend group separator before every 3th number, the hasSign check
               prevents case like this: -'123'456. */
            if (groupIndex == groupWidth &&
                (!hasSign || (hasSign && itNumber != itNumberPenultimate))
            ) {
                result.prepend(groupSeparator);
                groupIndex = 0;
            }
            result.prepend(*itNumber);
        }

        return result;
    }

    /*! Determine whether the given value is in the T type range (min/max). */
    template<IntegralNoBoolChar T, IntegralNoBoolChar V>
    requires (!std::is_reference_v<T> && !std::is_reference_v<V>)
    constexpr bool InRange(const V value) noexcept
    {
        /* This method must mirror or follow steps in Usual arithmetic conversions -
           Stage 5 as accurate as possible.
           Legend: S - signed, U - unsigned, C - common type (result) */

        // If T and V are the same type, C is that type
        if constexpr (std::is_same_v<T, V>)
            return true;

        /* If T and V are both unsigned integer types, C is the type of greater integer
           conversion rank. */
        else if constexpr (!std::numeric_limits<T>::is_signed &&
                           !std::numeric_limits<T>::is_signed ==
                           !std::numeric_limits<V>::is_signed
        )
            return value <= std::numeric_limits<T>::max();

        /* If T and V are both signed integer types, C is the type of greater integer
           conversion rank. */
        else if constexpr (std::numeric_limits<T>::is_signed &&
                           std::numeric_limits<T>::is_signed ==
                           std::numeric_limits<V>::is_signed
        )
            return value >= std::numeric_limits<T>::min() &&
                   value <= std::numeric_limits<T>::max();

        /* If one type between T and V is an signed integer type S, the other type is
           an unsigned integer type U. */

        /* MISSING
           - if the integer conversion rank of U is greater than or equal to the integer
             conversion rank of S, C is U
           - otherwise, if S can represent all of the values of U, C is S
           - otherwise, C is the unsigned integer type corresponding to S

           Would be possible to implement these by comparing conversion rank using
           the sizeof () operator, but it it would be waste of time. I thought I will
           use this IntegralCast<> everywhere but then I figured out that virtually
           everything can be handled using the static_cast<>.
           I don't have any code that would use this and another benefit is that
           currently, it reports diagnostic warning about Wsign-compare or similar so
           I will see what is happening. */

        /* The following two value <= std::numeric_limits<T>::max() comparisons are
           the most dangerous comparisons, especially if comparing 64-bit signed with
           unsigned and if signed contains negative integer. */
        else if constexpr (std::numeric_limits<T>::is_signed)
            return value <= std::numeric_limits<T>::max();

        else if constexpr (!std::numeric_limits<T>::is_signed)
                   // Comparing any type with 0 is always safe
            return value >= 0 && value <= std::numeric_limits<T>::max();

#if defined(__GNUG__) && defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE < 13
        else {
            Q_UNREACHABLE(); // Don't throw here, this must be handled/solved during development
            return false;
        }
#else
        else
            // This should never happen :/
            static_assert (false,
                    "Unhandled code branch in the Private::InRange<T, V>().");
#endif
    }
} // namespace Private

/*! Cast the given integral value to the T type with range checks, throw if failed. */
template<Private::IntegralNoBoolChar T, Private::IntegralNoBoolChar V>
requires (!std::is_reference_v<T> && !std::is_reference_v<V>)
auto IntegralCast(const V value)
{
    if (Private::InRange<T>(value))
        return static_cast<T>(value);

    throw Exceptions::OutOfRangeError(
                QStringLiteral(
                    "The given value %1 of '%2' integer type can't be cast "
                    "to the '%3' integer type with range [%4..%5].")
                .arg(Private::formatNumber(value))
                .arg(Private::IntegralTypeName<V>(),
                     Private::IntegralTypeName<T>())
                .arg(Private::formatNumber(std::numeric_limits<T>::min()))
                .arg(Private::formatNumber(std::numeric_limits<T>::max())));
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_INTEGRALCAST_HPP
