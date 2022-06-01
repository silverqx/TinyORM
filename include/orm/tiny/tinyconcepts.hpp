#pragma once
#ifndef ORM_TINY_TINYCONCEPTS_HPP
#define ORM_TINY_TINYCONCEPTS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <concepts>
#include <memory>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{

    /*! Concept for Model's AllRelations template parameter, AllRelations can not
        contain actual model type declared in the Derived template parameter. */
    template<typename Derived, typename ...AllRelations>
    concept AllRelationsConcept = (!std::same_as<Derived, AllRelations> && ...);

    class IsModel;

    /*! Concept to check whether a passed type is the model class. */
    template<typename T>
    concept ModelConcept = std::derived_from<T, IsModel>;

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TINYCONCEPTS_HPP
