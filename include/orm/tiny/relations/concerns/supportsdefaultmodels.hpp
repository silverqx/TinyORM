#pragma once
#ifndef ORM_TINY_RELATIONS_CONCERNS_SUPPORTSDEFAULTMODELS_HPP
#define ORM_TINY_RELATIONS_CONCERNS_SUPPORTSDEFAULTMODELS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

// FEATURE tiny types, only AttributeItem used silverqx
#include "orm/tiny/tinytypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations::Concerns
{

    /*! Default models. */
    template<class Model, class Related, template<class, class> class RelationType>
    class SupportsDefaultModels
    {
        Q_DISABLE_COPY_MOVE(SupportsDefaultModels)

        /*! Alias for the AttributeItem. */
        using AttributeItem = Orm::Tiny::AttributeItem;
        /*! Callback used to fill default attribute values for the default model. */
//        using Callback      = std::function<void(Related &, const Model &)>;

    public:
        /*! Default constructor. */
        SupportsDefaultModels() = default;
        /*! Pure virtual destructor. */
        inline virtual ~SupportsDefaultModels() = 0;

        /*! Return a new model instance in case the relationship does not exist. */
        RelationType<Model, Related> &
        withDefault(bool value = true);
        /*! Return a new model instance in case the relationship does not exist. */
        RelationType<Model, Related> &
        withDefault(const QList<AttributeItem> &attributes);
        /*! Return a new model instance in case the relationship does not exist. */
        RelationType<Model, Related> &
        withDefault(QList<AttributeItem> &&attributes);
        /*! Return a new model instance in case the relationship does not exist. */
//        RelationType<Model, Related> &
//        withDefault(Callback &&callback);

    protected:
        /*! Make a new related instance for the given model. */
        virtual Related newRelatedInstanceFor(const Model &parent) const = 0;

        /*! Get the default value for this relation. */
        std::optional<Related> getDefaultFor(const Model &parent) const;
        /*! Get the default value for this relation. */
        inline std::optional<Related> getDefaultFor(const Model *parent) const;

        /*! Indicates if a default model instance should be used. */
        std::variant<bool, QList<AttributeItem>/*, Callback*/> m_withDefault = false;

    private:
        /*! Static cast *this to the HasOne/BelongsTo & derived type. */
        inline RelationType<Model, Related> &relation() noexcept;
    };

    /* public */

    template<class Model, class Related, template<class, class> class RelationType>
    SupportsDefaultModels<Model, Related, RelationType>::
    ~SupportsDefaultModels() = default;

    template<class Model, class Related, template<class, class> class RelationType>
    RelationType<Model, Related> &
    SupportsDefaultModels<Model, Related, RelationType>::withDefault(const bool value)
    {
        m_withDefault = value;

        return relation();
    }

    template<class Model, class Related, template<class, class> class RelationType>
    RelationType<Model, Related> &
    SupportsDefaultModels<Model, Related, RelationType>::withDefault(
            const QList<AttributeItem> &attributes)
    {
        m_withDefault = attributes;

        return relation();
    }

    template<class Model, class Related, template<class, class> class RelationType>
    RelationType<Model, Related> &
    SupportsDefaultModels<Model, Related, RelationType>::withDefault(
            QList<AttributeItem> &&attributes)
    {
        m_withDefault = std::move(attributes);

        return relation();
    }

//    template<class Model, class Related, template<class, class> class RelationType>
//    RelationType<Model, Related> &
//    SupportsDefaultModels<Model, Related, RelationType>::withDefault(Callback &&callback)
//    {
//        m_withDefault = std::move(callback);

//        return relation();
//    }

    /* protected */

    template<class Model, class Related, template<class, class> class RelationType>
    std::optional<Related>
    SupportsDefaultModels<Model, Related, RelationType>::getDefaultFor(
            const Model &parent) const
    {
        // Defaul Model is disabled, return null
        if (std::holds_alternative<bool>(m_withDefault) &&
            !std::get<bool>(m_withDefault)
        )
            return std::nullopt;

        auto instance = newRelatedInstanceFor(parent);

        // If model attributes were passed then fill them
        if (std::holds_alternative<QList<AttributeItem>>(m_withDefault)) {
            if (const auto &attributes = std::get<QList<AttributeItem>>(m_withDefault);
                !attributes.isEmpty()
            )
                /* Don't use the std::move() here because can happen that the Default
                   Attribute values will be moved out from the m_withDefault and then
                   on subsequent calls this attributes vector will be empty. */
                instance.forceFill(attributes);
        }

        // FUTURE pass callback to withDefault(), I will need proxy model or better attribute proxy, something similar or the same like Model::AttributeReference class silverqx
        // Invoke passed callback
//        else if (std::holds_alternative<Callback>(m_withDefault))
//            if (const auto &callback = std::get<Callback>(m_withDefault);
//                callback
//            )
//                std::invoke(callback, instance, parent);

        /* This check is not needed in prod. builds, it's practically useless also
           in debug builds. */
#ifdef TINYORM_DEBUG
        // This should never happen :/
        else if (!std::holds_alternative<bool>(m_withDefault))
            Q_UNREACHABLE();
#endif

        // holds bool and has the true value
        return instance;
    }

    template<class Model, class Related, template<class, class> class RelationType>
    std::optional<Related>
    SupportsDefaultModels<Model, Related, RelationType>::getDefaultFor(
            const Model *const parent) const
    {
        return getDefaultFor(*parent);
    }

    /* private */

    template<class Model, class Related, template<class, class> class RelationType>
    RelationType<Model, Related> &
    SupportsDefaultModels<Model, Related, RelationType>::relation() noexcept
    {
        return static_cast<RelationType<Model, Related> &>(*this);
    }

} // namespace Orm::Tiny::Relations::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_CONCERNS_SUPPORTSDEFAULTMODELS_HPP
