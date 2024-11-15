#pragma once
#ifndef ORM_UTILS_HELPERS_HPP
#define ORM_UTILS_HELPERS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <functional>

#include "orm/constants.hpp"
#include "orm/macros/no_sanitize.hpp"

#ifndef TINYORM_EXTERN_CONSTANTS
#  include "orm/macros/export.hpp"
#endif

class QDateTime;
class QVariant; // clazy:exclude=qt6-fwd-fixes

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    struct QtTimeZoneConfig;

namespace Utils
{
    /*! Tests if the std::hash can hash T with noexcept. */
    template<typename T, typename U = std::remove_const_t<T>> // std::remove_const_t<> still makes sense in some cases
    constexpr auto IsNothrowHashable = noexcept(std::hash<U>()(
                                                    std::declval<const U &>()));

    /*! Concept for the Helpers::tap(), typename. */
    template<typename T>
    concept TapConcept = !std::is_reference_v<T> && !std::is_const_v<T>;
    /*! Concept for the Helpers::tap(), trailing requires. */
    template<typename T>
    concept TapTrailingConcept = std::is_rvalue_reference_v<T> &&
                                 !std::is_const_v<std::remove_reference_t<T>>;

    /*! Helpers library class. */
    class TINYORM_EXPORT Helpers
    {
        Q_DISABLE_COPY_MOVE(Helpers)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Helpers() = delete;
        /*! Deleted destructor. */
        ~Helpers() = delete;

        /*! Call the given callback with the given value then return the value. */
        template<TapConcept T>
        constexpr static T &&
        tap(T &&value, const std::function<void(T &)> &callback = nullptr)
        requires TapTrailingConcept<decltype (value)>;
        /*! Call the given callback with the given value then return the value. */
        template<TapConcept T>
        constexpr static T &&
        tap(T &&value, const std::function<void()> &callback = nullptr)
        requires TapTrailingConcept<decltype (value)>;

        /*! Call repeatedly to incrementally create a hash value from several
            variables. */
        template<typename T>
        inline static std::size_t &hashCombine(std::size_t &seed, const T &value)
        noexcept(IsNothrowHashable<T>);

        /*! Log exception caught in the main exception handler in a current thread. */
        [[maybe_unused]]
        static void logException(const std::exception &e, bool fatal = false);

        /* QDateTime related */
        /*! Determine if the given value is a standard date format. */
        static bool isStandardDateFormat(const QString &value);

        /*! Convert the QDateTime's time zone to the given time zone. */
        static QDateTime
        convertTimeZone(const QDateTime &datetime, const QtTimeZoneConfig &timezone);
        /*! Set the QDateTime's time zone to the given time zone. */
        static QDateTime &
        setTimeZone(QDateTime &datetime, const QtTimeZoneConfig &timezone);
        /*! Set the QDateTime's time zone to the given time zone. */
        static QDateTime
        setTimeZone(QDateTime &&datetime, const QtTimeZoneConfig &timezone);

        /*! Convert the QDateTime's time zone according to the given connection name. */
        static QDateTime
        convertTimeZone(const QDateTime &datetime,
                        const QString &connection = Constants::EMPTY);
        /*! Set the QDateTime's time zone according to the given connection. */
        static QDateTime &
        setTimeZone(QDateTime &datetime,
                    const QString &connection = Constants::EMPTY);
        /*! Set the QDateTime's time zone according to the given connection. */
        static QDateTime
        setTimeZone(QDateTime &&datetime,
                    const QString &connection = Constants::EMPTY);

    private:
        /*! Log the QLibraryInfo::PluginsPath if an exception message contains:
            QSqlError(Driver not loaded, Driver not loaded). */
        [[maybe_unused]]
        static void logPluginsPath(const QString &exceptionMessage);
    };

    /* public */

    template<TapConcept T>
    constexpr T &&
    Helpers::tap(T &&value, const std::function<void(T &)> &callback)
    requires TapTrailingConcept<decltype (value)>
    {
        if (callback)
            std::invoke(callback, value);

        /* forward not needed, also the std::move() would be ok (treated by constraint),
           forward prevents Clang warning. */
        return std::forward<T>(value);
    }

    template<TapConcept T>
    constexpr T &&
    Helpers::tap(T &&value, const std::function<void()> &callback)
    requires TapTrailingConcept<decltype (value)>
    {
        if (callback)
            std::invoke(callback);

        /* forward not needed, also the std::move() would be ok (treated by constraint),
           forward prevents Clang warning. */
        return std::forward<T>(value);
    }

    template<typename T>
    TINYORM_NO_SANITIZE("unsigned-integer-overflow")
    std::size_t &Helpers::hashCombine(std::size_t &seed, const T &value)
    noexcept(IsNothrowHashable<T>)
    {
        return seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

} // namespace Utils
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_HELPERS_HPP
