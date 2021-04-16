#ifndef MODEL_H
#define MODEL_H

#include "orm/concerns/hasconnectionresolver.hpp"
#include "orm/connectionresolverinterface.hpp"
#include "orm/invalidargumenterror.hpp"
#include "orm/invalidformaterror.hpp"
#include "orm/tiny/concerns/hasrelationstore.hpp"
#include "orm/tiny/massassignmenterror.hpp"
#include "orm/tiny/relationnotfounderror.hpp"
#include "orm/tiny/relationnotloadederror.hpp"
#include "orm/tiny/relations/belongsto.hpp"
#include "orm/tiny/relations/belongstomany.hpp"
#include "orm/tiny/relations/hasone.hpp"
#include "orm/tiny/relations/hasmany.hpp"
#include "orm/tiny/tinybuilder.hpp"
#include "orm/utils/string.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class DatabaseConnection;
namespace Query
{
    class Builder;
}

namespace Tiny
{
namespace Relations {
    class IsPivotModel;
}

#ifdef TINYORM_TESTS_CODE
    /*! Used by tests to override connection in the Model. */
    struct ConnectionOverride
    {
        /*! The connection to use in the Model, this data member is picked up
            in the Model::getConnectionName(). */
        inline static QString connection = "";
    };
#endif

    // TODO decide/unify when to use class/typename keywords for templates silverqx
    // TODO concept, AllRelations can not contain type defined in "Model" parameter silverqx
    // TODO next test no relation behavior silverqx
    // TODO model missing methods Soft Deleting, Model::trashed()/restore()/withTrashed()/forceDelete()/onlyTrashed(), check this methods also on EloquentBuilder and SoftDeletes trait silverqx
    // TODO model missing methods Model::replicate() silverqx
    // TODO model missing methods Comparing Models silverqx
    // TODO model missing methods Model::firstOr() silverqx
    // TODO model missing methods Model::updateOrInsert() silverqx
    // TODO model missing methods Model::loadMissing() silverqx
    // TODO model missing methods Model::whereExists() silverqx
    // TODO model missing methods Model::whereBetween() silverqx
    // TODO model missing methods Model::exists()/notExists() silverqx
    // TODO model missing methods Model::saveOrFail() silverqx
    // TODO model missing methods EloquentCollection::toQuery() silverqx
    // FEATURE EloquentCollection, solve how to implement, also look at Eloquent's Custom Collections silverqx
    // FEATURE next Constraining Eager Loads silverqx
    // TODO perf add pragma once to every header file, have branch pragma-once, but I can't get rid of the clang warning -Wpragma-once-outside-header in every file, I tried everything 😞 silverqx
    // TODO future try to compile every header file by itself and catch up missing dependencies and forward declaration, every header file should be compilable by itself silverqx
    // TODO future include every stl dependency in header files silverqx
    // FEATURE logging, add support for custom logging, logging to the defined stream?, I don't exactly know how I will solve this issue, design it 🤔 silverqx
    // TODO QueryBuilder::updateOrInsert() silverqx
    // CUR code coverage silverqx
    // CUR create future todo task silverqx
    // CUR search ~~ in docs silverqx
    template<typename Derived, typename ...AllRelations>
    class Model :
            public Concerns::HasRelationStore<Derived, AllRelations...>,
            public Orm::Concerns::HasConnectionResolver
    {
        // TODO future, try to solve problem with forward declarations for friend methods, to allow only relevant methods from TinyBuilder silverqx
        /* Used by TinyBuilder::eagerLoadRelationVisitor()/getRelationMethod(). */
        friend TinyBuilder<Derived>;

        using JoinClause = Orm::Query::JoinClause;

        /*! The type returned by Model's relation methods. */
        template<typename Related>
        using RelationType =
                std::unique_ptr<Relations::Relation<Derived, Related>>(Derived::*)();

    public:
        /*! The "type" of the primary key ID. */
        using KeyType = quint64;

        /*! Create a new TinORM model instance. */
        Model();

        /*! Create a new TinORM model instance from attributes
            (converting constructor). */
        Model(const QVector<AttributeItem> &attributes);
        /*! Create a new TinORM model instance from attributes
            (converting constructor). */
        Model(QVector<AttributeItem> &&attributes);

        /*! Create a new TinORM model instance from attributes
            (list initialization). */
        Model(std::initializer_list<AttributeItem> attributes);

        /* Static operations on a model instance */
        /*! Create a new TinyORM model instance with given attributes. */
        inline static Derived instance(const QVector<AttributeItem> &attributes);
        /*! Create a new TinyORM model instance with given attributes. */
        inline static Derived instance(QVector<AttributeItem> &&attributes);

        /*! Begin querying the model. */
        static std::unique_ptr<TinyBuilder<Derived>> query();
        /*! Begin querying the model on a given connection. */
        static std::unique_ptr<TinyBuilder<Derived>> on(const QString &connection = "");

        /* TinyBuilder proxy methods */
        /*! Get all of the models from the database. */
        static QVector<Derived> all(const QStringList &columns = {"*"});

        /*! Find a model by its primary key. */
        static std::optional<Derived>
        find(const QVariant &id, const QStringList &columns = {"*"});
        /*! Find a model by its primary key or return fresh model instance. */
        static Derived
        findOrNew(const QVariant &id, const QStringList &columns = {"*"});
        /*! Find a model by its primary key or throw an exception. */
        static Derived
        findOrFail(const QVariant &id, const QStringList &columns = {"*"});

        /*! Get the first record matching the attributes or instantiate it. */
        static Derived
        firstOrNew(const QVector<WhereItem> &attributes = {},
                   const QVector<AttributeItem> &values = {});
        /*! Get the first record matching the attributes or create it. */
        static Derived
        firstOrCreate(const QVector<WhereItem> &attributes = {},
                      const QVector<AttributeItem> &values = {});
        /*! Execute the query and get the first result or throw an exception. */
        static Derived firstOrFail(const QStringList &columns = {"*"});

        /*! Add a basic where clause to the query, and return the first result. */
        static std::optional<Derived>
        firstWhere(const QString &column, const QString &comparison,
                   const QVariant &value, const QString &condition = "and");
        /*! Add a basic equal where clause to the query, and return the first result. */
        static std::optional<Derived>
        firstWhereEq(const QString &column, const QVariant &value,
                     const QString &condition = "and");

        /*! Get a single column's value from the first result of a query. */
        static QVariant value(const QString &column);

        /*! Begin querying a model with eager loading. */
        static std::unique_ptr<TinyBuilder<Derived>>
        with(const QVector<WithItem> &relations);
        /*! Begin querying a model with eager loading. */
        static std::unique_ptr<TinyBuilder<Derived>>
        with(const QString &relation);

        /*! Prevent the specified relations from being eager loaded. */
        static std::unique_ptr<TinyBuilder<Derived>>
        without(const QVector<QString> &relations);
        /*! Prevent the specified relations from being eager loaded. */
        static std::unique_ptr<TinyBuilder<Derived>>
        without(const QString &relation);

        /*! Save a new model and return the instance. */
        static Derived create(const QVector<AttributeItem> &attributes);

        /* Proxies to TinyBuilder -> BuildsQueries */
        /*! Execute the query and get the first result. */
        static std::optional<Derived> first(const QStringList &columns = {"*"});

        /* Proxies to TinyBuilder -> QueryBuilder */
        /* Insert, Update, Delete */
        /*! Insert new records into the database. */
        static std::optional<QSqlQuery>
        insert(const QVector<AttributeItem> &attributes);
        /*! Insert a new record and get the value of the primary key. */
        static quint64
        insertGetId(const QVector<AttributeItem> &attributes);

        /*! Create or update a record matching the attributes, and fill it with values. */
        Derived updateOrCreate(const QVector<WhereItem> &attributes,
                               const QVector<AttributeItem> &values = {});

        /*! Destroy the models for the given IDs. */
        static std::size_t destroy(const QVector<QVariant> &ids);
        /*! Destroy the model by the given ID. */
        static std::size_t destroy(QVariant id);

        /*! Run a truncate statement on the table. */
        static void truncate();

        /* Select */
        /*! Set the columns to be selected. */
        static std::unique_ptr<TinyBuilder<Derived>>
        select(const QStringList columns = {"*"});
        /*! Set the column to be selected. */
        static std::unique_ptr<TinyBuilder<Derived>>
        select(const QString column);
        /*! Add new select columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        addSelect(const QStringList &columns);
        /*! Add a new select column to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        addSelect(const QString &column);

        /*! Force the query to only return distinct results. */
        static std::unique_ptr<TinyBuilder<Derived>> distinct();

        /*! Add a join clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        join(const QString &table, const QString &first, const QString &comparison,
             const QString &second, const QString &type = "inner", bool where = false);
        /*! Add an advanced join clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        join(const QString &table, const std::function<void(JoinClause &)> &callback,
             const QString &type = "inner");
        /*! Add a "join where" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        joinWhere(const QString &table, const QString &first, const QString &comparison,
                  const QString &second, const QString &type = "inner");
        /*! Add a left join to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        leftJoin(const QString &table, const QString &first,
                 const QString &comparison, const QString &second);
        /*! Add an advanced left join to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        leftJoin(const QString &table,
                 const std::function<void(JoinClause &)> &callback);
        /*! Add a "join where" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        leftJoinWhere(const QString &table, const QString &first,
                      const QString &comparison, const QString &second);
        /*! Add a right join to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        rightJoin(const QString &table, const QString &first,
                  const QString &comparison, const QString &second);
        /*! Add an advanced right join to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        rightJoin(const QString &table,
                  const std::function<void(JoinClause &)> &callback);
        /*! Add a "right join where" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        rightJoinWhere(const QString &table, const QString &first,
                       const QString &comparison, const QString &second);
        /*! Add a "cross join" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        crossJoin(const QString &table, const QString &first,
                  const QString &comparison, const QString &second);
        /*! Add an advanced "cross join" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        crossJoin(const QString &table,
                  const std::function<void(JoinClause &)> &callback);

        /*! Add a basic where clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        where(const QString &column, const QString &comparison,
              const QVariant &value, const QString &condition = "and");
        /*! Add an "or where" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhere(const QString &column, const QString &comparison,
                const QVariant &value);
        /*! Add a basic equal where clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereEq(const QString &column, const QVariant &value,
                const QString &condition = "and");
        /*! Add an equal "or where" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereEq(const QString &column, const QVariant &value);
        /*! Add a nested where clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        where(const std::function<void(TinyBuilder<Derived> &)> &callback,
              const QString &condition = "and");
        /*! Add a nested "or where" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhere(const std::function<void(TinyBuilder<Derived> &)> &callback);

        /*! Add a vector of basic where clauses to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        where(const QVector<WhereItem> &values, const QString &condition = "and");
        /*! Add a vector of basic "or where" clauses to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhere(const QVector<WhereItem> &values);

        /*! Add a vector of where clauses comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereColumn(const QVector<WhereColumnItem> &values,
                    const QString &condition = "and");
        /*! Add a vector of "or where" clauses comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereColumn(const QVector<WhereColumnItem> &values);

        /*! Add a "where" clause comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereColumn(const QString &first, const QString &comparison,
                    const QString &second, const QString &condition = "and");
        /*! Add a "or where" clause comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereColumn(const QString &first, const QString &comparison,
                      const QString &second);
        /*! Add an equal "where" clause comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereColumnEq(const QString &first, const QString &second,
                      const QString &condition = "and");
        /*! Add an equal "or where" clause comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereColumnEq(const QString &first, const QString &second);

        /*! Add a "where in" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereIn(const QString &column, const QVector<QVariant> &values,
                const QString &condition = "and", bool nope = false);
        /*! Add an "or where in" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereIn(const QString &column, const QVector<QVariant> &values);
        /*! Add a "where not in" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNotIn(const QString &column, const QVector<QVariant> &values,
                   const QString &condition = "and");
        /*! Add an "or where not in" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNotIn(const QString &column, const QVector<QVariant> &values);

        /*! Add a "where null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNull(const QStringList &columns = {"*"},
                  const QString &condition = "and", bool nope = false);
        /*! Add a "where null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNull(const QString &column, const QString &condition = "and",
                  bool nope = false);
        /*! Add an "or where null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNull(const QStringList &columns = {"*"});
        /*! Add an "or where null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNull(const QString &column);
        /*! Add a "where not null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNotNull(const QStringList &columns = {"*"},
                     const QString &condition = "and");
        /*! Add a "where not null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNotNull(const QString &column, const QString &condition = "and");
        /*! Add an "or where not null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNotNull(const QStringList &columns = {"*"});
        /*! Add an "or where not null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNotNull(const QString &column);

        /*! Add a "group by" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        groupBy(const QStringList &groups);
        /*! Add a "group by" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        groupBy(const QString &group);

        /*! Add a "having" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        having(const QString &column, const QString &comparison,
               const QVariant &value, const QString &condition = "and");
        /*! Add an "or having" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orHaving(const QString &column, const QString &comparison,
                 const QVariant &value);

        /*! Add an "order by" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orderBy(const QString &column, const QString &direction = "asc");
        /*! Add a descending "order by" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orderByDesc(const QString &column);

        /*! Add an "order by" clause for a timestamp to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        latest(QString column = "");
        /*! Add an "order by" clause for a timestamp to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        oldest(QString column = "");
        /*! Remove all existing orders. */
        static std::unique_ptr<TinyBuilder<Derived>>
        reorder();
        /*! Remove all existing orders and optionally add a new order. */
        static std::unique_ptr<TinyBuilder<Derived>>
        reorder(const QString &column, const QString &direction = "asc");

        /*! Set the "limit" value of the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        limit(int value);
        /*! Alias to set the "limit" value of the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        take(int value);
        /*! Set the "offset" value of the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        offset(int value);
        /*! Alias to set the "offset" value of the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        skip(int value);
        /*! Set the limit and offset for a given page. */
        static std::unique_ptr<TinyBuilder<Derived>>
        forPage(int page, int perPage = 30);

        // TODO next fuckin increment, finish later 👿 silverqx

        /* Proxies to TinyBuilder */
        /*! Add a where clause on the primary key to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereKey(const QVariant &id);
        /*! Add a where clause on the primary key to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereKeyNot(const QVariant &id);

        /* Operations on a model instance */
        /*! Save the model to the database. */
        bool save(const SaveOptions &options = {});
        /*! Save the model and all of its relationships. */
        bool push();

        /*! Update records in the database. */
        bool update(const QVector<AttributeItem> &attributes,
                    const SaveOptions &options = {});

        /*! Delete the model from the database. */
        bool remove();
        /*! Delete the model from the database (alias). */
        bool deleteModel();

        /*! Reload a fresh model instance from the database. */
        std::optional<Derived> fresh(const QVector<WithItem> &relations = {});
        /*! Reload a fresh model instance from the database. */
        std::optional<Derived> fresh(const QString &relation);
        /*! Reload the current model instance with fresh attributes from the database. */
        Derived &refresh();

        /*! Eager load relations on the model. */
        Derived &load(const QVector<WithItem> &relations);
        /*! Eager load relations on the model. */
        Derived &load(const QString &relation);

        /*! Determine if two models have the same ID and belong to the same table. */
        template<typename ModelToCompare>
        bool is(const std::optional<ModelToCompare> &model) const;
        /*! Determine if two models are not the same. */
        template<typename ModelToCompare>
        bool isNot(const std::optional<ModelToCompare> &model) const;

        /*! Fill the model with a vector of attributes. */
        Derived &fill(const QVector<AttributeItem> &attributes);
        /*! Fill the model with a vector of attributes. */
        Derived &fill(QVector<AttributeItem> &&attributes);
        /*! Fill the model with a vector of attributes. Force mass assignment. */
        Derived &forceFill(const QVector<AttributeItem> &attributes);

        /* Model Instance methods */
        /*! Get a new query builder for the model's table. */
        inline std::unique_ptr<TinyBuilder<Derived>> newQuery()
        { return newQueryWithoutScopes(); }
        /*! Get a new query builder that doesn't have any global scopes. */
        std::unique_ptr<TinyBuilder<Derived>> newQueryWithoutScopes();
        /*! Get a new query builder that doesn't have any global scopes or
            eager loading. */
        std::unique_ptr<TinyBuilder<Derived>> newModelQuery();
        /*! Get a new query builder with no relationships loaded. */
        std::unique_ptr<TinyBuilder<Derived>> newQueryWithoutRelationships();
        /*! Create a new Tiny query builder for the model. */
        std::unique_ptr<TinyBuilder<Derived>>
        newTinyBuilder(const QSharedPointer<QueryBuilder> query);

        /*! Create a new model instance that is existing. */
        Derived newFromBuilder(const QVector<AttributeItem> &attributes = {},
                               const std::optional<QString> connection = std::nullopt);
        /*! Create a new instance of the given model. */
        inline Derived newInstance() { return newInstance({}); }
        /*! Create a new instance of the given model. */
        Derived newInstance(const QVector<AttributeItem> &attributes,
                            bool exists = false);
        /*! Create a new instance of the given model. */
        Derived newInstance(QVector<AttributeItem> &&attributes,
                            bool exists = false);

        /*! Create a new pivot model instance. */
        template<typename PivotType = Relations::Pivot, typename Parent>
        PivotType newPivot(const Parent &parent, const QVector<AttributeItem> &attributes,
                           const QString &table, bool exists) const;

        // CUR cache result silverqx
        /*! Static cast this to a child's instance type (CRTP). */
        inline Derived &model()
        { return static_cast<Derived &>(*this); }
        /*! Static cast this to a child's instance type (CRTP), const version. */
        inline const Derived &model() const
        { return static_cast<const Derived &>(*this); }

        /* Getters / Setters */
        /*! Get the current connection name for the model. */
        const QString &getConnectionName() const;
        /*! Get the database connection for the model. */
        inline ConnectionInterface &getConnection() const
        { return m_resolver->connection(getConnectionName()); }
        /*! Set the connection associated with the model. */
        inline Derived &setConnection(const QString &name)
        { model().u_connection = name; return model(); }
        /*! Set the table associated with the model. */
        inline Derived &setTable(const QString &value)
        { model().u_table = value; return model(); }
        /*! Get the table associated with the model. */
        QString getTable() const;
        /*! Get the primary key for the model. */
        inline const QString &getKeyName() const
        { return model().u_primaryKey; }
        /*! Get the table qualified key name. */
        inline QString getQualifiedKeyName() const
        { return qualifyColumn(getKeyName()); }
        /*! Get the value of the model's primary key. */
        inline QVariant getKey() const
        { return getAttribute(getKeyName()); }
        /*! Get the value indicating whether the IDs are incrementing. */
        inline bool getIncrementing() const
        { return model().u_incrementing; }
        /*! Set whether IDs are incrementing. */
        inline Derived &setIncrementing(const bool value)
        { model().u_incrementing = value; return model(); }

        /* Others */
        /*! Qualify the given column name by the model's table. */
        QString qualifyColumn(const QString &column) const;

        /*! Indicates if the model exists. */
        bool exists = false;

        /* HasAttributes */
        /*! Set a given attribute on the model. */
        Derived &setAttribute(const QString &key, QVariant value);
        /*! Set a vector of model attributes. No checking is done. */
        Derived &setRawAttributes(const QVector<AttributeItem> &attributes,
                                  bool sync = false);
        /*! Sync the original attributes with the current. */
        Derived &syncOriginal();
        /*! Get all of the current attributes on the model (insert order). */
        inline const QVector<AttributeItem> &getAttributes() const;
        /*! Get all of the current attributes on the model (for fast lookup). */
        inline const std::unordered_map<QString, int> &getAttributesHash() const;
        /*! Get an attribute from the model. */
        QVariant getAttribute(const QString &key) const;
        /*! Get a plain attribute (not a relationship). */
        QVariant getAttributeValue(const QString &key) const;
        /*! Get an attribute from the m_attributes vector. */
        QVariant getAttributeFromArray(const QString &key) const;
        /*! Get the model's original attribute value (transformed). */
        QVariant getOriginal(const QString &key,
                             const QVariant &defaultValue = {}) const;
        /*! Get the model's original attribute values (transformed and insert order). */
        QVector<AttributeItem> getOriginals() const;
        /*! Get the model's original attributes hash (for fast lookup). */
        inline const std::unordered_map<QString, int> &getOriginalsHash() const;
        /*! Get the model's raw original attribute value. */
        QVariant getRawOriginal(const QString &key,
                                const QVariant &defaultValue = {}) const;
        /*! Get the model's raw original attribute values (insert order). */
        inline const QVector<AttributeItem> &getRawOriginals() const;
        /*! Unset an attribute on the model, returns the number of attributes removed. */
        Derived &unsetAttribute(const AttributeItem &value);
        /*! Unset an attribute on the model. */
        Derived &unsetAttribute(const QString &key);

        /*! Get a relationship for Many types relation. */
        template<typename Related,
                 template<typename> typename Container = QVector>
        const Container<Related *>
        getRelationValue(const QString &relation);
        /*! Get a relationship for a One type relation. */
        template<typename Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        Related *
        getRelationValue(const QString &relation);

        /*! Get the attributes that have been changed since last sync
            (insert order). */
        QVector<AttributeItem> getDirty() const;
        /*! Get the attributes that have been changed since last sync
            (for fast lookup). */
        std::unordered_map<QString, int> getDirtyHash() const;
        /*! Determine if the model or any of the given attribute(s) have
            been modified. */
        inline bool isDirty(const QStringList &attributes = {}) const
        { return hasChanges(getDirtyHash(), attributes); }
        /*! Determine if the model or any of the given attribute(s) have
            been modified. */
        inline bool isDirty(const QString &attribute) const
        { return hasChanges(getDirtyHash(), QStringList {attribute}); }
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        inline bool isClean(const QStringList &attributes = {}) const
        { return !isDirty(attributes); }
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        inline bool isClean(const QString &attribute) const
        { return !isDirty(attribute); }

        /*! Get the attributes that were changed (insert order). */
        inline const QVector<AttributeItem> &getChanges() const
        { return m_changes; }
        /*! Get the attributes that were changed (for fast lookup). */
        inline const std::unordered_map<QString, int> &getChangesHash() const
        { return m_changesHash; }
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        inline bool wasChanged(const QStringList &attributes = {}) const
        { return hasChanges(getChangesHash(), attributes); }
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        inline bool wasChanged(const QString &attribute) const
        { return hasChanges(getChangesHash(), QStringList {attribute}); }

        /*! Get the format for database stored dates. */
        const QString &getDateFormat() const;
        /*! Set the date format used by the model. */
        Derived &setDateFormat(const QString &format);
        /*! Convert a DateTime to a storable string. */
        QVariant fromDateTime(const QVariant &value) const;
        /*! Convert a DateTime to a storable string. */
        QString fromDateTime(const QDateTime &value) const;
        /*! Get the attributes that should be converted to dates. */
        const QStringList &getDates() const;

        /*! Proxy for an attribute element used in the operator[] &. */
        class AttributeReference
        {
            friend Model<Derived, AllRelations...>;

        public:
            /*! Assign a value of the QVariant to the referenced attribute. */
            const AttributeReference &operator=(const QVariant &value) const;
            /*! Assign a value of another attribute reference to the referenced
                attribute. */
            const AttributeReference &
            operator=(const AttributeReference &attributeReference) const;

            /*! Accesses the contained value, only const member functions. */
            inline const QVariant *operator->() const;
            /*! Accesses the contained value. */
            inline QVariant value() const;
            /*! Accesses the contained value. */
            inline QVariant operator*() const;
            /*! Converting operator to the QVariant type. */
            inline operator QVariant() const;

        private:
            /*! AttributeReference's private constructor. */
            AttributeReference(Model<Derived, AllRelations...> &model,
                               const QString &attribute);

            /*! The model on which is an attribute set. */
            Model<Derived, AllRelations...> &m_model;
            /*! Attribute key name. */
            const QString m_attribute;
            /*! The temporary cache used during operator->() call, to be able
                to return the QVariant *. */
            mutable QVariant m_attributeCache;
        };

        /*! Return modifiable attribute reference, can be used on the left-hand side
            of an assignment operator. */
        inline AttributeReference operator[](const QString &attribute) &;
        /*! Return an attribute by the given key. */
        inline QVariant operator[](const QString &attribute) const &;
        /*! Return an attribute by the given key. */
        inline QVariant operator[](const QString &attribute) &&;
        /*! Return an attribute by the given key. */
        inline QVariant operator[](const QString &attribute) const &&;

        /* HasRelationships */
        /*! Get a specified relationship. */
        template<typename Related,
                 template<typename> typename Container = QVector>
        const Container<Related *>
        getRelation(const QString &relation);
        /*! Get a specified relationship as Related type, for use with HasOne and
            BelongsTo relation types. */
        template<typename Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        Related *getRelation(const QString &relation);

        /*! Determine if the given relation is loaded. */
        bool relationLoaded(const QString &relation) const;

        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, const QVector<Related> &models);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, QVector<Related> &&models);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, const std::optional<Related> &model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, std::optional<Related> &&model);

        /*! Get the default foreign key name for the model. */
        QString getForeignKey() const;

        /*! Define a one-to-one relationship. */
        template<typename Related>
        std::unique_ptr<Relations::HasOne<Derived, Related>>
        hasOne(QString foreignKey = "", QString localKey = "");
        /*! Define an inverse one-to-one or many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::BelongsTo<Derived, Related>>
        belongsTo(QString foreignKey = "", QString ownerKey = "", QString relation = "");
        /*! Define a one-to-many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::HasMany<Derived, Related>>
        hasMany(QString foreignKey = "", QString localKey = "");
        /*! Define a many-to-many relationship. */
        template<typename Related, typename PivotType = Relations::Pivot>
        std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
        belongsToMany(QString table = "", QString foreignPivotKey = "",
                      QString relatedPivotKey = "", QString parentKey = "",
                      QString relatedKey = "", QString relation = "");

        /*! Touch the owning relations of the model. */
        void touchOwners();

        /*! Get the relationships that are touched on save. */
        inline const QStringList &getTouchedRelations() const
        { return model().u_touches; }
        /*! Determine if the model touches a given relation. */
        inline bool touches(const QString &relation) const
        { return getTouchedRelations().contains(relation); }

        /*! Get all the loaded relations for the instance. */
        inline const std::unordered_map<QString, RelationsType<AllRelations...>> &
        getRelations() const
        { return m_relations; }
        /*! Get all the loaded relations for the instance. */
        inline std::unordered_map<QString, RelationsType<AllRelations...>> &
        getRelations()
        { return m_relations; }

        /*! Unset all the loaded relations for the instance. */
        Derived &unsetRelations();
        /*! Unset a loaded relationship. */
        Derived &unsetRelation(const QString &relation);

        /* HasTimestamps */
        /*! Update the model's update timestamp. */
        bool touch();
        /*! Update the creation and update timestamps. */
        void updateTimestamps();

        /*! Set the value of the "created at" attribute. */
        Derived &setCreatedAt(const QDateTime &value);
        /*! Set the value of the "updated at" attribute. */
        Derived &setUpdatedAt(const QDateTime &value);

        /*! Get a fresh timestamp for the model. */
        inline QDateTime freshTimestamp() const
        { return QDateTime::currentDateTime(); }
        /*! Get a fresh timestamp for the model. */
        QString freshTimestampString() const;

        /*! Determine if the model uses timestamps. */
        inline bool usesTimestamps() const
        { return model().u_timestamps; }
        Derived &setUseTimestamps(bool value);

        /*! Get the name of the "created at" column. */
        inline static const QString &getCreatedAtColumn()
        { return Derived::CREATED_AT; }
        /*! Get the name of the "updated at" column. */
        inline static const QString &getUpdatedAtColumn()
        { return Derived::UPDATED_AT; }

        /*! Get the fully qualified "created at" column. */
        QString getQualifiedCreatedAtColumn() const;
        /*! Get the fully qualified "updated at" column. */
        QString getQualifiedUpdatedAtColumn() const;

        /*! Determine if the given model is ignoring touches. */
        template<typename ClassToCheck = Derived>
        static bool isIgnoringTouch();

        /* GuardsAttributes */
        /*! Get the fillable attributes for the model. */
        inline const QStringList &getFillable() const;
        /*! Set the fillable attributes for the model. */
        Derived &fillable(const QStringList &fillable);
        /*! Set the fillable attributes for the model. */
        Derived &fillable(QStringList &&fillable);
        /*! Merge new fillable attributes with existing fillable attributes
            on the model. */
        Derived &mergeFillable(const QStringList &fillable);
        Derived &mergeFillable(QStringList &&fillable);

        /*! Get the guarded attributes for the model. */
        const QStringList &getGuarded() const;
        /*! Set the guarded attributes for the model. */
        Derived &guard(const QStringList &guarded);
        /*! Set the guarded attributes for the model. */
        Derived &guard(QStringList &&guarded);
        /*! Merge new guarded attributes with existing guarded attributes
            on the model. */
        Derived &mergeGuarded(const QStringList &guarded);
        Derived &mergeGuarded(QStringList &&guarded);

        /*! Disable all mass assignable restrictions. */
        static void unguard(bool state = true);
        /*! Enable the mass assignment restrictions. */
        static void reguard();
        /*! Determine if the current state is "unguarded". */
        inline static bool isUnguarded();
        /*! Run the given callable while being unguarded. */
        static void unguarded(const std::function<void()> &callback);

        /*! Determine if the given attribute may be mass assigned. */
        bool isFillable(const QString &key) const;
        /*! Determine if the given key is guarded. */
        bool isGuarded(const QString &key) const;

        /*! Determine if the model is totally guarded. */
        bool totallyGuarded() const;

    protected:
        /*! Get a new query builder instance for the connection. */
        QSharedPointer<QueryBuilder> newBaseQueryBuilder() const;

        /*! The visitor to obtain a type for Related template parameter. */
        inline void relationVisitor(const QString &)
        {}

        /*! On the base of a type saved in the relation store decide, which action
            to call eager/push. */
        template<typename Related>
        void relationVisited();

        /* HasAttributes */
        /*! Transform a raw model value using mutators, casts, etc. */
        QVariant transformModelValue(const QString &key, const QVariant &value) const;
        /*! Get the model's original attribute values. */
        QVariant getOriginalWithoutRewindingModel(
                const QString &key, const QVariant &defaultValue = {}) const;

        /*! Get a relationship value from a method. */
        // TODO I think this can be merged to one template method, I want to preserve Orm::One/Many tags and use std::enable_if to switch types by Orm::One/Many tag 🤔 silverqx
        template<class Related,
                 template<typename> typename Container = QVector>
        Container<Related *>
        getRelationshipFromMethod(const QString &relation);
        /*! Get a relationship value from a method. */
        template<class Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        Related *
        getRelationshipFromMethod(const QString &relation);

        /*! Determine if any of the given attributes were changed. */
        bool hasChanges(const std::unordered_map<QString, int> &changes,
                        const QStringList &attributes = {}) const;
        /*! Sync the changed attributes. */
        Derived &syncChanges();

        /*! Determine if the new and old values for a given key are equivalent. */
        bool originalIsEquivalent(const QString &key) const;

        /*! Determine if the given attribute is a date. */
        bool isDateAttribute(const QString &key) const;
        /*! Return a timestamp as DateTime object. */
        QDateTime asDateTime(const QVariant &value) const;
        /*! Obtain timestamp column names. */
        const QStringList &timestampColumnNames() const;

        /*! Rehash attribute positions from the given index. */
        void rehashAttributePositions(
                const QVector<AttributeItem> &attributes,
                std::unordered_map<QString, int> &attributesHash,
                int from = 0);

        /* HasRelationships */
        /*! Create a new model instance for a related model. */
        template<typename Related>
        std::unique_ptr<Related> newRelatedInstance() const;

        /*! Instantiate a new HasOne relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::HasOne<Derived, Related>>
        newHasOne(std::unique_ptr<Related> &&related, Derived &parent,
                  const QString &foreignKey, const QString &localKey) const
        { return Relations::HasOne<Derived, Related>::instance(
                        std::move(related), parent, foreignKey, localKey); }
        /*! Instantiate a new BelongsTo relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::BelongsTo<Derived, Related>>
        newBelongsTo(std::unique_ptr<Related> &&related,
                     Derived &child, const QString &foreignKey,
                     const QString &ownerKey, const QString &relation) const
        { return Relations::BelongsTo<Derived, Related>::instance(
                        std::move(related), child, foreignKey, ownerKey, relation); }
        /*! Instantiate a new HasMany relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::HasMany<Derived, Related>>
        newHasMany(std::unique_ptr<Related> &&related, Derived &parent,
                   const QString &foreignKey, const QString &localKey) const
        { return Relations::HasMany<Derived, Related>::instance(
                        std::move(related), parent, foreignKey, localKey); }
        /*! Instantiate a new BelongsToMany relationship. */
        template<typename Related, typename PivotType>
        inline std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
        newBelongsToMany(std::unique_ptr<Related> &&related, Derived &parent,
                         const QString &table, const QString &foreignPivotKey,
                         const QString &relatedPivotKey, const QString &parentKey,
                         const QString &relatedKey, const QString &relation) const
        { return Relations::BelongsToMany<Derived, Related, PivotType>::instance(
                        std::move(related), parent, table, foreignPivotKey,
                        relatedPivotKey, parentKey, relatedKey, relation); }

        /*! Guess the "belongs to" relationship name. */
        template<typename Related>
        QString guessBelongsToRelation() const;
        /*! Guess the "belongs to many" relationship name. */
        template<typename Related>
        QString guessBelongsToManyRelation() const;
        /*! Get the joining table name for a many-to-many relation. */
        template<typename Related>
        QString pivotTableName() const;

        /*! Set the entire relations hash on the model. */
        Derived &setRelations(
                const std::unordered_map<QString,
                                         RelationsType<AllRelations...>> &relations);
        /*! Set the entire relations hash on the model. */
        Derived &setRelations(
                std::unordered_map<QString, RelationsType<AllRelations...>> &&relations);

        /* GuardsAttributes */
        /*! Determine if the given column is a valid, guardable column. */
        bool isGuardableColumn(const QString &key) const;
        /*! Th key for guardable columns hash cache. */
        QString getKeyForGuardableHash() const;
        /*! Get the fillable attributes of a given vector. */
        QVector<AttributeItem>
        fillableFromArray(const QVector<AttributeItem> &attributes) const;
        QVector<AttributeItem>
        fillableFromArray(QVector<AttributeItem> &&attributes) const;

        /* Others */
        /*! Perform the actual delete query on this model instance. */
        void performDeleteOnModel();

        /*! Set the keys for a save update query. */
        TinyBuilder<Derived> &
        setKeysForSaveQuery(TinyBuilder<Derived> &query);
        /*! Get the primary key value for a save query. */
        QVariant getKeyForSaveQuery() const;
        /*! Set the keys for a select query. */
        TinyBuilder<Derived> &
        setKeysForSelectQuery(TinyBuilder<Derived> &query);
        /*! Get the primary key value for a select query. */
        inline QVariant getKeyForSelectQuery() const;

        /*! Perform a model insert operation. */
        bool performInsert(const TinyBuilder<Derived> &query);
        /*! Perform a model insert operation. */
        bool performUpdate(TinyBuilder<Derived> &query);
        /*! Perform any actions that are necessary after the model is saved. */
        void finishSave(const SaveOptions &options = {});

        /*! Insert the given attributes and set the ID on the model. */
        quint64 insertAndSetId(const TinyBuilder<Derived> &query,
                               const QVector<AttributeItem> &attributes);

        /*! The table associated with the model. */
        QString u_table {""};
        /*! The connection name for the model. */
        QString u_connection {""};
        /*! Indicates if the model's ID is auto-incrementing. */
        bool u_incrementing = true;
        /*! The primary key associated with the table. */
        QString u_primaryKey {"id"};

        // TODO for std::any check, whether is appropriate to define template requirement std::is_nothrow_move_constructible ( to avoid dynamic allocations for small objects and how this internally works ) silverqx
        /*! Map of relation names to methods. */
        QHash<QString, std::any> u_relations;
        // TODO detect (best at compile time) circular eager relation problem, the exception which happens during this problem is stackoverflow in QRegularExpression silverqx
        /*! The relations to eager load on every query. */
        QVector<WithItem> u_with;
        /*! The relationship counts that should be eager loaded on every query. */
//        QVector<WithItem> u_withCount;

        /* HasAttributes */
        /*! The model's default values for attributes. */
        inline static const QVector<AttributeItem> u_attributes {};
        /*! The model's attributes (insert order). */
        QVector<AttributeItem> m_attributes;
        /*! The model attribute's original state (insert order).
            On the model from many-to-many relation also contains all pivot values,
            that is normal (insert order). */
        QVector<AttributeItem> m_original;
        /*! The changed model attributes (insert order). */
        QVector<AttributeItem> m_changes;

        /* Don't want to use std::reference_wrapper to attributes, because if a copy
           of the model is made, all references would be invalidated. */
        /*! The model's attributes hash (for fast lookup). */
        std::unordered_map<QString, int> m_attributesHash;
        /*! The model attribute's original state (for fast lookup). */
        std::unordered_map<QString, int> m_originalHash;
        /*! The changed model attributes (for fast lookup). */
        std::unordered_map<QString, int> m_changesHash;

        // TODO add support for 'U' like in PHP to support unix timestamp, I will have to manually check if u_dateFormat contains 'U' and use QDateTime::fromSecsSinceEpoch() silverqx
        /*! The storage format of the model's date columns. */
        inline static QString u_dateFormat {""};
        /*! The attributes that should be mutated to dates. @deprecated */
        inline static QStringList u_dates {};

        /* HasRelationships */
        /*! The loaded relationships for the model. */
        std::unordered_map<QString, RelationsType<AllRelations...>> m_relations;
        /*! The relationships that should be touched on save. */
        QStringList u_touches;

        /* HasTimestamps */
        /*! The name of the "created at" column. */
        inline static const QString CREATED_AT = QStringLiteral("created_at");
        /*! The name of the "updated at" column. */
        inline static const QString UPDATED_AT = QStringLiteral("updated_at");
        /*! Indicates if the model should be timestamped. */
        bool u_timestamps = true;

        /* GuardsAttributes */
        /*! The attributes that are mass assignable. */
        inline static QStringList u_fillable {};
        /*! The attributes that aren't mass assignable. */
        inline static QStringList u_guarded {"*"};
        /*! Indicates if all mass assignment is enabled. */
        inline static bool m_unguarded = false;
        /*! The actual columns that exist on the database and can be guarded. */
        inline static QHash<QString, QStringList> m_guardableColumns {};

    private:
        /* Eager load from TinyBuilder */
        /*! Invoke Model::eagerVisitor() to define template argument Related
            for eagerLoaded relation. */
        void eagerLoadRelationVisitor(
                const WithItem &relation, TinyBuilder<Derived> &builder,
                QVector<Derived> &models);
        /*! Get a relation method in the relations hash data member, defined
            in the current model instance. */
        template<typename Related>
        Model::template RelationType<Related>
        getRelationMethod(const QString &relation) const;

        /* HasAttributes */
        /*! Get all of the current attributes on the model. */
        inline const QVector<AttributeItem> &getRawAttributes() const
        { return m_attributes; }

        /*! Throw InvalidArgumentError if attributes passed to the constructor contain
            some value, which will cause access of some data member in a derived
            instance. */
        inline void
        throwIfCRTPctorProblem(const QVector<AttributeItem> &attributes) const;
        /*! The QDateTime attribute detected, causes CRTP ctor problem. */
        void throwIfQDateTimeAttribute(const QVector<AttributeItem> &attributes) const;

        /*! Get the attributes that should be converted to dates. */
        QStringList getDatesInternal() const;

        /* HasRelationships */
        /*! Throw exception if a relation is not defined. */
        void validateUserRelation(const QString &name) const;

        /*! Obtain related models from "relationships" data member hash
            without any checks. */
        template<class Related,
                 template<typename> typename Container = QVector>
        Container<Related *>
        getRelationFromHash(const QString &relation);
        /*! Obtain related models from "relationships" data member hash
            without any checks. */
        template<class Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        Related *
        getRelationFromHash(const QString &relation);

        /*! Get a relation method in the u_relations hash data member,
            defined in the current model instance. */
        template<typename Related>
        Model::template RelationType<Related>
        getRelationMethodRaw(const QString &relation) const;

        /* Eager loading and push */
        /*! Continue execution after a relation type was obtained ( by
            Related template parameter ). */
        template<typename Related>
        inline void eagerVisited() const
        { this->m_eagerStore->builder.template eagerLoadRelation<Related>(
                        this->m_eagerStore->models, this->m_eagerStore->relation); }

        /*! On the base of alternative held by m_relations decide, which
            pushVisitied() to execute. */
        template<typename Related>
        void pushVisited();
        /*! Push for Many relation types. */
        template<typename Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, Many>, bool> = true>
        void pushVisited();
        /*! Push for One relation type. */
        template<typename Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        void pushVisited();

        /*! On the base of alternative held by m_relations decide, which
            touchOwnersVisited() to execute. */
        template<typename Related>
        void touchOwnersVisited();

        /* Others */
        /*! Decide, if the Related model passed as the template parameter is
            the pivot model. */
        template<typename Related>
        void isPivotModelRelationVisited();
        /*! Obtain all loaded relation names except pivot relations. */
        QVector<WithItem> getLoadedRelationsWithoutPivot();
        /*! Create 'is pivot model relation store' and by an obtained Related
            template argument checks, if the model is a pivot model. */
        bool isPivotModelWithVisitor(const QString &relation);

        /*! Create push store and call push for every model. */
        bool pushWithVisitor(const QString &relation,
                             RelationsType<AllRelations...> &models);
        /*! Replace relations in the m_relation. */
        void replaceRelations(
                std::unordered_map<QString, RelationsType<AllRelations...>> &relations,
                const QVector<WithItem> &onlyRelations);

        /*! Create 'touch owners relation store' and touch all related models. */
        void touchOwnersWithVisitor(const QString &relation);

        /* Shortcuts for types related to the Relation Store */
        /*! Relation store type enum struct. */
        using RelationStoreType  =
            typename Concerns::HasRelationStore<Derived, AllRelations...>
                             ::RelationStoreType;

        /*! Obtain a pointer to member function from relation store, after
            relation was visited. */
        template<typename Related>
        const std::function<void(Model<Derived, AllRelations...> &)> &
        getMethodForRelationVisited(RelationStoreType storeType) const;
    };

    template<typename Derived, typename ...AllRelations>
    Model<Derived, AllRelations...>::Model()
    {
        // Compile time check if a primary key type is supported by a QVariant
        qMetaTypeId<typename Derived::KeyType>();

        // Default Attribute Values
        fill(Derived::u_attributes);

        syncOriginal();
    }

    template<typename Derived, typename ...AllRelations>
    Model<Derived, AllRelations...>::Model(const QVector<AttributeItem> &attributes)
        : Model()
    {
        throwIfCRTPctorProblem(attributes);

        fill(attributes);
    }

    template<typename Derived, typename ...AllRelations>
    Model<Derived, AllRelations...>::Model(QVector<AttributeItem> &&attributes)
        : Model()
    {
        throwIfCRTPctorProblem(attributes);

        fill(std::move(attributes));
    }

    template<typename Derived, typename ...AllRelations>
    Model<Derived, AllRelations...>::Model(
            std::initializer_list<AttributeItem> attributes
    )
        : Model(QVector<AttributeItem>(attributes.begin(), attributes.end()))
    {}

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(const QVector<AttributeItem> &attributes)
    {
        Derived model;

        model.fill(attributes);

        return model;
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(QVector<AttributeItem> &&attributes)
    {
        Derived model;

        model.fill(std::move(attributes));

        return model;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::query()
    {
        return Derived().newQuery();
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::on(const QString &connection)
    {
        /* First we will just create a fresh instance of this model, and then we can
           set the connection on the model so that it is used for the queries we
           execute, as well as being set on every relation we retrieve without
           a custom connection name. */
        Derived instance;

        instance.setConnection(connection);

        return instance.newQuery();
    }

    template<typename Derived, typename ...AllRelations>
    QVector<Derived>
    Model<Derived, AllRelations...>::all(const QStringList &columns)
    {
        return query()->get(columns);
    }

    template<typename Derived, typename ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::find(const QVariant &id,
                                            const QStringList &columns)
    {
        return query()->find(id, columns);
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::findOrNew(const QVariant &id,
                                               const QStringList &columns)
    {
        return query()->findOrNew(id, columns);
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::findOrFail(const QVariant &id,
                                                const QStringList &columns)
    {
        return query()->findOrFail(id, columns);
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::firstOrNew(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values)
    {
        return query()->firstOrNew(attributes, values);
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::firstOrCreate(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values)
    {
        return query()->firstOrCreate(attributes, values);
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::firstOrFail(const QStringList &columns)
    {
        return query()->firstOrFail(columns);
    }

    template<typename Derived, typename ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::firstWhere(
            const QString &column, const QString &comparison,
            const QVariant &value, const QString &condition)
    {
        return where(column, comparison, value, condition)->first();
    }

    template<typename Derived, typename ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::firstWhereEq(
            const QString &column, const QVariant &value, const QString &condition)
    {
        return where(column, QStringLiteral("="), value, condition)->first();
    }

    template<typename Derived, typename ...AllRelations>
    QVariant Model<Derived, AllRelations...>::value(const QString &column)
    {
        return query()->value(column);
    }

    // TODO now problem with QStringList overload, its ambiguou, I think I need only QStringList, because is implicitly convertible to QString silverqx
    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::with(const QVector<WithItem> &relations)
    {
        auto builder = query();

        builder->with(relations);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::with(const QString &relation)
    {
        return with(QVector<WithItem> {{relation}});
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::without(const QVector<QString> &relations)
    {
        auto builder = query();

        builder->without(relations);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::without(const QString &relation)
    {
        return without(QVector<QString> {relation});
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::create(const QVector<AttributeItem> &attributes)
    {
        return query()->create(attributes);
    }

    template<typename Derived, typename ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::first(const QStringList &columns)
    {
        return query()->first(columns);
    }

    template<typename Derived, typename ...AllRelations>
    std::optional<QSqlQuery>
    Model<Derived, AllRelations...>::insert(
            const QVector<AttributeItem> &attributes)
    {
        return query()->insert(attributes);
    }

    // FEATURE dilemma primarykey, Derived::KeyType vs QVariant silverqx
    template<typename Derived, typename ...AllRelations>
    quint64
    Model<Derived, AllRelations...>::insertGetId(
            const QVector<AttributeItem> &attributes)
    {
        return query()->insertGetId(attributes);
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::updateOrCreate(
            const QVector<WhereItem> &attributes, const QVector<AttributeItem> &values)
    {
        return query()->updateOrCreate(attributes, values);
    }

    // TODO cpp check all int types and use std::size_t where appropriate silverqx
    // FEATURE dilemma primarykey, id should be Derived::KeyType, if I don't solve this problem, do runtime type check, QVariant type has to be the same type like KeyType and throw exception silverqx
    // TODO next test all this remove()/destroy() methods, when deletion fails silverqx
    template<typename Derived, typename ...AllRelations>
    size_t
    Model<Derived, AllRelations...>::destroy(const QVector<QVariant> &ids)
    {
        if (ids.isEmpty())
            return 0;

        /* We will actually pull the models from the database table and call delete on
           each of them individually so that their events get fired properly with a
           correct set of attributes in case the developers wants to check these. */
        Derived instance;

        std::size_t count = 0;

        // Ownership of a unique_ptr()
        for (auto &model : instance.whereIn(instance.getKeyName(), ids)->get())
            if (model.remove())
                ++count;

        return count;
    }

    template<typename Derived, typename ...AllRelations>
    size_t
    Model<Derived, AllRelations...>::destroy(const QVariant id)
    {
        return destroy(QVector<QVariant> {id});
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::truncate()
    {
        query()->truncate();
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::select(const QStringList columns)
    {
        auto builder = query();

        builder->select(columns);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::select(const QString column)
    {
        auto builder = query();

        builder->select(column);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::addSelect(const QStringList &columns)
    {
        auto builder = query();

        builder->addSelect(columns);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::addSelect(const QString &column)
    {
        auto builder = query();

        builder->addSelect(column);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::distinct()
    {
        auto builder = query();

        builder->distinct();

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::join(
            const QString &table, const QString &first,  const QString &comparison,
            const QString &second, const QString &type, const bool where)
    {
        auto builder = query();

        builder->join(table, first, comparison, second, type, where);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::join(
            const QString &table, const std::function<void(JoinClause &)> &callback,
            const QString &type)
    {
        auto builder = query();

        builder->join(table, callback, type);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::joinWhere(
            const QString &table, const QString &first, const QString &comparison,
            const QString &second, const QString &type)
    {
        auto builder = query();

        builder->joinWhere(table, first, comparison, second, type);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::leftJoin(
            const QString &table, const QString &first,
            const QString &comparison, const QString &second)
    {
        auto builder = query();

        builder->leftJoin(table, first, comparison, second);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::leftJoin(
            const QString &table, const std::function<void(JoinClause &)> &callback)
    {
        auto builder = query();

        builder->leftJoin(table, callback);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::leftJoinWhere(
            const QString &table, const QString &first,
            const QString &comparison, const QString &second)
    {
        auto builder = query();

        builder->leftJoinWhere(table, first, comparison, second);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::rightJoin(
            const QString &table, const QString &first,
            const QString &comparison, const QString &second)
    {
        auto builder = query();

        builder->rightJoin(table, first, comparison, second);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::rightJoin(
            const QString &table, const std::function<void(JoinClause &)> &callback)
    {
        auto builder = query();

        builder->rightJoin(table, callback);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::rightJoinWhere(
            const QString &table, const QString &first,
            const QString &comparison, const QString &second)
    {
        auto builder = query();

        builder->rightJoinWhere(table, first, comparison, second);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::crossJoin(
            const QString &table, const QString &first,
            const QString &comparison, const QString &second)
    {
        auto builder = query();

        builder->crossJoin(table, first, comparison, second);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::crossJoin(
            const QString &table, const std::function<void(JoinClause &)> &callback)
    {
        auto builder = query();

        builder->crossJoin(table, callback);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::where(
            const QString &column, const QString &comparison,
            const QVariant &value, const QString &condition)
    {
        auto builder = query();

        builder->where(column, comparison, value, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhere(
            const QString &column, const QString &comparison, const QVariant &value)
    {
        auto builder = query();

        builder->orWhere(column, comparison, value);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereEq(
            const QString &column, const QVariant &value, const QString &condition)
    {
        auto builder = query();

        builder->whereEq(column, value, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhereEq(
            const QString &column, const QVariant &value)
    {
        auto builder = query();

        builder->orWhereEq(column, value);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::where(
            const std::function<void(TinyBuilder<Derived> &)> &callback,
            const QString &condition)
    {
        auto builder = query();

        builder->where(callback, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhere(
            const std::function<void(TinyBuilder<Derived> &)> &callback)
    {
        auto builder = query();

        builder->orWhere(callback);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::where(const QVector<WhereItem> &values,
                                             const QString &condition)
    {
        /* The parentheses in this query are ok:
           select * from xyz where (id = ?) */
        auto builder = query();

        builder->where(values, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhere(const QVector<WhereItem> &values)
    {
        auto builder = query();

        builder->orWhere(values);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereColumn(
            const QVector<WhereColumnItem> &values, const QString &condition)
    {
        auto builder = query();

        builder->whereColumn(values, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhereColumn(
            const QVector<WhereColumnItem> &values)
    {
        auto builder = query();

        builder->orWhereColumn(values);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereColumn(
            const QString &first, const QString &comparison,
            const QString &second, const QString &condition)
    {
        auto builder = query();

        builder->whereColumn(first, comparison, second, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhereColumn(
            const QString &first, const QString &comparison, const QString &second)
    {
        auto builder = query();

        builder->orWhereColumn(first, comparison, second);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereColumnEq(
            const QString &first, const QString &second, const QString &condition)
    {
        auto builder = query();

        builder->whereColumnEq(first, second, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhereColumnEq(
            const QString &first, const QString &second)
    {
        auto builder = query();

        builder->orWhereColumnEq(first, second);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereIn(
            const QString &column, const QVector<QVariant> &values,
            const QString &condition, const bool nope)
    {
        auto builder = query();

        builder->whereIn(column, values, condition, nope);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhereIn(
            const QString &column, const QVector<QVariant> &values)
    {
        auto builder = query();

        builder->orWhereIn(column, values);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereNotIn(
            const QString &column, const QVector<QVariant> &values,
            const QString &condition)
    {
        auto builder = query();

        builder->whereNotIn(column, values, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhereNotIn(
            const QString &column, const QVector<QVariant> &values)
    {
        auto builder = query();

        builder->orWhereNotIn(column, values);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereNull(
            const QStringList &columns, const QString &condition, const bool nope)
    {
        auto builder = query();

        builder->whereNull(columns, condition, nope);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereNull(
            const QString &column, const QString &condition, const bool nope)
    {
        auto builder = query();

        builder->whereNull(column, condition, nope);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhereNull(const QStringList &columns)
    {
        auto builder = query();

        builder->orWhereNull(columns);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhereNull(const QString &column)
    {
        auto builder = query();

        builder->orWhereNull(column);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereNotNull(
            const QStringList &columns, const QString &condition)
    {
        auto builder = query();

        builder->whereNotNull(columns, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereNotNull(
            const QString &column, const QString &condition)
    {
        auto builder = query();

        builder->whereNotNull(column, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhereNotNull(const QStringList &columns)
    {
        auto builder = query();

        builder->orWhereNotNull(columns);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orWhereNotNull(const QString &column)
    {
        auto builder = query();

        builder->orWhereNotNull(column);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::groupBy(const QStringList &groups)
    {
        auto builder = query();

        builder->groupBy(groups);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::groupBy(const QString &group)
    {
        auto builder = query();

        builder->groupBy(group);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::having(
            const QString &column, const QString &comparison,
            const QVariant &value, const QString &condition)
    {
        auto builder = query();

        builder->having(column, comparison, value, condition);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orHaving(
            const QString &column, const QString &comparison, const QVariant &value)
    {
        auto builder = query();

        builder->orHaving(column, comparison, value);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orderBy(const QString &column,
                                             const QString &direction)
    {
        auto builder = query();

        builder->orderBy(column, direction);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::orderByDesc(const QString &column)
    {
        auto builder = query();

        builder->orderByDesc(column);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::latest(QString column)
    {
        auto builder = query();

        builder->latest(column);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::oldest(QString column)
    {
        auto builder = query();

        builder->oldest(column);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::reorder()
    {
        auto builder = query();

        builder->reorder();

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::reorder(const QString &column,
                                             const QString &direction)
    {
        auto builder = query();

        builder->reorder(column, direction);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::limit(const int value)
    {
        auto builder = query();

        builder->limit(value);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::take(const int value)
    {
        auto builder = query();

        builder->take(value);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::offset(const int value)
    {
        auto builder = query();

        builder->offset(value);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::skip(const int value)
    {
        auto builder = query();

        builder->skip(value);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::forPage(const int page, const int perPage)
    {
        auto builder = query();

        builder->forPage(page, perPage);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereKey(const QVariant &id)
    {
        auto builder = query();

        builder->whereKey(id);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::whereKeyNot(const QVariant &id)
    {
        auto builder = query();

        builder->whereKeyNot(id);

        return builder;
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::save(const SaveOptions &options)
    {
//        mergeAttributesFromClassCasts();

        // Ownership of a unique_ptr()
        auto query = newModelQuery();

        auto saved = false;

        /* If the "saving" event returns false we'll bail out of the save and return
           false, indicating that the save failed. This provides a chance for any
           listeners to cancel save operations if validations fail or whatever. */
//        if (fireModelEvent('saving') === false) {
//            return false;
//        }

        /* If the model already exists in the database we can just update our record
           that is already in this database using the current IDs in this "where"
           clause to only update this model. Otherwise, we'll just insert them. */
        if (exists)
            saved = isDirty() ? performUpdate(*query) : true;

        // If the model is brand new, we'll insert it into our database and set the
        // ID attribute on the model to the value of the newly inserted row's ID
        // which is typically an auto-increment value managed by the database.
        else {
            saved = performInsert(*query);

            if (const auto &connection = query->getConnection();
                getConnectionName().isEmpty()
            )
                setConnection(connection.getName());
        }

        /* If the model is successfully saved, we need to do a few more things once
           that is done. We will call the "saved" method here to run any actions
           we need to happen after a model gets successfully saved right here. */
        if (saved)
            finishSave(options);

        return saved;
    }

    // TODO future support for SaveOptions parameter, Eloquent doesn't have this parameter, maybe there's a reason for that, but I didn't find anything on github issues silverqx
    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::push()
    {
        if (!save())
            return false;

        /* To sync all of the relationships to the database, we will simply spin through
           the relationships and save each model via this "push" method, which allows
           us to recurse into all of these nested relations for the model instance. */
        for (auto &[relation, models] : m_relations)
            // TODO future, Eloquent uses array_filter on models, investigate when this happens, null value (model) in many relations? silverqx
            /* Following Eloquent API, if any push failed, then quit, remaining push-es
               will not be processed. */
            if (!pushWithVisitor(relation, models))
                return false;

        return true;
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    void Model<Derived, AllRelations...>::pushVisited()
    {
        /* When relationship data member holds QVector, then it is definitely Many
           type relation. */
        if (std::holds_alternative<QVector<Related>>(this->m_pushStore->models))
            pushVisited<Related, Many>();
        else
            pushVisited<Related, One>();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, Many>, bool>>
    void Model<Derived, AllRelations...>::pushVisited()
    {
        for (auto &model : std::get<QVector<Related>>(this->m_pushStore->models))
            if (!model.push()) {
                this->m_pushStore->result = false;
                return;
            }

        this->m_pushStore->result = true;
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    void Model<Derived, AllRelations...>::pushVisited()
    {
        auto &model = std::get<std::optional<Related>>(this->m_pushStore->models);
        // CUR test push with default model silverqx
        Q_ASSERT(!!model);
        // Skip a null model, consider it as success
        if (!model) {
            this->m_pushStore->result = true;
            return;
        }

        this->m_pushStore->result = model->push();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    void Model<Derived, AllRelations...>::touchOwnersVisited()
    {
        const auto &relationName = this->m_touchOwnersStore->relation;

        // Unique pointer to the relation
        auto relation = std::invoke(getRelationMethodRaw<Related>(relationName),
                                    model());
        relation->touch();

        // Many type relation
        if (dynamic_cast<Relations::ManyRelation *>(relation.get()) != nullptr) {
            for (auto *const relatedModel : getRelationValue<Related>(relationName))
                // WARNING check and add note after, if many type relation QVector can contain nullptr silverqx
                if (relatedModel)
                    relatedModel->touchOwners();
        }
        // One type relation
        else
            if (auto *const relatedModel = getRelationValue<Related, One>(relationName);
                relatedModel
            )
                relatedModel->touchOwners();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    void Model<Derived, AllRelations...>::isPivotModelRelationVisited()
    {
        if constexpr (std::is_base_of_v<Relations::IsPivotModel, Related>)
            this->m_isPivotModelStore->result = true;
        else
            this->m_isPivotModelStore->result = false;
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    void Model<Derived, AllRelations...>::relationVisited()
    {
        const auto &method = getMethodForRelationVisited<Related>(
                                 this->m_relationStore->getStoreType());

        std::invoke(method, *this);
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    const std::function<void(Model<Derived, AllRelations...> &)> &
    Model<Derived, AllRelations...>::getMethodForRelationVisited(
            const RelationStoreType storeType) const
    {
        // Pointers to visited member methods by storeType, yes yes c++ 😂
        // An order has to be the same as in enum struct RelationStoreType
        // TODO future should be QHash, for faster lookup, do benchmark, high chance that qvector has faster lookup than qhash silverqx
        static const
        QVector<std::function<void(Model<Derived, AllRelations...> &)>> cached {
            &Model<Derived, AllRelations...>::eagerVisited<Related>,
            &Model<Derived, AllRelations...>::pushVisited<Related>,
            &Model<Derived, AllRelations...>::touchOwnersVisited<Related>,
            &Model<Derived, AllRelations...>::isPivotModelRelationVisited<Related>,
        };
        static const auto size = cached.size();

        // Check if storeType is in the range, just for sure 😏
        const auto type = static_cast<int>(storeType);
        Q_ASSERT((0 <= type) && (type < size));

        return cached.at(type);
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::update(
            const QVector<AttributeItem> &attributes,
            const SaveOptions &options)
    {
        if (!exists)
            return false;

        return fill(attributes).save(options);
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::remove()
    {
        // FEATURE castable silverqx
//        mergeAttributesFromClassCasts();

        if (getKeyName().isEmpty())
            throw RuntimeError("No primary key defined on model.");

        /* If the model doesn't exist, there is nothing to delete so we'll just return
           immediately and not do anything else. Otherwise, we will continue with a
           deletion process on the model, firing the proper events, and so forth. */
        if (!exists)
            // NOTE api different silverqx
            return false;

        // TODO future add support for model events silverqx
//        if ($this->fireModelEvent('deleting') === false) {
//            return false;
//        }

        /* Here, we'll touch the owning models, verifying these timestamps get updated
           for the models. This will allow any caching to get broken on the parents
           by the timestamp. Then we will go ahead and delete the model instance. */
        touchOwners();

        // TODO future performDeleteOnModel() and return value, check logic here, eg what happens when no model is delete and combinations silverqx
        // TODO future inconsistent return values save(), update(), remove(), ... silverqx
        performDeleteOnModel();

        /* Once the model has been deleted, we will fire off the deleted event so that
           the developers may hook into post-delete operations. We will then return
           a boolean true as the delete is presumably successful on the database. */
//        $this->fireModelEvent('deleted', false);

        return true;
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::deleteModel()
    {
        return model().remove();
    }

    template<typename Derived, typename ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::fresh(
            const QVector<WithItem> &relations)
    {
        if (!exists)
            return std::nullopt;

        return model().setKeysForSelectQuery(*newQueryWithoutScopes())
                .with(relations)
                .first();
    }

    template<typename Derived, typename ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::fresh(const QString &relation)
    {
        return fresh(QVector<WithItem> {{relation}});
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::refresh()
    {
        if (!exists)
            return model();

        setRawAttributes(model().setKeysForSelectQuery(*newQueryWithoutScopes())
                         .firstOrFail().getRawAttributes());

        // And reload them again, refresh relations
        load(getLoadedRelationsWithoutPivot());

        syncOriginal();

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    QVector<WithItem>
    Model<Derived, AllRelations...>::getLoadedRelationsWithoutPivot()
    {
        QVector<WithItem> relations;

        /* Current model (this) contains a pivot relation alternative
           in the m_relations std::variant. */
        auto hasPivotRelation = std::disjunction_v<std::is_base_of<
                                Relations::IsPivotModel, AllRelations>...>;

        /* Get all currently loaded relation names except pivot relations. We need
           to check for the pivot models, but only if the std::variant which holds
           relations also holds a pivot model alternative, otherwise it is useless. */
        for (const auto &relation : m_relations) {
            const auto &relationName = relation.first;

            if (hasPivotRelation)
                // Skip pivot relations
                if (isPivotModelWithVisitor(relationName))
                    continue;

            relations.append({relationName});
        }

        return relations;
    }

    template<typename Derived, typename ...AllRelations>
    bool
    Model<Derived, AllRelations...>::isPivotModelWithVisitor(const QString &relation)
    {
        this->createIsPivotModelStore();

        // TODO next create wrapper method for this with better name silverqx
        model().relationVisitor(relation);

        bool isPivotModel = this->m_isPivotModelStore->result;

        // Release stored pointers to the relation store
        this->resetRelationStore();

        return isPivotModel;
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::pushWithVisitor(
            const QString &relation, RelationsType<AllRelations...> &models)
    {
        // TODO prod remove, I don't exactly know if this can really happen silverqx
        /* Check for empty variant, the std::monostate is at zero index and
           consider it as success to continue 'pushing'. */
        const auto variantIndex = models.index();
        Q_ASSERT(variantIndex > 0);
        if (variantIndex == 0)
            return true;

        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->createPushStore(models);

        // TODO next create wrapper method for this with better name silverqx
        model().relationVisitor(relation);

        bool pushResult = this->m_pushStore->result;

        // Release stored pointers to the relation store
        this->resetRelationStore();

        return pushResult;
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::replaceRelations(
            std::unordered_map<QString, RelationsType<AllRelations...>> &relations,
            const QVector<WithItem> &onlyRelations)
    {
        /* Replace only relations which was passed to this method, leave other
           relations untouched. */
        for (auto itRelation = relations.begin(); itRelation != relations.end();
             ++itRelation)
        {
            const auto &key = itRelation->first;

            const auto relationsContainKey =
                    ranges::contains(onlyRelations, true, [&key](const auto &relation)
            {
                return relation.name == key;
            });

            if (!relationsContainKey)
                continue;

            m_relations[key] = std::move(itRelation->second);
        }
    }

    template<typename Derived, typename ...AllRelations>
    void
    Model<Derived, AllRelations...>::touchOwnersWithVisitor(const QString &relation)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->createTouchOwnersStore(relation);

        // TODO next create wrapper method for this with better name, over time, it seems fine to me, only thing I see now is that it has the same name for all visitations, but that looks ok too, will see, but this todo can be removed, looks good to me silverqx
        model().relationVisitor(relation);

        // Release stored pointers to the relation store
        this->resetRelationStore();
    }

    // TODO future LoadItem for Model::load() even it will have the same implmentation, or common parent and inherit silverqx
    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::load(
            const QVector<WithItem> &relations)
    {
        auto builder = newQueryWithoutRelationships()->with(relations);

        // TODO future, make possible to pass single model to eagerLoadRelations() and whole relation flow, I indicative counted how many methods would have to rewrite and it is around 12 methods silverqx
        /* I have to make a copy here of this, because of eagerLoadRelations(),
           the solution would be to add a whole new chain for eager load relations,
           which will be able to work only on one Model &, but it is around
           refactoring of 10-15 methods, or add a variant which can process
           QVector<std::reference_wrapper<Derived>>.
           For now, I have made a copy here and save it into the QVector and after
           that move relations from this copy to the real instance. */
        QVector<Derived> models {model()};

        builder.eagerLoadRelations(models);

        /* Replace only relations which was passed to this method, leave other
           relations untouched.
           They do not need to be removed before 'eagerLoadRelations(models)'
           call, because only the relations passed to the 'with' at the beginning
           will be loaded anyway. */
        replaceRelations(models.first().getRelations(), relations);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::load(const QString &relation)
    {
        return load(QVector<WithItem> {{relation}});
    }

    // TODO add clean Model overloads, I don't remember what it exactly mean or better why should I need Model overloads silverqx
    template<typename Derived, typename ...AllRelations>
    template<typename ModelToCompare>
    bool Model<Derived, AllRelations...>::is(
            const std::optional<ModelToCompare> &model) const
    {
        return model
                && getKey() == model->getKey()
                && this->model().getTable() == model->getTable()
                && getConnectionName() == model->getConnectionName();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename ModelToCompare>
    bool Model<Derived, AllRelations...>::isNot(
            const std::optional<ModelToCompare> &model) const
    {
        return !is(model);
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::fill(const QVector<AttributeItem> &attributes)
    {
        const auto totallyGuarded = this->totallyGuarded();

        for (auto &attribute : fillableFromArray(attributes))
            /* The developers may choose to place some attributes in the "fillable" vector
               which means only those attributes may be set through mass assignment to
               the model, and all others will just get ignored for security reasons. */
            if (auto &key = attribute.key;
                isFillable(key)
            )
                setAttribute(key, std::move(attribute.value));

            else if (totallyGuarded)
                throw MassAssignmentError(
                        QStringLiteral("Add '%1' to u_fillable data member to allow "
                                       "mass assignment on '%2'.")
                        .arg(key, Utils::Type::classPureBasename<Derived>()));

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::fill(QVector<AttributeItem> &&attributes)
    {
        const auto totallyGuarded = this->totallyGuarded();

        for (auto &attribute : fillableFromArray(std::move(attributes))) {
            /* The developers may choose to place some attributes in the "fillable" vector
               which means only those attributes may be set through mass assignment to
               the model, and all others will just get ignored for security reasons. */
            if (auto &key = attribute.key;
                isFillable(key)
            )
                setAttribute(key, std::move(attribute.value));

            else if (totallyGuarded)
                throw MassAssignmentError(
                        QStringLiteral("Add '%1' to u_fillable data member to allow "
                                       "mass assignment on '%2'.")
                        .arg(key, Utils::Type::classPureBasename<Derived>()));
        }

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::forceFill(
            const QVector<AttributeItem> &attributes)
    {
        // Prevent unnecessary unguard
        if (attributes.isEmpty())
            return model();

        unguarded([this, &attributes]
        {
            fill(attributes);
        });

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQueryWithoutScopes()
    {
        auto tinyBuilder = newModelQuery();

        tinyBuilder->with(model().u_with);

        return tinyBuilder;
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newModelQuery()
    {
        /* Model is passed to the TinyBuilder ctor, because of that setModel()
           isn't used here. Can't be const because of passed non-const model
           to the TinyBuilder. */
        return newTinyBuilder(newBaseQueryBuilder());
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQueryWithoutRelationships()
    {
        return newModelQuery();
    }

    template<typename Derived, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newTinyBuilder(
            const QSharedPointer<QueryBuilder> query)
    {
        return std::make_unique<TinyBuilder<Derived>>(query, model());
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newFromBuilder(
            const QVector<AttributeItem> &attributes,
            const std::optional<QString> connection)
    {
        auto model = newInstance({}, true);

        model.setRawAttributes(attributes, true);

        model.setConnection(connection ? *connection : getConnectionName());

        return model;
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newInstance(
            const QVector<AttributeItem> &attributes, const bool exists)
    {
        /* This method just provides a convenient way for us to generate fresh model
           instances of this current model. It is particularly useful during the
           hydration of new objects via the Eloquent query builder instances. */
        Derived model;

        /* setAttribute() can call getDateFormat() inside and it tries to obtain
           the date format from grammar which is obtained from the connection, so
           the connection have to be set before fill(). */
        model.setConnection(getConnectionName());

        model.fill(attributes);

        model.exists = exists;
        model.setTable(this->model().getTable());

        return model;
    }

    template<typename Derived, typename ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newInstance(
            QVector<AttributeItem> &&attributes, const bool exists)
    {
        /* This method just provides a convenient way for us to generate fresh model
           instances of this current model. It is particularly useful during the
           hydration of new objects via the Eloquent query builder instances. */
        Derived model;

        /* setAttribute() can call getDateFormat() inside and it tries to obtain
           the date format from grammar which is obtained from the connection, so
           the connection have to be set before fill(). */
        model.setConnection(getConnectionName());

        model.fill(std::move(attributes));

        model.exists = exists;
        model.setTable(this->model().getTable());

        return model;
    }

    template<typename Derived, typename ...AllRelations>
    template<typename PivotType, typename Parent>
    PivotType
    Model<Derived, AllRelations...>::newPivot(
            const Parent &parent, const QVector<AttributeItem> &attributes,
            const QString &table, const bool exists) const
    {
        if constexpr (std::is_same_v<PivotType, Relations::Pivot>)
            return PivotType::template fromAttributes<Parent>(
                        parent, attributes, table, exists);
        else
            return PivotType::template fromRawAttributes<Parent>(
                        parent, attributes, table, exists);
    }

    template<typename Derived, typename ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getConnectionName() const
    {
#ifdef TINYORM_TESTS_CODE
        // Used from tests to override connection
        if (const auto &connection = ConnectionOverride::connection;
            !connection.isEmpty()
        )
            return connection;
#endif

        return model().u_connection;
    }

    template<typename Derived, typename ...AllRelations>
    QString Model<Derived, AllRelations...>::getTable() const
    {
        const auto &table = model().u_table;

        // Get pluralized snake-case table name
        if (table.isEmpty())
            return Utils::String::toSnake(Utils::Type::classPureBasename<Derived>())
                    + QChar('s');

        return table;
    }

    template<typename Derived, typename ...AllRelations>
    QSharedPointer<QueryBuilder>
    Model<Derived, AllRelations...>::newBaseQueryBuilder() const
    {
        return getConnection().query();
    }

    // CUR return optional reference, I remember that I abandoned it, but it is much better to avoid nullptr checks, anyway investigate it silverqx
    template<typename Derived, typename ...AllRelations>
    template<typename Related, template<typename> typename Container>
    const Container<Related *>
    Model<Derived, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships hash, it just means the
            relationship has already been loaded, so we'll just return it out of
            here because there is no need to query within the relations twice. */
        if (relationLoaded(relation))
            return getRelationFromHash<Related, Container>(relation);

        /*! If the relation is defined on the model, then lazy load and return results
            from the query and hydrate the relationship's value on the "relationships"
            data member m_relations. */
        if (model().u_relations.contains(relation))
            return getRelationshipFromMethod<Related, Container>(relation);

        return {};
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    Related *
    Model<Derived, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships hash, it just means the
            relationship has already been loaded, so we'll just return it out of
            here because there is no need to query within the relations twice. */
        if (relationLoaded(relation))
            return getRelationFromHash<Related, One>(relation);

        /*! If the relation is defined on the model, then lazy load and return results
            from the query and hydrate the relationship's value on the "relationships"
            data member m_relations. */
        if (model().u_relations.contains(relation))
            return getRelationshipFromMethod<Related, One>(relation);

        return nullptr;
    }

    template<typename Derived, typename ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related *>
    Model<Derived, AllRelations...>::getRelationshipFromMethod(const QString &relation)
    {
        // Obtain related models
        auto relatedModels = std::get<QVector<Related>>(
                std::invoke(getRelationMethodRaw<Related>(relation), model())
                        ->getResults());

        setRelation(relation, std::move(relatedModels));

        return getRelationFromHash<Related, Container>(relation);
    }

    template<typename Derived, typename ...AllRelations>
    template<class Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    Related *
    Model<Derived, AllRelations...>::getRelationshipFromMethod(const QString &relation)
    {
        // Obtain related model
        auto relatedModel = std::get<std::optional<Related>>(
                std::invoke(getRelationMethodRaw<Related>(relation), model())
                        ->getResults());

        setRelation(relation, std::move(relatedModel));

        return getRelationFromHash<Related, One>(relation);
    }

    template<typename Derived, typename ...AllRelations>
    QVector<AttributeItem>
    Model<Derived, AllRelations...>::getDirty() const
    {
        QVector<AttributeItem> dirty;

        for (const auto &attribute : getAttributes())
            if (const auto &key = attribute.key;
                !originalIsEquivalent(key)
            )
                dirty.append({key, attribute.value});

        return dirty;
    }

    template<typename Derived, typename ...AllRelations>
    std::unordered_map<QString, int>
    Model<Derived, AllRelations...>::getDirtyHash() const
    {
        const auto size = m_attributes.size();
        std::unordered_map<QString, int> dirtyHash(size);

        for (auto i = 0; i < size; ++i)
            if (const auto &key = m_attributes.at(i).key;
                !originalIsEquivalent(key)
            )
                dirtyHash.emplace(m_attributes.at(i).key, i);

        return dirtyHash;
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::hasChanges(
            const std::unordered_map<QString, int> &changes,
            const QStringList &attributes) const
    {
        /* If no specific attributes were provided, we will just see if the dirty hash
           already contains any attributes. If it does we will just return that this
           count is greater than zero. Else, we need to check specific attributes. */
        if (attributes.isEmpty())
            return changes.size() > 0;

        /* Here we will spin through every attribute and see if this is in the hash of
           dirty attributes. If it is, we will return true and if we make it through
           all of the attributes for the entire vector we will return false at end. */
        for (const auto &attribute : attributes)
            if (changes.contains(attribute))
                return true;

        return false;
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::syncChanges()
    {
        m_changes = getDirty();

        rehashAttributePositions(m_changes, m_changesHash);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    bool
    Model<Derived, AllRelations...>::originalIsEquivalent(const QString &key) const
    {
        if (!m_originalHash.contains(key))
            return false;

        const auto attribute = getAttributeFromArray(key);
        const auto original = getRawOriginal(key);

        // Takes into account also milliseconds for the QDateTime attribute
        if (attribute == original)
            return true;
        // TODO next solve how to work with null values and what to do with invalid/unknown values silverqx
        else if (!attribute.isValid() || attribute.isNull())
            return false;
        // This check ignores milliseconds in the QDateTime attribute
        else if (isDateAttribute(key))
            return fromDateTime(attribute) == fromDateTime(original);
//        else if (hasCast(key, ['object', 'collection']))
//            return castAttribute(key, attribute) == castAttribute(key, original);
//        else if (hasCast(key, ['real', 'float', 'double'])) {
//            if (($attribute === null && $original !== null) || ($attribute !== null && $original === null))
//                return false;

//            return abs($this->castAttribute($key, $attribute) - $this->castAttribute($key, $original)) < PHP_FLOAT_EPSILON * 4;
//        } elseif ($this->hasCast($key, static::$primitiveCastTypes)) {
//            return $this->castAttribute($key, $attribute) ===
//                   $this->castAttribute($key, $original);
//        }

//        return is_numeric($attribute) && is_numeric($original)
//               && strcmp((string) $attribute, (string) $original) === 0;

        return false;
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::isDateAttribute(const QString &key) const
    {
        // FEATURE castable silverqx
        /* I don't have support for castable attributes, this solution is temporary. */
        return getDates().contains(key);
    }

    // TODO would be good to make it the c++ way, make overload for every type, asDateTime() is protected, so I have full control over it, but I leave it for now, because there will be more methods which will use this method in the future, and it will be more clear later on silverqx
    template<typename Derived, typename ...AllRelations>
    QDateTime
    Model<Derived, AllRelations...>::asDateTime(const QVariant &value) const
    {
        /* If this value is already a QDateTime instance, we shall just return it as is.
           This prevents us having to re-parse a QDateTime instance when we know
           it already is one. */
        if (
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            value.typeId() == QMetaType::QDateTime
#else
            value.userType() == QMetaType::QDateTime
#endif
        )
            return value.value<QDateTime>();

        /* If this value is an integer, we will assume it is a UNIX timestamp's value
           and format a Carbon object from this timestamp. This allows flexibility
           when defining your date fields as they might be UNIX timestamps here. */
        if (value.canConvert<qint64>() &&
            QRegularExpression(QStringLiteral("^\\d{1,20}$"))
            .match(value.value<QString>()).hasMatch()
        )
            // TODO switch ms accuracy? For the u_dateFormat too? silverqx
            return QDateTime::fromSecsSinceEpoch(value.value<qint64>());

        const auto &format = getDateFormat();

        /* Finally, we will just assume this date is in the format used by default on
           the database connection and use that format to create the QDateTime object
           that is returned back out to the developers after we convert it here. */
        if (const auto date = QDateTime::fromString(value.value<QString>(), format);
            date.isValid()
        )
            return date;

        throw InvalidFormatError(
                    QStringLiteral("Could not parse the datetime '%1' using "
                                   "the given format '%2'.")
                    .arg(value.value<QString>(), format));
    }

    template<typename Derived, typename ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::timestampColumnNames() const
    {
        /* Fuckin static, it works like is described here:
           https://stackoverflow.com/questions/2737013/static-variables-in-static-method-in-base-class-and-inheritance. */
        static const QStringList cached {
            getCreatedAtColumn(),
            getUpdatedAtColumn(),
        };

        return cached;
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::rehashAttributePositions(
            const QVector<AttributeItem> &attributes,
            std::unordered_map<QString, int> &attributesHash,
            const int from)
    {
        /* This member function is universal and can be used for m_attributes,
           m_changes and m_original and it associated unordered_maps m_attributesHash,
           m_changesHash and m_originalHash. */
        for (auto i = from; i < attributes.size(); ++i)
            // 'i' is the position
            attributesHash[attributes.at(i).key] = i;
    }

    template<typename Derived, typename ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related *>
    Model<Derived, AllRelations...>::getRelationFromHash(const QString &relation)
    {
        /* Obtain related models from data member hash as QVector, it is internal
           format and transform it into a Container of pointers to related models,
           so a user can directly modify these models and push or save them
           afterward. */
        using namespace ranges;
        return std::get<QVector<Related>>(m_relations.find(relation)->second)
                | views::transform([](Related &model) -> Related * { return &model; })
                | ranges::to<Container<Related *>>();
    }

    template<typename Derived, typename ...AllRelations>
    template<class Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    Related *
    Model<Derived, AllRelations...>::getRelationFromHash(const QString &relation)
    {
        /* Obtain related model from data member hash and return it as a pointer or
           nullptr if no model is associated, so a user can directly modify this
           model and push or save it afterward. */

        auto &relatedModel =
                std::get<std::optional<Related>>(m_relations.find(relation)->second);

        return relatedModel ? &*relatedModel : nullptr;
    }

    // TODO make getRelation() Container argument compatible with STL containers API silverqx
    // TODO solve different behavior like Eloquent getRelation() silverqx
    // TODO next many relation compiles with Orm::One and exception during runtime occures, solve this during compile, One relation only with Orm::One and many relation type only with Container version silverqx
    // Return const value to prevent clazy warning about detaching container
    template<typename Derived, typename ...AllRelations>
    template<typename Related, template<typename> typename Container>
    const Container<Related *>
    Model<Derived, AllRelations...>::getRelation(const QString &relation)
    {
        if (!relationLoaded(relation))
            throw RelationNotLoadedError(
                    Orm::Utils::Type::classPureBasename<Derived>(), relation);

        return getRelationFromHash<Related, Container>(relation);
    }

    // TODO smart pointer for this relation stuffs? silverqx
    template<typename Derived, typename ...AllRelations>
    template<typename Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    Related *
    Model<Derived, AllRelations...>::getRelation(const QString &relation)
    {
        if (!relationLoaded(relation))
            throw RelationNotLoadedError(
                    Orm::Utils::Type::classPureBasename<Derived>(), relation);

        // TODO instantiate relation by name and check if is_base_of OneRelation/ManyRelation, to have nice exception message (in debug mode only), because is impossible to check this during compile time silverqx

        return getRelationFromHash<Related, One>(relation);
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 const QVector<Related> &models)
    {
        m_relations[relation] = models;

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 QVector<Related> &&models)
    {
        m_relations[relation] = std::move(models);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 const std::optional<Related> &model)
    {
        m_relations[relation] = model;

        return this->model();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 std::optional<Related> &&model)
    {
        m_relations[relation] = std::move(model);

        return this->model();
    }

    template<typename Derived, typename ...AllRelations>
    QString
    Model<Derived, AllRelations...>::qualifyColumn(const QString &column) const
    {
        if (column.contains('.'))
            return column;

        return model().getTable() + '.' + column;
    }

    // TODO move, add rvalue version, for key parameter too silverqx
    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::setAttribute(
            const QString &key, QVariant value)
    {
        /* If an attribute is listed as a "date", we'll convert it from a DateTime
           instance into a form proper for storage on the database tables using
           the connection grammar's date format. We will auto set the values. */
        if (value.isValid() && !value.isNull() && (isDateAttribute(key) ||
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            value.typeId() == QMetaType::QDateTime
#else
            value.userType() == QMetaType::QDateTime
#endif
        ))
            value = fromDateTime(value);

        // Found
        if (const auto attribute = m_attributesHash.find(key);
            attribute != m_attributesHash.end()
        )
            m_attributes[attribute->second].value.swap(value);

        // Not Found
        else {
            auto position = m_attributes.size();

            m_attributes.append({key, value});
            m_attributesHash.emplace(key, position);
        }

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setRawAttributes(
            const QVector<AttributeItem> &attributes,
            const bool sync)
    {
        m_attributes.reserve(attributes.size());
        m_attributes = Utils::Attribute::removeDuplicitKeys(attributes);

        // Build attributes hash
        m_attributesHash.clear();
        m_attributesHash.reserve(m_attributes.size());

        rehashAttributePositions(m_attributes, m_attributesHash);

        if (sync)
            syncOriginal();

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::syncOriginal()
    {
        m_original = getAttributes();

        rehashAttributePositions(m_original, m_originalHash);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    const QVector<AttributeItem> &
    Model<Derived, AllRelations...>::getAttributes() const
    {
        // FEATURE castable silverqx
//        mergeAttributesFromClassCasts();

        // TODO attributes, getAttributes() doesn't apply transformModelValue() on attributes, worth considering to make getRawAttributes() to return raw and getAttributes() to return transformed values, after this changes would be this api different than Eloquent silverqx
        return m_attributes;
    }

    template<typename Derived, typename ...AllRelations>
    const std::unordered_map<QString, int> &
    Model<Derived, AllRelations...>::getAttributesHash() const
    {
        // FEATURE castable silverqx
//        mergeAttributesFromClassCasts();

        return m_attributesHash;
    }

    template<typename Derived, typename ...AllRelations>
    QVariant Model<Derived, AllRelations...>::getAttribute(const QString &key) const
    {
        if (key.isEmpty() || key.isNull())
            return {};

        /* If the attribute exists in the attribute hash or has a "get" mutator we will
           get the attribute's value. Otherwise, we will proceed as if the developers
           are asking for a relationship's value. This covers both types of values. */
        if (m_attributesHash.contains(key)
//            || array_key_exists($key, $this->casts)
//            || hasGetMutator(key)
//            || isClassCastable(key)
        )
            return getAttributeValue(key);

        // NOTE api different silverqx
        return {};
        // TODO Eloquent returns relation when didn't find attribute, decide how to solve this silverqx
//        return $this->getRelationValue($key);
    }

    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getAttributeValue(const QString &key) const
    {
        return transformModelValue(key, getAttributeFromArray(key));
    }

    // TODO candidate for optional const reference, to be able return null value and use reference at the same time silverqx
    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getAttributeFromArray(const QString &key) const
    {
        // Not found
        if (!m_attributesHash.contains(key))
            return {};

        return m_attributes.at(m_attributesHash.at(key)).value;
    }

    // NOTE api different silverqx
    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getOriginal(
            const QString &key, const QVariant &defaultValue) const
    {
        return Derived().setRawAttributes(m_original, true)
                .getOriginalWithoutRewindingModel(key, defaultValue);
    }

    template<typename Derived, typename ...AllRelations>
    QVector<AttributeItem>
    Model<Derived, AllRelations...>::getOriginals() const
    {
        QVector<AttributeItem> originals;
        originals.reserve(m_original.size());

        for (const auto &original : m_original) {
            const auto &key = original.key;

            originals.append({key, transformModelValue(key, original.value)});
        }

        return originals;
    }

    template<typename Derived, typename ...AllRelations>
    const std::unordered_map<QString, int> &
    Model<Derived, AllRelations...>::getOriginalsHash() const
    {
        return m_originalHash;
    }

    // NOTE api different silverqx
    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getRawOriginal(
            const QString &key, const QVariant &defaultValue) const
    {
        // Found
        if (m_originalHash.contains(key))
            return m_original.at(m_originalHash.at(key)).value;

        // Not found, return the default value
        return defaultValue;
    }

    template<typename Derived, typename ...AllRelations>
    const QVector<AttributeItem> &
    Model<Derived, AllRelations...>::getRawOriginals() const
    {
        return m_original;
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::unsetAttribute(const AttributeItem &value)
    {
        const auto &key = value.key;

        // Not found
        if (!m_attributesHash.contains(key))
            return model();

        const auto position = m_attributesHash.at(key);

        // TODO future, all the operations on this containers should be synchronized silverqx
        m_attributes.removeAt(position);
        m_attributesHash.erase(key);

        // Rehash attributes, but only attributes which were shifted
        rehashAttributePositions(m_attributes, m_attributesHash, position);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::unsetAttribute(const QString &key)
    {
        // Not found
        if (!m_attributesHash.contains(key))
            return model();

        const auto position = m_attributesHash.at(key);

        m_attributes.removeAt(position);
        m_attributesHash.erase(key);

        // Rehash attributes, but only attributes which were shifted
        rehashAttributePositions(m_attributes, m_attributesHash, position);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getDateFormat() const
    {
        return Derived::u_dateFormat.isEmpty()
                ? getConnection().getQueryGrammar().getDateFormat()
                : Derived::u_dateFormat;
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setDateFormat(const QString &format)
    {
        Derived::u_dateFormat = format;

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::fromDateTime(const QVariant &value) const
    {
        if (value.isNull())
            return value;

        return asDateTime(value).toString(getDateFormat());
    }

    template<typename Derived, typename ...AllRelations>
    QString
    Model<Derived, AllRelations...>::fromDateTime(const QDateTime &value) const
    {
        if (value.isValid())
            return value.toString(getDateFormat());

        return {};
    }

    template<typename Derived, typename ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getDates() const
    {
        static const QStringList &dates = getDatesInternal();

        return dates;
    }

    /* Model::AttributeReference - begin */

    template<typename Derived, typename ...AllRelations>
    Model<Derived, AllRelations...>::AttributeReference::AttributeReference(
            Model<Derived, AllRelations...> &model, const QString &attribute
    )
        : m_model(model)
        , m_attribute(attribute)
    {}

    template<typename Derived, typename ...AllRelations>
    const typename Model<Derived, AllRelations...>::AttributeReference &
    Model<Derived, AllRelations...>::AttributeReference::operator=(
            const QVariant &value) const
    {
        m_model.setAttribute(m_attribute, value);

        return *this;
    }

    template<typename Derived, typename ...AllRelations>
    const typename Model<Derived, AllRelations...>::AttributeReference &
    Model<Derived, AllRelations...>::AttributeReference::operator=(
            const AttributeReference &attributeReference) const
    {
        m_model.setAttribute(m_attribute, attributeReference.value());

        return *this;
    }


    template<typename Derived, typename ...AllRelations>
    const QVariant *
    Model<Derived, AllRelations...>::AttributeReference::operator->() const
    {
        m_attributeCache = value();

        return &m_attributeCache;
    }

    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::AttributeReference::value() const
    {
        return m_model.getAttribute(m_attribute);
    }

    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::AttributeReference::operator*() const
    {
        return value();
    }

    template<typename Derived, typename ...AllRelations>
    Model<Derived, AllRelations...>::AttributeReference::operator QVariant() const
    {
        return value();
    }

    /* Model::AttributeReference - end */

    template<typename Derived, typename ...AllRelations>
    typename Model<Derived, AllRelations...>::AttributeReference
    Model<Derived, AllRelations...>::operator[](const QString &attribute) &
    {
        return AttributeReference(*this, attribute);
    }

    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::operator[](const QString &attribute) const &
    {
        return getAttribute(attribute);
    }

    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::operator[](const QString &attribute) &&
    {
        return getAttribute(attribute);
    }

    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::operator[](const QString &attribute) const &&
    {
        return getAttribute(attribute);
    }

    template<typename Derived, typename ...AllRelations>
    QVariant Model<Derived, AllRelations...>::transformModelValue(
            const QString &key,
            const QVariant &value) const
    {
        /* Qt's SQLite driver doesn't apply any logic on the returned types, it returns
           them without type detection, and it is logical, because SQLite only supports
           numeric and string types, it doesn't distinguish datetime type or any other
           types.
           Qt's MySql driver behaves differently, QVariant already contains the QDateTime
           values, because Qt's MySQL driver returns QDateTime when the value from
           the database is datetime, the same is true for all other types, Qt's driver
           detects it and creates QVariant with proper types. */

        if (!value.isValid() || value.isNull())
            return value;

        /* If the attribute is listed as a date, we will convert it to a QDateTime
           instance on retrieval, which makes it quite convenient to work with
           date fields without having to create a mutator for each property. */
        if (getDates().contains(key))
            return asDateTime(value);

        return value;
    }

    template<typename Derived, typename ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getOriginalWithoutRewindingModel(
            const QString &key, const QVariant &defaultValue) const
    {
        // Found
        if (m_originalHash.contains(key))
            return transformModelValue(key, m_original.at(m_originalHash.at(key)).value);

        // Not found, return the default value
        return defaultValue;
    }

    template<typename Derived, typename ...AllRelations>
    bool
    Model<Derived, AllRelations...>::relationLoaded(const QString &relation) const
    {
        return m_relations.contains(relation);
    }

    template<typename Derived, typename ...AllRelations>
    QString Model<Derived, AllRelations...>::getForeignKey() const
    {
        return QStringLiteral("%1_%2").arg(
                    Utils::String::toSnake(
                        Utils::Type::classPureBasename<Derived>()),
                    getKeyName());
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::touchOwners()
    {
        for (const auto &relation : getTouchedRelations())
            touchOwnersWithVisitor(relation);
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::unsetRelations()
    {
        m_relations.clear();

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::unsetRelation(const QString &relation)
    {
        m_relations.erase(relation);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Related>
    Model<Derived, AllRelations...>::newRelatedInstance() const
    {
        auto instance = std::make_unique<Related>();

        if (instance->getConnectionName().isEmpty())
            instance->setConnection(getConnectionName());

        return instance;
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasOne<Derived, Related>>
    Model<Derived, AllRelations...>::hasOne(QString foreignKey, QString localKey)
    {
        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = model().getForeignKey();

        if (localKey.isEmpty())
            localKey = getKeyName();

        return newHasOne<Related>(std::move(instance), model(),
                                  instance->getTable() + '.' + foreignKey, localKey);
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::BelongsTo<Derived, Related>>
    Model<Derived, AllRelations...>::belongsTo(QString foreignKey, QString ownerKey,
                                               QString relation)
    {
        /* If no relation name was given, we will use the Related class type to extract
           the name and use that as the relationship name as most of the time this
           will be what we desire to use for the belongsTo relationships. */
        if (relation.isEmpty())
            relation = guessBelongsToRelation<Related>();

        auto instance = newRelatedInstance<Related>();

        const auto &primaryKey = instance->getKeyName();

        /* If no foreign key was supplied, we can guess the proper foreign key name
           by using the snake case name of the relationship, which when combined
           with an "_id" should conventionally match the columns. */
        if (foreignKey.isEmpty())
            foreignKey = Utils::String::toSnake(relation) + '_' + primaryKey;

        /* Once we have the foreign key names, we return the relationship instance,
           which will actually be responsible for retrieving and hydrating every
           relations. */
        if (ownerKey.isEmpty())
            ownerKey = primaryKey;

        return newBelongsTo<Related>(std::move(instance), model(),
                                     foreignKey, ownerKey, relation);
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasMany<Derived, Related>>
    Model<Derived, AllRelations...>::hasMany(QString foreignKey, QString localKey)
    {
        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = model().getForeignKey();

        if (localKey.isEmpty())
            localKey = getKeyName();

        return newHasMany<Related>(std::move(instance), model(),
                                   instance->getTable() + '.' + foreignKey, localKey);
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related, typename PivotType>
    std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
    Model<Derived, AllRelations...>::belongsToMany(
            QString table, QString foreignPivotKey, QString relatedPivotKey,
            QString parentKey, QString relatedKey, QString relation)
    {
        /* If no relation name was given, we will use the Related class type to extract
           the name, suffix it with 's' and use that as the relationship name, as most
           of the time this will be what we desire to use for the belongsToMany
           relationships. */
        if (relation.isEmpty())
            relation = guessBelongsToManyRelation<Related>();

        /* First, we'll need to determine the foreign key and "other key"
           for the relationship. Once we have determined the keys, we'll make
           the relationship instance we need for this. */
        auto instance = newRelatedInstance<Related>();

        if (foreignPivotKey.isEmpty())
            foreignPivotKey = model().getForeignKey();

        if (relatedPivotKey.isEmpty())
            relatedPivotKey = instance->getForeignKey();

        /* If no table name was provided, we can guess it by concatenating the two
           models using underscores in alphabetical order. The two model names
           are transformed to snake case from their default CamelCase also. */
        if (table.isEmpty())
            table = pivotTableName<Related>();

        if (parentKey.isEmpty())
            parentKey = getKeyName();

        if (relatedKey.isEmpty())
            relatedKey = instance->getKeyName();

        return newBelongsToMany<Related, PivotType>(
                    std::move(instance), model(), table, foreignPivotKey,
                    relatedPivotKey, parentKey, relatedKey, relation);
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::guessBelongsToRelation() const
    {
        // TODO reliability, also add Utils::String::studly silverqx
        auto relation = Utils::Type::classPureBasename<Related>();

        relation[0] = relation[0].toLower();

        return relation;
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::guessBelongsToManyRelation() const
    {
        return guessBelongsToRelation<Related>() + QChar('s');
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::pivotTableName() const
    {
        /* The joining table name, by convention, is simply the snake cased, models
           sorted alphabetically and concatenated with an underscore, so we can
           just sort the models and join them together to get the table name. */
        QStringList segments {
            // The table name of the current model instance
            Utils::Type::classPureBasename<Derived>(),
            // The table name of the related model instance
            Utils::Type::classPureBasename<Related>(),
        };

        /* Now that we have the model names in the vector, we can just sort them and
           use the join function to join them together with an underscore,
           which is typically used by convention within the database system. */
        segments.sort(Qt::CaseInsensitive);

        return segments.join(QChar('_')).toLower();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setRelations(
            const std::unordered_map<QString, RelationsType<AllRelations...>> &relations)
    {
        m_relations = relations;

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setRelations(
            std::unordered_map<QString, RelationsType<AllRelations...>> &&relations)
    {
        m_relations = std::move(relations);

        return model();
    }

    /* GuardsAttributes */

    template<typename Derived, typename ...AllRelations>
    bool
    Model<Derived, AllRelations...>::isGuardableColumn(const QString &key) const
    {
        // NOTE api different, Eloquent caches it only by the model name silverqx
        // Cache columns by the connection and model name
        const auto guardableKey = getKeyForGuardableHash();

        if (!m_guardableColumns.contains(guardableKey))
            m_guardableColumns[guardableKey] = getConnection()
                                          .getSchemaBuilder()
                                          ->getColumnListing(getTable());

        return m_guardableColumns[guardableKey].contains(key);
    }

    template<typename Derived, typename ...AllRelations>
    QString Model<Derived, AllRelations...>::getKeyForGuardableHash() const
    {
        return QStringLiteral("%1-%2").arg(getConnectionName(),
                                           Utils::Type::classPureBasename<Derived>());
    }

    template<typename Derived, typename ...AllRelations>
    QVector<AttributeItem>
    Model<Derived, AllRelations...>::fillableFromArray(
            const QVector<AttributeItem> &attributes) const
    {
        const auto &fillable = getFillable();

        if (fillable.isEmpty() || m_unguarded)
            return attributes;

        QVector<AttributeItem> result;

        for (const auto &attribute : attributes)
            if (fillable.contains(attribute.key))
                result.append(attribute);

        return result;
    }

    template<typename Derived, typename ...AllRelations>
    QVector<AttributeItem>
    Model<Derived, AllRelations...>::fillableFromArray(
            QVector<AttributeItem> &&attributes) const
    {
        const auto &fillable = getFillable();

        if (fillable.isEmpty() || m_unguarded)
            return std::move(attributes);

        QVector<AttributeItem> result;

        for (auto &attribute : attributes)
            if (fillable.contains(attribute.key))
                result.append(std::move(attribute));

        return result;
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::eagerLoadRelationVisitor(
            const WithItem &relation, TinyBuilder<Derived> &builder,
            QVector<Derived> &models)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation.name);

        // Save the needed variables to the store to avoid passing variables to the visitor
        this->createEagerStore(relation, builder, models);

        model().relationVisitor(relation.name);

        this->resetRelationStore();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    typename Model<Derived, AllRelations...>::template RelationType<Related>
    Model<Derived, AllRelations...>::getRelationMethod(const QString &relation) const
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        return getRelationMethodRaw<Related>(relation);
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::throwIfCRTPctorProblem(
            const QVector<AttributeItem> &attributes) const
    {
        throwIfQDateTimeAttribute(attributes);
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::throwIfQDateTimeAttribute(
            const QVector<AttributeItem> &attributes) const
    {
        static const QString message = QStringLiteral(
            "Attributes passed to the '%1' model's constructor can't contain the "
            "QDateTime attribute, to create a '%1' model instance with attributes "
            "that contain the QDateTime attribute use %1::instance() "
            "method instead.");

        for (const auto &attribute : attributes)
            if (const auto &value = attribute.value;
                value.isValid() && !value.isNull() &&
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                value.typeId() == QMetaType::QDateTime
#else
                value.userType() == QMetaType::QDateTime
#endif
            )
                throw InvalidArgumentError(
                        message.arg(Utils::Type::classPureBasename<Derived>()));
    }

    template<typename Derived, typename ...AllRelations>
    QStringList
    Model<Derived, AllRelations...>::getDatesInternal() const
    {
        if (!usesTimestamps())
            return Derived::u_dates;

        // It can be static, doesn't matter anyway
        static const QStringList &defaults = timestampColumnNames();

        auto dates = Derived::u_dates + defaults;

        dates.removeDuplicates();

        return dates;
    }

    template<typename Derived, typename ...AllRelations>
    void
    Model<Derived, AllRelations...>::validateUserRelation(const QString &name) const
    {
        if (!model().u_relations.contains(name))
            throw RelationNotFoundError(
                    Orm::Utils::Type::classPureBasename<Derived>(), name);
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    typename Model<Derived, AllRelations...>::template RelationType<Related>
    Model<Derived, AllRelations...>::getRelationMethodRaw(
            const QString &relation) const
    {
        return std::any_cast<RelationType<Related>>(
                model().u_relations.find(relation).value());
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::performDeleteOnModel()
    {
        /* Ownership of a unique_ptr(), if right passed down, then the
           will be destroyed right after this command. */
        model().setKeysForSaveQuery(*newModelQuery()).remove();

        this->exists = false;
    }

    template<typename Derived, typename ...AllRelations>
    TinyBuilder<Derived> &
    Model<Derived, AllRelations...>::setKeysForSaveQuery(TinyBuilder<Derived> &query)
    {
        return query.where(getKeyName(), QStringLiteral("="), getKeyForSaveQuery());
    }

    template<typename Derived, typename ...AllRelations>
    QVariant Model<Derived, AllRelations...>::getKeyForSaveQuery() const
    {
        // Found
        if (const auto keyName = getKeyName();
            m_originalHash.contains(keyName)
        )
            return m_original.at(m_originalHash.at(keyName)).value;

        // Not found, return the primary key value
        return getKey();
    }

    template<typename Derived, typename ...AllRelations>
    TinyBuilder<Derived> &
    Model<Derived, AllRelations...>::setKeysForSelectQuery(TinyBuilder<Derived> &query)
    {
        return query.where(getKeyName(), QStringLiteral("="), getKeyForSelectQuery());
    }

    template<typename Derived, typename ...AllRelations>
    QVariant Model<Derived, AllRelations...>::getKeyForSelectQuery() const
    {
        // Currently is the implementation exactly the same, so I can call it
        return getKeyForSaveQuery();
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::performInsert(
            const TinyBuilder<Derived> &query)
    {
//        if (!fireModelEvent("creating"))
//            return false;

        /* First we'll need to create a fresh query instance and touch the creation and
           update timestamps on this model, which are maintained by us for developer
           convenience. After, we will just continue saving these model instances. */
        if (usesTimestamps())
            updateTimestamps();

        /* If the model has an incrementing key, we can use the "insertGetId" method on
           the query builder, which will give us back the final inserted ID for this
           table from the database. Not all tables have to be incrementing though. */
        const auto &attributes = getAttributes();

        if (getIncrementing())
            insertAndSetId(query, attributes);

        /* If the table isn't incrementing we'll simply insert these attributes as they
           are. These attribute vectors must contain an "id" column previously placed
           there by the developer as the manually determined key for these models. */
        else
            if (attributes.isEmpty())
                return true;
            else
                query.insert(attributes);

        /* We will go ahead and set the exists property to true, so that it is set when
           the created event is fired, just in case the developer tries to update it
           during the event. This will allow them to do so and run an update here. */
        this->exists = true;

//        fireModelEvent("created", false);

        return true;
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::performUpdate(TinyBuilder<Derived> &query)
    {
        /* If the updating event returns false, we will cancel the update operation so
           developers can hook Validation systems into their models and cancel this
           operation if the model does not pass validation. Otherwise, we update. */
//        if (!fireModelEvent("updating"))
//            return false;

        /* First we need to create a fresh query instance and touch the creation and
           update timestamp on the model which are maintained by us for developer
           convenience. Then we will just continue saving the model instances. */
        if (usesTimestamps())
            updateTimestamps();

        /* Once we have run the update operation, we will fire the "updated" event for
           this model instance. This will allow developers to hook into these after
           models are updated, giving them a chance to do any special processing. */
        const auto dirty = getDirty();

        if (!dirty.isEmpty()) {
            model().setKeysForSaveQuery(query).update(
                        Utils::Attribute::convertVectorToUpdateItem(dirty));

            syncChanges();

//            fireModelEvent("updated", false);
        }

        return true;
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::finishSave(const SaveOptions &options)
    {
//        fireModelEvent('saved', false);

        if (isDirty() && options.touch)
            touchOwners();

        syncOriginal();
    }

    // FEATURE dilemma primarykey, add support for Derived::KeyType silverqx
    template<typename Derived, typename ...AllRelations>
    quint64 Model<Derived, AllRelations...>::insertAndSetId(
            const TinyBuilder<Derived> &query,
            const QVector<AttributeItem> &attributes)
    {
        // FEATURE postgres, insertGetId() and getKeyName() for sequence parameter silverqx
//        const auto &keyName = getKeyName();

        const auto id = query.insertGetId(attributes/*, keyName*/);

        // When insert was successful
        if (id != 0)
            setAttribute(getKeyName(), id);

        /* QSqlQuery returns an invalid QVariant if can't obtain last inserted id,
           which is converted to 0. */
        return id;
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::touch()
    {
        if (!usesTimestamps())
            return false;

        updateTimestamps();

        return save();
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::updateTimestamps()
    {
        const auto time = freshTimestamp();

        const QString &updatedAtColumn = getUpdatedAtColumn();

        if (!updatedAtColumn.isEmpty() && !isDirty(updatedAtColumn))
            setUpdatedAt(time);

        const QString &createdAtColumn = getCreatedAtColumn();

        if (!exists && !createdAtColumn.isEmpty() && !isDirty(createdAtColumn))
            setCreatedAt(time);
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::setCreatedAt(const QDateTime &value)
    {
        return setAttribute(getCreatedAtColumn(), value);
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::setUpdatedAt(const QDateTime &value)
    {
        return setAttribute(getUpdatedAtColumn(), value);
    }

    template<typename Derived, typename ...AllRelations>
    QString Model<Derived, AllRelations...>::freshTimestampString() const
    {
        return fromDateTime(freshTimestamp());
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::setUseTimestamps(const bool value)
    {
        model().u_timestamps = value;

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    QString Model<Derived, AllRelations...>::getQualifiedCreatedAtColumn() const
    {
        return qualifyColumn(getCreatedAtColumn());
    }

    template<typename Derived, typename ...AllRelations>
    QString Model<Derived, AllRelations...>::getQualifiedUpdatedAtColumn() const
    {
        return qualifyColumn(getUpdatedAtColumn());
    }

    template<typename Derived, typename ...AllRelations>
    template<typename ClassToCheck>
    bool Model<Derived, AllRelations...>::isIgnoringTouch()
    {
        if (!ClassToCheck().usesTimestamps()
            || ClassToCheck::getUpdatedAtColumn().isEmpty()
        )
            return true;

        // TODO future implement withoutTouching() and related data member $ignoreOnTouch silverqx

        return false;
    }

    /* GuardsAttributes */

    /* These methods may look a little strange because they are non-static, but it is
       intentional because I want to preserve the same API as Eloquent and return
       a Model &, but because of the CRTP pattern and the need of calling fill() method
       from the Model::ctor all of the u_xx mass asignment related data members have
       to be static. ✌ */

    template<typename Derived, typename ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getFillable() const
    {
        return Derived::u_fillable;
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::fillable(const QStringList &fillable)
    {
        Derived::u_fillable = fillable;

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::fillable(QStringList &&fillable)
    {
        Derived::u_fillable = std::move(fillable);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::mergeFillable(const QStringList &fillable)
    {
        auto &fillable_ = Derived::u_fillable;

        for (const auto &value : fillable)
            if (!fillable_.contains(value))
                fillable_.append(value);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::mergeFillable(QStringList &&fillable)
    {
        auto &fillable_ = Derived::u_fillable;

        for (auto &value : fillable)
            if (!fillable_.contains(value))
                fillable_.append(std::move(value));

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    const QStringList &Model<Derived, AllRelations...>::getGuarded() const
    {
        return Derived::u_guarded;
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::guard(const QStringList &guarded)
    {
        Derived::u_guarded = guarded;

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::guard(QStringList &&guarded)
    {
        Derived::u_guarded = std::move(guarded);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::mergeGuarded(const QStringList &guarded)
    {
        auto &guarded_ = Derived::u_guarded;

        for (const auto &value : guarded)
            if (!guarded_.contains(value))
                guarded_.append(value);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &Model<Derived, AllRelations...>::mergeGuarded(QStringList &&guarded)
    {
        auto &guarded_ = Derived::u_guarded;

        for (auto &value : guarded)
            if (!guarded_.contains(value))
                guarded_.append(std::move(value));

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::unguard(const bool state)
    {
        // NOTE api different, Eloquent use late static binding for unguarded silverqx
        m_unguarded = state;
    }

    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::reguard()
    {
        m_unguarded = false;
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::isUnguarded()
    {
        return m_unguarded;
    }

    // NOTE api different, Eloquent returns whatever callback returns silverqx
    template<typename Derived, typename ...AllRelations>
    void Model<Derived, AllRelations...>::unguarded(
            const std::function<void()> &callback)
    {
        if (m_unguarded)
            return std::invoke(callback);

        unguard();

        try {
            std::invoke(callback);
        } catch (...) {
        }

        reguard();
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::isFillable(const QString &key) const
    {
        if (m_unguarded)
            return true;

        const auto &fillable = getFillable();

        /* If the key is in the "fillable" vector, we can of course assume that it's
           a fillable attribute. Otherwise, we will check the guarded vector when
           we need to determine if the attribute is black-listed on the model. */
        if (fillable.contains(key))
            return true;

        /* If the attribute is explicitly listed in the "guarded" vector then we can
           return false immediately. This means this attribute is definitely not
           fillable and there is no point in going any further in this method. */
        if (isGuarded(key))
            return false;

        return fillable.isEmpty()
                // Don't allow mass filling with table names
                && !key.contains(QChar('.'));
                // NOTE api different, isFillable() !key.startsWith(), what is this good for? silverqx
//                && !key.startsWith(QChar('_'));
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::isGuarded(const QString &key) const
    {
        const auto &guarded = getGuarded();

        if (guarded.isEmpty())
            return false;

        return guarded == QStringList {"*"}
                // NOTE api different, Eloquent uses CaseInsensitive compare, silverqx
                || guarded.contains(key)
                /* Not a VALID guardable column is guarded, so it is not possible to fill
                   a column that is not in the database. */
                || !isGuardableColumn(key);
    }

    template<typename Derived, typename ...AllRelations>
    bool Model<Derived, AllRelations...>::totallyGuarded() const
    {
        return getFillable().isEmpty() && getGuarded() == QStringList {"*"};
    }

} // namespace Orm::Tiny
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

// TODO study, find out how to avoid pivot include at the end of Model's header file silverqx
#include "orm/tiny/relations/pivot.hpp"

#endif // MODEL_H
