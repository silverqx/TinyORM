#pragma once
#ifndef ORM_TINY_TINYBUILDER_HPP
#define ORM_TINY_TINYBUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtSql/QSqlRecord>

#include <range/v3/action/transform.hpp>
#include <range/v3/algorithm/contains.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/remove_if.hpp>

#include "orm/tiny/concerns/buildsqueries.hpp"
#include "orm/tiny/concerns/queriesrelationships.hpp"
#include "orm/tiny/exceptions/modelnotfounderror.hpp"
#include "orm/tiny/tinybuilderproxies.hpp"
#include "orm/utils/helpers.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{

    /*! ORM Tiny builder (returns a hydrated models instead of the QSqlQuery). */
    template<typename Model>
    class Builder :
            public Concerns::BuildsQueries<Model>,
            public BuilderProxies<Model>,
            public Concerns::QueriesRelationships<Model>
    {
        // Used by TinyBuilderProxies::where/latest/oldest/update()
        friend BuilderProxies<Model>;

        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;
        /*! Alias for the helper utils. */
        using Helpers = Orm::Utils::Helpers;
        /*! Alias for the type utils. */
        using TypeUtils = Orm::Utils::Type;

        // To access enforceOrderBy(), and defaultKeyName()
        template<ModelConcept T>
        friend class Concerns::BuildsQueries;

    public:
        /*! Constructor. */
        Builder(std::shared_ptr<QueryBuilder> &&query, const Model &model);

        /*! Copy constructor (needed by the chunkById() -> clone() method). */
        inline Builder(const Builder &) = default;
        /*! Deleted copy assignment operator (not needed). */
        Builder &operator=(const Builder &) = delete;

        /*! Move constructor (copy ctor needed so enable also the move ctor). */
        inline Builder(Builder &&) noexcept = default;
        /*! Deleted move assignment operator (not needed). */
        Builder &operator=(Builder &&) = delete;

        /*! Get the SQL representation of the query. */
        inline QString toSql() const;
        /*! Get the current query value bindings as flattened QVector. */
        inline QVector<QVariant> getBindings() const;

        /* Retrieving results */
        /*! Execute the query as a "select" statement. */
        QVector<Model> get(const QVector<Column> &columns = {ASTERISK});

        /*! Get a single column's value from the first result of a query. */
        QVariant value(const Column &column);
        /*! Get a single column's value from the first result of a query if it's
            the sole matching record. */
        QVariant soleValue(const Column &column);

        /*! Get the vector with the values of a given column. */
        QVector<QVariant> pluck(const QString &column);
        /*! Get the vector with the values of a given column. */
        template<typename T>
        std::map<T, QVariant> pluck(const QString &column, const QString &key);

        /*! Find a model by its primary key. */
        std::optional<Model>
        find(const QVariant &id, const QVector<Column> &columns = {ASTERISK});
        /*! Find a model by its primary key or return fresh model instance. */
        Model findOrNew(const QVariant &id, const QVector<Column> &columns = {ASTERISK});
        /*! Find a model by its primary key or throw an exception. */
        Model findOrFail(const QVariant &id,
                         const QVector<Column> &columns = {ASTERISK});
        /*! Find multiple models by their primary keys. */
        QVector<Model>
        findMany(const QVector<QVariant> &ids,
                 const QVector<Column> &columns = {ASTERISK});

        /*! Execute a query for a single record by ID or call a callback. */
        std::optional<Model>
        findOr(const QVariant &id, const QVector<Column> &columns,
               const std::function<void()> &callback = nullptr);
        /*! Execute a query for a single record by ID or call a callback. */
        std::optional<Model>
        findOr(const QVariant &id, const std::function<void()> &callback = nullptr);

        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        std::pair<std::optional<Model>, R>
        findOr(const QVariant &id, const QVector<Column> &columns,
               const std::function<R()> &callback);
        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        std::pair<std::optional<Model>, R>
        findOr(const QVariant &id, const std::function<R()> &callback);

        /*! Execute the query and get the first result. */
        std::optional<Model> first(const QVector<Column> &columns = {ASTERISK});
        /*! Get the first record matching the attributes or instantiate it. */
        Model firstOrNew(const QVector<WhereItem> &attributes = {},
                         const QVector<AttributeItem> &values = {});
        /*! Get the first record matching the attributes or create it. */
        Model firstOrCreate(const QVector<WhereItem> &attributes = {},
                            const QVector<AttributeItem> &values = {});
        /*! Execute the query and get the first result or throw an exception. */
        Model firstOrFail(const QVector<Column> &columns = {ASTERISK});

        /*! Add a basic where clause to the query, and return the first result. */
        std::optional<Model>
        firstWhere(const Column &column, const QString &comparison,
                   const QVariant &value, const QString &condition = AND);
        /*! Add a basic equal where clause to the query, and return the first result. */
        std::optional<Model>
        firstWhereEq(const Column &column, const QVariant &value,
                     const QString &condition = AND);

        /*! Add a where clause on the primary key to the query. */
        Builder &whereKey(const QVariant &id);
        /*! Add a where clause on the primary key to the query. */
        Builder &whereKey(const QVector<QVariant> &ids);
        /*! Add a where clause on the primary key to the query. */
        Builder &whereKeyNot(const QVariant &id);
        /*! Add a where clause on the primary key to the query. */
        Builder &whereKeyNot(const QVector<QVariant> &ids);

        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        Builder &with(const QVector<WithItem> &relations);
        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        Builder &with(const QString &relation);
        /*! Set the relationships that should be eager loaded. */
        Builder &with(const QVector<QString> &relations);
        /*! Set the relationships that should be eager loaded. */
        Builder &with(QVector<QString> &&relations);

        /*! Prevent the specified relations from being eager loaded. */
        Builder &without(const QVector<QString> &relations);
        /*! Prevent the specified relations from being eager loaded. */
        Builder &without(const QString &relation);

        /*! Set the relationships that should be eager loaded while removing
            any previously added eager loading specifications. */
        Builder &withOnly(const QVector<WithItem> &relations);
        /*! Set the relationship that should be eager loaded while removing
            any previously added eager loading specifications. */
        Builder &withOnly(const QString &relation);

        /* Insert, Update, Delete */
        /*! Save a new model and return the instance. */
        Model create(const QVector<AttributeItem> &attributes = {});
        /*! Save a new model and return the instance. */
        Model create(QVector<AttributeItem> &&attributes = {});

        /*! Create or update a record matching the attributes, and fill it with
            values. */
        Model updateOrCreate(const QVector<WhereItem> &attributes,
                             const QVector<AttributeItem> &values = {});

        /* TinyBuilder methods */
        inline Builder clone() const;
        /*! Create a new instance of the model being queried. */
        Model newModelInstance(const QVector<AttributeItem> &attributes = {});

        /*! Get the hydrated models without eager loading. */
        QVector<Model> getModels(const QVector<Column> &columns = {ASTERISK});

        /*! Eager load the relationships for the models. */
        void eagerLoadRelations(QVector<Model> &models);
        /*! Eagerly load the relationship on a set of models. */
        template<typename Relation>
        void eagerLoadRelationVisited(Relation &&relation, QVector<Model> &models,
                                      const WithItem &relationItem) const;
        /*! Create a vector of models from the QSqlQuery. */
        QVector<Model> hydrate(QSqlQuery &&result);

        /*! Get the model instance being queried. */
        inline Model &getModel();
        /*! Get the underlying query builder instance. */
        inline QueryBuilder &getQuery() const;
        /*! Get the underlying query builder instance as a std::shared_ptr. */
        inline const std::shared_ptr<QueryBuilder> &
        getQuerySharedPointer() const;

        /*! Get a database connection. */
        inline DatabaseConnection &getConnection() const;

        /*! Get a base query builder instance. */
        inline QueryBuilder &toBase() const;

        /*! Qualify the given column name by the model's table. */
        inline QString qualifyColumn(const QString &column) const;

    protected:
        /*! Expression alias. */
        using Expression = Orm::Query::Expression;

        /*! Get the default key name of the table. */
        inline const QString &defaultKeyName() const;

        /*! Parse a list of relations into individuals. */
        QVector<WithItem> parseWithRelations(const QVector<WithItem> &relations);
        /*! Create a constraint to select the given columns for the relation. */
        WithItem createSelectWithConstraint(const QString &name);
        /*! Parse the nested relationships in a relation. */
        void addNestedWiths(const QString &name, QVector<WithItem> &results) const;

        /*! Get the deeply nested relations for a given top-level relation. */
        QVector<WithItem>
        relationsNestedUnder(const QString &topRelationName) const;
        /*! Determine if the relationship is nested. */
        bool isNestedUnder(const QString &topRelation,
                           const QString &nestedRelation) const;

        /*! Add the "updated at" column to the vector of values. */
        QVector<UpdateItem>
        addUpdatedAtColumn(QVector<UpdateItem> values) const;

        /*! Get the name of the "created at" column. */
        Column getCreatedAtColumnForLatestOldest(Column column) const;

        /*! Add a generic "order by" clause if the query doesn't already have one. */
        void enforceOrderBy();

        /*! Apply the given scope on the current builder instance. */
//        template<typename ...Args>
//        Builder &callScope(const std::function<void(Builder &, Args ...)> &scope,
//                           Args &&...parameters);

        /*! The base query builder instance. */
        const std::shared_ptr<QueryBuilder> m_query;
        /* This can't be a reference because the model is created on the stack
           in Model::query(), then copied here and the original is destroyed
           immediately. */
        /*! The model being queried. */
        Model m_model;
        /*! The relationships that should be eager loaded. */
        QVector<WithItem> m_eagerLoad;
    };

    /* public */

    template<typename Model>
    Builder<Model>::Builder(std::shared_ptr<QueryBuilder> &&query, const Model &model)
        : m_query(std::move(query))
        , m_model(model)
    {
        m_query->from(m_model.getTable());
    }

    template<typename Model>
    QString Builder<Model>::toSql() const
    {
        return toBase().toSql();
    }

    template<typename Model>
    QVector<QVariant> Builder<Model>::getBindings() const
    {
        return toBase().getBindings();
    }

    /* Retrieving results */

    // TODO now name QVector<Model> model collections by using, eg CollectionType silverqx
    template<typename Model>
    QVector<Model>
    Builder<Model>::get(const QVector<Column> &columns)
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
    QVariant Builder<Model>::value(const Column &column)
    {
        auto model = first({column});

        if (!model)
            return {};

        // Expression support
        QString column_;

        if (std::holds_alternative<Expression>(column))
            column_ = std::get<Expression>(column).getValue().value<QString>();
        else
            column_ = std::get<QString>(column);

        return model->getAttribute(column_.mid(column_.lastIndexOf(DOT) + 1));
    }

    template<typename Model>
    QVariant Builder<Model>::soleValue(const Column &column)
    {
        auto model = this->sole({column});

        // Expression support
        QString column_;

        if (std::holds_alternative<Expression>(column))
            column_ = std::get<Expression>(column).getValue().value<QString>();
        else
            column_ = std::get<QString>(column);

        return model.getAttribute(column_.mid(column_.lastIndexOf(DOT) + 1));
    }

    template<typename Model>
    QVector<QVariant> Builder<Model>::pluck(const QString &column)
    {
        auto result = toBase().pluck(column);

        // Nothing to pluck-ing 😎
        if (result.empty())
            return result;

        /* If the column is qualified with a table or have an alias, we cannot use
           those directly in the "pluck" operations, we have to strip the table out or
           use the alias name instead. */
        const auto unqualifiedColumn = getQuery().stripTableForPluck(column);

        /* If the model has a mutator for the requested column, we will spin through
           the results and mutate the values so that the mutated version of these
           columns are returned as you would expect from these Eloquent models. */
        if (!m_model.getDates().contains(unqualifiedColumn))
            return result;

        return result |= ranges::actions::transform([this, &unqualifiedColumn]
                                                    (auto &&value)
        {
            return m_model.newFromBuilder({{unqualifiedColumn,
                                            std::forward<decltype (value)>(value)}})
                    .getAttribute(unqualifiedColumn);
        });
    }

    template<typename Model>
    template<typename T>
    std::map<T, QVariant>
    Builder<Model>::pluck(const QString &column, const QString &key)
    {
        auto result = toBase().template pluck<T>(column, key);

        // Nothing to pluck-ing 😎
        if (result.empty())
            return result;

        /* If the column is qualified with a table or have an alias, we cannot use
           those directly in the "pluck" operations, we have to strip the table out or
           use the alias name instead. */
        const auto unqualifiedColumn = getQuery().stripTableForPluck(column);

        /* If the model has a mutator for the requested column, we will spin through
           the results and mutate the values so that the mutated version of these
           columns are returned as you would expect from these Eloquent models. */
        if (!m_model.getDates().contains(unqualifiedColumn))
            return result;

        for (auto &&[_, value] : result)
            value = m_model.newFromBuilder({{unqualifiedColumn, std::move(value)}})
                    .getAttribute(unqualifiedColumn);

        return result;
    }

    // FEATURE dilemma primarykey, Model::KeyType for id silverqx
    template<typename Model>
    std::optional<Model>
    Builder<Model>::find(const QVariant &id, const QVector<Column> &columns)
    {
        return whereKey(id).first(columns);
    }

    template<typename Model>
    Model Builder<Model>::findOrNew(const QVariant &id, const QVector<Column> &columns)
    {
        auto model = find(id, columns);

        // Found
        if (model)
            return *model;

        return newModelInstance();
    }

    template<typename Model>
    Model Builder<Model>::findOrFail(const QVariant &id, const QVector<Column> &columns)
    {
        auto model = find(id, columns);

        // Found
        if (model)
            return *model;

        throw Exceptions::ModelNotFoundError(
                    TypeUtils::classPureBasename<Model>(), {id});
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::findMany(const QVector<QVariant> &ids,
                             const QVector<Column> &columns)
    {
        if (ids.isEmpty())
            return {};

        return whereKey(ids).get(columns);
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::findOr(const QVariant &id, const QVector<Column> &columns,
                           const std::function<void()> &callback)
    {
        auto model = find(id, columns);

        if (model)
            return model;

        // Optionally invoke the callback
        if (callback)
            std::invoke(callback);

        // Return an original model from the find() method
        return model;
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::findOr(const QVariant &id, const std::function<void()> &callback)
    {
        return findOr(id, {ASTERISK}, callback);
    }

    template<typename Model>
    template<typename R>
    std::pair<std::optional<Model>, R>
    Builder<Model>::findOr(const QVariant &id, const QVector<Column> &columns,
                           const std::function<R()> &callback)
    {
        auto model = find(id, columns);

        if (model)
            return {std::move(model), R {}};

        // Return an original model from the find() method instead of the default Model{}

        // Optionally invoke the callback
        if (callback)
            return {std::move(model), std::invoke(callback)};

        return {std::move(model), R {}};
    }

    template<typename Model>
    template<typename R>
    std::pair<std::optional<Model>, R>
    Builder<Model>::findOr(const QVariant &id, const std::function<R()> &callback)
    {
        return findOr<R>(id, {ASTERISK}, callback);
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::first(const QVector<Column> &columns)
    {
        auto models = this->take(1).get(columns);

        if (models.isEmpty())
            return std::nullopt;

        return std::move(models.first());
    }

    template<typename Model>
    Model
    Builder<Model>::firstOrNew(const QVector<WhereItem> &attributes,
                               const QVector<AttributeItem> &values)
    {
        auto instance = this->where(attributes).first();

        // Model found in db
        if (instance)
            return *instance;

        return newModelInstance(AttributeUtils::joinAttributesForFirstOr(
                                    attributes, values, m_model.getKeyName()));
    }

    template<typename Model>
    Model
    Builder<Model>::firstOrCreate(const QVector<WhereItem> &attributes,
                                  const QVector<AttributeItem> &values)
    {
        // Model found in db
        if (auto instance = this->where(attributes).first(); instance)
            return *instance;

        return Helpers::tap<Model>(
                    newModelInstance(AttributeUtils::joinAttributesForFirstOr(
                                         attributes, values, m_model.getKeyName())),
                    [](auto &newInstance)
        {
            newInstance.save();
        });
    }

    template<typename Model>
    Model Builder<Model>::firstOrFail(const QVector<Column> &columns)
    {
        auto model = first(columns);

        // Found
        if (model)
            return *model;

        throw Exceptions::ModelNotFoundError(TypeUtils::classPureBasename<Model>());
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::firstWhere(const Column &column, const QString &comparison,
                               const QVariant &value, const QString &condition)
    {
        return this->where(column, comparison, value, condition).first();
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::firstWhereEq(const Column &column, const QVariant &value,
                                 const QString &condition)
    {
        return this->where(column, EQ, value, condition).first();
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereKey(const QVariant &id)
    {
        return this->where(m_model.getQualifiedKeyName(), EQ, id);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereKey(const QVector<QVariant> &ids)
    {
        m_query->whereIn(m_model.getQualifiedKeyName(), ids);

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereKeyNot(const QVariant &id)
    {
        return this->where(m_model.getQualifiedKeyName(), NE, id);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereKeyNot(const QVector<QVariant> &ids)
    {
        m_query->whereNotIn(m_model.getQualifiedKeyName(), ids);

        return *this;
    }

    template<typename Model>
    template<typename>
    Builder<Model> &
    Builder<Model>::with(const QVector<WithItem> &relations)
    {
        auto eagerLoad = parseWithRelations(relations);

        std::ranges::move(eagerLoad, std::back_inserter(m_eagerLoad));

        return *this;
    }

    template<typename Model>
    template<typename>
    Builder<Model> &
    Builder<Model>::with(const QString &relation)
    {
        return with(QVector<WithItem> {{relation}});
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::with(const QVector<QString> &relations)
    {
        QVector<WithItem> relationsConverted;
        relationsConverted.reserve(relations.size());

        for (const auto &relation : relations)
            relationsConverted.append({relation});

        return with(relationsConverted);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::with(QVector<QString> &&relations)
    {
        QVector<WithItem> relationsConverted;
        relationsConverted.reserve(relations.size());

        for (auto &relation : relations)
            relationsConverted.append({std::move(relation)});

        return with(relationsConverted);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::without(const QVector<QString> &relations)
    {
        // Remove relations in the "relations" vector from m_eagerLoad vector
        m_eagerLoad = m_eagerLoad
                      | ranges::views::remove_if([&relations](const WithItem &with)
        {
            return relations.contains(with.name);
        })
                | ranges::to<QVector<WithItem>>();

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::without(const QString &relation)
    {
        return without(QVector<QString> {relation});
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::withOnly(const QVector<WithItem> &relations)
    {
        m_eagerLoad.clear();

        return with(relations);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::withOnly(const QString &relation)
    {
        return withOnly(QVector<WithItem> {{relation}});
    }

    /* Insert, Update, Delete */

    template<typename Model>
    Model Builder<Model>::create(const QVector<AttributeItem> &attributes)
    {
        auto model = newModelInstance(attributes);

        model.save();

        return model;
    }

    template<typename Model>
    Model Builder<Model>::create(QVector<AttributeItem> &&attributes)
    {
        auto model = newModelInstance(std::move(attributes));

        model.save();

        return model;
    }

    template<typename Model>
    Model Builder<Model>::updateOrCreate(const QVector<WhereItem> &attributes,
                                         const QVector<AttributeItem> &values)
    {
        auto instance = firstOrNew(attributes);

        instance.fill(values).save();

        return instance;
    }

    template<typename Model>
    Builder<Model> Builder<Model>::clone() const
    {
        return *this;
    }

    /* TinyBuilder methods */

    template<typename Model>
    Model Builder<Model>::newModelInstance(const QVector<AttributeItem> &attributes)
    {
        return m_model.newInstance(attributes)
                .setConnection(m_query->getConnection().getName());
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::getModels(const QVector<Column> &columns)
    {
        return hydrate(m_query->get(columns));
    }

    template<typename Model>
    void Builder<Model>::eagerLoadRelations(QVector<Model> &models)
    {
        if (m_eagerLoad.isEmpty())
            return;

        for (const auto &relation : std::as_const(m_eagerLoad))
            /* For nested eager loads we'll skip loading them here and they will be set
               as an eager load on the query to retrieve the relation so that they will
               be eager loaded on that query, because that is where they get hydrated
               as models. */
            if (!relation.name.contains(DOT))
                /* Get the relation instance for the given relation name, have to be done
                   through the visitor pattern. */
                m_model.eagerLoadRelationWithVisitor(relation, *this, models);
    }

    template<typename Model>
    template<typename Relation>
    void Builder<Model>::eagerLoadRelationVisited(
            Relation &&relation, QVector<Model> &models,
            const WithItem &relationItem) const
    {
        // TODO docs add similar note for lazy load silverqx
        /* Look also at EagerRelationStore::visited(), where the whole flow begins.
           How this relation flow works:
           EagerRelationStore::visited() obtains a reference by the relation name
           to the relation method, these relation methods are defined on models
           as member functions.
           A reference to the relation methods are defined in the Model::u_relations
           hash as lambda expressions. These lambda expressions will be visited/invoked
           by EagerRelationStore::visited() to obtain references to the relation methods.
           Relation constraints will be disabled for eager relations by
           Relation::noConstraints() method, these default constraints are only used
           for lazy loading, for eager constraints are used constraints, which are
           defined by Relation::addEagerConstraints() virtual methods.
           To the Relation::noConstraints() method is passed lambda, which invokes
           obtained reference to the relation method defined on the model and invokes it
           on the 'new' model instance refered as 'dummyModel'.
           The Relation instance is created by this relation method, this relation
           method calls factory method, which creates the Relation instance.
           Every Relation has it's own Relation::create() factory method, to which
           the following parameters are passed, newly created Related model instance,
           current/parent model instance, database column names of the relationship, and
           for a BelongsTo relation also the name of the relation.
           The Relation instance saves a non-const reference to the current/parent model
           instance, a copy of the related model instance because it is created
           on the stack.
           The Relation instance creates a new TinyBuilder instance from the Related
           model instance by TinyBuilder::newQuery() and saves ownership as
           the unique pointer.
           Then eager constraints are applied to this newly created TinyBuilder and
           the result is returned back to the initial model.
           The result is transformed into models and these models are hydrated.
           Hydrated models are saved to the Model::m_relations data member. */

        /* First we will "back up" the existing where conditions on the query so we can
           add our eager constraints, this is done in the EagerRelationStore::visited()
           by help of the Relations::Relation::noConstraints().
           Folowing is not implemented for now, it is true for relationItem.constraints:
           Then we will merge the wheres that were on the query back to it in order
           that any where conditions might be specified. */
        const auto nested = relationsNestedUnder(relationItem.name);

        /* If there are nested relationships set on the query, we will put those onto
           the query instances so that they can be handled after this relationship
           is loaded. In this way they will all trickle down as they are loaded. */
        if (nested.size() > 0)
            relation->getQuery().with(nested);

        relation->addEagerConstraints(models);

        // Add relation contraints defined in a user callback
        // NOTE api different, Eloquent is passing the Relation reference into the lambda, it would be almost impossible to do it silverqx
        if (relationItem.constraints)
            std::invoke(relationItem.constraints, relation->getBaseQuery());

        /* Once we have the results, we just match those back up to their parent models
           using the relationship instance. Then we just return the finished vectors
           of models which have been eagerly hydrated and are readied for return. */
        relation->match(relation->initRelation(models, relationItem.name),
                        relation->getEager(), relationItem.name);
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::hydrate(QSqlQuery &&result)
    {
        auto instance = newModelInstance();

        QVector<Model> models;

        // Table row, instantiate the QVector once and then re-use
        QVector<AttributeItem> row;
        row.reserve(result.record().count());

        while (result.next()) {
            row.clear();

            // Populate table row with data from the database
            const auto record = result.record();
            for (int i = 0; i < record.count(); ++i)
                row.append({record.fieldName(i), result.value(i)});

            // Create a new model instance from the table row
            models.append(instance.newFromBuilder(row));
        }

        return models;
    }

    template<typename Model>
    Model &Builder<Model>::getModel()
    {
        return m_model;
    }

    template<typename Model>
    QueryBuilder &Builder<Model>::getQuery() const
    {
        return *m_query;
    }

    template<typename Model>
    const std::shared_ptr<QueryBuilder> &
    Builder<Model>::getQuerySharedPointer() const
    {
        return m_query;
    }

    template<typename Model>
    DatabaseConnection &
    Builder<Model>::getConnection() const
    {
        return m_query->getConnection();
    }

    template<typename Model>
    QueryBuilder &Builder<Model>::toBase() const
    {
        // FUTURE add Query Scopes feature silverqx
        // retutn applyScopes().getQuery();
        return getQuery();
    }

    template<typename Model>
    QString Builder<Model>::qualifyColumn(const QString &column) const
    {
        return m_model.qualifyColumn(column);
    }

    /* protected */

    template<typename Model>
    const QString &Builder<Model>::defaultKeyName() const
    {
        return m_model.getKeyName();
    }

    template<typename Model>
    QVector<WithItem>
    Builder<Model>::parseWithRelations(const QVector<WithItem> &relations)
    {
        QVector<WithItem> results;
        // Can contain nested relations
        results.reserve(relations.size() * 2);

        for (auto relation : relations) {
            const auto isSelectConstraint = relation.name.contains(COLON);

            if (isSelectConstraint && relation.constraints)
                throw Orm::Exceptions::InvalidArgumentError(
                        "Passing both 'Select constraint' and 'Lambda expression "
                        "constraint' to the Model::with() method is not allowed, use "
                        "only one of them.");

            if (isSelectConstraint)
                relation = createSelectWithConstraint(relation.name);

            /* We need to separate out any nested includes, which allows the developers
               to load deep relationships using "dots" without stating each level of
               the relationship with its own key in the vector of eager-load names. */
            addNestedWiths(relation.name, results);

            results.append(std::move(relation));
        }

        return results;
    }

    template<typename Model>
    WithItem Builder<Model>::createSelectWithConstraint(const QString &name)
    {
        auto splitted = name.split(COLON);
        auto relation = splitted.at(0).trimmed();
        auto &columns = splitted[1];

        auto belongsToManyRelatedTable =
                m_model.getRelatedTableForBelongsToManyWithVisitor(relation);

        return {
            std::move(relation),
            [columns = std::move(columns),
                    belongsToManyRelatedTable = std::move(belongsToManyRelatedTable)]
                    (auto &query)
            {
                QVector<Column> columnsList;
                columnsList.reserve(columns.count(COMMA_C) + 1);

                // Avoid 'clazy might detach' warning
                for (const auto columns_ = columns.split(COMMA_C);
                     auto column : columns_)
                {
                    column = column.trimmed();

                    // Fully qualified column passed, not needed to process
                    if (column.contains(DOT)) {
                        columnsList << std::move(column);
                        continue;
                    }

                    /* Generate fully qualified column name for the BelongsToMany
                       relation. */
                    if (belongsToManyRelatedTable) {
#ifdef __GNUG__
                        columnsList << QString("%1.%2")
                                       .arg(*belongsToManyRelatedTable, column);
#else
                        columnsList << QStringLiteral("%1.%2")
                                       .arg(*belongsToManyRelatedTable, column);
#endif
                        continue;
                    }

                    columnsList << std::move(column);
                }

                // TODO move, query.select() silverqx
                query.select(std::move(columnsList));
            }
        };
    }

    template<typename Model>
    void Builder<Model>::addNestedWiths(const QString &name,
                                        QVector<WithItem> &results) const
    {
        QStringList progress;

        /* If the relation has already been set on the result vector, we will not set it
           again, since that would override any constraints that were already placed
           on the relationships. We will only set the ones that are not specified. */
        // Prevent container detach
        const auto names = name.split(DOT);

        progress.reserve(names.size());

        for (const auto &segment : names) {
            progress << segment;

            auto last = progress.join(DOT);
            const auto containsRelation = [&last](const auto &relation)
            {
                return relation.name == last;
            };
            const auto contains = ranges::contains(results, true, containsRelation);

            // Don't add a relation in the 'name' variable
            if (!contains && (last != name))
                results.append({std::move(last)});
        }
    }

    template<typename Model>
    QVector<WithItem>
    Builder<Model>::relationsNestedUnder(const QString &topRelationName) const
    {
        QVector<WithItem> nested;

        /* We are basically looking for any relationships that are nested deeper than
           the given top-level relationship. We will just check for any relations
           that start with the given top relations and adds them to our vectors. */
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
        return nestedRelation.contains(DOT)
                && nestedRelation.startsWith(QStringLiteral("%1.").arg(topRelation));
    }

    template<typename Model>
    QVector<UpdateItem>
    Builder<Model>::addUpdatedAtColumn(QVector<UpdateItem> values) const
    {
        const auto &updatedAtColumn = m_model.getUpdatedAtColumn();
        const auto &qualifiedUpdatedAtColumn = m_model.getQualifiedUpdatedAtColumn();

        if (!m_model.usesTimestamps() || updatedAtColumn.isEmpty())
            return values;

        const auto valuesUpdatedAtColumn =
                std::ranges::find_if(values,
                                     [&updatedAtColumn](const auto &updateItem)
        {
            return updateItem.column == updatedAtColumn;
        });

        // Not found
        if (valuesUpdatedAtColumn == std::ranges::cend(values))
            values.append({qualifiedUpdatedAtColumn,
                           m_model.freshTimestampString()});
        else
            // Rename updated_at column to the qualified column
            valuesUpdatedAtColumn->column = qualifiedUpdatedAtColumn;

        return values;
    }

    template<typename Model>
    Column
    Builder<Model>::getCreatedAtColumnForLatestOldest(Column column) const
    {
        /* Don't initialize column when user passed column expression, only when it
           holds the QString type. */
        if (std::holds_alternative<QString>(column) &&
            std::get<QString>(column).isEmpty()
        ) {
            if (const auto &createdAtColumn = m_model.getCreatedAtColumn();
                createdAtColumn.isEmpty()
            )
                column = CREATED_AT;
            else
                column = createdAtColumn;
        }

        return column;
    }

    template<typename Model>
    void Builder<Model>::enforceOrderBy()
    {
        if (!m_query->getOrders().isEmpty())
            return;

        this->orderBy(m_model.getQualifiedKeyName(), ASC);
    }

    // FEATURE scopes, anyway std::apply() do the same, will have to investigate it silverqx
//    template<typename Model>
//    template<typename ...Args>
//    Builder<Model> &
//    Builder<Model>::callScope(
//            const std::function<void(Builder &, Args ...)> &scope,
//            Args &&...parameters)
//    {
//        std::invoke(scope, *this, std::forward<Args>(parameters)...);

//        return *this;
//    }

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TINYBUILDER_HPP
