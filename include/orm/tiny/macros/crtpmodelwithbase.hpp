#pragma once
#ifndef ORM_TINY_MACROS_CRTPMODELWITHBASE_HPP
#define ORM_TINY_MACROS_CRTPMODELWITHBASE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/macros/crtpmodel.hpp"

/*! CRTP declarations for model()/basemodel() methods. */
#define TINY_CRTP_MODEL_WITH_BASE_DECLARATIONS                                    \
        TINY_CRTP_MODEL_DECLARATIONS                                              \
        /*! Static cast this to a child's instance Model type. */                 \
        inline Model<Derived, AllRelations...> &basemodel() noexcept;             \
        /*! Static cast this to a child's instance Model type, const version. */  \
        inline const Model<Derived, AllRelations...> &basemodel() const noexcept;

/*! CRTP definitions for model()/basemodel() methods. */
#define TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(Class)                              \
    TINY_CRTP_MODEL_DEFINITIONS(Class)                                            \
                                                                                  \
    template<typename Derived, AllRelationsConcept ...AllRelations>               \
    Model<Derived, AllRelations...> &                                             \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                              \
    Class<Derived, AllRelations...>::basemodel() noexcept                         \
    {                                                                             \
        /* Can not be cached with static because a copy can be made */            \
        return static_cast<Model<Derived, AllRelations...> &>(*this);             \
    }                                                                             \
                                                                                  \
    template<typename Derived, AllRelationsConcept ...AllRelations>               \
    const Model<Derived, AllRelations...> &                                       \
    /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                              \
    Class<Derived, AllRelations...>::basemodel() const noexcept                   \
    {                                                                             \
        return static_cast<const Model<Derived, AllRelations...> &>(*this);       \
    }

#endif // ORM_TINY_MACROS_CRTPMODELWITHBASE_HPP
