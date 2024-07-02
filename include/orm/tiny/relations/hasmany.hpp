#pragma once
#ifndef ORM_TINY_RELATIONS_HASMANY_HPP
#define ORM_TINY_RELATIONS_HASMANY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/relations/hasoneormany.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{

    /*! Has many relation. */
    template<class Model, class Related>
    class HasMany : public IsManyRelation,
                    public HasOneOrMany<Model, Related>
    {
        Q_DISABLE_COPY_MOVE(HasMany)

    protected:
        /*! Protected constructor. */
        HasMany(std::unique_ptr<Related> &&related, Model &parent,
                const QString &foreignKey, const QString &localKey);

    public:
        /*! Parent Model type. */
        using ModelType = Model;
        /*! Related type. */
        using RelatedType = Related;

        /*! Virtual destructor. */
        ~HasMany() override = default;

        /*! Instantiate and initialize a new HasMany instance. */
        static std::unique_ptr<HasMany<Model, Related>>
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
        /*! Match the eagerly loaded results to their parents. */
        inline void
        match(ModelsCollection<Model *> &models, ModelsCollection<Related> &&results,
              const QString &relation) const override;

        /*! Get the results of the relationship. */
        std::variant<ModelsCollection<Related>, std::optional<Related>>
        getResults() const override;

        /* TinyBuilder proxy methods */
        /*! Find multiple models by their primary keys. */
        inline ModelsCollection<Related>
        findMany(const QList<QVariant> &ids,
                 const QList<Column> &columns = {ASTERISK}) const;

        /* Others */
        /*! The textual representation of the Relation type. */
        inline const QString &relationTypeName() const override;

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
    HasMany<Model, Related>::HasMany(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey
    )
        : HasOneOrMany<Model, Related>(std::move(related), parent, foreignKey, localKey)
    {}

    /* public */

    template<class Model, class Related>
    std::unique_ptr<HasMany<Model, Related>>
    HasMany<Model, Related>::instance(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey)
    {
        // Relations have private ctors, std::make_unique() can't be used
        auto instance =
                std::unique_ptr<HasMany<Model, Related>>(
                    new HasMany(std::move(related), parent, foreignKey, localKey));

        instance->init();

        return instance;
    }

    /* Relation related operations */

    template<class Model, class Related>
    ModelsCollection<Model> &
    HasMany<Model, Related>::initRelation(ModelsCollection<Model> &models,
                                          const QString &relation) const
    {
        return initRelationInternal(models, relation);
    }

    template<class Model, class Related>
    ModelsCollection<Model *> &
    HasMany<Model, Related>::initRelation(ModelsCollection<Model *> &models,
                                          const QString &relation) const
    {
        return initRelationInternal(models, relation);
    }

    template<class Model, class Related>
    void HasMany<Model, Related>::match(
            ModelsCollection<Model> &models, ModelsCollection<Related> &&results,
            const QString &relation) const
    {
        this->template matchOneOrMany<ModelsCollection<Related>>(
                    models, std::move(results), relation);
    }

    template<class Model, class Related>
    void HasMany<Model, Related>::match(
            ModelsCollection<Model *> &models, ModelsCollection<Related> &&results,
            const QString &relation) const
    {
        this->template matchOneOrMany<ModelsCollection<Related>>(
                    models, std::move(results), relation);
    }

    template<class Model, class Related>
    std::variant<ModelsCollection<Related>, std::optional<Related>>
    HasMany<Model, Related>::getResults() const
    {
        // If a Model doesn't contain primary key (eg. empty Model instance)
        if (const auto parentKey = this->getParentKey();
            !parentKey.isValid() || parentKey.isNull()
        )
            return ModelsCollection<Related>();

        return this->m_query->get();
    }

    /* TinyBuilder proxy methods */

    template<class Model, class Related>
    ModelsCollection<Related>
    HasMany<Model, Related>::findMany(
            const QList<QVariant> &ids, const QList<Column> &columns) const
    {
        return this->m_query->findMany(ids, columns);
    }

    /* Others */

    template<class Model, class Related>
    const QString &HasMany<Model, Related>::relationTypeName() const
    {
        static const auto cached = QStringLiteral("HasMany");
        return cached;
    }

    /* private */

    /* Relation related operations */

    template<class Model, class Related>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    ModelsCollection<CollectionModel> &
    HasMany<Model, Related>::initRelationInternal(
            ModelsCollection<CollectionModel> &models,
            const QString &relation) const
    {
        /*! Model type used in the for-ranged loops. */
        using ModelLoopType = typename ModelsCollection<CollectionModel>::ModelLoopType;

        for (ModelLoopType model : models)
            Relation<Model,Related>::toPointer(model)
                    ->template setRelation<Related>(relation,
                                                    ModelsCollection<Related>());

        return models;
    }

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_HASMANY_HPP
