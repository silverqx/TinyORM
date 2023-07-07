#pragma once
#ifndef ORM_TINY_CONCERNS_HIDESATTRIBUTES_HPP
#define ORM_TINY_CONCERNS_HIDESATTRIBUTES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <set>

#include <range/v3/algorithm/set_algorithm.hpp>
#include <range/v3/iterator/insert_iterators.hpp>

#include "orm/macros/threadlocal.hpp"
#include "orm/tiny/macros/crtpmodelwithbase.hpp"
#include "orm/tiny/tinyconcepts.hpp" // IWYU pragma: keep

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model;

namespace Concerns
{

    /*! Hides attributes. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HidesAttributes
    {
        // To access u_visible and u_hidden
        friend Model<Derived, AllRelations...>;

    public:

        /*! Get the visible attributes for the model. */
        inline const std::set<QString> &getVisible() const noexcept;
        /*! Set the visible attributes for the model. */
        inline void setVisible(const std::set<QString> &visible);

        /*! Get the hidden attributes for the model. */
        inline const std::set<QString> &getHidden() const noexcept;
        /*! Set the hidden attributes for the model. */
        inline void setHidden(const std::set<QString> &hidden);

        /*! Clear the visible attributes for the model. */
        inline void clearVisible() noexcept;
        /*! Clear the hidden attributes for the model. */
        inline void clearHidden() noexcept;

        /*! Make the given, typically hidden, attributes visible. */
        inline void makeVisible(const std::set<QString> &attributes);
        /*! Make the given, typically visible, attributes hidden. */
        inline void makeHidden(const std::set<QString> &attributes);

    private:
        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_WITH_BASE_DECLARATIONS

        /*! The attributes that should be visible during serialization. */
        T_THREAD_LOCAL
        inline static std::set<QString> u_visible;
        /*! The attributes that should be hidden during serialization. */
        T_THREAD_LOCAL
        inline static std::set<QString> u_hidden;
    };

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::set<QString> &
    HidesAttributes<Derived, AllRelations...>::getVisible() const noexcept
    {
        return basemodel().getUserVisible();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HidesAttributes<Derived, AllRelations...>::setVisible(
            const std::set<QString> &visible)
    {
        basemodel().getUserVisible() = visible;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::set<QString> &
    HidesAttributes<Derived, AllRelations...>::getHidden() const noexcept
    {
        return basemodel().getUserHidden();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HidesAttributes<Derived, AllRelations...>::setHidden(
            const std::set<QString> &hidden)
    {
        basemodel().getUserHidden() = hidden;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HidesAttributes<Derived, AllRelations...>::clearVisible() noexcept
    {
        basemodel().getUserVisible().clear();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HidesAttributes<Derived, AllRelations...>::clearHidden() noexcept
    {
        basemodel().getUserHidden().clear();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HidesAttributes<Derived, AllRelations...>::makeVisible(
            const std::set<QString> &attributes)
    {
        // First, remove attributes from the u_hidden set
        std::set<QString> newHidden;
        auto &hidden = basemodel().getUserHidden();
        ranges::set_difference(hidden, attributes,
                               ranges::inserter(newHidden, newHidden.cend()));
        hidden = std::move(newHidden);

        /* And merge them into the u_visible (empty u_visible means that all attributes
           are visible, so no merge is needed). */
        if (auto &visible = basemodel().getUserVisible();
            !visible.empty()
        )
            std::ranges::copy(attributes, std::inserter(visible, visible.end()));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HidesAttributes<Derived, AllRelations...>::makeHidden(
            const std::set<QString> &attributes)
    {
        /* Removing attributes from u_visible is not needed because u_hidden overrides
           u_visible. */
        auto &hidden = basemodel().getUserHidden();

        std::ranges::copy(attributes, std::inserter(hidden, hidden.end()));
    }

    /* private */

    /* Static cast this to a child's instance type (CRTP) */
    TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(HidesAttributes)

} // namespace Concerns
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_HIDESATTRIBUTES_HPP
