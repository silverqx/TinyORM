#ifndef BELONGSTO_H
#define BELONGSTO_H

#include <QDebug>

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
    class BelongsTo : public Relation<Model, Related>, public OneRelation
    {
    public:
        /*! Instantiate and initialize a new HasOne instance. */
        static std::unique_ptr<Relation<Model, Related>>
        create(std::unique_ptr<Builder<Related>> &&query,
               Model &child, const QString &foreignKey,
               const QString &ownerKey, const QString &relation);

        /*! Set the base constraints on the relation query. */
        void addConstraints() const override;

        /*! Set the constraints for an eager load of the relation. */
        void addEagerConstraints(const QVector<Model> &models) const override;
        /*! Initialize the relation on a set of models. */
        QVector<Model> &
        initRelation(QVector<Model> &models, const QString &relation) const override;
        /*! Match the eagerly loaded results to their parents. */
        void match(QVector<Model> &models, QVector<Related> results,
                   const QString &relation) const override;
        /*! Get the results of the relationship. */
        std::variant<QVector<Related>, std::optional<Related>>
        getResults() const override;

    protected:
        /*! Gather the keys from an array of related models. */
        QVector<QVariant> getEagerModelKeys(const QVector<Model> &models) const;
        /*! Get the default value for this relation. */
        Model &getDefaultFor(Model &parent) const;
        /*! Build model dictionary keyed by the parent's primary key. */
        QHash<typename Model::KeyType, Related>
        buildDictionary(const QVector<Related> &results) const;

        // WARNING don't forget to make them references silverqx
        /*! The child model instance of the relation. */
        const Model &m_child;
        /*! The foreign key of the parent model. */
        QString m_foreignKey;
        /*! The associated key on the parent model. */
        QString m_ownerKey;
        /*! The name of the relationship. */
        QString m_relationName;
        /*! The count of self joins. */
        constexpr static int selfJoinCount = 0;

    private:
        BelongsTo(std::unique_ptr<Builder<Related>> &&query, const Model &child,
                  const QString &foreignKey, const QString &ownerKey,
                  const QString &relationName);
    };

    template<class Model, class Related>
    BelongsTo<Model, Related>::BelongsTo(std::unique_ptr<Builder<Related>> &&query,
                                         const Model &child, const QString &foreignKey,
                                         const QString &ownerKey, const QString &relationName)
        : Relation<Model, Related>(std::move(query), child)
        /* In the underlying base relationship class, this variable is referred to as
           the "parent" since most relationships are not inversed. But, since this
           one is we will create a "child" variable for much better readability. */
        , m_child(child)
        , m_foreignKey(foreignKey)
        , m_ownerKey(ownerKey)
        , m_relationName(relationName)
    {}

    template<class Model, class Related>
    std::unique_ptr<Relation<Model, Related>>
    BelongsTo<Model, Related>::create(std::unique_ptr<Builder<Related>> &&query,
                                      Model &child, const QString &foreignKey,
                                      const QString &ownerKey, const QString &relation)
    {
        auto instance = std::unique_ptr<BelongsTo<Model, Related>>(
                    new BelongsTo(std::move(query), child, foreignKey, ownerKey, relation));

        instance->init();

        return instance;
    }

    template<class Model, class Related>
    void BelongsTo<Model, Related>::addConstraints() const
    {
        if (!this->constraints)
            return;

        /* For belongs to relationships, which are essentially the inverse of has one
               or has many relationships, we need to actually query on the primary key
               of the related models matching on the foreign key that's on a parent. */
        const auto &table = this->m_related.getTable();

        this->m_query->where(table + '.' + m_ownerKey, QStringLiteral("="),
                             m_child.getAttribute(m_foreignKey));
    }

    template<class Model, class Related>
    void BelongsTo<Model, Related>::addEagerConstraints(const QVector<Model> &models) const
    {
        /* We'll grab the primary key name of the related models since it could be set to
           a non-standard name and not "id". We will then construct the constraint for
           our eagerly loading query so it returns the proper models from execution. */
        this->m_query->getQuery().whereIn(this->m_related.getTable() + '.' + m_ownerKey,
                                          getEagerModelKeys(models));
    }

    template<class Model, class Related>
    QVector<Model> &
    BelongsTo<Model, Related>::initRelation(QVector<Model> &models,
                                            const QString &relation) const
    {
        for (auto &model : models)
            model.template setRelation<Related>(relation, std::nullopt);

        return models;
    }

    template<class Model, class Related>
    void BelongsTo<Model, Related>::match(QVector<Model> &models,
                                          QVector<Related> results,
                                          const QString &relation) const
    {
        /* First we will get to build a dictionary of the child models by their primary
           key of the relationship, then we can easily match the children back onto
           the parents using that dictionary and the primary key of the children. */
        const auto dictionary = buildDictionary(results);

        /* Once we have the dictionary constructed, we can loop through all the parents
           and match back onto their children using these keys of the dictionary and
           the primary key of the children to map them onto the correct instances. */
        for (auto &model : models) {
            const auto foreign = model.getAttribute(m_foreignKey)
                                 .template value<typename Model::KeyType>();

            if (dictionary.contains(foreign))
                model.setRelation(relation,
                                  std::optional<Related>(dictionary.find(foreign).value()));
        }
    }

    template<class Model, class Related>
    QHash<typename Model::KeyType, Related>
    BelongsTo<Model, Related>::buildDictionary(const QVector<Related> &results) const
    {
        QHash<typename Model::KeyType, Related> dictionary;

        /*! Build model dictionary keyed by the parent's primary key. */
        for (const auto &result : results)
            dictionary.insert(result.getAttribute(m_ownerKey)
                              .template value<typename Model::KeyType>(),
                              result);

        return dictionary;
    }

    template<class Model, class Related>
    std::variant<QVector<Related>, std::optional<Related>>
    BelongsTo<Model, Related>::getResults() const
    {
        // Model doesn't contain foreign key ( eg empty Model instance )
        if (const auto foreign = m_child.getAttribute(m_foreignKey);
            !foreign.isValid() || foreign.isNull()
        )
            return std::nullopt;

        // TODO add support for getDefaultFor() silverqx
        const auto first = this->m_query->first();

        // TODO check if I can return reference silverqx
        return  first ? first : std::nullopt;
    }

    template<class Model, class Related>
    QVector<QVariant>
    BelongsTo<Model, Related>::getEagerModelKeys(const QVector<Model> &models) const
    {
        QVector<QVariant> keys;

        /* First we need to gather all of the keys from the parent models so we know what
           to query for via the eager loading query. We will add them to an array then
           execute a "where in" statement to gather up all of those related records. */
        for (const auto &model : models) {
            const auto &value = model.getAttribute(m_foreignKey);

            if (!value.isNull())
                // TODO add support for non-int primary keys, ranges::acrions doesn't accept QVariant container silverqx
                keys.append(value);
        }

        using namespace ranges;
        return keys |= actions::sort(less {}, &QVariant::value<typename Model::KeyType>)
                       | actions::unique;
    }

    template<class Model, class Related>
    Model &BelongsTo<Model, Related>::getDefaultFor(Model &parent) const
    {
        Q_UNUSED(parent)

        return parent;
    }

} // namespace Orm::Tiny::Relations
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif

#endif // BELONGSTO_H
