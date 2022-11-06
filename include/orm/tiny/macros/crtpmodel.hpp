#pragma once
#ifndef ORM_TINY_MACROS_CRTPMODEL_HPP
#define ORM_TINY_MACROS_CRTPMODEL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

/*! CRTP declarations for model() methods. */
#define TINY_CRTP_MODEL_DECLARATIONS                                              \
        /*! Static cast this to a child's instance type (CRTP). */                \
        inline Derived &model() noexcept;                                         \
        /*! Static cast this to a child's instance type (CRTP), const version. */ \
        inline const Derived &model() const noexcept;

/*! CRTP definitions for model() methods. */
#define TINY_CRTP_MODEL_DEFINITIONS(Class)                                        \
    template<typename Derived, AllRelationsConcept ...AllRelations>               \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                              \
    Derived &Class<Derived, AllRelations...>::model() noexcept                    \
    {                                                                             \
        /* Can not be cached with static because a copy can be made */            \
        return static_cast<Derived &>(*this);                                     \
    }                                                                             \
                                                                                  \
    template<typename Derived, AllRelationsConcept ...AllRelations>               \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                              \
    const Derived &Class<Derived, AllRelations...>::model() const noexcept        \
    {                                                                             \
        return static_cast<const Derived &>(*this);                               \
    }

#endif // ORM_TINY_MACROS_CRTPMODEL_HPP
