#ifndef HASONEORMANY_H
#define HASONEORMANY_H

#include "orm/tiny/relations/relation.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{

    // TODO add create, createMany, save, findOrNew, firstOrNew, firstOrCreate and updateOrCreate silverqx
    template<class Model, class Related>
    class HasOneOrMany : public Relation<Model, Related>
    {
    protected:
        HasOneOrMany(std::unique_ptr<Related> &&related,
                     const Model &parent,
                     const QString &foreignKey, const QString &localKey);

    public:
        /*! Set the base constraints on the relation query. */
        void addConstraints() const override;

        /*! Set the constraints for an eager load of the relation. */
        void addEagerConstraints(const QVector<Model> &models) const override;

        /* Getters / Setters */
        /*! Get the key value of the parent's local key. */
        QVariant getParentKey() const;

        /* Inserting operations on relation */
        /*! Attach a model instance to the parent model. */
        std::tuple<bool, Related &> save(Related &model) const override;
        /*! Attach a model instance to the parent model. */
        std::tuple<bool, Related> save(Related &&model) const override;
        /*! Attach a collection of models to the parent instance. */
        QVector<Related> &saveMany(QVector<Related> &models) const override;
        /*! Attach a collection of models to the parent instance. */
        QVector<Related> saveMany(QVector<Related> &&models) const override;

        /*! Create a new instance of the related model. */
        Related create(const QVector<AttributeItem> &attributes = {}) const override;
        /*! Create a new instance of the related model. */
        Related create(QVector<AttributeItem> &&attributes = {}) const override;
        /*! Create a Collection of new instances of the related model. */
        QVector<Related>
        createMany(const QVector<QVector<AttributeItem>> &records) const override;
        /*! Create a Collection of new instances of the related model. */
        QVector<Related>
        createMany(QVector<QVector<AttributeItem>> &&records) const override;

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
        inline const QString &getQualifiedForeignKeyName() const
        { return m_foreignKey; }

        /* Inserting operations on relation */
        /*! Set the foreign ID for creating a related model. */
        void setForeignAttributesForCreate(Related &model) const;

        /* Much safer to make a copy here than save references, original objects get
           out of scope, because they are defined in member function blocks. */
        /*! The foreign key of the parent model. */
        QString m_foreignKey;
        /*! The local key of the parent model. */
        QString m_localKey;
        // TODO next use inline static (better constexpr?) when appropriate, check all the code silverqx
        /*! The count of self joins. */
        inline static int selfJoinCount = 0;
    };

    template<class Model, class Related>
    HasOneOrMany<Model, Related>::HasOneOrMany(
            std::unique_ptr<Related> &&related, const Model &parent,
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

        this->m_query->where(m_foreignKey, QStringLiteral("="), getParentKey());

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
    QVariant HasOneOrMany<Model, Related>::getParentKey() const
    {
        return this->m_parent.getAttribute(m_localKey);
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

        return {model.save(), model};
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

        return models;
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

        // BUG dictionary.insert how is possible that it moves if qhash doesn't have move insert, but the same code doesn't move in the belongsTo::buildDictionary, may be because of 'if constexpr'? silverqx
        for (auto &result : results)
            if constexpr (
                const auto &foreign = result.getAttribute(getForeignKeyName())
                                      .template value<typename Model::KeyType>();
                std::is_same_v<RelationType, QVector<Related>>
            )
                dictionary[foreign].append(std::move(result));
            else
                dictionary.insert(foreign, std::move(result));

        return dictionary;
    }

    template<class Model, class Related>
    QString HasOneOrMany<Model, Related>::getForeignKeyName() const
    {
        const auto segments = getQualifiedForeignKeyName().split(QChar('.'));

        return segments.last();
    }

    template<class Model, class Related>
    void
    HasOneOrMany<Model, Related>::setForeignAttributesForCreate(Related &model) const
    {
        model.setAttribute(getForeignKeyName(), getParentKey());
    }

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // HASONEORMANY_H
