#pragma once
#ifndef ORM_UTILS_NOTNULL_HPP
#define ORM_UTILS_NOTNULL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/commonnamespace.hpp"

#ifdef TINYORM_DEBUG
#  include <type_traits>
#else
#  include <exception>
#endif

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
            std::is_convertible_v<decltype (std::declval<T>() != nullptr), bool>>>
        : std::true_type
    {};

    /*! Resolves to the more efficient of "const T" or "const T &", in the context
        of returning a const-qualified value of type T.
        Copied from cppfront's implementation of the CppCoreGuidelines F.16
        (https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-in) */
    template<typename T>
    using value_or_reference_return_t =
            std::conditional_t<sizeof (T) < 2 * sizeof (void *) &&
                               std::is_trivially_copy_constructible_v<T>,
                               const T, const T &>;

} // namespace Private

    /*! Restrict a pointer or smart pointer to only hold non-null values.
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
        static_assert (Private::is_comparable_to_nullptr<T>::value,
                "The NotNull T template argument cannot be the nullptr.");

        template<std::convertible_to<T> U>
        constexpr NotNull(U &&u) // NOLINT(google-explicit-constructor)
            : m_ptr(std::forward<U>(u))
        {
#ifdef TINYORM_DEBUG
            Q_ASSERT_X(m_ptr != nullptr, "NotNull", "NotNull pointer can't be nullptr.");
#else
            if (m_ptr == nullptr)
                std::terminate();
#endif
        }

        constexpr NotNull(T u) requires (!std::is_same_v<std::nullptr_t, T>) // NOLINT(google-explicit-constructor)
            : m_ptr(std::move(u))
        {
#ifdef TINYORM_DEBUG
            Q_ASSERT_X(m_ptr != nullptr, "NotNull", "NotNull pointer can't be nullptr.");
#else
            if (m_ptr == nullptr)
                std::terminate();
#endif
        }

        template<std::convertible_to<T> U>
        constexpr NotNull(const NotNull<U> &other) // NOLINT(google-explicit-constructor)
            : NotNull(other.get())
        {}

        NotNull(const NotNull &) = default;
        NotNull &operator=(const NotNull &) = default;

        NotNull(NotNull &&) = default;
        NotNull &operator=(NotNull &&) = default;

        ~NotNull() = default;

        /*! Return the managed pointer (allows access to the smart pointer). */
        constexpr Private::value_or_reference_return_t<T> get() const // NOLINT(readability-const-return-type)
        {
            return m_ptr;
        }

        /*! Converting operator to the T (use carefully as it always returns a copy!). */
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
        return NotNull<std::remove_cvref_t<T>>(std::forward<T>(t));
    }

    template<typename T, typename U>
    auto operator==(const NotNull<T> &left,
                    const NotNull<U> &right) noexcept(noexcept(left.get() == right.get()))
        -> decltype (left.get() == right.get())
    {
        return left.get() == right.get();
    }

    template<typename T, typename U>
    auto operator!=(const NotNull<T> &left,
                    const NotNull<U> &right) noexcept(noexcept(left.get() != right.get()))
        -> decltype (left.get() != right.get())
    {
        return left.get() != right.get();
    }

    template<typename T, typename U>
    auto operator<(const NotNull<T> &left,
                   const NotNull<U> &right) noexcept(noexcept(left.get() < right.get()))
        -> decltype (left.get() < right.get())
    {
        return left.get() < right.get();
    }

    template<typename T, typename U>
    auto operator<=(const NotNull<T> &left,
                    const NotNull<U> &right) noexcept(noexcept(left.get() <= right.get()))
        -> decltype (left.get() <= right.get())
    {
        return left.get() <= right.get();
    }

    template<typename T, typename U>
    auto operator>(const NotNull<T> &left,
                   const NotNull<U> &right) noexcept(noexcept(left.get() > right.get()))
        -> decltype (left.get() > right.get())
    {
        return left.get() > right.get();
    }

    template<typename T, typename U>
    auto operator>=(const NotNull<T> &left,
                    const NotNull<U> &right) noexcept(noexcept(left.get() >= right.get()))
        -> decltype (left.get() >= right.get())
    {
        return left.get() >= right.get();
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

/*! The std::hash partial specialization for the Orm::Utils::NotNull<T>. */
template<typename T>
struct std::hash<Orm::Utils::NotNull<T>>
{
    /*! Generate hash for the given Orm::Utils::NotNull<T>. */
    std::size_t operator()(const Orm::Utils::NotNull<T> &value) const
    {
        return std::hash<T>()(value.get());
    }
};

#endif // ORM_UTILS_NOTNULL_HPP
