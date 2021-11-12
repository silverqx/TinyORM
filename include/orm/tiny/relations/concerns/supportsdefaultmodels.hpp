#pragma once
#ifndef ORM_SUPPORTSDEFAULTMODELS_HPP
#define ORM_SUPPORTSDEFAULTMODELS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/threadlocal.hpp"
// FEATURE orm types, only AttributeItem used silverqx
#include "orm/ormtypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{
    template<class Model, class Related>
    class Relation;

namespace Concerns {

    /*! Default models. */
    template<class Model, class Related>
    class SupportsDefaultModels
    {
        using AttributeItem = Orm::AttributeItem;
//        using Callback      = std::function<void(Related &, const Model &)>;

    public:
        /*! Pure virtual destructor. */
        inline virtual ~SupportsDefaultModels() = 0;

        /*! Return a new model instance in case the relationship does not exist. */
        Relation<Model, Related> &withDefault(bool value = true);
        /*! Return a new model instance in case the relationship does not exist. */
        Relation<Model, Related> &withDefault(const QVector<AttributeItem> &attributes);
        /*! Return a new model instance in case the relationship does not exist. */
        Relation<Model, Related> &withDefault(QVector<AttributeItem> &&attributes);
        /*! Return a new model instance in case the relationship does not exist. */
//        Relation<Model, Related> &withDefault(Callback &&callback);

    protected:
        /*! Make a new related instance for the given model. */
        virtual Related newRelatedInstanceFor(const Model &parent) const = 0;

        /*! Get the default value for this relation. */
        std::optional<Related> getDefaultFor(const Model &parent) const;

        /*! Indicates if a default model instance should be used. */
        std::variant<bool, QVector<AttributeItem>/*, Callback*/> m_withDefault = false;

    private:
        /*! Return cached reference to the base relation instance. */
        Relation<Model, Related> &relation();
    };

    template<class Model, class Related>
    SupportsDefaultModels<Model, Related>::~SupportsDefaultModels() = default;

    template<class Model, class Related>
    Relation<Model, Related> &
    SupportsDefaultModels<Model, Related>::withDefault(const bool value)
    {
        m_withDefault = value;

        return relation();
    }

    template<class Model, class Related>
    Relation<Model, Related> &
    SupportsDefaultModels<Model, Related>::withDefault(
            const QVector<AttributeItem> &attributes)
    {
        m_withDefault = attributes;

        return relation();
    }

    template<class Model, class Related>
    Relation<Model, Related> &
    SupportsDefaultModels<Model, Related>::withDefault(
            QVector<AttributeItem> &&attributes)
    {
        m_withDefault = std::move(attributes);

        return relation();
    }

//    template<class Model, class Related>
//    Relation<Model, Related> &
//    SupportsDefaultModels<Model, Related>::withDefault(Callback &&callback)
//    {
//        m_withDefault = std::move(callback);

//        return relation();
//    }

    template<class Model, class Related>
    std::optional<Related>
    SupportsDefaultModels<Model, Related>::getDefaultFor(const Model &parent) const
    {
        const auto index = m_withDefault.index();

        // Defaul Model is disabled, return null
        if (index == 0 && !std::get<bool>(m_withDefault))
            return std::nullopt;

        auto instance = newRelatedInstanceFor(parent);

        // FUTURE pass callback to withDefault(), I will need proxy model or better attribute proxy, something similar or the same like Model::AttributeReference class silverqx
        // Invoke passed callback
//        if (index == 2) {
//            std::invoke(std::get<Callback>(m_withDefault), instance, parent);

//            return instance;
//        }

        // If model attributes were passed, then fill them
        if (index == 1)
            if (const auto attributes = std::get<QVector<AttributeItem>>(m_withDefault);
                !attributes.isEmpty()
            )
                instance.forceFill(attributes);

        return instance;
    }

    template<class Model, class Related>
    inline Relation<Model, Related> &
    SupportsDefaultModels<Model, Related>::relation()
    {
        T_THREAD_LOCAL
        static auto &cached = dynamic_cast<Relation<Model, Related> &>(*this);

        return cached;
    }

} // namespace Concerns
} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SUPPORTSDEFAULTMODELS_HPP
