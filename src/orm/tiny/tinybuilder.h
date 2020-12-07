#ifndef TINYBUILDER_H
#define TINYBUILDER_H

#include <QDebug>
#include <QtSql/QSqlRecord>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/iterator/insert_iterators.hpp>

#include "orm/databaseconnection.h"
#include "orm/query/querybuilder.h"
#include "orm/tiny/relations/relation.h"
#include "orm/tiny/relations/belongsto.h"
#include "orm/tiny/relations/hasmany.h"
#include "orm/utils/attribute.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{

    template<typename Model, typename ...AllRelations>
    class BaseModel;

namespace Relations
{
    template<class Model, class Related>
    class Relation;
}

    template<typename Model>
    class Builder
    {
    public:
        Builder(const QSharedPointer<QueryBuilder> query, Model &model);

        /*! Create a new instance of the model being queried. */
        Model newModelInstance(const QVector<AttributeItem> &attributes = {});

        /*! Execute the query as a "select" statement. */
        QVector<Model> get(const QStringList &columns = {"*"});

        /* BuildsQueries */
        // TODO BuildsQueries contains duplicit methods in TinyBuilder and QueryBuilder, make it by multi inheritance, I discovered now, that TinyBuilder will return different types than QueryBuilder, look eg at first() or get(), but investigate if there are cases, when API is same and use multi inheritance patter for this methods silver
        /*! Execute the query and get the first result. */
        std::optional<Model> first(const QStringList &columns = {"*"});

        /*! Set the relationships that should be eager loaded. */
        Builder &with(const QVector<WithItem> &relations);
        /*! Set the relationships that should be eager loaded. */
        inline Builder &with(const QString &relation)
        { return with({relation}); }

        /*! Insert new records into the database. */
        inline std::tuple<bool, std::optional<QSqlQuery>>
        insert(const QVector<AttributeItem> &attributes) const
        { return m_query->insert(Utils::Attribute::convertVectorToMap(attributes)); }
        // TODO primarykey dilema, Model::KeyType vs QVariant silverqx
        /*! Insert a new record and get the value of the primary key. */
        inline quint64 insertGetId(const QVector<AttributeItem> &attributes) const
        { return m_query->insertGetId(Utils::Attribute::convertVectorToMap(attributes)); }

        // TODO future add onDelete (and similar) callback feature silverqx
        /*! Delete records from the database. */
        inline std::tuple<int, QSqlQuery>
        remove()
        { return toBase().deleteRow(); }
        /*! Delete records from the database. */
        inline std::tuple<int, QSqlQuery>
        deleteModels()
        { return remove(); }

        /* Proxy methods to the QueryBuilder */
        // TODO for all proxy methods can be used parameter pack, check consequences silverqx
        /*! Add a basic where clause to the query. */
        Builder &where(const QString &column, const QString &comparison,
                       const QVariant &value, const QString &condition = "and");

        /*! Add a "where null" clause to the query. */
        Builder &whereNull(const QStringList &columns = {"*"},
                           const QString &condition = "and", bool nope = false);
        /*! Add a "where null" clause to the query. */
        Builder &whereNull(const QString &column, const QString &condition = "and",
                           bool nope = false);
        /*! Add an "or where null" clause to the query. */
        Builder &orWhereNull(const QStringList &columns = {"*"});
        /*! Add an "or where null" clause to the query. */
        Builder &orWhereNull(const QString &column);
        /*! Add a "where not null" clause to the query. */
        Builder &whereNotNull(const QStringList &columns = {"*"},
                              const QString &condition = "and");
        /*! Add a "where not null" clause to the query. */
        Builder &whereNotNull(const QString &column, const QString &condition = "and");
        /*! Add an "or where not null" clause to the query. */
        Builder &orWhereNotNull(const QStringList &columns = {"*"});
        /*! Add an "or where not null" clause to the query. */
        Builder &orWhereNotNull(const QString &column);

        /*! Add a "where in" clause to the query. */
        Builder &whereIn(const QString &column, const QVector<QVariant> &values,
                         const QString &condition = "and", bool nope = false);
        /*! Add an "or where in" clause to the query. */
        Builder &orWhereIn(const QString &column, const QVector<QVariant> &values);
        /*! Add a "where not in" clause to the query. */
        Builder &whereNotIn(const QString &column, const QVector<QVariant> &values,
                            const QString &condition = "and");
        /*! Add an "or where not in" clause to the query. */
        Builder &orWhereNotIn(const QString &column, const QVector<QVariant> &values);

        /*! Set the "limit" value of the query. */
        Builder &limit(int value);
        /*! Alias to set the "limit" value of the query. */
        Builder &take(int value);
        /*! Set the "offset" value of the query. */
        Builder &offset(int value);
        /*! Alias to set the "offset" value of the query. */
        Builder &skip(int value);
        /*! Set the limit and offset for a given page. */
        Builder &forPage(int page, int perPage = 30);

        /*! Get the hydrated models without eager loading. */
        QVector<Model> getModels(const QStringList &columns = {"*"});
        /*! Eager load the relationships for the models. */
        void eagerLoadRelations(QVector<Model> &models);
        /*! Eagerly load the relationship on a set of models. */
        template<typename Related>
        void eagerLoadRelation(QVector<Model> &models, const WithItem &relationItem);
        /*! Get the relation instance for the given relation name. */
        template<typename Related>
        auto getRelation(const QString &name);
        /*! Create a collection of models from plain arrays. */
        QVector<Model> hydrate(QSqlQuery result);

        /*! Get the model instance being queried. */
        inline Model &getModel()
        { return m_model; }
        /*! Get the underlying query builder instance. */
        inline QueryBuilder &getQuery() const
        { return *m_query; }

        /*! Get a database connection. */
        inline const DatabaseConnection &getConnection() const
        { return m_query->getConnection(); }

        /*! Get a base query builder instance. */
        inline QueryBuilder &toBase() const
        { return getQuery(); }
        // TODO future add Query Scopes feature silverqx
//        { return $this->applyScopes()->getQuery(); }

    protected:
        /*! Parse a list of relations into individuals. */
        QVector<WithItem> parseWithRelations(const QVector<WithItem> &relations) const;
        /*! Create a constraint to select the given columns for the relation. */
        WithItem createSelectWithConstraint(const QString &name) const;
        /*! Parse the nested relationships in a relation. */
        void addNestedWiths(const QString &name, QVector<WithItem> &results) const;

        /*! Get the deeply nested relations for a given top-level relation. */
        QVector<WithItem>
        relationsNestedUnder(const QString &topRelationName) const;
        /*! Determine if the relationship is nested. */
        bool isNestedUnder(const QString &topRelation, const QString &nestedRelation) const;

        /*! The base query builder instance. */
        const QSharedPointer<QueryBuilder> m_query;
        /*! The model being queried. */
        Model m_model;
        /*! The relationships that should be eager loaded. */
        QVector<WithItem> m_eagerLoad;
    };

    template<typename Model>
    Builder<Model>::Builder(const QSharedPointer<QueryBuilder> query,
                            Model &model)
        : m_query(query)
        , m_model(model)
    {
        m_query->from(m_model.getTable());
    }

    template<typename Model>
    Model Builder<Model>::newModelInstance(const QVector<AttributeItem> &attributes)
    {
        return m_model.newInstance(attributes)
                .setConnection(m_query->getConnection().getName());
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::get(const QStringList &columns)
    {
        auto models = getModels(columns);

        /* If we actually found models we will also eager load any relationships that
           have been specified as needing to be eager loaded, which will solve the
           n+1 query issue for the developers to avoid running a lot of queries. */
        if (models.size() > 0)
            /* 'models' are passed down as the reference and relations are set on models
               at the end of the call tree, no need to return models. */
            eagerLoadRelations(models);

        return models;
        // Laravel does it this way
        //        return $builder->getModel()->newCollection($models);
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::first(const QStringList &columns)
    {
        const auto models = take(1).get(columns);

        if (models.isEmpty())
            return std::nullopt;

        return models.first();
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::with(const QVector<WithItem> &relations)
    {
        auto eagerLoad = parseWithRelations(relations);

        ranges::move(eagerLoad, ranges::back_inserter(m_eagerLoad));

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::where(const QString &column, const QString &comparison,
                          const QVariant &value, const QString &condition)
    {
        m_query->where(column, comparison, value, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNull(const QStringList &columns, const QString &condition,
                              const bool nope)
    {
        m_query->whereNull(columns, condition, nope);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNull(const QString &column, const QString &condition,
                              const bool nope)
    {
        return whereNull(QStringList(column), condition, nope);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereNull(const QStringList &columns)
    {
        m_query->orWhereNull(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereNull(const QString &column)
    {
        return orWhereNull(QStringList(column));
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNotNull(const QStringList &columns, const QString &condition)
    {
        m_query->whereNotNull(columns, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNotNull(const QString &column, const QString &condition)
    {
        return whereNotNull(QStringList(column), condition);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereNotNull(const QStringList &columns)
    {
        m_query->orWhereNotNull(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereNotNull(const QString &column)
    {
        return orWhereNotNull(QStringList(column));
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereIn(const QString &column, const QVector<QVariant> &values,
                            const QString &condition, bool nope)
    {
        m_query->whereIn(column, values, condition, nope);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereIn(const QString &column, const QVector<QVariant> &values)
    {
        m_query->orWhereIn(column, values);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNotIn(const QString &column, const QVector<QVariant> &values,
                               const QString &condition)
    {
        m_query->whereNotIn(column, values, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereNotIn(const QString &column, const QVector<QVariant> &values)
    {
        m_query->orWhereNotIn(column, values);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::limit(const int value)
    {
        m_query->limit(value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::take(const int value)
    {
        return limit(value);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::offset(const int value)
    {
        m_query->offset(value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::skip(const int value)
    {
        return offset(value);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::forPage(const int page, const int perPage)
    {
        m_query->forPage(page, perPage);
        return *this;
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::getModels(const QStringList &columns)
    {
        return hydrate(std::get<1>(m_query->get(columns)));
    }

    template<typename Model>
    void Builder<Model>::eagerLoadRelations(QVector<Model> &models)
    {
        if (m_eagerLoad.isEmpty())
            return;

        for (const auto &relation : qAsConst(m_eagerLoad))
            /* For nested eager loads we'll skip loading them here and they will be set as an
               eager load on the query to retrieve the relation so that they will be eager
               loaded on that query, because that is where they get hydrated as models. */
            if (!relation.name.contains(QChar('.')))
                m_model.eagerLoadRelationVisitor(relation, *this, models);
    }

    template<typename Model>
    template<typename Related>
    void Builder<Model>::eagerLoadRelation(QVector<Model> &models,
                                           const WithItem &relationItem)
    {
        /* First we will "back up" the existing where conditions on the query so we can
           add our eager constraints. Then we will merge the wheres that were on the
           query back to it in order that any where conditions might be specified. */
        auto relation = getRelation<Related>(relationItem.name);

        relation->addEagerConstraints(models);

        // TODO next add support for std::invoke(relationItem.constraints); silverqx
        // Add relation contraints defined in a user callback
//        std::invoke(relationItem.constraints);

        /* Once we have the results, we just match those back up to their parent models
           using the relationship instance. Then we just return the finished arrays
           of models which have been eagerly hydrated and are readied for return. */
        relation->match(relation->initRelation(models, relationItem.name),
                        relation->getEager(), relationItem.name);
    }

    template<typename Model>
    template<typename Related>
    auto Builder<Model>::getRelation(const QString &name)
    {
        const auto &method = m_model.getRelationMethod(name);

        /* We want to run a relationship query without any constrains so that we will
           not have to remove these where clauses manually which gets really hacky
           and error prone. We don't want constraints because we add eager ones. */
        auto relation = Relations::Relation<Model, Related>::noConstraints(
                    [this, &method]
        {
            return std::invoke(std::any_cast<RelationType<Model, Related>>(method),
                               getModel().newInstance());
        });

        const auto nested = relationsNestedUnder(name);

        /* If there are nested relationships set on the query, we will put those onto
           the query instances so that they can be handled after this relationship
           is loaded. In this way they will all trickle down as they are loaded. */
        if (nested.size() > 0)
            relation->getQuery().with(nested);

        return relation;
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::hydrate(QSqlQuery result)
    {
        auto instance = newModelInstance();

        QVector<Model> models;

        while (result.next()) {
            // Table row
            QVector<AttributeItem> row;

            // Populate table row with data from the database
            const auto record = result.record();
            for (int i = 0; i < record.count(); ++i)
                row.append({record.fieldName(i), result.value(i)});

            // Create a new model instance from the table row
            models.append(m_model.newFromBuilder(row));
        }

        return models;
    }

    template<typename Model>
    QVector<WithItem>
    Builder<Model>::parseWithRelations(const QVector<WithItem> &relations) const
    {
        QVector<WithItem> results;

        for (auto relation : relations) {
            const auto emptyConstraints = !relation.constraints;
            const auto isSelectConstraint = relation.name.contains(QChar(':'));

            /* Select columns constraints are only allowed, when relation.constraints
               is nullptr. */
            if (isSelectConstraint)
                Q_ASSERT(!emptyConstraints);

            if (emptyConstraints && isSelectConstraint)
                relation = createSelectWithConstraint(relation.name);

            /* We need to separate out any nested includes, which allows the developers
               to load deep relationships using "dots" without stating each level of
               the relationship with its own key in the array of eager-load names. */
            addNestedWiths(relation.name, results);

            results.append(relation);
        }

        return results;
    }

    template<typename Model>
    WithItem Builder<Model>::createSelectWithConstraint(const QString &name) const
    {
        return {
            name.split(QChar(':')).first(), [name](/*auto &query*/)
            {
//                query->select(name.split(QChar(':')).last().split(QChar(',')));
            }
        };
    }

    template<typename Model>
    void Builder<Model>::addNestedWiths(const QString &name,
                                        QVector<WithItem> &results) const
    {
        QStringList progress;

        /* If the relation has already been set on the result array, we will not set it
           again, since that would override any constraints that were already placed
           on the relationships. We will only set the ones that are not specified. */
        // Prevent container detach
        const auto names = name.split(QChar('.'));
        for (const auto &segment : names) {
            progress << segment;

            const auto last = progress.join(QChar('.'));
            const auto containsRelation = [&last](const auto &relation)
            {
                return relation.name == last;
            };
            const auto contains = ranges::contains(results, true, containsRelation);

            // Don't add a relation in the 'name' variable
            if (!contains && (last != name))
                results.append({last});
        }
    }

    template<typename Model>
    QVector<WithItem>
    Builder<Model>::relationsNestedUnder(const QString &topRelationName) const
    {
        QVector<WithItem> nested;

        /* We are basically looking for any relationships that are nested deeper than
           the given top-level relationship. We will just check for any relations
           that start with the given top relations and adds them to our arrays. */
        for (const auto &relation : m_eagerLoad)
            if (isNestedUnder(topRelationName, relation.name))
                nested.append({relation.name.mid(topRelationName.size() + 1),
                               relation.constraints});

        return nested;
    }

    template<typename Model>
    bool Builder<Model>::isNestedUnder(const QString &topRelation,
                                       const QString &nestedRelation) const
    {
        return nestedRelation.contains(QChar('.'))
                && nestedRelation.startsWith(topRelation + QChar('.'));
    }

} // namespace Orm::Tiny
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif

#endif // TINYBUILDER_H
