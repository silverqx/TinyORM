#pragma once
#ifndef ORM_TINY_TINYCONCEPTS_HPP
#define ORM_TINY_TINYCONCEPTS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtCore/qcontainerfwd.h>

#include <concepts>

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
    concept DerivedCollectionModel =
            // Common for all
            (!std::is_reference_v<T> && !std::is_const_v<T>) &&
             // For the Model *
            ((std::is_pointer_v<T> &&
              std::derived_from<std::remove_pointer_t<T>, IsModel> &&
              std::derived_from<std::remove_pointer_t<T>,
                                typename std::remove_pointer_t<T>::BaseModelType>) ||
             // For the Model
             // The same check can't be used because the model classes are undefined here
             !std::is_pointer_v<T>);

    /*! Concept for Derived Model and Model * for ModelsCollection that has to be
        the same as another Derived Model. */
    template<typename T, typename U>
    concept SameDerivedCollectionModel = DerivedCollectionModel<T> &&
                                         DerivedCollectionModel<U> &&
                                         std::same_as<std::remove_pointer_t<T>, U>;
    /* Serialization */

    struct AttributeItem;

    /*! Concept for Attributes vector container. */
    template<typename C>
    concept AttributesContainerConcept = std::convertible_to<C, QList<AttributeItem>>;

    /*! Concept to check the container for serialized model attributes. */
    template<typename C>
    concept SerializedAttributes = std::same_as<C, QVariantMap> ||
                                   std::same_as<C, QList<AttributeItem>>;

    /* Others */

    /*! Concept to check if the given type has the reserve() method (Qt or std). */
    template<typename C>
    concept HasReserveMethod = requires(C c)
    {
        typename C::size_type;
        // Good enough, I won't invest more effort into this 🙃
        requires std::same_as<typename C::size_type, std::size_t> ||
                 std::same_as<typename C::size_type, qsizetype>;
        { c.reserve(typename C::size_type()) } -> std::same_as<void>;
    };

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TINYCONCEPTS_HPP
