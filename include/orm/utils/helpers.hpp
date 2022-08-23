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

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_HELPERS_HPP
