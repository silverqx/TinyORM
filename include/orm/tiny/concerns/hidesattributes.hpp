#pragma once
#ifndef ORM_TINY_CONCERNS_HIDESATTRIBUTES_HPP
#define ORM_TINY_CONCERNS_HIDESATTRIBUTES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

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
        inline Derived &setVisible(const std::set<QString> &visible);
        /*! Set the visible attributes for the model. */
        inline Derived &setVisible(std::set<QString> &&visible);

        /*! Get the hidden attributes for the model. */
        inline const std::set<QString> &getHidden() const noexcept;
        /*! Set the hidden attributes for the model. */
        inline Derived &setHidden(const std::set<QString> &hidden);
        /*! Set the hidden attributes for the model. */
        inline Derived &setHidden(std::set<QString> &&hidden);

        /*! Determine whether the u_visible set contains the given accessor attribute. */
        inline bool hasVisible(const QString &attribute) const;
        /*! Determine whether the u_hidden set contains the given accessor attribute. */
        inline bool hasHidden(const QString &attribute) const;

        /*! Clear the visible attributes for the model. */
        inline Derived &clearVisible() noexcept;
        /*! Clear the hidden attributes for the model. */
        inline Derived &clearHidden() noexcept;

        /*! Make the given, typically hidden, attributes visible. */
        inline Derived &makeVisible(const std::set<QString> &attributes);
        /*! Make the given, typically hidden, attributes visible. */
        inline Derived &makeVisible(QString attribute);

        /*! Make the given, typically visible, attributes hidden. */
        inline Derived &makeHidden(const std::set<QString> &attributes);
        /*! Make the given, typically visible, attributes hidden. */
        inline Derived &makeHidden(QString attribute);

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
    Derived &
    HidesAttributes<Derived, AllRelations...>::setVisible(
            const std::set<QString> &visible)
    {
        basemodel().getUserVisible() = visible;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HidesAttributes<Derived, AllRelations...>::setVisible(std::set<QString> &&visible)
    {
        basemodel().getUserVisible() = std::move(visible);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::set<QString> &
    HidesAttributes<Derived, AllRelations...>::getHidden() const noexcept
    {
        return basemodel().getUserHidden();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HidesAttributes<Derived, AllRelations...>::setHidden(
            const std::set<QString> &hidden)
    {
        basemodel().getUserHidden() = hidden;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HidesAttributes<Derived, AllRelations...>::setHidden(std::set<QString> &&hidden)
    {
        basemodel().getUserHidden() = std::move(hidden);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HidesAttributes<Derived, AllRelations...>::hasVisible(const QString &attribute) const
    {
        return basemodel().getUserVisible().contains(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HidesAttributes<Derived, AllRelations...>::hasHidden(const QString &attribute) const
    {
        return basemodel().getUserHidden().contains(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HidesAttributes<Derived, AllRelations...>::clearVisible() noexcept
    {
        basemodel().getUserVisible().clear();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HidesAttributes<Derived, AllRelations...>::clearHidden() noexcept
    {
        basemodel().getUserHidden().clear();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HidesAttributes<Derived, AllRelations...>::makeVisible(
            const std::set<QString> &attributes)
    {
        auto &basemodel = this->basemodel();

        // First, remove attributes from the u_hidden set
        std::set<QString> newHidden;
        auto &hidden = basemodel.getUserHidden();
        ranges::set_difference(hidden, attributes,
                               ranges::inserter(newHidden, newHidden.cend()));
        hidden = std::move(newHidden);

        /* And merge them into the u_visible (empty u_visible means that all attributes
           are visible, so no merge is needed). */
        if (auto &visible = basemodel.getUserVisible();
            !visible.empty()
        )
            std::ranges::copy(attributes, std::inserter(visible, visible.end()));

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HidesAttributes<Derived, AllRelations...>::makeVisible(QString attribute)
    {
        return makeVisible(std::set<QString> {std::move(attribute)});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HidesAttributes<Derived, AllRelations...>::makeHidden(
            const std::set<QString> &attributes)
    {
        /* Removing attributes from u_visible is not needed because u_hidden overrides
           u_visible. */
        auto &hidden = basemodel().getUserHidden();

        std::ranges::copy(attributes, std::inserter(hidden, hidden.end()));

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HidesAttributes<Derived, AllRelations...>::makeHidden(QString attribute)
    {
        return makeHidden(std::set<QString> {std::move(attribute)});
    }

    /* private */

    /* Static cast this to a child's instance type (CRTP) */

    TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(HidesAttributes)

} // namespace Concerns
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_HIDESATTRIBUTES_HPP
