#ifndef BELONGSTO_H
#define BELONGSTO_H

#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>

#include "orm/tiny/relations/concerns/supportsdefaultmodels.hpp"
#include "orm/tiny/relations/relation.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{

    template<class Model, class Related>
    class BelongsTo :
            public OneRelation,
            public Relation<Model, Related>,
            public Concerns::SupportsDefaultModels<Model, Related>
    {
    protected:
        BelongsTo(std::unique_ptr<Related> &&related, Model &child,
                  const QString &foreignKey, const QString &ownerKey,
                  const QString &relationName);

    public:
        /*! Instantiate and initialize a new BelongsTo instance. */
        static std::unique_ptr<BelongsTo<Model, Related>>
        instance(std::unique_ptr<Related> &&related,
                 Model &child, const QString &foreignKey,
                 const QString &ownerKey, const QString &relation);

        /* Updating relationship */
        /*! Associate the model instance to the given parent. */
        Model &associate(const Related &model) const;
        /*! Associate the model instance to the given parent. */
        Model &associate(const QVariant &id) const;
        /*! Dissociate previously associated model from the given parent. */
        Model &dissociate() const;
        /*! Alias of "dissociate" method. */
        Model &disassociate() const;

        /* Basic operations */
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

        /* Getters / Setters */
        /*! Get the name of the relationship. */
        QString getRelationName() const;

        /* Others */
        /*! The textual representation of the Relation type. */
        inline QString relationTypeName() const override
        { return "BelongsTo"; };

    protected:
        /*! Gather the keys from a vector of related models. */
        QVector<QVariant> getEagerModelKeys(const QVector<Model> &models) const;
        /*! Build model dictionary keyed by the parent's primary key. */
        QHash<typename Model::KeyType, Related>
        buildDictionary(const QVector<Related> &results) const;

        /*! Make a new related instance for the given model. */
        Related newRelatedInstanceFor(const Model &) const override;

        /*! The child model instance of the relation. */
        Model &m_child;
        /*! The foreign key of the parent model. */
        QString m_foreignKey;
        /*! The associated key on the parent model. */
        QString m_ownerKey;
        /*! The name of the relationship. */
        QString m_relationName;
        /*! The count of self joins. */
        inline static int selfJoinCount = 0;
    };

    template<class Model, class Related>
    BelongsTo<Model, Related>::BelongsTo(
            std::unique_ptr<Related> &&related, Model &child,
            const QString &foreignKey, const QString &ownerKey,
            const QString &relationName
    )
        : Relation<Model, Related>(std::move(related), child)
        /* In the underlying base relationship class, this variable is referred to as
           the "parent" since most relationships are not inversed. But, since this
           one is we will create a "child" variable for much better readability. */
        , m_child(this->m_parent)
        , m_foreignKey(foreignKey)
        , m_ownerKey(ownerKey)
        , m_relationName(relationName)
    {}

    template<class Model, class Related>
    std::unique_ptr<BelongsTo<Model, Related>>
    BelongsTo<Model, Related>::instance(
            std::unique_ptr<Related> &&related, Model &child,
            const QString &foreignKey, const QString &ownerKey, const QString &relation)
    {
        auto instance = std::unique_ptr<BelongsTo<Model, Related>>(
                    new BelongsTo(std::move(related), child, foreignKey,
                                  ownerKey, relation));

        instance->init();

        return instance;
    }

    template<class Model, class Related>
    Model &BelongsTo<Model, Related>::associate(const Related &model) const
    {
        m_child.setAttribute(m_foreignKey,
                             model.getAttribute(m_ownerKey));

        m_child.template setRelation<Related>(m_relationName, model);

        return m_child;
    }

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<class Model, class Related>
    Model &BelongsTo<Model, Related>::associate(const QVariant &id) const
    {
        m_child.setAttribute(m_foreignKey, id);

        // FEATURE relations, check if relation is loaded and if has the same id, if so, then don't unset relation silverqx
        m_child.unsetRelation(m_relationName);

        return m_child;
    }

    template<class Model, class Related>
    Model &BelongsTo<Model, Related>::dissociate() const
    {
        // TEST Model::save with null key silverqx
        // FEATURE dilemma primarykey, Model::KeyType vs QVariant, set to null, will be different for Qt5 (QVariant(QVariant::Type(qMetaTypeId<Model::KeyType>()))) and Qt6 (QVariant(QMetaType(qMetaTypeId<Model::KeyType>())))) ; ALSO current problem is, that I check that foreignKey !isValid || isNull, but when QVariant with type (Model::KeyType) and also with null is created by the above commands, then it is still null (isNull == true), but is considered as !!VALID!! (isValid == true) silverqx
        m_child.setAttribute(m_foreignKey, {});

        // TEST operations that are related on the Model::m_relation data member how they behave, when m_relations value contains the std::nullopt value silverqx
        return m_child.template setRelation<Related>(m_relationName, std::nullopt);
    }

    template<class Model, class Related>
    inline Model &BelongsTo<Model, Related>::disassociate() const
    {
        return dissociate();
    }

    template<class Model, class Related>
    void BelongsTo<Model, Related>::addConstraints() const
    {
        if (!this->constraints)
            return;

        /* For belongs to relationships, which are essentially the inverse of has one
               or has many relationships, we need to actually query on the primary key
               of the related models matching on the foreign key that's on a parent. */
        const auto &table = this->m_related->getTable();

        this->m_query->where(table + '.' + m_ownerKey, QStringLiteral("="),
                             m_child.getAttribute(m_foreignKey));
    }

    template<class Model, class Related>
    void
    BelongsTo<Model, Related>::addEagerConstraints(const QVector<Model> &models) const
    {
        /* We'll grab the primary key name of the related models since it could be set to
           a non-standard name and not "id". We will then construct the constraint for
           our eagerly loading query so it returns the proper models from execution. */
        this->m_query->getQuery().whereIn(this->m_related->getTable() + '.' + m_ownerKey,
                                          getEagerModelKeys(models));
    }

    template<class Model, class Related>
    QVector<Model> &
    BelongsTo<Model, Related>::initRelation(QVector<Model> &models,
                                            const QString &relation) const
    {
        for (auto &model : models)
            model.template setRelation<Related>(relation,
                                                this->getDefaultFor(model));

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
        auto dictionary = buildDictionary(results);

        /* Once we have the dictionary constructed, we can loop through all the parents
           and match back onto their children using these keys of the dictionary and
           the primary key of the children to map them onto the correct instances. */
        for (auto &model : models)
            if (const auto foreign = model.getAttribute(m_foreignKey)
                .template value<typename Model::KeyType>();
                dictionary.contains(foreign)
            )
                model.setRelation(relation,
                                  std::optional<Related>(
                                      std::move(dictionary.find(foreign).value())));
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
            return this->getDefaultFor(m_child);

        const auto first = this->m_query->first();

        return first ? first : this->getDefaultFor(m_child);
    }

    template<class Model, class Related>
    inline QString BelongsTo<Model, Related>::getRelationName() const
    {
        return m_relationName;
    }

    template<class Model, class Related>
    QVector<QVariant>
    BelongsTo<Model, Related>::getEagerModelKeys(const QVector<Model> &models) const
    {
        QVector<QVariant> keys;

        /* First we need to gather all of the keys from the parent models so we know what
           to query for via the eager loading query. We will add them to the vector then
           execute a "where in" statement to gather up all of those related records. */
        for (const auto &model : models) {
            const auto &value = model.getAttribute(m_foreignKey);

            if (!value.isNull())
                // TODO add support for non-int primary keys, ranges::actions doesn't accept QVariant container silverqx
                keys.append(value);
        }

        using namespace ranges;
        return keys |= actions::sort(less {}, &QVariant::value<typename Model::KeyType>)
                       | actions::unique;
    }

    template<class Model, class Related>
    inline Related
    BelongsTo<Model, Related>::newRelatedInstanceFor(const Model &) const
    {
        return this->m_related->newInstance();
    }

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // BELONGSTO_H
