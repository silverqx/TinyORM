#pragma once
#ifndef ORM_TINY_TINYBUILDER_HPP
#define ORM_TINY_TINYBUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlRecord

#include <range/v3/action/transform.hpp>

#include "orm/databaseconnection.hpp"
#include "orm/utils/helpers.hpp"

#include "orm/tiny/concerns/buildsqueries.hpp"
#include "orm/tiny/concerns/buildssoftdeletes.hpp"
#include "orm/tiny/concerns/queriesrelationships.hpp"
#include "orm/tiny/exceptions/modelnotfounderror.hpp"
#include "orm/tiny/tinybuilderproxies.hpp"

#ifdef TINYORM_NO_DEBUG
#  include "orm/utils/query.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{

    /*! ORM Tiny builder (returns hydrated models instead of the QSqlQuery). */
    template<typename Model>
    class Builder : public Concerns::BuildsQueries<Model>,
                    public BuilderProxies<Model>,
                    public Concerns::QueriesRelationships<Model>,
                    public Concerns::BuildsSoftDeletes<Model, Model::extendsSoftDeletes()>
    {
        // Used by TinyBuilderProxies::where/latest/oldest/update()
        friend BuilderProxies<Model>;
        // To access enforceOrderBy(), and defaultKeyName()
        friend class Concerns::BuildsQueries<Model>;

        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;
        /*! Alias for the helper utils. */
        using Helpers = Orm::Utils::Helpers;
        /*! Alias for the query utils. */
        using QueryUtils = Orm::Utils::Query;
        /*! Alias for the type utils. */
        using TypeUtils = Orm::Utils::Type;

    public:
        /*! Constructor. */
        Builder(std::shared_ptr<QueryBuilder> &&query, const Model &model); // NOLINT(modernize-pass-by-value)
        /*! Default destructor. */
        ~Builder() = default;

        /*! Copy constructor (needed by the chunkById() -> clone() method). */
        Builder(const Builder &) = default;
        /*! Deleted copy assignment operator (not needed). */
        Builder &operator=(const Builder &) = delete;

        /*! Move constructor (copy ctor needed so enable also the move ctor). */
        Builder(Builder &&) noexcept = default;
        /*! Deleted move assignment operator (not needed). */
        Builder &operator=(Builder &&) = delete;

        /*! Get the SQL representation of the query. */
        inline QString toSql();
        /*! Get the current query value bindings as flattened QList. */
        inline QList<QVariant> getBindings() const;

        /* Retrieving results */
        /*! Execute the query as a "select" statement. */
        ModelsCollection<Model> get(const QList<Column> &columns = {ASTERISK});

        /*! Get a single column's value from the first result of a query. */
        QVariant value(const Column &column);
        /*! Get a single column's value from the first result of a query if it's
            the sole matching record. */
        QVariant soleValue(const Column &column);

        /*! Get a vector with values in the given column. */
        QList<QVariant> pluck(const Column &column);
        /*! Get a map with values in the given column and keyed by values in the key
            column. */
        template<typename T>
        std::map<T, QVariant> pluck(const Column &column, const Column &key);

        /*! Find a model by its primary key. */
        std::optional<Model>
        find(const QVariant &id, const QList<Column> &columns = {ASTERISK});
        /*! Find a model by its primary key or return fresh model instance. */
        Model findOrNew(const QVariant &id, const QList<Column> &columns = {ASTERISK});
        /*! Find a model by its primary key or throw an exception. */
        Model findOrFail(const QVariant &id,
                         const QList<Column> &columns = {ASTERISK});
        /*! Find multiple models by their primary keys. */
        ModelsCollection<Model>
        findMany(const QList<QVariant> &ids,
                 const QList<Column> &columns = {ASTERISK});

        /*! Execute a query for a single record by ID or call a callback. */
        std::optional<Model>
        findOr(const QVariant &id, const QList<Column> &columns,
               const std::function<void()> &callback = nullptr);
        /*! Execute a query for a single record by ID or call a callback. */
        std::optional<Model>
        findOr(const QVariant &id, const std::function<void()> &callback = nullptr);

        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        std::pair<std::optional<Model>, R>
        findOr(const QVariant &id, const QList<Column> &columns,
               const std::function<R()> &callback);
        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        std::pair<std::optional<Model>, R>
        findOr(const QVariant &id, const std::function<R()> &callback);

        /*! Execute the query and get the first result. */
        std::optional<Model> first(const QList<Column> &columns = {ASTERISK});
        /*! Get the first record matching the attributes or instantiate it. */
        Model firstOrNew(const QList<WhereItem> &attributes = {},
                         const QList<AttributeItem> &values = {});
        /*! Get the first record matching the attributes or create it. */
        Model firstOrCreate(const QList<WhereItem> &attributes = {},
                            const QList<AttributeItem> &values = {});
        /*! Execute the query and get the first result or throw an exception. */
        Model firstOrFail(const QList<Column> &columns = {ASTERISK});

        /*! Execute the query and get the first result or call a callback. */
        std::optional<Model>
        firstOr(const QList<Column> &columns,
                const std::function<void()> &callback = nullptr);
        /*! Execute the query and get the first result or call a callback. */
        std::optional<Model>
        firstOr(const std::function<void()> &callback = nullptr);

        /*! Execute the query and get the first result or call a callback. */
        template<typename R>
        std::pair<std::optional<Model>, R>
        firstOr(const QList<Column> &columns, const std::function<R()> &callback);
        /*! Execute the query and get the first result or call a callback. */
        template<typename R>
        std::pair<std::optional<Model>, R>
        firstOr(const std::function<R()> &callback);

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
        Builder &whereKey(const QList<QVariant> &ids);
        /*! Add a where clause on the primary key to the query. */
        Builder &whereKeyNot(const QVariant &id);
        /*! Add a where clause on the primary key to the query. */
        Builder &whereKeyNot(const QList<QVariant> &ids);

        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        Builder &with(const QList<WithItem> &relations);
        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        Builder &with(QString relation);
        /*! Set the relationships that should be eager loaded. */
        inline Builder &with(const QList<QString> &relations);
        /*! Set the relationships that should be eager loaded. */
        inline Builder &with(QList<QString> &&relations);

        /*! Prevent the specified relations from being eager loaded. */
        Builder &without(const QList<QString> &relations);
        /*! Prevent the specified relations from being eager loaded. */
        inline Builder &without(QString relation);

        /*! Set the relationships that should be eager loaded while removing
            any previously added eager loading specifications. */
        template<typename = void>
        Builder &withOnly(const QList<WithItem> &relations);
        /*! Set the relationship that should be eager loaded while removing
            any previously added eager loading specifications. */
        template<typename = void>
        Builder &withOnly(QString relation);
        /*! Set the relationships that should be eager loaded while removing
            any previously added eager loading specifications. */
        inline Builder &withOnly(const QList<QString> &relations);
        /*! Set the relationships that should be eager loaded while removing
            any previously added eager loading specifications. */
        inline Builder &withOnly(QList<QString> &&relations);

        /* Insert, Update, Delete */
        /*! Save a new model and return the instance. */
        Model create(const QList<AttributeItem> &attributes = {});
        /*! Save a new model and return the instance. */
        Model create(QList<AttributeItem> &&attributes = {});

        /*! Create or update a record matching the attributes, and fill it with
            values. */
        Model updateOrCreate(const QList<WhereItem> &attributes,
                             const QList<AttributeItem> &values = {});

        /*! Update the column's update timestamp. */
        std::tuple<int, std::optional<TSqlQuery>>
        touch(const QString &column = "");

        /* QueryBuilder proxy methods that need modifications */
        /*! Update records in the database. */
        std::tuple<int, TSqlQuery>
        update(const QList<UpdateItem> &values);

        /*! Delete records from the database. */
        std::tuple<int, TSqlQuery> remove();
        /*! Delete records from the database, alias. */
        inline std::tuple<int, TSqlQuery> deleteModels();

        /*! Insert new records or update the existing ones. */
        std::tuple<int, std::optional<TSqlQuery>>
        upsert(const QList<QVariantMap> &values, const QStringList &uniqueBy,
               const QStringList &update);
        /*! Insert new records or update the existing ones (update all columns). */
        std::tuple<int, std::optional<TSqlQuery>>
        upsert(const QList<QVariantMap> &values, const QStringList &uniqueBy);

        /* Casting Attributes */
        /*! Apply query-time casts to the model instance. */
        inline Builder &withCasts(const std::unordered_map<QString, CastItem> &casts);
        /*! Apply query-time casts to the model instance. */
        inline Builder &withCasts(std::unordered_map<QString, CastItem> &casts);
        /*! Apply query-time casts to the model instance. */
        inline Builder &withCasts(std::unordered_map<QString, CastItem> &&casts);
        /*! Apply query-time cast to the model instance. */
        inline Builder &withCast(std::pair<QString, CastItem> cast);

        /* TinyBuilder methods */
        /*! Clone the Tiny query. */
        inline Builder clone() const;
        /*! Create a new instance of the model being queried. */
        Model newModelInstance(const QList<AttributeItem> &attributes) const;
        /*! Create a new instance of the model being queried. */
        Model newModelInstance(QList<AttributeItem> &&attributes = {}) const;

        /*! Get the hydrated models without eager loading. */
        ModelsCollection<Model> getModels(const QList<Column> &columns = {ASTERISK});

        /*! Eager load the relationships for the models. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        void eagerLoadRelations(ModelsCollection<CollectionModel> &models) const;
        /*! Eager load the relationships on the model. */
        void eagerLoadRelations(Model &model) const;

        /*! Eagerly load the relationship on a set of models. */
        template<typename Relation, SameDerivedCollectionModel<Model> CollectionModel>
        void eagerLoadRelationVisited(
                Relation &relation, ModelsCollection<CollectionModel> &models,
                const WithItem &relationItem) const;

        /*! Create a vector of models from the SqlQuery. */
        ModelsCollection<Model> hydrate(SqlQuery &&result) const; // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)

        /*! Get the model instance being queried. */
        inline Model &getModel() noexcept;
        /*! Get the underlying query builder instance. */
        inline QueryBuilder &getQuery() const noexcept;
        /*! Get the underlying query builder instance as a std::shared_ptr. */
        inline const std::shared_ptr<QueryBuilder> &getQueryShared() const noexcept;

        /*! Get a database connection. */
        inline DatabaseConnection &getConnection();
        /*! Get the query grammar instance. */
        inline const QueryGrammar &getGrammar();

        /*! Get a base query builder instance. */
        QueryBuilder &toBase();

        /*! Qualify the given column name by the model's table. */
        inline QString qualifyColumn(const QString &column) const;

        /*! Register a replacement for the default delete function. */
        inline void
        onDelete(std::function<std::tuple<int, TSqlQuery>(Builder<Model> &)> &&callback);

        /* BuildsSoftDeletes */
        /*! Apply the SoftDeletes where null condition for the deleted_at column. */
        Builder<Model> &applySoftDeletes();
        /*! Determine whether the Model the TinyBuilder manages extends SoftDeletes. */
        constexpr static bool extendsSoftDeletes() noexcept;

    protected:
        /*! Alias for the Expression. */
        using Expression = Orm::Query::Expression;

        /*! Get the default key name of the table. */
        inline const QString &defaultKeyName() const;

        /*! Parse a list of relations into individuals. */
        QList<WithItem> parseWithRelations(const QList<WithItem> &relations);
        /*! Create a constraint to select the given columns for the relation. */
        WithItem createSelectWithConstraint(const QString &name);
        /*! Parse the nested relationships in a relation. */
        void addNestedWiths(const QString &name, QList<WithItem> &results) const;

        /*! Guess number of relations for the reserve (including nested relations). */
        static QList<WithItem>::size_type
        guessParseWithRelationsSize(const QList<WithItem> &relations);

        /*! Get the deeply nested relations for a given top-level relation. */
        QList<WithItem>
        relationsNestedUnder(const QString &topRelationName) const;
        /*! Determine if the relationship is nested. */
        bool isNestedUnder(const QString &topRelation,
                           const QString &nestedRelation) const;

        /*! Add the "updated at" column to the vector of values. */
        QList<UpdateItem>
        addUpdatedAtColumn(QList<UpdateItem> values) const;

        /*! Add timestamps to the inserted values. */
        QList<QVariantMap>
        addTimestampsToUpsertValues(const QList<QVariantMap> &values) const;
        /*! Add the "updated at" column to the updated columns. */
        QStringList addUpdatedAtToUpsertColumns(const QStringList &update) const;

        /*! Get the name of the "created at" column. */
        Column getCreatedAtColumnForLatestOldest(Column column) const;

        /*! Add a generic "order by" clause if the query doesn't already have one. */
        void enforceOrderBy();

        /*! Apply the given scope on the current builder instance. */
//        template<typename ...Args>
//        Builder &callScope(const std::function<void(Builder &, Args ...)> &scope,
//                           Args &&...parameters);

        /*! The base query builder instance. */
        /*const*/ std::shared_ptr<QueryBuilder> m_query;
        /* This can't be a reference because the model is created on the stack
           in Model::query(), then copied here and the original is destroyed
           immediately. */
        /*! The model being queried. */
        Model m_model;
        /*! The relationships that should be eager loaded. */
        QList<WithItem> m_eagerLoad;

        /*! A replacement for the typical delete function. */
        std::function<std::tuple<int, TSqlQuery>(Builder<Model> &)> m_onDelete = nullptr;

        /* BuildsSoftDeletes */
        /*! Determine whether the Model the TinyBuilder manages extends SoftDeletes. */
        constexpr static bool m_extendsSoftDeletes = Model::extendsSoftDeletes();
    };

    /* public */

    template<typename Model>
    Builder<Model>::Builder(std::shared_ptr<QueryBuilder> &&query, const Model &model)
        : m_query(std::move(query))
        , m_model(model)
    {
        m_query->from(m_model.getTable());

        // Initialize the BuildsSoftDeletes concern (registers onDelete callback)
        if constexpr (m_extendsSoftDeletes)
            this->initializeBuildsSoftDeletes();
    }

    template<typename Model>
    QString Builder<Model>::toSql()
    {
        return toBase().toSql();
    }

    template<typename Model>
    QList<QVariant> Builder<Model>::getBindings() const
    {
        // toBase() not needed as the BuildsSoftDeletes is not adding any new bindings
        return getQuery().getBindings();
    }

    /* Retrieving results */

    template<typename Model>
    ModelsCollection<Model>
    Builder<Model>::get(const QList<Column> &columns)
    {
        applySoftDeletes();

        ModelsCollection<Model> models = getModels(columns);

        /* If we actually found models we will also eager load any relationships that
           have been specified as needing to be eager loaded, which will solve the
           n+1 query issue for the developers to avoid running a lot of queries. */
        if (!models.isEmpty())
            /* 'models' are passed down as the reference and relations are set on models
               at the end of the call tree, no need to return models. */
            eagerLoadRelations(models);

        return models;
        // FUTURE if I will implement custom container for the Models, this is right place to do it silverqx
//        return getModel().newCollection(models);
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

        if (column_.contains(DOT))
            column_ = column_.sliced(column_.lastIndexOf(DOT) + 1);

        return model->getAttribute(column_);
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

        if (column_.contains(DOT))
            column_ = column_.sliced(column_.lastIndexOf(DOT) + 1);

        return model.getAttribute(column_);
    }

    template<typename Model>
    QList<QVariant> Builder<Model>::pluck(const Column &column)
    {
        auto result = toBase().pluck(column);

        // Nothing to pluck-ing 😎
        if (result.empty())
            return result;

        /* If the column is qualified with a table or have an alias, we cannot use
           those directly in the "pluck" operations, we have to strip the table out or
           use the alias name instead. */
        const auto unqualifiedColumn = getQuery().stripTableForPluck(column);

        /* If the model has a mutator for the requested column or it's a datetime column,
           we will spin through the results and mutate the values so that the mutated
           version of these columns are returned as you would expect from these TinyORM
           models. */
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
    Builder<Model>::pluck(const Column &column, const Column &key)
    {
        auto result = toBase().template pluck<T>(column, key);

        // Nothing to pluck-ing 😎
        if (result.empty())
            return result;

        /* If the column is qualified with a table or have an alias, we cannot use
           those directly in the "pluck" operations, we have to strip the table out or
           use the alias name instead. */
        const auto unqualifiedColumn = getQuery().stripTableForPluck(column);

        /* If the model has a mutator for the requested column or it's a datetime column,
           we will spin through the results and mutate the values so that the mutated
           version of these columns are returned as you would expect from these TinyORM
           models. */
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
    Builder<Model>::find(const QVariant &id, const QList<Column> &columns)
    {
        return whereKey(id).first(columns);
    }

    template<typename Model>
    Model Builder<Model>::findOrNew(const QVariant &id, const QList<Column> &columns)
    {
        auto model = find(id, columns);

        // Found
        if (model)
            return std::move(*model);

        return newModelInstance();
    }

    template<typename Model>
    Model Builder<Model>::findOrFail(const QVariant &id, const QList<Column> &columns)
    {
        auto model = find(id, columns);

        // Found
        if (model)
            return std::move(*model);

        throw Exceptions::ModelNotFoundError(
                    TypeUtils::classPureBasename<Model>(), {id});
    }

    template<typename Model>
    ModelsCollection<Model>
    Builder<Model>::findMany(const QList<QVariant> &ids,
                             const QList<Column> &columns)
    {
        if (ids.isEmpty())
            return {};

        return whereKey(ids).get(columns);
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::findOr(const QVariant &id, const QList<Column> &columns,
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
    Builder<Model>::findOr(const QVariant &id, const QList<Column> &columns,
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
    Builder<Model>::first(const QList<Column> &columns)
    {
        auto models = this->take(1).get(columns);

        if (models.isEmpty())
            return std::nullopt;

        return std::move(models.first());
    }

    template<typename Model>
    Model
    Builder<Model>::firstOrNew(const QList<WhereItem> &attributes,
                               const QList<AttributeItem> &values)
    {
        auto instance = this->where(attributes).first();

        // Model found in db
        if (instance)
            return std::move(*instance);

        return newModelInstance(AttributeUtils::joinAttributesForFirstOr(
                                    attributes, values, m_model.getKeyName()));
    }

    template<typename Model>
    Model
    Builder<Model>::firstOrCreate(const QList<WhereItem> &attributes,
                                  const QList<AttributeItem> &values)
    {
        // Model found in db
        if (auto instance = this->where(attributes).first(); instance)
            return std::move(*instance);

        return Helpers::tap<Model>(
                    newModelInstance(AttributeUtils::joinAttributesForFirstOr(
                                         attributes, values, m_model.getKeyName())),
                    [](auto &newInstance)
        {
            newInstance.save();
        });
    }

    template<typename Model>
    Model Builder<Model>::firstOrFail(const QList<Column> &columns)
    {
        auto model = first(columns);

        // Found
        if (model)
            return std::move(*model);

        throw Exceptions::ModelNotFoundError(TypeUtils::classPureBasename<Model>());
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::firstOr(const QList<Column> &columns,
                            const std::function<void()> &callback)
    {
        auto model = first(columns);

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
    Builder<Model>::firstOr(const std::function<void()> &callback)
    {
        return firstOr({ASTERISK}, callback);
    }

    template<typename Model>
    template<typename R>
    std::pair<std::optional<Model>, R>
    Builder<Model>::firstOr(const QList<Column> &columns,
                            const std::function<R()> &callback)
    {
        auto model = first(columns);

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
    Builder<Model>::firstOr(const std::function<R()> &callback)
    {
        return firstOr<R>({ASTERISK}, callback);
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
    Builder<Model>::whereKey(const QList<QVariant> &ids)
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
    Builder<Model>::whereKeyNot(const QList<QVariant> &ids)
    {
        m_query->whereNotIn(m_model.getQualifiedKeyName(), ids);

        return *this;
    }

    template<typename Model>
    template<typename>
    Builder<Model> &
    Builder<Model>::with(const QList<WithItem> &relations)
    {
        /* Don't make the rvalue variant or pass relations by value, I have tested it and
           it's ~0.4ms slower, very interesting. 😮 Talking about the with() and
           the parseWithRelations() methods. */
        auto eagerLoad = parseWithRelations(relations);

        m_eagerLoad.reserve(eagerLoad.size());

        std::ranges::move(eagerLoad, std::back_inserter(m_eagerLoad));

        return *this;
    }

    template<typename Model>
    template<typename>
    Builder<Model> &
    Builder<Model>::with(QString relation)
    {
        return with(QList<WithItem> {{std::move(relation)}});
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::with(const QList<QString> &relations)
    {
        return with(WithItem::fromStringVector(relations));
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::with(QList<QString> &&relations)
    {
        return with(WithItem::fromStringVector(std::move(relations)));
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::without(const QList<QString> &relations)
    {
        // Remove relations in the "relations" vector from the m_eagerLoad vector
        m_eagerLoad = m_eagerLoad
                | ranges::views::remove_if([&relations](const WithItem &with)
        {
            return relations.contains(with.name);
        })
                | ranges::to<QList<WithItem>>();

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::without(QString relation)
    {
        return without(QList<QString> {std::move(relation)});
    }

    template<typename Model>
    template<typename>
    Builder<Model> &
    Builder<Model>::withOnly(const QList<WithItem> &relations)
    {
        m_eagerLoad.clear();

        return with(relations);
    }

    template<typename Model>
    template<typename>
    Builder<Model> &
    Builder<Model>::withOnly(QString relation)
    {
        return withOnly(QList<WithItem> {{std::move(relation)}});
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::withOnly(const QList<QString> &relations)
    {
        return withOnly(WithItem::fromStringVector(relations));
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::withOnly(QList<QString> &&relations)
    {
        return withOnly(WithItem::fromStringVector(std::move(relations)));
    }

    /* Insert, Update, Delete */

    template<typename Model>
    Model Builder<Model>::create(const QList<AttributeItem> &attributes)
    {
        auto model = newModelInstance(attributes);

        model.save();

        return model;
    }

    template<typename Model>
    Model Builder<Model>::create(QList<AttributeItem> &&attributes)
    {
        auto model = newModelInstance(std::move(attributes));

        model.save();

        return model;
    }

    template<typename Model>
    Model Builder<Model>::updateOrCreate(const QList<WhereItem> &attributes,
                                         const QList<AttributeItem> &values)
    {
        auto instance = firstOrNew(attributes);

        instance.fill(values).save();

        return instance;
    }

    template<typename Model>
    std::tuple<int, std::optional<TSqlQuery>>
    Builder<Model>::touch(const QString &column)
    {
        auto time = m_model.freshTimestamp();

        if (!column.isEmpty())
            return toBase().update({{column, std::move(time)}});

        const auto &updatedAtColumn = m_model.getUpdatedAtColumn();

        if (!m_model.usesTimestamps() || updatedAtColumn.isEmpty())
            return {0, std::nullopt};

        return toBase().update({{updatedAtColumn, std::move(time)}});
    }

    /* QueryBuilder proxy methods that need modifications */

    template<typename Model>
    std::tuple<int, TSqlQuery>
    Builder<Model>::update(const QList<UpdateItem> &values)
    {
        return toBase().update(addUpdatedAtColumn(values));
    }

    template<typename Model>
    std::tuple<int, TSqlQuery> Builder<Model>::remove()
    {
        // Custom onDelete callback registered
        if (m_onDelete)
            return std::invoke(m_onDelete, *this);

        return toBase().deleteRow();
    }

    template<typename Model>
    std::tuple<int, TSqlQuery> Builder<Model>::deleteModels()
    {
        return remove();
    }

    template<typename Model>
    std::tuple<int, std::optional<TSqlQuery>>
    Builder<Model>::upsert(
            const QList<QVariantMap> &values, const QStringList &uniqueBy,
            const QStringList &update)
    {
        // Nothing to do, no values to insert or update
        if (values.isEmpty())
            return {0, std::nullopt};

        // NOTE api different, don't call insert, it's useless silverqx
        // If the update is an empty vector then throw and don't insert
        if (update.isEmpty())
            throw Orm::Exceptions::InvalidArgumentError(
                    "The upsert method doesn't support an empty update argument, please "
                    "use the insert method instead.");

        return toBase().upsert(addTimestampsToUpsertValues(values), uniqueBy,
                               addUpdatedAtToUpsertColumns(update));
    }

    template<typename Model>
    std::tuple<int, std::optional<TSqlQuery>>
    Builder<Model>::upsert(
            const QList<QVariantMap> &values, const QStringList &uniqueBy)
    {
        // Update all columns
        // Columns are obtained only from a first QMap
        const auto update = values.constFirst().keys();

        return upsert(values, uniqueBy, update);
    }

    /* Casting Attributes */

    template<typename Model>
    Builder<Model> &
    Builder<Model>::withCasts(const std::unordered_map<QString, CastItem> &casts)
    {
        m_model.mergeCasts(casts);

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::withCasts(std::unordered_map<QString, CastItem> &casts)
    {
        m_model.mergeCasts(casts);

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::withCasts(std::unordered_map<QString, CastItem> &&casts)
    {
        m_model.mergeCasts(std::move(casts));

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::withCast(std::pair<QString, CastItem> cast)
    {
        m_model.mergeCasts({std::move(cast)});

        return *this;
    }

    /* TinyBuilder methods */

    template<typename Model>
    Builder<Model> Builder<Model>::clone() const
    {
        return *this;
    }

    template<typename Model>
    Model Builder<Model>::newModelInstance(const QList<AttributeItem> &attributes) const
    {
        return m_model.newInstance(attributes)
                .setConnection(m_query->getConnection().getName());
    }

    template<typename Model>
    Model Builder<Model>::newModelInstance(QList<AttributeItem> &&attributes) const
    {
        return m_model.newInstance(std::move(attributes))
                .setConnection(m_query->getConnection().getName());
    }

    template<typename Model>
    ModelsCollection<Model>
    Builder<Model>::getModels(const QList<Column> &columns)
    {
        return hydrate(m_query->get(columns));
    }

    // TODO docs add similar note for lazy load silverqx
    /* Look also at EagerRelationStore::visited(), where the whole flow begins.
       How this relation flow works:
       EagerRelationStore::visited() obtains a reference by the relation name
       to the relation method, these relation methods are defined on models
       as member functions.
       References to the relation methods are defined in the Model::u_relations
       hash as lambda expressions. These lambda expressions will be visited/invoked
       by the EagerRelationStore::visited() to obtain references to the relation
       methods.
       Relation constraints will be disabled for eager relations by
       the Relation::noConstraints() method, these default constraints are only used
       for lazy loading, for eager constraints are used constraints, which are
       defined by the Relation::addEagerConstraints() virtual methods.
       To the Relation::noConstraints() method is passed a lambda, which invokes
       obtained reference to the relation method defined on the model and invokes it
       on the 'new' model instance refered as the 'dummyModel'.
       The Relation instance is created by this relation method, this relation
       method calls a factory method, which creates this Relation instance.
       Every Relation has it's own Relation::create() factory method, to which
       the following parameters are passed, a newly created Related model instance,
       current/parent model instance, database column names of the relationship, and
       for BelongsTo relations also the name of a relation.
       The Relation instance saves a non-const reference to the current/parent model
       instance, a copy of the related model instance because it is created
       on the stack.
       The Relation instance creates a new TinyBuilder instance from the Related
       model instance using the TinyBuilder::newQuery() and saves a ownership as
       the shared pointer (because this class is copyable).
       Then eager constraints are applied to this newly created TinyBuilder and
       the result is returned back to the initial model.
       The result is transformed into models and these models are hydrated.
       Hydrated models are saved to the templated Model::m_relations data member
       hash.
       Also, look the NOTES.txt for eagerLoadRelations() and Model::load() history. */

    template<typename Model>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    void
    Builder<Model>::eagerLoadRelations(ModelsCollection<CollectionModel> &models) const
    {
        // Nothing to load
        if (m_eagerLoad.isEmpty())
            return;

        for (const auto &relation : m_eagerLoad)
            /* For nested eager loads we'll skip loading them here and they will be
               loaded later using the nested query which retrieves this nested relations,
               because that is where they get hydrated as models. */
            if (!relation.name.contains(DOT))
                /* Get the relation instance for the given relation name, have to be done
                   through the visitor pattern which obtains also the Related type.
                   After the visitation the eagerLoadRelationVisited() will be called. */
                m_model.eagerLoadRelationWithVisitor(relation, *this, models);
    }

    template<typename Model>
    void Builder<Model>::eagerLoadRelations(Model &model) const
    {
        // Nothing to load
        if (m_eagerLoad.isEmpty())
            return;

        /* The eagerLoadRelations() methods chain can only operate on a ModelsCollection
           it can't accept a single Model instance, would be possible to create
           a templated EagerRelationStore that would accept a single Model but the effort
           is not worth it.
           Also, I checked now what would be required to implement this and it would be
           really sketchy as all methods like addEagerConstraints(), initRelation(),
           match(), getKeys() would have to be able to operate on a single Model. */
        ModelsCollection<Model *> models {&model};

        eagerLoadRelations(models);
    }

    template<typename Model>
    template<typename Relation, SameDerivedCollectionModel<Model> CollectionModel>
    void Builder<Model>::eagerLoadRelationVisited(
            Relation &relation, ModelsCollection<CollectionModel> &models,
            const WithItem &relationItem) const
    {
        /* First we will "back up" the existing where conditions (Relation::noConstraints)
           on the query so we can add our eager constraints, this is done
           in the EagerRelationStore::visited() using the Relation::noConstraints().
           Following is true for the relationItem.constraints (Constraining Eager Loads):
           Then we will merge the user defined constraints that were on the query
           back to it, this ensures that a user can specify any where constraints or
           ordering (where, orderBy, and maybe more). */
        auto nested = relationsNestedUnder(relationItem.name);

        /* If there are nested relationships set on this query, we will put those onto
           the relation's query instance so they can be handled after this relationship
           is loaded. In this way they will all trickle down as they are loaded. */
        if (nested.size() > 0)
            relation->getQuery().with(std::move(nested));

        relation->addEagerConstraints(models);

        // Add relation constraints defined in the user callback
        // NOTE api different, Eloquent is passing the Relation reference into the lambda, it would be almost impossible to do it silverqx
        if (relationItem.constraints)
            std::invoke(relationItem.constraints, relation->getBaseQuery());

        /* Once we have the results, we just match those back up to their parent models
           using the relationship instance. Then we just return the finished vector
           of models which have been eagerly hydrated and are readied for return. */
        relation->match(relation->initRelation(models, relationItem.name),
                        relation->getEager(), relationItem.name);
    }

    template<typename Model>
    ModelsCollection<Model>
    Builder<Model>::hydrate(SqlQuery &&result) const // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        auto instance = newModelInstance();

        ModelsCollection<Model> models;
        models.reserve(QueryUtils::queryResultSize(result));

        while (result.next()) {
            const auto record = result.record();
            const auto fieldsCount = record.count();

            QList<AttributeItem> row;
            row.reserve(fieldsCount);

            // Populate model attributes with data from the database (one table row)
            for (int i = 0; i < fieldsCount; ++i)
                row.append({record.fieldName(i), result.value(i)});

            // Create a new model instance from the table row
            models << instance.newFromBuilder(std::move(row));
        }

        return models;
    }

    template<typename Model>
    Model &Builder<Model>::getModel() noexcept
    {
        return m_model;
    }

    template<typename Model>
    QueryBuilder &Builder<Model>::getQuery() const noexcept
    {
        return *m_query;
    }

    template<typename Model>
    const std::shared_ptr<QueryBuilder> &
    Builder<Model>::getQueryShared() const noexcept
    {
        return m_query;
    }

    template<typename Model>
    DatabaseConnection &
    Builder<Model>::getConnection()
    {
        return toBase().getConnection();
    }

    template<typename Model>
    const QueryGrammar &
    Builder<Model>::getGrammar()
    {
        return toBase().getGrammar();
    }

    template<typename Model>
    QueryBuilder &Builder<Model>::toBase()
    {
        // FUTURE add Query Scopes feature silverqx
        // retutn applyScopes().getQuery();

        return applySoftDeletes().getQuery();
    }

    template<typename Model>
    QString Builder<Model>::qualifyColumn(const QString &column) const
    {
        return m_model.qualifyColumn(column);
    }

    template<typename Model>
    void Builder<Model>::onDelete(
            std::function<std::tuple<int, TSqlQuery>(Builder<Model> &)> &&callback)
    {
        m_onDelete = std::move(callback);
    }

    /* BuildsSoftDeletes */

    template<typename Model>
    Builder<Model> &Builder<Model>::applySoftDeletes()
    {
        if constexpr (m_extendsSoftDeletes)
            return Concerns::BuildsSoftDeletes<Model, true>::applySoftDeletes();
        else
            return *this;
    }

    template<typename Model>
    constexpr bool Builder<Model>::extendsSoftDeletes() noexcept
    {
        return m_extendsSoftDeletes;
    }

    /* protected */

    template<typename Model>
    const QString &Builder<Model>::defaultKeyName() const
    {
        return m_model.getKeyName();
    }

    template<typename Model>
    QList<WithItem>
    Builder<Model>::parseWithRelations(const QList<WithItem> &relations)
    {
        // Guess number of relations (including nested relations)
        const auto relationsSize = guessParseWithRelationsSize(relations);

        // Nothing to prepare (no nested relations)
        if (relationsSize == 0)
            return {};

        QList<WithItem> results;
        results.reserve(relationsSize);

        for (auto relation : relations) {
            const auto isSelectConstraint = relation.name.contains(COLON);

            if (isSelectConstraint && relation.constraints)
                throw Orm::Exceptions::InvalidArgumentError(
                        "Passing both 'Select constraint' and 'Lambda expression "
                        "constraint' to the Model::with() method is not allowed, use "
                        "only one of them.");

            /* I have to write a note here, this !relation.name.contains(DOT) bugfix
               was unbelievable, it took me a whole day of debugging, writing unit tests,
               and figuring out how to solve this problem. I wrote dozen of lines to make
               it work but at the end I started removing what was not needed and ended
               with this one condition. 😮🙃
               If the relation name is a nested relation, then the select constraints
               lambda will not be generated and will be nullptr, so will be skipped here,
               the problem was that the getRelatedTableForBelongsToManyWithVisitor()
               could not be invoked correctly because it's a nested relation.
               The getRelatedTableForBelongsToManyWithVisitor() will be visited or
               resolved later, right before it will be needed and it will be done during
               relation->getQuery().with(std::move(nested));
               in the eagerLoadRelationVisited(), the magic is done
               in the relationsNestedUnder() when the nested relation is unwrapped.
               The super paradox is that this was the first think I wrote but I still got
               crash and then a whole day of fixing started. 🐛 */
            if (isSelectConstraint && !relation.name.contains(DOT))
                relation = createSelectWithConstraint(relation.name);

            /* We need to separate out any nested includes, which allows the developers
               to load deep relationships using "dots" without stating each level of
               the relationship with its own key in the vector of eager-load names. */
            addNestedWiths(relation.name, results);

            results << std::move(relation);
        }

        return results;
    }

    template<typename Model>
    WithItem Builder<Model>::createSelectWithConstraint(const QString &name)
    {
        auto splitted = name.split(COLON);
        auto relation = splitted.constFirst().trimmed();
        auto &columns = splitted[1];

        /* Get the Related model table name if the relation is BelongsToMany, otherwise
           return an empty std::optional. */
        auto belongsToManyRelatedTable =
                m_model.getRelatedTableForBelongsToManyWithVisitor(relation);

        // Move the relation and also values to the lambda, to avoid dangling references
        return {
            std::move(relation),
            [columns = std::move(columns),
             belongsToManyRelatedTable = std::move(belongsToManyRelatedTable)]
            (QueryBuilder &query)
            {
                const auto columnsSplitted = QStringView(columns)
                                             .split(COMMA_C, Qt::SkipEmptyParts);

                // Nothing to do
                if (columnsSplitted.isEmpty())
                    return;

                QList<Column> columnsList;
                columnsList.reserve(columnsSplitted.size());

                // Avoid 'clazy might detach' warning
                for (const auto column_ : columnsSplitted)
                {
                    const auto column = column_.trimmed();

                    /* Nothing to process, already a fully qualified column name or
                       it's not a column for the BelongsToMany relation, in this case,
                       an unqualified column name is ok. */
                    if (column.contains(DOT) || !belongsToManyRelatedTable) {
                        columnsList << column.toString();
                        continue;
                    }

                    /* Generate the fully qualified column name for the BelongsToMany
                       relation. */
                    columnsList << DOT_IN.arg(*belongsToManyRelatedTable, column);
                }

                query.select(std::move(columnsList));
            }
        };
    }

    template<typename Model>
    void Builder<Model>::addNestedWiths(const QString &name,
                                        QList<WithItem> &results) const
    {
        /* If the relation has already been set on the result vector, we will not set it
           again, since that would override any constraints that were already placed
           on the relationships. We will only set the ones that are not specified. */
        auto names = name.split(DOT, Qt::SkipEmptyParts, Qt::CaseSensitive);

        // Nothing to do (no nested relations)
        if (names.isEmpty())
            return;

        QStringList progress;
        progress.reserve(names.size());

        for (auto &&segment : names) {
            progress << std::move(segment);

            auto last = progress.join(DOT);
            const auto containsRelation = [&last](const auto &relation)
            {
                return relation.name == last;
            };
            const auto contains = ranges::contains(results, true, containsRelation);

            // Don't add a relation in the 'name' variable
            if (!contains && last != name)
                results.append({std::move(last)});
        }
    }

    template<typename Model>
    QList<WithItem>::size_type
    Builder<Model>::guessParseWithRelationsSize(const QList<WithItem> &relations)
    {
        QList<WithItem>::size_type size = 0;

        for (const auto &[relation, _] : relations)
            // Nested relations (x.y.z == 3 relations)
            if (relation.contains(DOT))
                size += relation.count(DOT) + 1;

            // All others, with the ':' (Select Constraints) or only the relation name
            else
                ++size;

        return size;
    }

    template<typename Model>
    QList<WithItem>
    Builder<Model>::relationsNestedUnder(const QString &topRelationName) const
    {
        /*! Count the number of nested relations, it always returns the qint64 difference
            type. */
        const auto nestedSize = std::ranges::count_if(
                                    m_eagerLoad, [this, &topRelationName]
                                                 (const auto &relation)
        {
            return isNestedUnder(topRelationName, relation.name);
        });

        // Nothing to prepare (no nested relations)
        if (nestedSize == 0)
            return {};

        QList<WithItem> nested;
        nested.reserve(nestedSize);

        /* We are basically looking for any relationships that are nested deeper than
           the given top-level relationship. We will just check for any relations
           that start with the given top relations and add them to our vector. */
        for (const auto &[relationName, constraints] : m_eagerLoad)
            if (isNestedUnder(topRelationName, relationName))
#if defined(__clang__) && __clang_major__ < 16
                nested.append({relationName.mid(topRelationName.size() + 1),
                               constraints});
#else
                nested.emplaceBack(relationName.sliced(topRelationName.size() + 1),
                                   constraints);
#endif

        return nested;
    }

    template<typename Model>
    bool Builder<Model>::isNestedUnder(const QString &topRelation,
                                       const QString &nestedRelation) const
    {
        return nestedRelation.contains(DOT) &&
               nestedRelation.startsWith(QStringLiteral("%1.").arg(topRelation));
    }

    template<typename Model>
    QList<UpdateItem>
    Builder<Model>::addUpdatedAtColumn(QList<UpdateItem> values) const
    {
        const auto &updatedAtColumn = m_model.getUpdatedAtColumn();

        // Nothing to do (model doesn't use timestamps)
        if (!m_model.usesTimestamps() || updatedAtColumn.isEmpty())
            return values;

        /*! Find updated_at column. */
        const auto valuesUpdatedAtColumn =
                std::ranges::find_if(values,
                                     [&updatedAtColumn](const auto &updateItem)
        {
            return updateItem.column == updatedAtColumn;
        });

        auto qualifiedUpdatedAtColumn = m_model.getQualifiedUpdatedAtColumn();

        // Not found, append a fresh timestamp
        if (valuesUpdatedAtColumn == std::ranges::cend(values))
            values.append({std::move(qualifiedUpdatedAtColumn),
                           m_model.freshTimestampString()});

        // Found, rename the updated_at column to the qualified column
        else
            valuesUpdatedAtColumn->column = std::move(qualifiedUpdatedAtColumn);

        return values;
    }

    template<typename Model>
    QList<QVariantMap>
    Builder<Model>::addTimestampsToUpsertValues(const QList<QVariantMap> &values) const
    {
        // Nothing to do (model doesn't use timestamps)
        if (!m_model.usesTimestamps())
            return values;

        // Prepare timestamp columns to add
        std::vector<QString> columns;
        columns.reserve(2);

        // Don't use qualified columns here, they are not needed
        if (const auto &createdAtColumn = m_model.getCreatedAtColumn();
            !createdAtColumn.isEmpty()
        )
            columns.push_back(createdAtColumn);
        if (const auto &updatedAtColumn = m_model.getUpdatedAtColumn();
            !updatedAtColumn.isEmpty()
        )
            columns.push_back(updatedAtColumn);

        // Nothing to insert, both timestamp column names are empty 🙃
        if (columns.empty())
            return values;

        const auto timestamp = m_model.freshTimestampString();
        auto valuesCopy = values;

        // Insert timestamp columns if a row doesn't already contain one
        for (const auto &column : columns)
            for (auto &row : valuesCopy)
                if (!row.contains(column))
                    row.insert(column, timestamp);

        return valuesCopy;
    }

    template<typename Model>
    QStringList
    Builder<Model>::addUpdatedAtToUpsertColumns(const QStringList &update) const
    {
        // Nothing to do (model doesn't use timestamps)
        if (!m_model.usesTimestamps())
            return update;

        const auto &updatedAtColumn = m_model.getUpdatedAtColumn();

        // Nothing to do
        if (updatedAtColumn.isEmpty() || update.contains(updatedAtColumn))
            return update;

        auto updateCopy = update;

        // Don't use qualified column here, it's not needed
        return updateCopy << updatedAtColumn;
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
