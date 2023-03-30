#pragma once
#ifndef ORM_UTILS_NOTNULL_HPP
#define ORM_UTILS_NOTNULL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/commonnamespace.hpp"

#include <algorithm>    // for forward
#include <cstddef>      // for ptrdiff_t, nullptr_t, size_t
#include <memory>       // for shared_ptr, unique_ptr
#include <system_error> // for hash
#include <type_traits>  // for enable_if_t, is_convertible, is_assignable

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{
namespace Private
{
    template<typename T, typename = void>
    struct is_comparable_to_nullptr : std::false_type
    {};

    template<typename T>
    struct is_comparable_to_nullptr<
        T,
        std::enable_if_t<
            std::is_convertible<decltype (std::declval<T>() != nullptr), bool>::value>>
        : std::true_type
    {};

    /*! Resolves to the more efficient of "const T" or "const T &", in the context
        of returning a const-qualified value of type T.
        Copied from cppfront's implementation of the CppCoreGuidelines F.16
        (https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-in) */
    template<typename T>
    using value_or_reference_return_t =
            std::conditional_t<sizeof(T) < 2 * sizeof(void *) &&
                               std::is_trivially_copy_constructible<T>::value,
                               const T, const T &>;

} // namespace Private

    /*! Restricts a pointer or smart pointer to only hold non-null values.
        Has zero size overhead over T.
        If T is a pointer (i.e. T == U*) then:
        - allow construction from U*
        - disallow construction from nullptr_t
        - disallow default construction
        - ensure construction from null U* fails
        - allow implicit conversion to U* */
    template<typename T>
    class NotNull
    {
    public:
        static_assert(Private::is_comparable_to_nullptr<T>::value,
                      "T cannot be compared to nullptr.");

        template<typename U,
                 typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        constexpr NotNull(U &&u) // NOLINT(google-explicit-constructor)
            : m_ptr(std::forward<U>(u))
        {
            if (m_ptr == nullptr)
                std::terminate();
        }

        template<typename = std::enable_if_t<!std::is_same<std::nullptr_t, T>::value>>
        constexpr NotNull(T u) // NOLINT(google-explicit-constructor)
            : m_ptr(std::move(u))
        {
            if (m_ptr == nullptr)
                std::terminate();
        }

        template<typename U,
                 typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        constexpr NotNull(const NotNull<U> &other) // NOLINT(google-explicit-constructor)
            : NotNull(other.get())
        {}

        NotNull(const NotNull &other) = default;
        NotNull &operator=(const NotNull &other) = default;

        /*! Return the manager pointer. */
        constexpr Private::value_or_reference_return_t<T> get() const
        {
            return m_ptr;
        }

        constexpr operator T() const { return get(); } // NOLINT(google-explicit-constructor)
        constexpr decltype (auto) operator->() const { return get(); }
        constexpr decltype (auto) operator*() const { return *get(); }

        /*! Prevent compilation when someone attempts to assign a null pointer
            constant. */
        NotNull(std::nullptr_t) = delete;
        /*! Prevent compilation when someone attempts to assign a null pointer
            constant. */
        NotNull &operator=(std::nullptr_t) = delete;

        // Unwanted operators, pointers only point to single objects!
        NotNull &operator++() = delete;
        NotNull &operator--() = delete;
        NotNull operator++(int) = delete;
        NotNull operator--(int) = delete;
        NotNull &operator+=(std::ptrdiff_t) = delete;
        NotNull &operator-=(std::ptrdiff_t) = delete;
        void operator[](std::ptrdiff_t) const = delete;

    private:
        T m_ptr;
    };

    /*! Factory method for NotNull<T>. */
    template<typename T>
    auto makeNotNull(T &&t) noexcept
    {
        using Type = std::remove_cv_t<std::remove_reference_t<T>>;

        return NotNull<Type>(std::forward<T>(t));
    }

    template<typename T, typename U>
    auto operator==(const NotNull<T> &lhs,
                    const NotNull<U> &rhs) noexcept(noexcept(lhs.get() == rhs.get()))
        -> decltype (lhs.get() == rhs.get())
    {
        return lhs.get() == rhs.get();
    }

    template<typename T, typename U>
    auto operator!=(const NotNull<T> &lhs,
                    const NotNull<U> &rhs) noexcept(noexcept(lhs.get() != rhs.get()))
        -> decltype (lhs.get() != rhs.get())
    {
        return lhs.get() != rhs.get();
    }

    template<typename T, typename U>
    auto operator<(const NotNull<T> &lhs,
                   const NotNull<U> &rhs) noexcept(noexcept(lhs.get() < rhs.get()))
        -> decltype (lhs.get() < rhs.get())
    {
        return lhs.get() < rhs.get();
    }

    template<typename T, typename U>
    auto operator<=(const NotNull<T> &lhs,
                    const NotNull<U> &rhs) noexcept(noexcept(lhs.get() <= rhs.get()))
        -> decltype (lhs.get() <= rhs.get())
    {
        return lhs.get() <= rhs.get();
    }

    template<typename T, typename U>
    auto operator>(const NotNull<T> &lhs,
                   const NotNull<U> &rhs) noexcept(noexcept(lhs.get() > rhs.get()))
        -> decltype (lhs.get() > rhs.get())
    {
        return lhs.get() > rhs.get();
    }

    template<typename T, typename U>
    auto operator>=(const NotNull<T> &lhs,
                    const NotNull<U> &rhs) noexcept(noexcept(lhs.get() >= rhs.get()))
        -> decltype (lhs.get() >= rhs.get())
    {
        return lhs.get() >= rhs.get();
    }

    // More unwanted operators
    template<typename T, typename U>
    std::ptrdiff_t operator-(const NotNull<T> &, const NotNull<U> &) = delete;
    template<typename T>
    NotNull<T> operator-(const NotNull<T> &, std::ptrdiff_t) = delete;
    template<typename T>
    NotNull<T> operator+(const NotNull<T> &, std::ptrdiff_t) = delete;
    template<typename T>
    NotNull<T> operator+(std::ptrdiff_t, const NotNull<T> &) = delete;

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

namespace std
{
    template <typename T>
    struct hash<Orm::Utils::NotNull<T>>
    {
        std::size_t operator()(const Orm::Utils::NotNull<T> &value) const
        {
            return hash<T>()(value.get());
        }
    };

} // namespace std

#endif // ORM_UTILS_NOTNULL_HPP
