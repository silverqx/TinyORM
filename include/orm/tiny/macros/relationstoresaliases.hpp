#pragma once
#ifndef ORM_TINY_MACROS_RELATIONSTORESALIASES_HPP
#define ORM_TINY_MACROS_RELATIONSTORESALIASES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

/* Legend:
   RSA - RELATIONSTORESALIASES
   ERS - EagerRelationStore */

/* clang <16 crash during compilation during the EagerRelationStore template
   instantiation, may be it has a problem with the forward declaration. */

#define TINY_RSA_ERS_TMPL_CLANG16 SameDerivedCollectionModel<Derived>
#define TINY_RSA_ERS_TMPL_NOT_CLANG16 typename

#define TINY_RSA_ERS_CLANG16_2(cond) TINY_RSA_ERS_TMPL_##cond
#define TINY_RSA_ERS_CLANG16(cond) TINY_RSA_ERS_CLANG16_2(cond)

#if defined(__clang__) && __clang_major__ >= 16
# define TINY_RSA_ERS_IS_CLANG16 CLANG16
#else
# define TINY_RSA_ERS_IS_CLANG16 NOT_CLANG16
#endif

/*! Relation stores aliases to shorten type names. */
#define TINY_RELATIONSTORES_ALIASES                                                     \
    /*! Alias for the BelongsToManyRelatedTableStore (for shorter name). */             \
    using BelongsToManyRelatedTableStore =                                              \
          Support::Stores::BelongsToManyRelatedTableStore<Derived, AllRelations...>;    \
                                                                                        \
    /*! Alias for the EagerRelationStore (for shorter name). */                         \
    template<TINY_RSA_ERS_CLANG16(TINY_RSA_ERS_IS_CLANG16) CollectionModel>             \
    using EagerRelationStore =                                                          \
          Support::Stores::                                                             \
                   EagerRelationStore<Derived, CollectionModel, AllRelations...>;       \
                                                                                        \
    /*! Alias for the LazyRelationStore (for shorter name). */                          \
    template<typename Related>                                                          \
    using LazyRelationStore =                                                           \
          Support::Stores::LazyRelationStore<Derived, Related, AllRelations...>;        \
                                                                                        \
    /*! Alias for the PushRelationStore (for shorter name). */                          \
    using PushRelationStore =                                                           \
          Support::Stores::PushRelationStore<Derived, AllRelations...>;                 \
                                                                                        \
    /*! Alias for the QueriesRelationshipsStore (for shorter name). */                  \
    template<typename Related>                                                          \
    using QueriesRelationshipsStore =                                                   \
          Support::Stores::                                                             \
                   QueriesRelationshipsStore<Derived, Related, AllRelations...>;        \
                                                                                        \
    /*! Alias for the SerializeRelationStore (for shorter name). */                     \
    template<SerializedAttributes C>                                                    \
    using SerializeRelationStore =                                                      \
          Support::Stores::SerializeRelationStore<C, Derived, AllRelations...>;         \
                                                                                        \
    /*! Alias for the TouchOwnersRelationStore (for shorter name). */                   \
    using TouchOwnersRelationStore =                                                    \
          Support::Stores::TouchOwnersRelationStore<Derived, AllRelations...>;

#endif // ORM_TINY_MACROS_RELATIONSTORESALIASES_HPP
