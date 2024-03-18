#pragma once
#ifndef ORM_TINY_CASTS_ATTRIBUTE_HPP
#define ORM_TINY_CASTS_ATTRIBUTE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/commonnamespace.hpp"
#include "orm/tiny/types/modelattributes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Casts
{

    /*! Concept for the Attribute::get<> (std::get(std::variant)). */
    template<typename T>
    concept GetAttributeCallback =
            std::same_as<T, std::function<QVariant()>> ||
            std::same_as<T, std::function<QVariant(const ModelAttributes &)>>;

    /*! The Attribute class to return from mutators. */
    class Attribute
    {
    public:
        /*! Alias for a callback without parameters. */
        using CallbackWithoutParameters = std::function<QVariant()>;
        /*! Alias for a callback with attributes map parameter. */
        using CallbackWithAttributes = std::function<QVariant(const ModelAttributes &)>;

        /*! Alias for the attribute accessor/mutator callback. */
        using CallbackType = std::variant<CallbackWithoutParameters,
                                          CallbackWithAttributes>;

        /*! Deleted default constructor (not needed). */
        Attribute() = delete;
        /*! Default destructor. */
        ~Attribute() = default;

        /*! Converting constructor from the CallbackType (typically lambda). */
        inline explicit Attribute(CallbackType &&get) noexcept;

        /*! Copy constructor. */
        Attribute(const Attribute &) = default;
        /*! Copy assignment operator. */
        Attribute &operator=(const Attribute &) = default;

        /*! Move constructor. */
        Attribute(Attribute &&) noexcept = default;
        /*! Move assignment operator. */
        Attribute &operator=(Attribute &&) noexcept = default;

        /*! Get the attribute accessor. */
        inline const CallbackType &get() const noexcept;
        /*! Get the attribute accessor. */
        template<GetAttributeCallback T>
        inline const T &get() const;

        /*! Create a new attribute accessor/mutator. */
        inline static Attribute make(CallbackType &&get) noexcept;
        /*! Create a new attribute accessor. */
        inline static Attribute get(CallbackType &&get) noexcept;

        /*! Determine whether the caching is enabled. */
        inline bool withCaching() const noexcept;
        /*! Enable caching for this attribute. */
        inline Attribute &shouldCache() noexcept;

    private:
        /*! The attribute accessor. */
        CallbackType m_get;

        /*! Indicates if caching is enabled for this attribute. */
        bool m_withCaching = false;
    };

    /* public */

    Attribute::Attribute(CallbackType &&get) noexcept
        : m_get(std::move(get))
    {}

    const Attribute::CallbackType &Attribute::get() const noexcept
    {
        return m_get;
    }

    template<GetAttributeCallback T>
    const T &Attribute::get() const
    {
        return std::get<T>(m_get);
    }

    Attribute Attribute::make(CallbackType &&get) noexcept
    {
        return Attribute(std::move(get));
    }

    Attribute Attribute::get(CallbackType &&get) noexcept
    {
        return Attribute(std::move(get));
    }

    bool Attribute::withCaching() const noexcept
    {
        return m_withCaching;
    }

    Attribute &Attribute::shouldCache() noexcept
    {
        m_withCaching = true;

        return *this;
    }

} // namespace Orm::Tiny::Casts

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CASTS_ATTRIBUTE_HPP
