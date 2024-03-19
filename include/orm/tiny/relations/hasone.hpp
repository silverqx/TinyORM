#pragma once
#ifndef ORM_TINY_RELATIONS_HASONE_HPP
#define ORM_TINY_RELATIONS_HASONE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/relations/concerns/comparesrelatedmodels.hpp"
#include "orm/tiny/relations/concerns/supportsdefaultmodels.hpp"
#include "orm/tiny/relations/hasoneormany.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{

    /*! Has one relation. */
    template<class Model, class Related>
    class HasOne : public IsOneRelation,
                   public HasOneOrMany<Model, Related>,
                   public Concerns::SupportsDefaultModels<Model, Related, HasOne>,
                   public Concerns::ComparesRelatedModels<Model, Related, HasOne>
    {
        Q_DISABLE_COPY_MOVE(HasOne)

        // To access getRelatedKeyFrom()
        friend Concerns::ComparesRelatedModels<Model, Related, HasOne>;

    protected:
        /*! Protected constructor. */
        HasOne(std::unique_ptr<Related> &&related, Model &parent,
               const QString &foreignKey, const QString &localKey);

    public:
        /*! Parent Model type. */
        using ModelType = Model;
        /*! Related type. */
        using RelatedType = Related;

        /*! Virtual destructor. */
        ~HasOne() override = default;

        /*! Instantiate and initialize a new HasOne instance. */
        static std::unique_ptr<HasOne<Model, Related>>
        instance(std::unique_ptr<Related> &&related, Model &parent,
                 const QString &foreignKey, const QString &localKey);

        /* Relation related operations */
        /*! Initialize the relation on a set of models. */
        inline ModelsCollection<Model> &
        initRelation(ModelsCollection<Model> &models,
                     const QString &relation) const override;
        /*! Initialize the relation on a set of models. */
        inline ModelsCollection<Model *> &
        initRelation(ModelsCollection<Model *> &models,
                     const QString &relation) const override;

        /*! Match the eagerly loaded results to their parents. */
        inline void
        match(ModelsCollection<Model> &models, ModelsCollection<Related> &&results,
              const QString &relation) const override;
        inline void
        match(ModelsCollection<Model *> &models, ModelsCollection<Related> &&results,
              const QString &relation) const override;

        /*! Get the results of the relationship. */
        std::variant<ModelsCollection<Related>, std::optional<Related>>
        getResults() const override;

        /* Others */
        /*! The textual representation of the Relation type. */
        inline const QString &relationTypeName() const override;

    protected:
        /* Relation related operations */
        /*! Make a new related instance for the given model. */
        inline Related newRelatedInstanceFor(const Model &/*unused*/) const override;

        /* Getters */
        /*! Get the value of the model's foreign key. */
        template<ModelConcept M>
        QVariant getRelatedKeyFrom(const M &model) const;

    private:
        /* Relation related operations */
        /*! Initialize the relation on a set of models, common code. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        ModelsCollection<CollectionModel> &
        initRelationInternal(ModelsCollection<CollectionModel> &models,
                             const QString &relation) const;
    };

    /* protected */

    template<class Model, class Related>
    HasOne<Model, Related>::HasOne(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey
    )
        : HasOneOrMany<Model, Related>(std::move(related), parent, foreignKey, localKey)
    {}

    /* public */

    template<class Model, class Related>
    std::unique_ptr<HasOne<Model, Related>>
    HasOne<Model, Related>::instance(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey)
    {
        // Relations have private ctors, std::make_unique() can't be used
        auto instance =
                std::unique_ptr<HasOne<Model, Related>>(
                    new HasOne(std::move(related), parent, foreignKey, localKey));

        instance->init();

        return instance;
    }

    /* Relation related operations */

    template<class Model, class Related>
    ModelsCollection<Model> &
    HasOne<Model, Related>::initRelation(ModelsCollection<Model> &models,
                                         const QString &relation) const
    {
        return initRelationInternal(models, relation);
    }

    template<class Model, class Related>
    ModelsCollection<Model *> &
    HasOne<Model, Related>::initRelation(ModelsCollection<Model *> &models,
                                         const QString &relation) const
    {
        return initRelationInternal(models, relation);
    }

    template<class Model, class Related>
    void HasOne<Model, Related>::match(
            ModelsCollection<Model> &models, ModelsCollection<Related> &&results,
            const QString &relation) const
    {
        this->template matchOneOrMany<std::optional<Related>>(models, std::move(results),
                                                              relation);
    }

    template<class Model, class Related>
    void HasOne<Model, Related>::match(
            ModelsCollection<Model *> &models, ModelsCollection<Related> &&results,
            const QString &relation) const
    {
        this->template matchOneOrMany<std::optional<Related>>(models, std::move(results),
                                                              relation);
    }

    template<class Model, class Related>
    std::variant<ModelsCollection<Related>, std::optional<Related>>
    HasOne<Model, Related>::getResults() const
    {
        // If a Model doesn't contain primary key (eg. empty Model instance)
        if (const auto parentKey = this->getParentKey();
            !parentKey.isValid() || parentKey.isNull()
        )
            return this->getDefaultFor(*this->m_parent);

        // NRVO doesn't kick in so I have to move
        auto first = this->m_query->first();

        return first ? std::move(first) : this->getDefaultFor(*this->m_parent);
    }

    /* Others */

    template<class Model, class Related>
    const QString &HasOne<Model, Related>::relationTypeName() const
    {
        static const auto cached = QStringLiteral("HasOne");
        return cached;
    }

    /* protected */

    /* Relation related operations */

    template<class Model, class Related>
    Related
    HasOne<Model, Related>::newRelatedInstanceFor(const Model &parent) const
    {
        return this->m_related->newInstance().setAttribute(
                    this->getForeignKeyName(),
                    parent.getAttribute(this->m_localKey));
    }

    /* Getters */

    template<class Model, class Related>
    template<ModelConcept M>
    QVariant HasOne<Model, Related>::getRelatedKeyFrom(const M &model) const
    {
        return model.getAttribute(this->getForeignKeyName());
    }

    /* private */

    /* Relation related operations */

    template<class Model, class Related>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    ModelsCollection<CollectionModel> &
    HasOne<Model, Related>::initRelationInternal(
            ModelsCollection<CollectionModel> &models,
            const QString &relation) const
    {
        /*! Model type used in the for-ranged loops. */
        using ModelLoopType = typename ModelsCollection<CollectionModel>::ModelLoopType;

        for (ModelLoopType model : models)
            Relation<Model,Related>::toPointer(model)
                    ->template setRelation<Related>(relation,
                                                    this->getDefaultFor(model));

        return models;
    }

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_HASONE_HPP
