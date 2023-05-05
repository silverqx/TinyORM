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
    concept ModelConcept = std::derived_from<T, IsModel> &&
                           std::derived_from<T, typename T::BaseModelType>;

    /*! Concept for Derived Model and Model * for ModelsCollection. */
    template<typename T>
    concept DerivedModel =
            (!std::is_reference_v<T> &&
             !std::is_const_v<std::remove_reference_t<T>>) &&
            ((std::is_pointer_v<T> &&
              std::derived_from<std::remove_pointer_t<T>, IsModel> &&
              std::derived_from<std::remove_pointer_t<T>,
                                typename std::remove_pointer_t<T>::BaseModelType>) ||
            // The same check can't be used as the model classes are undefined here
            !std::is_pointer_v<T>);

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TINYCONCEPTS_HPP
