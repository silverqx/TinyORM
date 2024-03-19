#pragma once
#ifndef ORM_TINY_RELATIONS_CONCERNS_COMPARESRELATEDMODELS_HPP
#define ORM_TINY_RELATIONS_CONCERNS_COMPARESRELATEDMODELS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/tiny/tinyconcepts.hpp" // IWYU pragma: keep

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model;

namespace Relations::Concerns
{

    /*! Compares the related instance of the relationship. */
    template<class Model, class Related, template<class, class> class RelationType>
    class ComparesRelatedModels
    {
        Q_DISABLE_COPY_MOVE(ComparesRelatedModels)

        /*! Model alias, helps to avoid conflict with the Model template parameter. */
        template<typename Derived>
        using BaseModel = Orm::Tiny::Model<Derived>;

    public:
        /*! Alias for the parent model's key type (for shorter name). */
        using ParentKeyType = typename BaseModel<Model>::KeyType;
        /*! Alias for the related model's key type (for shorter name). */
        using RelatedKeyType = typename BaseModel<Related>::KeyType;

        /*! Default constructor. */
        ComparesRelatedModels() = default;
        /*! Pure virtual destructor. */
        inline virtual ~ComparesRelatedModels() = 0;

        /*! Determine if the model is the related instance of the relationship. */
        template<ModelConcept ModelToCompare>
        bool is(const std::optional<ModelToCompare> &model) const;
        /*! Determine if the model is not the related instance of the relationship. */
        template<ModelConcept ModelToCompare>
        inline bool isNot(const std::optional<ModelToCompare> &model) const;

    private:
        /*! Compare the parent key with the related key. */
        bool compareKeys(const QVariant &parentKey, const QVariant &relatedKey) const;

        /*! Static cast *this to the HasOne/BelongsTo & derived type, const version. */
        inline const RelationType<Model, Related> &relation() const noexcept;
    };

    /* public */

    template<class Model, class Related, template<class, class> class RelationType>
    ComparesRelatedModels<Model, Related, RelationType>::
    ~ComparesRelatedModels() = default;

    template<class Model, class Related, template<class, class> class RelationType>
    template<ModelConcept ModelToCompare>
    bool ComparesRelatedModels<Model, Related, RelationType>::is(
            const std::optional<ModelToCompare> &model) const
    {
        const auto &related = relation().getRelated();

        return model &&
               // First compare the same table (same model type)
               related.getTable() == model->getTable() &&
               compareKeys(relation().getParentKey(),
                           relation().getRelatedKeyFrom(*model)) &&
               related.getConnectionName() == model->getConnectionName();
    }

    template<class Model, class Related, template<class, class> class RelationType>
    template<ModelConcept ModelToCompare>
    bool ComparesRelatedModels<Model, Related, RelationType>::isNot(
            const std::optional<ModelToCompare> &model) const
    {
        return !is(model);
    }

    /* private */

    template<class Model, class Related, template<class, class> class RelationType>
    bool ComparesRelatedModels<Model, Related, RelationType>::compareKeys(
            const QVariant &parentKey, const QVariant &relatedKey) const
    {
        if (!parentKey.isValid() || !relatedKey.isValid() ||
            !parentKey.canConvert<ParentKeyType>() ||
            !relatedKey.canConvert<RelatedKeyType>()
        )
            return false;

        return parentKey.template value<ParentKeyType>() ==
               relatedKey.template value<RelatedKeyType>();
    }

    template<class Model, class Related, template<class, class> class RelationType>
    const RelationType<Model, Related> &
    ComparesRelatedModels<Model, Related, RelationType>::relation() const noexcept
    {
        return static_cast<const RelationType<Model, Related> &>(*this);
    }

} // namespace Relations::Concerns
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_CONCERNS_COMPARESRELATEDMODELS_HPP
