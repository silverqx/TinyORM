#pragma once
#ifndef ORM_TINY_RELATIONS_HASONEORMANY_HPP
#define ORM_TINY_RELATIONS_HASONEORMANY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/relations/relation.hpp"
#include "orm/tiny/utils/attribute.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{

    /*! Has one/many relation base class. */
    template<class Model, class Related>
    class HasOneOrMany : public Relation<Model, Related>
    {
        Q_DISABLE_COPY_MOVE(HasOneOrMany)

        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;

    protected:
        /*! Protected constructor. */
        HasOneOrMany(std::unique_ptr<Related> &&related, Model &parent,
                     const QString &foreignKey, const QString &localKey); // NOLINT(modernize-pass-by-value)

    public:
        /*! Parent Model type. */
        using ModelType = Model;
        /*! Related type. */
        using RelatedType = Related;

        /*! Pure virtual destructor. */
        inline ~HasOneOrMany() override = 0;

        /* Relation related operations */
        /*! Set the base constraints on the relation query. */
        void addConstraints() const override;

        /*! Set the constraints for an eager load of the relation. */
        inline void addEagerConstraints(const ModelsCollection<Model> &models) override;
        /*! Set the constraints for an eager load of the relation. */
        inline void addEagerConstraints(const ModelsCollection<Model *> &models) override;

        /* Getters / Setters */
        /*! Get the key value of the parent's local key. */
        inline QVariant getParentKey() const;
        /*! Get the fully qualified parent key name. */
        inline QString getQualifiedParentKeyName() const override;
        /*! Get the key for comparing against the parent key in "has" query. */
        inline QString getExistenceCompareKey() const override;

        /* TinyBuilder proxy methods that need modifications */
        /*! Find a model by its primary key or return a new instance of the related
            model. */
        Related findOrNew(const QVariant &id, // NOLINT(google-default-arguments)
                          const QList<Column> &columns = {ASTERISK}) const override;

        /*! Get the first related model record matching the attributes or instantiate
            it. */
        Related firstOrNew(const QList<WhereItem> &attributes = {}, // NOLINT(google-default-arguments)
                           const QList<AttributeItem> &values = {}) const override;
        /*! Get the first related record matching the attributes or create it. */
        Related firstOrCreate(const QList<WhereItem> &attributes = {},
                              const QList<AttributeItem> &values = {}) const;

        /*! Create or update a related record matching the attributes, and fill it
            with values. */
        Related updateOrCreate(const QList<WhereItem> &attributes,
                               const QList<AttributeItem> &values = {}) const;

        /* Inserting operations on the relationship */
        /*! Attach a model instance to the parent model. */
        std::tuple<bool, Related &> save(Related &model) const;
        /*! Attach a model instance to the parent model. */
        std::tuple<bool, Related> save(Related &&model) const;
        /*! Attach a vector of models to the parent instance. */
        ModelsCollection<Related> &saveMany(ModelsCollection<Related> &models) const;
        /*! Attach a vector of models to the parent instance. */
        ModelsCollection<Related> saveMany(ModelsCollection<Related> &&models) const;

        /*! Create a new instance of the related model. */
        Related create(const QList<AttributeItem> &attributes = {}) const;
        /*! Create a new instance of the related model. */
        Related create(QList<AttributeItem> &&attributes = {}) const;
        /*! Create a vector of new instances of the related model. */
        ModelsCollection<Related>
        createMany(const QList<QList<AttributeItem>> &records) const;
        /*! Create a vector of new instances of the related model. */
        ModelsCollection<Related>
        createMany(QList<QList<AttributeItem>> &&records) const; // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)

    protected:
        /* Relation related operations */
        /*! Match the eagerly loaded results to their many parents. */
        template<typename RelationType, SameDerivedCollectionModel<Model> CollectionModel>
        void matchOneOrMany(
                ModelsCollection<CollectionModel> &models,
                ModelsCollection<Related> &&results, const QString &relation) const;

        /*! Build model dictionary keyed by the relation's foreign key. */
        template<typename RelationType>
        QHash<typename Model::KeyType, RelationType>
        buildDictionary(ModelsCollection<Related> &&results) const; // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)

        /* Getters / Setters */
        /*! Get the plain foreign key. */
        QString getForeignKeyName() const;
        /*! Get the foreign key for the relationship. */
        inline const QString &getQualifiedForeignKeyName() const;

        /* Inserting operations on the relationship */
        /*! Set the foreign ID for creating a related model. */
        void setForeignAttributesForCreate(Related &model) const;

        /* Querying Relationship Existence/Absence */
        /*! Add the constraints for a relationship query. */
        std::unique_ptr<Builder<Related>>
        getRelationExistenceQuery( // NOLINT(google-default-arguments)
                std::unique_ptr<Builder<Related>> &&query,
                const Builder<Model> &parentQuery,
                const QList<Column> &columns = {ASTERISK}) const override;

        /* Much safer to make a copy here than save references, original objects get
           out of scope, because they are defined in member function blocks. */
        /*! The foreign key of the parent model. */
        QString m_foreignKey;
        /*! The local key of the parent model. */
        QString m_localKey;
        /*! The count of self joins. */
        T_THREAD_LOCAL
        inline static int selfJoinCount = 0;

    private:
        /* Relation related operations */
        /*! Set the constraints for an eager load of the relation, common code. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        void addEagerConstraintsInternal(const ModelsCollection<CollectionModel> &models);
    };

    /* protected */

    template<class Model, class Related>
    HasOneOrMany<Model, Related>::HasOneOrMany(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey // NOLINT(modernize-pass-by-value)
    )
        : Relation<Model, Related>(std::move(related), parent)
        , m_foreignKey(foreignKey)
        , m_localKey(localKey)
    {}

    /* public */

    template<class Model, class Related>
    HasOneOrMany<Model, Related>::~HasOneOrMany() = default;

    /* Relation related operations */

    template<class Model, class Related>
    void HasOneOrMany<Model, Related>::addConstraints() const
    {
        if (!this->constraints)
            return;

        this->m_query->where(m_foreignKey, EQ, getParentKey());

        this->m_query->whereNotNull(m_foreignKey);
    }

    template<class Model, class Related>
    void HasOneOrMany<Model, Related>::addEagerConstraints(
            const ModelsCollection<Model> &models)
    {
        addEagerConstraintsInternal(models);
    }

    template<class Model, class Related>
    void HasOneOrMany<Model, Related>::addEagerConstraints(
            const ModelsCollection<Model *> &models)
    {
        addEagerConstraintsInternal(models);
    }

    /* Getters / Setters */

    template<class Model, class Related>
    QVariant HasOneOrMany<Model, Related>::getParentKey() const
    {
        return this->m_parent->getAttribute(m_localKey);
    }

    template<class Model, class Related>
    QString HasOneOrMany<Model, Related>::getQualifiedParentKeyName() const
    {
        return this->m_parent->qualifyColumn(m_localKey);
    }

    template<class Model, class Related>
    QString HasOneOrMany<Model, Related>::getExistenceCompareKey() const
    {
        return getQualifiedForeignKeyName();
    }

    /* TinyBuilder proxy methods that need modifications */

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::findOrNew(const QVariant &id, // NOLINT(google-default-arguments)
                                                    const QList<Column> &columns) const
    {
        // Found
        if (auto instance = this->find(id, columns); instance)
            return std::move(*instance);

        auto newInstance = this->m_related->newInstance();

        setForeignAttributesForCreate(newInstance);

        return newInstance;
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::firstOrNew( // NOLINT(google-default-arguments)
            const QList<WhereItem> &attributes,
            const QList<AttributeItem> &values) const
    {
        // Model found in db
        if (auto instance = this->where(attributes).first(); instance)
            return std::move(*instance);

        auto newInstance =
                this->m_related->newInstance(
                    AttributeUtils::joinAttributesForFirstOr(
                        attributes, values, this->m_relatedKey));

        setForeignAttributesForCreate(newInstance);

        return newInstance;
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::firstOrCreate(
            const QList<WhereItem> &attributes,
            const QList<AttributeItem> &values) const
    {
        auto instance = this->where(attributes).first();

        // Model found in db
        if (instance)
            return std::move(*instance);

        return create(AttributeUtils::joinAttributesForFirstOr(
                          attributes, values, this->m_relatedKey));
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::updateOrCreate(
            const QList<WhereItem> &attributes,
            const QList<AttributeItem> &values) const
    {
        auto instance = firstOrNew(attributes);

        instance.fill(values).save();

        return instance;

    }

    /* Inserting operations on the relationship */

    // NOTE api different silverqx
    template<class Model, class Related>
    std::tuple<bool, Related &>
    HasOneOrMany<Model, Related>::save(Related &model) const
    {
        setForeignAttributesForCreate(model);

        return {model.save(), model};
    }

    template<class Model, class Related>
    std::tuple<bool, Related>
    HasOneOrMany<Model, Related>::save(Related &&model) const
    {
        setForeignAttributesForCreate(model);

        return {model.save(), std::move(model)};
    }

    template<class Model, class Related>
    ModelsCollection<Related> &
    HasOneOrMany<Model, Related>::saveMany(ModelsCollection<Related> &models) const
    {
        for (auto &model : models)
            save(model);

        return models;
    }

    template<class Model, class Related>
    ModelsCollection<Related>
    HasOneOrMany<Model, Related>::saveMany(ModelsCollection<Related> &&models) const
    {
        for (auto &model : models)
            save(model);

        return std::move(models);
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::create(
            const QList<AttributeItem> &attributes) const
    {
        auto instance = this->m_related->newInstance(attributes);

        setForeignAttributesForCreate(instance);

        instance.save();

        return instance;
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::create(
            QList<AttributeItem> &&attributes) const
    {
        auto instance = this->m_related->newInstance(std::move(attributes));

        setForeignAttributesForCreate(instance);

        instance.save();

        return instance;
    }

    template<class Model, class Related>
    ModelsCollection<Related>
    HasOneOrMany<Model, Related>::createMany(
            const QList<QList<AttributeItem>> &records) const
    {
        ModelsCollection<Related> instances;
        instances.reserve(records.size());

        for (const auto &record : records)
            instances << create(record);

        return instances;
    }

    template<class Model, class Related>
    ModelsCollection<Related>
    HasOneOrMany<Model, Related>::createMany(
            QList<QList<AttributeItem>> &&records) const // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        ModelsCollection<Related> instances;
        instances.reserve(records.size());

        for (auto &&record : records)
            instances << create(std::move(record));

        return instances;
    }

    /* protected */

    /* Relation related operations */

    template<class Model, class Related>
    template<typename RelationType, SameDerivedCollectionModel<Model> CollectionModel>
    void HasOneOrMany<Model, Related>::matchOneOrMany(
            ModelsCollection<CollectionModel> &models,
            ModelsCollection<Related> &&results, const QString &relation) const
    {
        auto dictionary = buildDictionary<RelationType>(std::move(results));

        /*! Model type used in the for-ranged loops. */
        using ModelLoopType = typename ModelsCollection<CollectionModel>::ModelLoopType;

        /* Once we have the dictionary we can simply spin through the parent models to
           link them up with their children using the keyed dictionary to make the
           matching very convenient and easy work. Then we'll just return them. */
        for (ModelLoopType model : models) {
            auto *const modelPointer = Relation<Model,Related>::toPointer(model);

            if (const auto parentKey = modelPointer->getAttribute(m_localKey)
                                       .template value<typename Model::KeyType>();
                dictionary.contains(parentKey)
            )
                /* HasMany - is safe to move as the hash is keyed by IDs so every ID
                             has its own related models prepared.
                   HasOne  - is also safe to move as the hash is keyed by IDs so every ID
                             has its own related model prepared and this related model
                             will not be assigned to any other parent model. */
                modelPointer->setRelation(relation,
                                          std::move(dictionary.find(parentKey).value()));
        }
    }

    template<class Model, class Related>
    template<typename RelationType>
    QHash<typename Model::KeyType, RelationType>
    HasOneOrMany<Model, Related>::buildDictionary(
            ModelsCollection<Related> &&results) const // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        QHash<typename Model::KeyType, RelationType> dictionary;
        dictionary.reserve(results.size());

        for (auto &&result : results)
            if constexpr (
                const auto foreignKey = result.getAttribute(getForeignKeyName())
                                        .template value<typename Model::KeyType>();
                std::is_same_v<RelationType, ModelsCollection<Related>>
            )
                dictionary[foreignKey] << std::move(result);

            else
                // Move to the std::optional
                dictionary.emplace(foreignKey, std::move(result));

        return dictionary;
    }

    /* Getters / Setters */

    template<class Model, class Related>
    QString HasOneOrMany<Model, Related>::getForeignKeyName() const
    {
        auto segments = getQualifiedForeignKeyName().split(DOT);

        return std::move(segments.last());
    }

    template<class Model, class Related>
    const QString &
    HasOneOrMany<Model, Related>::getQualifiedForeignKeyName() const
    {
        /* Foreign key is already qualified, it is done in Model::hasMany/hasOne() and
           will be qulified even if a user pass unqualified foreign column name. */
        return m_foreignKey;
    }

    /* Inserting operations on the relationship */

    template<class Model, class Related>
    void
    HasOneOrMany<Model, Related>::setForeignAttributesForCreate(Related &model) const
    {
        model.setAttribute(getForeignKeyName(), getParentKey());
    }

    /* Querying Relationship Existence/Absence */

    template<class Model, class Related>
    std::unique_ptr<Builder<Related>>
    HasOneOrMany<Model, Related>::getRelationExistenceQuery( // NOLINT(google-default-arguments)
            std::unique_ptr<Builder<Related>> &&query,
            const Builder<Model> &parentQuery,
            const QList<Column> &columns) const
    {
        // CUR1 finish self query silverqx
//        if (query->getQuery()->from == parentQuery.getQuery()->from)
//            return this->getRelationExistenceQueryForSelfRelation(query, parentQuery,
//                                                                  columns);

        return Relation<Model, Related>::getRelationExistenceQuery(
                    std::move(query), parentQuery, columns);
    }

    /* private */

    /* Relation related operations */

    template<class Model, class Related>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    void HasOneOrMany<Model, Related>::addEagerConstraintsInternal(
            const ModelsCollection<CollectionModel> &models)
    {
        this->whereInEager(m_foreignKey, this->getKeys(models, m_localKey));
    }

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_HASONEORMANY_HPP
