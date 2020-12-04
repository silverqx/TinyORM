#ifndef HASONEORMANY_H
#define HASONEORMANY_H

#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>

#include "orm/tiny/relations/relation.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{

    template<class Model, class Related>
    class HasOneOrMany : public Relation<Model, Related>
    {
    public:
        HasOneOrMany(std::unique_ptr<Builder<Related>> &&query,
                     const Model &parent,
                     const QString &foreignKey, const QString &localKey);

        /*! Set the base constraints on the relation query. */
        void addConstraints() const override;

        /*! Set the constraints for an eager load of the relation. */
        void addEagerConstraints(const QVector<Model> &models) const override;
        /*! Get the key value of the parent's local key. */
        QVariant getParentKey() const
        { return this->m_parent.getAttribute(m_localKey); }

    protected:
        /*! Get all of the primary keys for an array of models. */
        QVector<QVariant>
        getKeys(const QVector<Model> &models, const QString &key = "") const;
        /*! Match the eagerly loaded results to their many parents. */
        template<typename RelationValue>
        void matchOneOrMany(QVector<Model> &models, QVector<Related> &results,
                            const QString &relation) const;
        /*! Build model dictionary keyed by the relation's foreign key. */
        template<typename HashValue>
        QHash<typename Model::KeyType, HashValue>
        buildDictionary(QVector<Related> &results) const;
        /*! Get the plain foreign key. */
        QString getForeignKeyName() const;
        /*! Get the foreign key for the relationship. */
        inline const QString &getQualifiedForeignKeyName() const
        { return m_foreignKey; }

        // WARNING don't forget to make them references silverqx
        /*! The foreign key of the parent model. */
        QString m_foreignKey;
        /*! The local key of the parent model. */
        QString m_localKey;
        // TODO next use inline static (better constexpr?) when appropriate, check all the code silverqx
        /*! The count of self joins. */
        constexpr static int selfJoinCount = 0;
    };

    template<class Model, class Related>
    HasOneOrMany<Model, Related>::HasOneOrMany(std::unique_ptr<Builder<Related>> &&query,
                                               const Model &parent,
                                               const QString &foreignKey, const QString &localKey)
        : Relation<Model, Related>(std::move(query), parent)
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
    void HasOneOrMany<Model, Related>::addEagerConstraints(const QVector<Model> &models) const
    {
        this->m_query->getQuery().whereIn(m_foreignKey, getKeys(models, m_localKey));
    }

    template<class Model, class Related>
    QVector<QVariant>
    HasOneOrMany<Model, Related>::getKeys(const QVector<Model> &models, const QString &key) const
    {
        QVector<QVariant> keys;

        for (const auto &model : models)
            keys.append(key.isEmpty() ? model.getKey()
                                      : model.getAttribute(key));

        using namespace ranges;
        return keys |= actions::sort(less {}, &QVariant::value<typename Model::KeyType>)
                       | actions::unique;
    }

    template<class Model, class Related>
    template<typename RelationValue>
    void HasOneOrMany<Model, Related>::matchOneOrMany(
            QVector<Model> &models, QVector<Related> &results,
            const QString &relation) const
    {
        auto dictionary = buildDictionary<RelationValue>(results);

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
    template<typename RelationValue>
    QHash<typename Model::KeyType, RelationValue>
    HasOneOrMany<Model, Related>::buildDictionary(QVector<Related> &results) const
    {
        QHash<typename Model::KeyType, RelationValue> dictionary;

        for (auto &result : results)
            if constexpr (
                const auto &foreign = result.getAttribute(getForeignKeyName())
                                      .template value<typename Model::KeyType>();
                std::is_same_v<RelationValue, QVector<Related>>
            )
                dictionary[foreign].append(std::move(result));
            else
                dictionary.insert(foreign, std::move(result));

        return dictionary;
    }

    template<class Model, class Related>
    QString HasOneOrMany<Model, Related>::getForeignKeyName() const
    {
        const auto segments = getQualifiedForeignKeyName().splitRef(QChar('.'));

        return segments.last().toString();
    }

} // namespace Orm::Tiny::Relations
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif

#endif // HASONEORMANY_H
