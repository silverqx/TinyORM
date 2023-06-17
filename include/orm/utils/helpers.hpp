#pragma once
#ifndef ORM_UTILS_HELPERS_HPP
#define ORM_UTILS_HELPERS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include <functional>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

class QDateTime;
class QVariant; // clazy:exclude=qt6-fwd-fixes

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    struct QtTimeZoneConfig;

namespace Utils
{
    /*! Tests if the std::hash can hash T with noexcept. */
    template<class T, class = void>
    struct IsNothrowHashable : std::false_type
    {};

    /*! Tests if the std::hash can hash T with noexcept. */
    template<class T>
    struct IsNothrowHashable<T, std::void_t<decltype (std::hash<T>()(
                                                          std::declval<const T &>()))>>
        : std::bool_constant<noexcept(std::hash<T>()(std::declval<const T &>()))>
    {};

    /*! Helpers library class. */
    class SHAREDLIB_EXPORT Helpers
    {
        Q_DISABLE_COPY_MOVE(Helpers)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Helpers() = delete;
        /*! Deleted destructor. */
        ~Helpers() = delete;

        /*! Call the given callback with the given value then return the value. */
        template<typename T>
        static T &&
        tap(T &&value, std::function<void(T &)> &&callback = nullptr)
        requires (!std::is_reference_v<T>);
        /*! Call the given callback with the given value then return the value. */
        template<typename T>
        static T &&
        tap(T &&value, std::function<void()> &&callback = nullptr)
        requires (!std::is_reference_v<T>);

        /*! Call repeatedly to incrementally create a hash value from several
            variables. */
        template<typename T>
        inline static std::size_t &hashCombine(std::size_t &seed, const T &value)
        noexcept(IsNothrowHashable<std::remove_const_t<T>>::value);

        /*! Get the storage type of the value stored in the QVariant. */
        static int qVariantTypeId(const QVariant &value);

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
        convertTimeZone(const QDateTime &datetime, const QString &connection = "");
        /*! Set the QDateTime's time zone according to the given connection. */
        static QDateTime &
        setTimeZone(QDateTime &datetime, const QString &connection = "");
        /*! Set the QDateTime's time zone according to the given connection. */
        static QDateTime
        setTimeZone(QDateTime &&datetime, const QString &connection = "");
    };

    /* public */

    template<typename T>
    T &&
    Helpers::tap(T &&value, std::function<void(T &)> &&callback)
    requires (!std::is_reference_v<T>)
    {
        if (callback)
            std::invoke(callback, value);

        /* forward not needed, also the std::move() would be ok (treated by constraint),
           forward prevents clang warning. */
        return std::forward<T>(value);
    }

    template<typename T>
    T &&
    Helpers::tap(T &&value, std::function<void()> &&callback)
    requires (!std::is_reference_v<T>)
    {
        if (callback)
            std::invoke(callback);

        /* forward not needed, also the std::move() would be ok (treated by constraint),
           forward prevents clang warning. */
        return std::forward<T>(value);
    }

    template<typename T>
    std::size_t &Helpers::hashCombine(std::size_t &seed, const T &value)
    noexcept(IsNothrowHashable<std::remove_const_t<T>>::value)
    {
        return seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

} // namespace Utils
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_HELPERS_HPP
