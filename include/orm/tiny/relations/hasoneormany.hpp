#pragma once
#ifndef HASONEORMANY_H
#define HASONEORMANY_H

#include "orm/tiny/relations/relation.hpp"
#include "orm/utils/attribute.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{

    /*! Has one/many relation base class. */
    template<class Model, class Related>
    class HasOneOrMany : public Relation<Model, Related>
    {
    protected:
        /*! Protected constructor. */
        HasOneOrMany(std::unique_ptr<Related> &&related, Model &parent,
                     const QString &foreignKey, const QString &localKey);

    public:
        /*! Set the base constraints on the relation query. */
        void addConstraints() const override;

        /*! Set the constraints for an eager load of the relation. */
        void addEagerConstraints(const QVector<Model> &models) const override;

        /* Getters / Setters */
        /*! Get the key value of the parent's local key. */
        QVariant getParentKey() const;
        /*! Get the fully qualified parent key name. */
        QString getQualifiedParentKeyName() const override;
        /*! Get the key for comparing against the parent key in "has" query. */
        QString getExistenceCompareKey() const override;

        /* TinyBuilder proxy methods */
        /*! Find a model by its primary key or return a new instance of the related
            model. */
        Related findOrNew(const QVariant &id,
                          const QVector<Column> &columns = {ASTERISK}) const override;

        /*! Get the first related model record matching the attributes or instantiate
            it. */
        Related firstOrNew(const QVector<WhereItem> &attributes = {},
                           const QVector<AttributeItem> &values = {}) const override;
        /*! Get the first related record matching the attributes or create it. */
        Related firstOrCreate(const QVector<WhereItem> &attributes = {},
                              const QVector<AttributeItem> &values = {}) const;

        /*! Create or update a related record matching the attributes, and fill it
            with values. */
        Related updateOrCreate(const QVector<WhereItem> &attributes,
                               const QVector<AttributeItem> &values = {}) const;

        /* Inserting operations on the relationship */
        /*! Attach a model instance to the parent model. */
        std::tuple<bool, Related &> save(Related &model) const;
        /*! Attach a model instance to the parent model. */
        std::tuple<bool, Related> save(Related &&model) const;
        /*! Attach a vector of models to the parent instance. */
        QVector<Related> &saveMany(QVector<Related> &models) const;
        /*! Attach a vector of models to the parent instance. */
        QVector<Related> saveMany(QVector<Related> &&models) const;

        /*! Create a new instance of the related model. */
        Related create(const QVector<AttributeItem> &attributes = {}) const;
        /*! Create a new instance of the related model. */
        Related create(QVector<AttributeItem> &&attributes = {}) const;
        /*! Create a vector of new instances of the related model. */
        QVector<Related>
        createMany(const QVector<QVector<AttributeItem>> &records) const;
        /*! Create a vector of new instances of the related model. */
        QVector<Related>
        createMany(QVector<QVector<AttributeItem>> &&records) const;

    protected:
        /*! Match the eagerly loaded results to their many parents. */
        template<typename RelationType>
        void matchOneOrMany(QVector<Model> &models, QVector<Related> &results,
                            const QString &relation) const;
        /*! Build model dictionary keyed by the relation's foreign key. */
        template<typename RelationType>
        QHash<typename Model::KeyType, RelationType>
        buildDictionary(QVector<Related> &results) const;

        /* Getters / Setters */
        /*! Get the plain foreign key. */
        QString getForeignKeyName() const;
        /*! Get the foreign key for the relationship. */
        const QString &getQualifiedForeignKeyName() const;

        /* Inserting operations on the relationship */
        /*! Set the foreign ID for creating a related model. */
        void setForeignAttributesForCreate(Related &model) const;

        /* Querying Relationship Existence/Absence */
        /*! Add the constraints for a relationship query. */
        std::unique_ptr<Builder<Related>>
        getRelationExistenceQuery(
                std::unique_ptr<Builder<Related>> &&query,
                const Builder<Model> &parentQuery,
                const QVector<Column> &columns = {ASTERISK}) const override;

        /* Much safer to make a copy here than save references, original objects get
           out of scope, because they are defined in member function blocks. */
        /*! The foreign key of the parent model. */
        QString m_foreignKey;
        /*! The local key of the parent model. */
        QString m_localKey;
        /*! The count of self joins. */
        inline static int selfJoinCount = 0;
    };

    template<class Model, class Related>
    HasOneOrMany<Model, Related>::HasOneOrMany(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &foreignKey, const QString &localKey
    )
        : Relation<Model, Related>(std::move(related), parent)
        , m_foreignKey(foreignKey)
        , m_localKey(localKey)
    {}

    template<class Model, class Related>
    void HasOneOrMany<Model, Related>::addConstraints() const
    {
        if (!this->constraints)
            return;

        this->m_query->where(m_foreignKey, EQ, getParentKey());

        this->m_query->whereNotNull(m_foreignKey);
    }

    template<class Model, class Related>
    void
    HasOneOrMany<Model, Related>::addEagerConstraints(const QVector<Model> &models) const
    {
        this->m_query->getQuery().whereIn(m_foreignKey,
                                          this->getKeys(models, m_localKey));
    }

    template<class Model, class Related>
    inline QVariant HasOneOrMany<Model, Related>::getParentKey() const
    {
        return this->m_parent.getAttribute(m_localKey);
    }

    template<class Model, class Related>
    inline QString HasOneOrMany<Model, Related>::getQualifiedParentKeyName() const
    {
        return this->m_parent.qualifyColumn(m_localKey);
    }

    template<class Model, class Related>
    inline QString HasOneOrMany<Model, Related>::getExistenceCompareKey() const
    {
        return getQualifiedForeignKeyName();
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::findOrNew(const QVariant &id,
                                                    const QVector<Column> &columns) const
    {
        // Found
        if (auto instance = this->find(id, columns); instance)
            return *instance;

        auto newInstance = this->m_related->newInstance();

        setForeignAttributesForCreate(newInstance);

        return newInstance;
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::firstOrNew(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values) const
    {
        // Model found in db
        if (auto instance = this->where(attributes).first(); instance)
            return *instance;

        auto newInstance =
                this->m_related->newInstance(
                    Utils::Attribute::joinAttributesForFirstOr(attributes, values,
                                                               this->m_relatedKey));

        setForeignAttributesForCreate(newInstance);

        return newInstance;
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::firstOrCreate(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values) const
    {
        auto instance = this->where(attributes).first();

        // Model found in db
        if (instance)
            return *instance;

        return create(Utils::Attribute::joinAttributesForFirstOr(attributes, values,
                                                                 this->m_relatedKey));
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::updateOrCreate(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values) const
    {
        auto instance = firstOrNew(attributes);

        instance.fill(values).save();

        return instance;

    }

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
    QVector<Related> &
    HasOneOrMany<Model, Related>::saveMany(QVector<Related> &models) const
    {
        for (auto &model : models)
            save(model);

        return models;
    }

    template<class Model, class Related>
    QVector<Related>
    HasOneOrMany<Model, Related>::saveMany(QVector<Related> &&models) const
    {
        for (auto &model : models)
            save(model);

        return std::move(models);
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::create(
            const QVector<AttributeItem> &attributes) const
    {
        auto instance = this->m_related->newInstance(attributes);

        setForeignAttributesForCreate(instance);

        instance.save();

        return instance;
    }

    template<class Model, class Related>
    Related HasOneOrMany<Model, Related>::create(
            QVector<AttributeItem> &&attributes) const
    {
        auto instance = this->m_related->newInstance(std::move(attributes));

        setForeignAttributesForCreate(instance);

        instance.save();

        return instance;
    }

    template<class Model, class Related>
    QVector<Related>
    HasOneOrMany<Model, Related>::createMany(
            const QVector<QVector<AttributeItem>> &records) const
    {
        QVector<Related> instances;

        for (auto &record : records)
            instances << create(record);

        return instances;
    }

    template<class Model, class Related>
    QVector<Related>
    HasOneOrMany<Model, Related>::createMany(
            QVector<QVector<AttributeItem>> &&records) const
    {
        QVector<Related> instances;

        for (auto &&record : records)
            instances << create(std::move(record));

        return instances;
    }

    template<class Model, class Related>
    template<typename RelationType>
    void HasOneOrMany<Model, Related>::matchOneOrMany(
            QVector<Model> &models, QVector<Related> &results,
            const QString &relation) const
    {
        auto dictionary = buildDictionary<RelationType>(results);

        /* Once we have the dictionary we can simply spin through the parent models to
           link them up with their children using the keyed dictionary to make the
           matching very convenient and easy work. Then we'll just return them. */
        for (auto &model : models) {
            if (const auto &key = model.getAttribute(m_localKey)
                .template value<typename Model::KeyType>();

                dictionary.contains(key)
            )
                model.setRelation(relation,
                                  std::move(dictionary.find(key).value()));
        }
    }

    template<class Model, class Related>
    template<typename RelationType>
    QHash<typename Model::KeyType, RelationType>
    HasOneOrMany<Model, Related>::buildDictionary(QVector<Related> &results) const
    {
        QHash<typename Model::KeyType, RelationType> dictionary;

        for (auto &result : results)
            if constexpr (
                const auto &foreign = result.getAttribute(getForeignKeyName())
                                      .template value<typename Model::KeyType>();
                std::is_same_v<RelationType, QVector<Related>>
            )
                dictionary[foreign].append(std::move(result));
            else
                // Moves to the std::optional
                dictionary.insert(foreign, std::move(result));

        return dictionary;
    }

    template<class Model, class Related>
    QString HasOneOrMany<Model, Related>::getForeignKeyName() const
    {
        const auto segments = getQualifiedForeignKeyName().split(DOT);

        return segments.last();
    }

    template<class Model, class Related>
    inline const QString &
    HasOneOrMany<Model, Related>::getQualifiedForeignKeyName() const
    {
        /* Foreign key is already qualified, it is done in Model::hasMany/hasOne() and
           will be qulified even if a user pass unqualified foreign column name. */
        return m_foreignKey;
    }

    template<class Model, class Related>
    void
    HasOneOrMany<Model, Related>::setForeignAttributesForCreate(Related &model) const
    {
        model.setAttribute(getForeignKeyName(), getParentKey());
    }

    template<class Model, class Related>
    std::unique_ptr<Builder<Related>>
    HasOneOrMany<Model, Related>::getRelationExistenceQuery(
            std::unique_ptr<Builder<Related>> &&query,
            const Builder<Model> &parentQuery,
            const QVector<Column> &columns) const
    {
        // CUR finish self query silverqx
//        if (query->getQuery()->from == parentQuery.getQuery()->from)
//            return this->getRelationExistenceQueryForSelfRelation(query, parentQuery,
//                                                                  columns);

        return Relation<Model, Related>::getRelationExistenceQuery(
                    std::move(query), parentQuery, columns);
    }

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // HASONEORMANY_H
