#pragma once
#ifndef ORM_UTILS_HELPERS_HPP
#define ORM_UTILS_HELPERS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include <functional>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{
    /*! Tests if the std::hash can hash T with noexcept. */
    template <class T, class = void>
    struct IsNothrowHashable : std::false_type
    {};

    /*! Tests if the std::hash can hash T with noexcept. */
    template <class T>
    struct IsNothrowHashable<T, std::void_t<decltype(std::hash<T>()(
                                                         std::declval<const T &>()))>>
        : std::bool_constant<noexcept(std::hash<T>()(std::declval<const T &>()))>
    {};

    /*! Helpers library class. */
    class Helpers
    {
        Q_DISABLE_COPY(Helpers)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Helpers() = delete;
        /*! Deleted destructor. */
        ~Helpers() = delete;

        /*! Call the given callback with the given value then return the value. */
        template<typename T>
        static T &&tap(std::remove_reference_t<T> &&value,
                       std::function<void(T &)> &&callback = nullptr) noexcept;
        /*! Call the given callback with the given value then return the value. */
        template<typename T>
        static T &&tap(std::remove_reference_t<T> &&value,
                       std::function<void()> &&callback = nullptr) noexcept;

        /*! Call repeatedly to incrementally create a hash value from several
            variables. */
        template <typename T>
        inline static std::size_t &hashCombine(std::size_t &seed, const T &value)
        noexcept(IsNothrowHashable<std::remove_const_t<T>>::value);
    };

    /* public */

    template<typename T>
    T &&Helpers::tap(std::remove_reference_t<T> &&value,
                     std::function<void(T &)> &&callback) noexcept
    {
        if (callback)
            std::invoke(callback, value);

        return std::move(value);
    }

    template<typename T>
    T &&Helpers::tap(std::remove_reference_t<T> &&value,
                     std::function<void()> &&callback) noexcept
    {
        if (callback)
            std::invoke(callback);

        return std::move(value);
    }

    template<typename T>
    std::size_t &Helpers::hashCombine(std::size_t &seed, const T &value)
    noexcept(IsNothrowHashable<std::remove_const_t<T>>::value)
    {
        return seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_HELPERS_HPP
