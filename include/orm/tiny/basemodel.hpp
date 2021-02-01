#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <QDateTime>

#include <range/v3/view/transform.hpp>

#include "orm/concerns/hasconnectionresolver.hpp"
#include "orm/connectionresolverinterface.hpp"
#include "orm/invalidformaterror.hpp"
#include "orm/tiny/concerns/hasrelationstore.hpp"
#include "orm/tiny/relations/belongsto.hpp"
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
    using QueryBuilder = Query::Builder;

    // TODO decide/unify when to use class/typename keywords for templates silverqx
    // TODO add concept, AllRelations can not contain type defined in "Model" parameter silverqx
    // TODO next test no relation behavior silverqx
    // TODO now exceptions for model CRUD methods? silverqx
    // TODO model missing methods Model::getOriginal() silverqx
    // TODO model missing methods Model::fresh()/refresh() silverqx
    // TODO model missing methods Model::addSelect() silverqx
    // TODO model missing methods Model::orderByDesc() silverqx
    // TODO model missing methods Soft Deleting, Model::trashed()/restore()/withTrashed()/forceDelete()/onlyTrashed(), check this methods also on EloquentBuilder and SoftDeletes trait silverqx
    // TODO model missing methods Model::replicate() silverqx
    // TODO model missing methods Comparing Models, Model::is() silverqx
    // TODO model missing methods Model::firstOr() silverqx
    // TODO model missing methods Model::findOrFail()/firstOrFail() silverqx
    // TODO model missing methods Model::updateOrCreate() silverqx
    template<typename Model, typename ...AllRelations>
    class BaseModel :
            public Concerns::HasRelationStore<Model, AllRelations...>,
            public Orm::Concerns::HasConnectionResolver
    {
    protected:
        BaseModel(const QVector<AttributeItem> &attributes = {});

    public:
        /*! The "type" of the primary key ID. */
        using KeyType = quint64;

        /* Methods that start TinyBuilder */
        // TODO inline static method vs constexpr static, check it silverqx
        /*! Begin querying the model. */
        inline static std::unique_ptr<TinyBuilder<Model>> query()
        { return Model().newQuery(); }

        /*! Get all of the models from the database. */
        inline static QVector<Model> all(const QStringList &columns = {"*"})
        { return query()->get(columns); }

        /*! Find a model by its primary key. */
        inline static std::optional<Model>
        find(const QVariant &id, const QStringList &columns = {"*"})
        { return query()->find(id, columns); }
        /*! Find a model by its primary key or return fresh model instance. */
        inline static Model
        findOrNew(const QVariant &id, const QStringList &columns = {"*"})
        { return query()->findOrNew(id, columns); }
        /*! Find a model by its primary key or throw an exception. */
        inline static Model
        findOrFail(const QVariant &id, const QStringList &columns = {"*"})
        { return query()->findOrFail(id, columns); }

        /*! Get the first record matching the attributes or instantiate it. */
        inline static Model
        firstOrNew(const QVector<WhereItem> &attributes = {},
                   const QVector<AttributeItem> &values = {})
        { return query()->firstOrNew(attributes, values); }
        /*! Get the first record matching the attributes or create it. */
        inline static Model
        firstOrCreate(const QVector<WhereItem> &attributes = {},
                      const QVector<AttributeItem> &values = {})
        { return query()->firstOrCreate(attributes, values); }

        /*! Add a basic where clause to the query, and return the first result. */
        inline static std::optional<Model>
        firstWhere(const QString &column, const QString &comparison,
                   const QVariant &value, const QString &condition = "and")
        { return where(column, comparison, value, condition)->first(); }
        /*! Add a basic equal where clause to the query, and return the first result. */
        inline static std::optional<Model>
        firstWhereEq(const QString &column, const QVariant &value,
                     const QString &condition = "and")
        { return where(column, QStringLiteral("="), value, condition)->first(); }

        /*! Add a basic where clause to the query. */
        static std::unique_ptr<TinyBuilder<Model>>
        where(const QString &column, const QString &comparison,
              const QVariant &value, const QString &condition = "and");
        /*! Add a basic equal where clause to the query. */
        inline static std::unique_ptr<TinyBuilder<Model>>
        whereEq(const QString &column, const QVariant &value,
                const QString &condition = "and")
        { return where(column, QStringLiteral("="), value, condition); }

        /*! Add an array of basic where clauses to the query. */
        static std::unique_ptr<TinyBuilder<Model>>
        where(const QVector<WhereItem> &values, const QString &condition = "and");

        /*! Add an "order by" clause for a timestamp to the query. */
        static std::unique_ptr<TinyBuilder<Model>>
        latest(QString column = "");
        /*! Add an "order by" clause for a timestamp to the query. */
        static std::unique_ptr<TinyBuilder<Model>>
        oldest(QString column = "");

        // TODO next fuckin increment, finish later 👿 silverqx

        /*! Begin querying a model with eager loading. */
        static std::unique_ptr<TinyBuilder<Model>>
        with(const QVector<WithItem> &relations);
        /*! Begin querying a model with eager loading. */
        inline static std::unique_ptr<TinyBuilder<Model>>
        with(const QString &relation)
        { return with(QVector<WithItem> {{relation}}); }

        /*! Save a new model and return the instance. */
        inline static Model
        create(const QVector<AttributeItem> &attributes)
        { return query()->create(attributes); }

        // TODO cpp check all int types and use std::size_t where appropriate silverqx
        // WARNING id should be Model::KeyType, if I don't solve this problem, do runtime type check, QVariant type has to be the same type like KeyType and throw exception silverqx
        /*! Destroy the models for the given IDs. */
        static std::size_t
        destroy(const QVector<QVariant> &ids);
        inline static std::size_t
        destroy(const QVariant id)
        { return destroy(QVector<QVariant> {id}); }

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
        /*! Delete the model from the database. */
        inline bool deleteModel()
        { return remove(); }

        /*! Get a new query builder for the model's table. */
        inline std::unique_ptr<TinyBuilder<Model>> newQuery()
        { return newQueryWithoutScopes(); }
        /*! Get a new query builder that doesn't have any global scopes. */
        std::unique_ptr<TinyBuilder<Model>> newQueryWithoutScopes();
        /*! Get a new query builder that doesn't have any global scopes or eager loading. */
        std::unique_ptr<TinyBuilder<Model>> newModelQuery();
        /*! Create a new Tiny query builder for the model. */
        std::unique_ptr<TinyBuilder<Model>>
        newTinyBuilder(const QSharedPointer<QueryBuilder> query);

        /*! Create a new model instance that is existing. */
        Model newFromBuilder(const QVector<AttributeItem> &attributes = {},
                             const std::optional<QString> connection = std::nullopt);
        /*! Create a new instance of the given model. */
        Model newInstance(const QVector<AttributeItem> &attributes = {},
                          bool exists = false);

        /*! Static cast this to a child's instance type instance (CRTP). */
        inline Model &model()
        { return static_cast<Model &>(*this); }
        /*! Static cast this to a child's instance type (CRTP), const version. */
        inline const Model &model() const
        { return static_cast<const Model &>(*this); }

        /*! Get the current connection name for the model. */
        inline const QString &getConnectionName() const
        { return model().u_connection; }
        /*! Get the database connection for the model. */
        inline ConnectionInterface &getConnection() const
        { return m_resolver->connection(getConnectionName()); }
        /*! Set the connection associated with the model. */
        inline Model &setConnection(const QString &name)
        { model().u_connection = name; return model(); }
        /*! Set the table associated with the model. */
        inline Model &setTable(const QString &value)
        { model().u_table = value; return model(); }
        /*! Get the table associated with the model. */
        inline const QString &getTable() const
        { return model().u_table; }
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
        inline Model &setIncrementing(const bool value)
        { model().u_incrementing = value; return model(); }

        /*! Qualify the given column name by the model's table. */
        QString qualifyColumn(const QString &column) const;

        /*! Fill the model with an array of attributes. */
        Model &fill(const QVector<AttributeItem> &attributes);

        /*! Indicates if the model exists. */
        bool exists = false;

        /* HasAttributes */
        /*! Set a given attribute on the model. */
        Model &setAttribute(const QString &key, QVariant value);
        /*! Set the array of model attributes. No checking is done. */
        Model &setRawAttributes(const QVector<AttributeItem> &attributes,
                                bool sync = false);
        /*! Sync the original attributes with the current. */
        Model &syncOriginal();
        /*! Get all of the current attributes on the model. */
        const QVector<AttributeItem> &getAttributes() const;
        /*! Get an attribute from the model. */
        QVariant getAttribute(const QString &key) const;
        /*! Get a plain attribute (not a relationship). */
        QVariant getAttributeValue(const QString &key) const;
        /*! Get an attribute from the $attributes array. */
        QVariant getAttributeFromArray(const QString &key) const;
        /*! Get the model's raw original attribute values. */
        QVariant getRawOriginal(const QString &key) const;
        /*! Transform a raw model value using mutators, casts, etc. */
        QVariant transformModelValue(const QString &key, const QVariant &value) const;
        /*! Get a relationship for Many types relation. */
        template<typename Related,
                 template<typename> typename Container = QVector>
        Container<Related *>
        getRelationValue(const QString &relation);
        /*! Get a relationship for a One type relation. */
        template<typename Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        Related *
        getRelationValue(const QString &relation);

        /*! Return an attribute by the given key. */
        inline QVariant operator[](const QString &key) const
        { return getAttribute(key); }

        /*! Get the attributes that have been changed since last sync. */
        QVector<AttributeItem> getDirty() const;
        /*! Determine if the model or any of the given attribute(s) have been modified. */
        inline bool isDirty(const QStringList &attributes = {}) const
        { return hasChanges(getDirty(), attributes); }
        /*! Determine if the model or any of the given attribute(s) have been modified. */
        inline bool isDirty(const QString &attribute) const
        { return hasChanges(getDirty(), QStringList {attribute}); }
        /*! Determine if the model and all the given attribute(s) have remained the same. */
        inline bool isClean(const QStringList &attributes = {}) const
        { return !isDirty(attributes); }
        /*! Determine if the model and all the given attribute(s) have remained the same. */
        inline bool isClean(const QString &attribute) const
        { return !isDirty(attribute); }
        /*! Get the attributes that were changed. */
        inline const QVector<AttributeItem> &getChanges() const
        { return m_changes; }
        /*! Determine if the model and all the given attribute(s) have remained the same. */
        inline bool wasChanged(const QStringList &attributes = {}) const
        { return hasChanges(getChanges(), attributes); }
        /*! Determine if the model and all the given attribute(s) have remained the same. */
        inline bool wasChanged(const QString &attribute) const
        { return hasChanges(getChanges(), QStringList {attribute}); }

        /*! Get the format for database stored dates. */
        const QString &getDateFormat() const;
        /*! Set the date format used by the model. */
        Model &setDateFormat(const QString &format);
        /*! Convert a DateTime to a storable string. */
        QVariant fromDateTime(const QVariant &value) const;
        /*! Convert a DateTime to a storable string. */
        QString fromDateTime(const QDateTime &value) const;

        /* HasRelationships */
        // TODO make getRelation() Container argument compatible with STL containers API silverqx
        /*! Get a specified relationship. */
        template<typename Related,
                 template<typename> typename Container = QVector>
        Container<Related *> getRelation(const QString &relation);
        /*! Get a specified relationship as Related type, for use with HasOne and BelongsTo relation types. */
        template<typename Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        Related *getRelation(const QString &relation);
        /*! Determine if the given relation is loaded. */
        inline bool relationLoaded(const QString &relation) const
        { return m_relations.contains(relation); };
        /*! Set the given relationship on the model. */
        template<typename Related>
        Model &setRelation(const QString &relation, const QVector<Related> &models);
        // TODO perf, debug setRelation() and use move when possible silverqx
        /*! Set the given relationship on the model. */
        template<typename Related>
        Model &setRelation(const QString &relation, QVector<Related> &&models);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Model &setRelation(const QString &relation, const std::optional<Related> &model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Model &setRelation(const QString &relation, std::optional<Related> &&model);
        /*! Get the default foreign key name for the model. */
        QString getForeignKey() const;
        /*! Define a one-to-one relationship. */
        template<typename Related>
        std::unique_ptr<Relations::Relation<Model, Related>>
        hasOne(QString foreignKey = "", QString localKey = "");
        /*! Define an inverse one-to-one or many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::Relation<Model, Related>>
        belongsTo(QString foreignKey = "", QString ownerKey = "", QString relation = "");
        /*! Define a one-to-many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::Relation<Model, Related>>
        hasMany(QString foreignKey = "", QString localKey = "");
        /*! Touch the owning relations of the model. */
        void touchOwners();
        /*! Get the relationships that are touched on save. */
        inline const QStringList &getTouchedRelations() const
        { return model().u_touches; }

        /* HasTimestamps */
        /*! Update the model's update timestamp. */
        bool touch();
        /*! Update the creation and update timestamps. */
        void updateTimestamps();
        /*! Set the value of the "created at" attribute. */
        Model &setCreatedAt(const QDateTime &value);
        /*! Set the value of the "updated at" attribute. */
        Model &setUpdatedAt(const QDateTime &value);
        /*! Get a fresh timestamp for the model. */
        inline QDateTime freshTimestamp() const
        { return QDateTime::currentDateTime(); }
        /*! Get a fresh timestamp for the model. */
        QString freshTimestampString() const;
        /*! Determine if the model uses timestamps. */
        inline bool usesTimestamps() const
        { return model().u_timestamps; }
        /*! Get the name of the "created at" column. */
        inline static const QString &getCreatedAtColumn()
        { return Model::CREATED_AT; }
        /*! Get the name of the "updated at" column. */
        inline static const QString &getUpdatedAtColumn()
        { return Model::UPDATED_AT; }
        /*! Get the fully qualified "created at" column. */
        QString getQualifiedCreatedAtColumn() const;
        /*! Get the fully qualified "updated at" column. */
        QString getQualifiedUpdatedAtColumn() const;
        /*! Determine if the given model is ignoring touches. */
        template<typename ClassToCheck = Model>
        static bool isIgnoringTouch();

        /* Eager load from TinyBuilder */
        /*! Invoke Model::eagerVisitor() to define template argument Related for eagerLoaded relation. */
        void eagerLoadRelationVisitor(const WithItem &relation, TinyBuilder<Model> &builder,
                                      QVector<Model> &models);
        /*! Get a relation method in the relations hash data member, defined in the current model instance. */
        template<typename Related>
        RelationType<Model, Related>
        getRelationMethod(const QString &relation) const;

    protected:
        /*! Get a new query builder instance for the connection. */
        QSharedPointer<QueryBuilder> newBaseQueryBuilder() const;

        /*! The visitor to obtain a type for Related template parameter. */
        inline void relationVisitor(const QString &)
        {}

        /*! On the base of type saved in the relation store decide, which action to call eager/push. */
        template<typename Related>
        void relationVisited();

        /* HasAttributes */
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
        bool hasChanges(const QVector<AttributeItem> &changes,
                        const QStringList &attributes = {}) const;
        /*! Sync the changed attributes. */
        inline Model &syncChanges()
        { m_changes = std::move(getDirty()); return model(); }
        /*! Determine if the new and old values for a given key are equivalent. */
        bool originalIsEquivalent(const QString &key) const;

        /*! Determine whether a value is Date / DateTime castable for inbound manipulation. */
        bool isDateCastable(const QString &key) const;
        /*! Return a timestamp as DateTime object. */
        QDateTime asDateTime(const QVariant &value) const;

        /* HasRelationships */
        /*! Create a new model instance for a related model. */
        template<typename Related>
        std::unique_ptr<Related>
        newRelatedInstance() const;
        // TODO can be unified to a one templated method by relation type silverqx
        /*! Instantiate a new HasOne relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::Relation<Model, Related>>
        newHasOne(std::unique_ptr<Related> &&related, Model &parent,
                  const QString &foreignKey, const QString &localKey) const
        { return Relations::HasOne<Model, Related>::create(
                        std::move(related), parent, foreignKey, localKey); }
        /*! Instantiate a new BelongsTo relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::Relation<Model, Related>>
        newBelongsTo(std::unique_ptr<Related> &&related,
                     Model &child, const QString &foreignKey,
                     const QString &ownerKey, const QString &relation) const
        { return Relations::BelongsTo<Model, Related>::create(
                        std::move(related), child, foreignKey, ownerKey, relation); }
        /*! Instantiate a new HasMany relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::Relation<Model, Related>>
        newHasMany(std::unique_ptr<Related> &&related, Model &parent,
                   const QString &foreignKey, const QString &localKey) const
        { return Relations::HasMany<Model, Related>::create(
                        std::move(related), parent, foreignKey, localKey); }
        /*! Guess the "belongs to" relationship name. */
        template<typename Related>
        QString guessBelongsToRelation() const;

        /* Others */
        /*! Perform the actual delete query on this model instance. */
        void performDeleteOnModel();

        /*! Set the keys for a save update query. */
        TinyBuilder<Model> &
        setKeysForSaveQuery(TinyBuilder<Model> &query);
        /*! Get the primary key value for a save query. */
        QVariant getKeyForSaveQuery() const;

        /*! Perform a model insert operation. */
        bool performInsert(const TinyBuilder<Model> &query);
        /*! Perform a model insert operation. */
        bool performUpdate(TinyBuilder<Model> &query);
        /*! Perform any actions that are necessary after the model is saved. */
        void finishSave(const SaveOptions &options = {});
        // TODO primary key dilema, add support for Model::KeyType silverqx
        /*! Insert the given attributes and set the ID on the model. */
        quint64 insertAndSetId(const TinyBuilder<Model> &query,
                               const QVector<AttributeItem> &attributes);

        /*! The connection name for the model. */
        QString u_connection {""};
        /*! Indicates if the IDs are auto-incrementing. */
        bool u_incrementing = true;
        /*! The primary key for the model. */
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
        // TODO should be QHash, I choosen QVector, becuase I wanted to preserve attributes order, think about this, would be solution to use undered_map which preserves insert order? and do I really need to preserve insert order? 🤔, the same is true for m_original field silverqx
        // TODO future Default Attribute Values, can not be u_attributes because of CRTP, The best I've come up with was BaseModel.init() and init default attrs. from there silverqx
        /*! The model's attributes. */
        QVector<AttributeItem> m_attributes;
        /*! The model attribute's original state. */
        QVector<AttributeItem> m_original;
        /*! The changed model attributes. */
        QVector<AttributeItem> m_changes;
        /*! The storage format of the model's date columns. */
        QString u_dateFormat;

        /* HasRelationships */
        /*! The loaded relationships for the model. */
        QHash<QString, RelationsType<AllRelations...>> m_relations;
        /*! The relationships that should be touched on save. */
        QStringList u_touches;

        /* HasTimestamps */
        /*! The name of the "created at" column. */
        inline static const QString CREATED_AT = QStringLiteral("created_at");
        /*! The name of the "updated at" column. */
        inline static const QString UPDATED_AT = QStringLiteral("updated_at");
        /*! Indicates if the model should be timestamped. */
        bool u_timestamps = true;

    private:
        /* HasRelationships */
        /*! Throw exception if a relation is not defined. */
        void validateUserRelation(const QString &name) const;
        /*! Obtain related models from "relationships" data member hash without any checks. */
        template<class Related,
                 template<typename> typename Container = QVector>
        Container<Related *>
        getRelationFromHash(const QString &relation);
        /*! Obtain related models from "relationships" data member hash without any checks. */
        template<class Related, typename Tag,
                 std::enable_if_t<std::is_same_v<Tag, One>, bool> = true>
        Related *
        getRelationFromHash(const QString &relation);
        /*! Get a relation method in the u_relations hash data member, defined in the current model instance. */
        template<typename Related>
        RelationType<Model, Related>
        getRelationMethodRaw(const QString &relation) const;

        /* Eager loading and push */
        /*! Continue execution after a relation type was obtained ( by Related template parameter ). */
        template<typename Related>
        inline void eagerVisited() const
        { this->m_eagerStore->builder.template eagerLoadRelation<Related>(
                        this->m_eagerStore->models, this->m_eagerStore->relation); }

        /*! On the base of alternative held by m_relations decide, which pushVisitied() to execute. */
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

        /*! On the base of alternative held by m_relations decide, which touchOwnersVisited() to execute. */
        template<typename Related>
        void touchOwnersVisited();

        /* Shortcuts for types related to the Relation Store */
        /*! Eager relation store. */
        using EagerRelationStore =
            typename Concerns::HasRelationStore<Model, AllRelations...>::EagerRelationStore;
        /*! Push relation store. */
        using PushRelationStore  =
            typename Concerns::HasRelationStore<Model, AllRelations...>::PushRelationStore;
        /*! Relation store type enum struct. */
        using RelationStoreType  =
            typename Concerns::HasRelationStore<Model, AllRelations...>::RelationStoreType;

        /*! Obtain a pointer to member function from relation store, after relation was visited. */
        template<typename Related>
        const std::function<void(BaseModel<Model, AllRelations...> &)> &
        getMethodForRelationVisited(RelationStoreType storeType) const;

#ifdef QT_DEBUG
        /*! The table associated with the model. */
        [[maybe_unused]]
        const QString *m_table;
#endif
    };

    template<typename Model, typename ...AllRelations>
    BaseModel<Model, AllRelations...>::BaseModel(const QVector<AttributeItem> &attributes)
#ifdef QT_DEBUG
        : m_table(&model().u_table)
#endif
    {
        // Compile time check if a primary key type is supported by a QVariant
        qMetaTypeId<typename Model::KeyType>();

        syncOriginal();

        fill(attributes);
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::where(
            const QString &column, const QString &comparison,
            const QVariant &value, const QString &condition)
    {
        auto query = BaseModel<Model, AllRelations...>::query();

        query->where(column, comparison, value, condition);

        return query;
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::where(const QVector<WhereItem> &values,
                                             const QString &condition)
    {
        /* The parentheses in this query are ok:
           select * from xyz where (id = ?) */
        auto query = BaseModel<Model, AllRelations...>::query();

        query->where(values, condition);

        return query;
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::latest(QString column)
    {
        auto query = BaseModel<Model, AllRelations...>::query();

        query->latest(column);

        return query;
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::oldest(QString column)
    {
        // TODO now query to builder, like with() silverqx
        auto query = BaseModel<Model, AllRelations...>::query();

        query->oldest(column);

        return query;
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::with(const QVector<WithItem> &relations)
    {
        auto builder = query();

        builder->with(relations);

        return builder;
    }

    // TODO next test all this remove()/destroy() methods, when deletion fails silverqx
    template<typename Model, typename ...AllRelations>
    size_t
    BaseModel<Model, AllRelations...>::destroy(const QVector<QVariant> &ids)
    {
        if (ids.isEmpty())
            return 0;

        /* We will actually pull the models from the database table and call delete on
           each of them individually so that their events get fired properly with a
           correct set of attributes in case the developers wants to check these. */
        Model instance;
        const auto &key = instance.getKeyName();

        std::size_t count = 0;

        // TODO diff call whereIn() on Model vs TinyBuilder silverqx
        // Ownership of a unique_ptr()
        for (auto &model : instance.newQuery()->whereIn(key, ids).get())
            if (model.remove())
                ++count;

        return count;
    }

    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::save(const SaveOptions &options)
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
    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::push()
    {
        if (!save())
            return false;

        if (!(m_relations.size() > 0))
            return true;

        /* To sync all of the relationships to the database, we will simply spin through
           the relationships and save each model via this "push" method, which allows
           us to recurse into all of these nested relations for the model instance. */
        // TODO future Eloquent uses foreach (array_filter($models) as $model), check if relation can actually be null or empty silverqx
        auto itModels = m_relations.begin();
        while (itModels != m_relations.end()) {
            const auto &relation = itModels.key();
            auto &models = itModels.value();

            // TODO prod remove, I don't exactly know if this can really happen silverqx
            const auto variantIndex = models.index();
            Q_ASSERT(variantIndex > 0);
            if (variantIndex == 0)
                continue;

            // Throw excpetion if a relation is not defined
            validateUserRelation(relation);

            // Save model/s to the store to avoid passing variables to the visitor
            this->createPushStore(models);

            // TODO next create wrapper method for this with better name silverqx
            model().relationVisitor(relation);

            bool pushResult = this->m_pushStore->result;

            // Release stored pointers to the relation store
            this->resetRelationStore();

            /* Following Eloquent API, if any push failed, quit, remaining push-es
               will not be processed. */
            if (!pushResult)
                return false;

            ++itModels;
        }

        return true;
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    void BaseModel<Model, AllRelations...>::pushVisited()
    {
        /* When relationship data member holds QVector, then it is definitely Many
           type relation. */
        if (std::holds_alternative<QVector<Related>>(this->m_pushStore->models))
            pushVisited<Related, Many>();
        else
            pushVisited<Related, One>();
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, Many>, bool>>
    void BaseModel<Model, AllRelations...>::pushVisited()
    {
        for (auto &model : std::get<QVector<Related>>(this->m_pushStore->models))
            if (!model.push()) {
                this->m_pushStore->result = false;
                return;
            }

        this->m_pushStore->result = true;
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    void BaseModel<Model, AllRelations...>::pushVisited()
    {
        auto &model = std::get<std::optional<Related>>(this->m_pushStore->models);
        // TODO prod remove, this assert is only to catch the case, when std::optional() is empty, because I don't know if this can actually happen? silverqx
        Q_ASSERT(!!model);
        if (!model)
            return;

        this->m_pushStore->result = model->push();
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    void BaseModel<Model, AllRelations...>::touchOwnersVisited()
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

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    void BaseModel<Model, AllRelations...>::relationVisited()
    {
        const auto &method = getMethodForRelationVisited<Related>(
                                 this->m_relationStore->getStoreType());

        std::invoke(method, *this);
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    const std::function<void(BaseModel<Model, AllRelations...> &)> &
    BaseModel<Model, AllRelations...>::getMethodForRelationVisited(
            const RelationStoreType storeType) const
    {
        // Pointers to visited member methods by storeType, yes yes c++ 😂
        // An order has to be the same as in enum struct RelationStoreType
        // TODO future should be QHash, for faster lookup, do benchmark, high chance that qvector has faster lookup than qhash silverqx
        static const
        QVector<std::function<void(BaseModel<Model, AllRelations...> &)>> cached {
            &BaseModel<Model, AllRelations...>::eagerVisited<Related>,
            &BaseModel<Model, AllRelations...>::pushVisited<Related>,
            &BaseModel<Model, AllRelations...>::touchOwnersVisited<Related>,
        };
        static const auto size = cached.size();

        // Check if storeType is in the range, just for sure 😏
        const auto type = static_cast<int>(storeType);
        Q_ASSERT((0 <= type) && (type < size));

        return cached.at(type);
    }

    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::update(
            const QVector<AttributeItem> &attributes,
            const SaveOptions &options)
    {
        if (!exists)
            return false;

        return fill(attributes).save(options);
    }

    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::remove()
    {
        // TODO future add support for attributes casting silverqx
//        $this->mergeAttributesFromClassCasts();

        if (getKeyName().isEmpty())
            throw OrmRuntimeError("No primary key defined on model.");

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

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::newQueryWithoutScopes()
    {
        auto tinyBuilder = newModelQuery();

        tinyBuilder->with(model().u_with);

        return tinyBuilder;
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::newModelQuery()
    {
        /* Model is passed to the TinyBuilder ctor, because of that setModel()
           isn't used here. Can't be const because of passed non-const model
           to the TinyBuilder. */
        return newTinyBuilder(newBaseQueryBuilder());
    }

    template<typename Model, typename ...AllRelations>
    std::unique_ptr<TinyBuilder<Model>>
    BaseModel<Model, AllRelations...>::newTinyBuilder(
            const QSharedPointer<QueryBuilder> query)
    {
        return std::make_unique<TinyBuilder<Model>>(query, model());
    }

    template<typename Model, typename ...AllRelations>
    Model
    BaseModel<Model, AllRelations...>::newFromBuilder(const QVector<AttributeItem> &attributes,
                                                      const std::optional<QString> connection)
    {
        auto model = newInstance({}, true);

        model.setRawAttributes(attributes, true);

        model.setConnection(connection ? *connection : getConnectionName());

        return model;
    }

    template<typename Model, typename ...AllRelations>
    Model
    BaseModel<Model, AllRelations...>::newInstance(const QVector<AttributeItem> &attributes,
                                                   const bool exists)
    {
        /* This method just provides a convenient way for us to generate fresh model
           instances of this current model. It is particularly useful during the
           hydration of new objects via the Eloquent query builder instances. */
        Model model(attributes);

        model.exists = exists;
        model.setConnection(getConnectionName());
        model.setTable(getTable());

        return model;
    }

    template<typename Model, typename ...AllRelations>
    QSharedPointer<QueryBuilder>
    BaseModel<Model, AllRelations...>::newBaseQueryBuilder() const
    {
        return getConnection().query();
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related, template<typename> typename Container>
    Container<Related *>
    BaseModel<Model, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships array, it just means the
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

    template<typename Model, typename ...AllRelations>
    template<typename Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    Related *
    BaseModel<Model, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships array, it just means the
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

    template<typename Model, typename ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related *>
    BaseModel<Model, AllRelations...>::getRelationshipFromMethod(const QString &relation)
    {
        // Obtain related models
        auto relatedModels = std::get<QVector<Related>>(
                std::invoke(getRelationMethodRaw<Related>(relation), model())
                        ->getResults());

        setRelation(relation, relatedModels);
//        setRelation(relation, std::move(relatedModel));

        return getRelationFromHash<Related, Container>(relation);
    }

    template<typename Model, typename ...AllRelations>
    template<class Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    Related *
    BaseModel<Model, AllRelations...>::getRelationshipFromMethod(const QString &relation)
    {
        // Obtain related model
        auto relatedModel = std::get<std::optional<Related>>(
                std::invoke(getRelationMethodRaw<Related>(relation), model())
                        ->getResults());

        setRelation(relation, relatedModel);
//        setRelation(relation, std::move(relatedModel));

        return getRelationFromHash<Related, One>(relation);
    }

    template<typename Model, typename ...AllRelations>
    QVector<AttributeItem>
    BaseModel<Model, AllRelations...>::getDirty() const
    {
        QVector<AttributeItem> dirty;

        for (const auto &attribute : getAttributes())
            if (const auto &key = attribute.key;
                !originalIsEquivalent(key)
            )
                dirty.append({key, attribute.value});

        return dirty;
    }

    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::hasChanges(
            const QVector<AttributeItem> &changes,
            const QStringList &attributes) const
    {
        /* If no specific attributes were provided, we will just see if the dirty array
           already contains any attributes. If it does we will just return that this
           count is greater than zero. Else, we need to check specific attributes. */
        if (attributes.isEmpty())
            return changes.size() > 0;

        /* Here we will spin through every attribute and see if this is in the array of
           dirty attributes. If it is, we will return true and if we make it through
           all of the attributes for the entire array we will return false at end. */
        for (const auto &attribute : attributes) {
            // TODO future hasOriginal() silverqx
            const auto changesContainKey =
                    ranges::contains(changes, true,
                                     [&attribute](const auto &changed)
            {
                return attribute == changed.key;
            });

            if (changesContainKey)
                return true;
        }

        return false;
    }

    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::originalIsEquivalent(const QString &key) const
    {
        // TODO future hasOriginal() silverqx
        const auto originalContainKey = ranges::contains(m_original, true,
                                                         [&key](const auto &original)
        {
            return original.key == key;
        });

        if (!originalContainKey)
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
        else if (isDateCastable(key))
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

    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::isDateCastable(const QString &key) const
    {
        // TODO castable silverqx
        /* I don't have support for castable attributes, this solution is temporary. */
        static const QVector<QString> defaults {
            getCreatedAtColumn(),
            getUpdatedAtColumn(),
        };

        return defaults.contains(key);
    }

    // TODO would be good to make it the c++ way, make overload for every type, asDateTime() is protected, so I have full control over it, but I leave it for now, because there will be more methods which will use this method in the future, and it will be more clear later on silverqx
    template<typename Model, typename ...AllRelations>
    QDateTime
    BaseModel<Model, AllRelations...>::asDateTime(const QVariant &value) const
    {
        /* If this value is already a QDateTime instance, we shall just return it as is.
           This prevents us having to re-parse a QDateTime instance when we know
           it already is one. */
        if (
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            value.typeId() == QMetaType::value
#else
            value.userType() == QMetaType::QDateTime
#endif
        )
            return value.value<QDateTime>();

        // TODO next timestamps, add unix timestamp and simple date support silverqx
        /* If this value is an integer, we will assume it is a UNIX timestamp's value
           and format a Carbon object from this timestamp. This allows flexibility
           when defining your date fields as they might be UNIX timestamps here. */
//        if (is_numeric($value))
//            return Date::createFromTimestamp($value);

        /* If the value is in simply year, month, day format, we will instantiate the
           Carbon instances from that format. Again, this provides for simple date
           fields on the database, while still supporting Carbonized conversion. */
//        if ($this->isStandardDateFormat($value))
//            return Date::instance(Carbon::createFromFormat('Y-m-d', $value)->startOfDay());

        const auto &format = getDateFormat();

        /* Finally, we will just assume this date is in the format used by default on
           the database connection and use that format to create the QDateTime object
           that is returned back out to the developers after we convert it here. */
        const auto date = QDateTime::fromString(value.value<QString>(), format);

        if (date.isValid())
            return date;

        throw InvalidFormatError(
                    QStringLiteral("Could not parse the datetime '%1' using "
                                   "the given format '%2'.")
                    // TODO next QVariant value<>() everywhere silverqx
                    .arg(value.value<QString>(), format));
    }

    template<typename Model, typename ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related *>
    BaseModel<Model, AllRelations...>::getRelationFromHash(const QString &relation)
    {
        /* Obtain related models from data member hash as QVector, it is internal
           format and transform it into a Container of pointers to related models,
           so a user can directly modify these models and push or save them
           afterward. */
        using namespace ranges;
        return std::get<QVector<Related>>(m_relations.find(relation).value())
                | views::transform([](Related &model) -> Related * { return &model; })
                | ranges::to<Container<Related *>>();
    }

    template<typename Model, typename ...AllRelations>
    template<class Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    Related *
    BaseModel<Model, AllRelations...>::getRelationFromHash(const QString &relation)
    {
        /* Obtain related model from data member hash and return it as a pointer or
           nullptr if no model is associated, so a user can directly modify this
           model and push or save it afterward. */

        auto &relatedModel =
                std::get<std::optional<Related>>(m_relations.find(relation).value());

        return relatedModel ? &*relatedModel : nullptr;
    }

    // TODO solve different behavior like Eloquent getRelation() silverqx
    // TODO next many relation compiles with Orm::One and exception during runtime occures, solve this during compile, One relation only with Orm::One and many relation type only with Container version silverqx
    template<typename Model, typename ...AllRelations>
    template<typename Related, template<typename> typename Container>
    Container<Related *>
    BaseModel<Model, AllRelations...>::getRelation(const QString &relation)
    {
        if (!relationLoaded(relation))
            // TODO create RelationError class silverqx
            throw OrmRuntimeError("Undefined relation key (in m_relations) : " + relation);

        return getRelationFromHash<Related, Container>(relation);
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related, typename Tag,
             std::enable_if_t<std::is_same_v<Tag, One>, bool>>
    Related *
    BaseModel<Model, AllRelations...>::getRelation(const QString &relation)
    {
        if (!relationLoaded(relation))
            // TODO create RelationError class silverqx
            throw OrmRuntimeError("Undefined relation key (in m_relations) : " + relation);

        // TODO instantiate relation by name and check if is_base_of OneRelation/ManyRelation, to have nice exception message (in debug mode only), because is impossible to check this during compile time silverqx

        return getRelationFromHash<Related, One>(relation);
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    Model &
    BaseModel<Model, AllRelations...>::setRelation(const QString &relation,
                                                   const QVector<Related> &models)
    {
        m_relations.insert(relation, models);

        return model();
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    Model &
    BaseModel<Model, AllRelations...>::setRelation(const QString &relation,
                                                   QVector<Related> &&models)
    {
        m_relations.insert(relation, std::move(models));

        return model();
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    Model &
    BaseModel<Model, AllRelations...>::setRelation(const QString &relation,
                                                   const std::optional<Related> &model)
    {
        m_relations.insert(relation, model);

        return this->model();
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    Model &
    BaseModel<Model, AllRelations...>::setRelation(const QString &relation,
                                                   std::optional<Related> &&model)
    {
        m_relations.insert(relation, std::move(model));

        return this->model();
    }

    template<typename Model, typename ...AllRelations>
    QString
    BaseModel<Model, AllRelations...>::qualifyColumn(const QString &column) const
    {
        if (column.contains('.'))
            return column;

        return getTable() + '.' + column;
    }

    template<typename Model, typename ...AllRelations>
    Model &
    BaseModel<Model, AllRelations...>::fill(const QVector<AttributeItem> &attributes)
    {
        for (const auto &attribute : attributes)
            setAttribute(attribute.key, attribute.value);

        return model();
    }

    template<typename Model, typename ...AllRelations>
    Model &BaseModel<Model, AllRelations...>::setAttribute(
            const QString &key, QVariant value)
    {
        /* If an attribute is listed as a "date", we'll convert it from a DateTime
           instance into a form proper for storage on the database tables using
           the connection grammar's date format. We will auto set the values. */
        if (!value.isNull() && isDateCastable(key))
            value = fromDateTime(value);

        // TODO mistake m_attributes/m_original 😭 silverqx
        // TODO extract to hasAttribute() silverqx
        const auto size = m_attributes.size();

        // Search an attribute in the vector, omg
        int i;
        for (i = 0; i < size; ++i)
            if (m_attributes[i].key == key)
                break;

        // Not found
        if (i == size)
            m_attributes.append({key, value});
        else
            m_attributes[i] = {key, value};

        return model();
    }

    template<typename Model, typename ...AllRelations>
    Model &
    BaseModel<Model, AllRelations...>::setRawAttributes(
            const QVector<AttributeItem> &attributes,
            const bool sync)
    {
        m_attributes = attributes;

        if (sync)
            syncOriginal();

        return model();
    }

    template<typename Model, typename ...AllRelations>
    Model &BaseModel<Model, AllRelations...>::syncOriginal()
    {
        m_original = getAttributes();

        return model();
    }

    template<typename Model, typename ...AllRelations>
    const QVector<AttributeItem> &
    BaseModel<Model, AllRelations...>::getAttributes() const
    {
        /*mergeAttributesFromClassCasts();*/
        return m_attributes;
    }

    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::getAttribute(const QString &key) const
    {
        if (key.isEmpty() || key.isNull())
            return {};

        const auto containsKey = ranges::contains(m_attributes, true,
                                                  [&key](const auto &attribute)
        {
            return attribute.key == key;
        });

        /* If the attribute exists in the attribute array or has a "get" mutator we will
           get the attribute's value. Otherwise, we will proceed as if the developers
           are asking for a relationship's value. This covers both types of values. */
        if (containsKey
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

    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::getAttributeValue(const QString &key) const
    {
        return transformModelValue(key, getAttributeFromArray(key));
    }

    // TODO candidate for optional const reference, to be able return null value and use reference at the same time silverqx
    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::getAttributeFromArray(const QString &key) const
    {
        const auto &attributes = getAttributes();
        const auto itAttribute = ranges::find_if(attributes,
                                                 [&key](const auto &attribute)
        {
            return attribute.key == key;
        });

        // Not found
        if (itAttribute == ranges::end(attributes))
            return {};

        return itAttribute->value;
    }

    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::getRawOriginal(const QString &key) const
    {
        const auto itOriginal = ranges::find_if(m_original,
                                                [&key](const auto &original)
        {
            return original.key == key;
        });

        // Not found
        if (itOriginal == ranges::end(m_original))
            return {};

        return itOriginal->value;
    }

    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::transformModelValue(
            const QString &key,
            const QVariant &value) const
    {
        Q_UNUSED(key)

        return value;
    }

    template<typename Model, typename ...AllRelations>
    const QString &
    BaseModel<Model, AllRelations...>::getDateFormat() const
    {
        return u_dateFormat.isEmpty()
                ? getConnection().getQueryGrammar().getDateFormat()
                : u_dateFormat;
    }

    template<typename Model, typename ...AllRelations>
    Model &
    BaseModel<Model, AllRelations...>::setDateFormat(const QString &format)
    {
        model().u_dateFormat = format;

        return model();
    }

    template<typename Model, typename ...AllRelations>
    QVariant
    BaseModel<Model, AllRelations...>::fromDateTime(const QVariant &value) const
    {
        if (value.isNull())
            return value;

        return asDateTime(value).toString(getDateFormat());
    }

    template<typename Model, typename ...AllRelations>
    QString
    BaseModel<Model, AllRelations...>::fromDateTime(const QDateTime &value) const
    {
        if (value.isValid())
            return value.toString(getDateFormat());

        return {};
    }

    template<typename Model, typename ...AllRelations>
    QString BaseModel<Model, AllRelations...>::getForeignKey() const
    {
        return QStringLiteral("%1_%2").arg(
                    Utils::String::toSnake(Utils::Type::classPureBasename<decltype (model())>()),
                    getKeyName());
    }

    template<typename Model, typename ...AllRelations>
    void BaseModel<Model, AllRelations...>::touchOwners()
    {
        for (const auto &relation : getTouchedRelations()) {
            // Throw excpetion if a relation is not defined
            validateUserRelation(relation);

            // Save model/s to the store to avoid passing variables to the visitor
            this->createTouchOwnersStore(relation);

            // TODO next create wrapper method for this with better name, over time, it seems fine to me, only thing I see now is that it has the same name for all visitations, but that looks ok too, will see, but this todo can be removed, looks good to me silverqx
            model().relationVisitor(relation);

            // Release stored pointers to the relation store
            this->resetRelationStore();
        }
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Related>
    BaseModel<Model, AllRelations...>::newRelatedInstance() const
    {
        auto instance = std::make_unique<Related>();

        if (instance->getConnectionName().isEmpty())
            instance->setConnection(getConnectionName());

        return instance;
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::Relation<Model, Related>>
    BaseModel<Model, AllRelations...>::hasOne(QString foreignKey, QString localKey)
    {
        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = getForeignKey();

        if (localKey.isEmpty())
            localKey = getKeyName();

        return newHasOne<Related>(std::move(instance), model(),
                                  instance->getTable() + '.' + foreignKey, localKey);
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::Relation<Model, Related>>
    BaseModel<Model, AllRelations...>::belongsTo(QString foreignKey, QString ownerKey,
                                                 QString relation)
    {
        /* If no relation name was given, we will use the Related class type to extract
           the name and use that as the relationship name as most of the time this
           will be what we desire to use for the relationships. */
        if (relation.isEmpty())
            relation = guessBelongsToRelation<Related>();

        auto instance = newRelatedInstance<Related>();

        const auto &primaryKey = instance->getKeyName();

        /* If no foreign key was supplied, we can guess the proper foreign key name
           by using the snake case name of the relationship, which when combined
           with an "_id" should conventionally match the columns. */
        if (foreignKey.isEmpty())
            foreignKey = Utils::String::toSnake(relation) + '_' + primaryKey;

        /* Once we have the foreign key names, we'll just create a new Eloquent query
           for the related models and returns the relationship instance which will
           actually be responsible for retrieving and hydrating every relations. */
        if (ownerKey.isEmpty())
            ownerKey = primaryKey;

        return newBelongsTo<Related>(std::move(instance), model(),
                                     foreignKey, ownerKey, relation);
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::Relation<Model, Related>>
    BaseModel<Model, AllRelations...>::hasMany(QString foreignKey, QString localKey)
    {
        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = getForeignKey();

        if (localKey.isEmpty())
            localKey = getKeyName();

        return newHasMany<Related>(std::move(instance), model(),
                                   instance->getTable() + '.' + foreignKey, localKey);
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    QString BaseModel<Model, AllRelations...>::guessBelongsToRelation() const
    {
        auto relation = Utils::Type::classPureBasename<Related>();

        relation[0] = relation[0].toLower();

        return relation;
    }

    template<typename Model, typename ...AllRelations>
    void BaseModel<Model, AllRelations...>::eagerLoadRelationVisitor(
            const WithItem &relation, TinyBuilder<Model> &builder, QVector<Model> &models)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation.name);

        // Save the needed variables to the store to avoid passing variables to the visitor
        this->createEagerStore(relation, builder, models);

        model().relationVisitor(relation.name);

        this->resetRelationStore();
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    RelationType<Model, Related>
    BaseModel<Model, AllRelations...>::getRelationMethod(const QString &relation) const
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        return getRelationMethodRaw<Related>(relation);
    }

    template<typename Model, typename ...AllRelations>
    void BaseModel<Model, AllRelations...>::validateUserRelation(const QString &name) const
    {
        if (!model().u_relations.contains(name))
            throw OrmRuntimeError("Undefined relation key (in u_relations) : " + name);
    }

    template<typename Model, typename ...AllRelations>
    template<typename Related>
    RelationType<Model, Related>
    BaseModel<Model, AllRelations...>::getRelationMethodRaw(const QString &relation) const
    {
        return std::any_cast<RelationType<Model, Related>>(
                model().u_relations.find(relation).value());
    }

    template<typename Model, typename ...AllRelations>
    void BaseModel<Model, AllRelations...>::performDeleteOnModel()
    {
        /* Ownership of a unique_ptr(), if right passed down, then the
           will be destroyed right after this command. */
        setKeysForSaveQuery(*newModelQuery()).remove();

        this->exists = false;
    }

    template<typename Model, typename ...AllRelations>
    TinyBuilder<Model> &
    BaseModel<Model, AllRelations...>::setKeysForSaveQuery(TinyBuilder<Model> &query)
    {
        return query.where(getKeyName(), QStringLiteral("="), getKeyForSaveQuery());
    }

    template<typename Model, typename ...AllRelations>
    QVariant BaseModel<Model, AllRelations...>::getKeyForSaveQuery() const
    {
        // TODO reason, why m_attributes and m_original should be QHash silverqx
        const auto itOriginal = ranges::find_if(m_original,
                                                [&key = getKeyName()](const auto &original)
        {
            return original.key == key;
        });

        return itOriginal != ranges::end(m_original) ? itOriginal->value : getKey();
    }

    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::performInsert(const TinyBuilder<Model> &query)
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

        if (getIncrementing()) {
            if (insertAndSetId(query, attributes) == 0)
                return false;
        }

        /* If the table isn't incrementing we'll simply insert these attributes as they
           are. These attribute arrays must contain an "id" column previously placed
           there by the developer as the manually determined key for these models. */
        else
            if (attributes.isEmpty())
                return true;
            else {
                bool ok;
                std::tie(ok, std::ignore) = query.insert(attributes);;
                // TODO next TinyBuilder return values dilema silverqx
                if (!ok)
                    return false;
            }

        /* We will go ahead and set the exists property to true, so that it is set when
           the created event is fired, just in case the developer tries to update it
           during the event. This will allow them to do so and run an update here. */
        this->exists = true;

//        fireModelEvent("created", false);

        return true;
    }

    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::performUpdate(TinyBuilder<Model> &query)
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
            QSqlQuery sqlQuery;
            std::tie(std::ignore, sqlQuery) =
                    setKeysForSaveQuery(query).update(
                        Utils::Attribute::convertVectorToUpdateItem(dirty));

            // TODO next TinyBuilder return values dilema silverqx
            if (sqlQuery.lastError().isValid())
                return false;

            syncChanges();

//            fireModelEvent("updated", false);
        }

        return true;
    }

    template<typename Model, typename ...AllRelations>
    void BaseModel<Model, AllRelations...>::finishSave(const SaveOptions &options)
    {
//        fireModelEvent('saved', false);

        if (isDirty() && options.touch)
            touchOwners();

        syncOriginal();
    }

    template<typename Model, typename ...AllRelations>
    quint64 BaseModel<Model, AllRelations...>::insertAndSetId(
            const TinyBuilder<Model> &query,
            const QVector<AttributeItem> &attributes)
    {
        // TODO now insertGetId() and getKeyName() silverqx
//        const auto &keyName = getKeyName();

        const auto id = query.insertGetId(attributes/*, keyName*/);

        // When insert was successful
        if (id != 0)
            setAttribute(getKeyName(), id);

        return id;
    }

    template<typename Model, typename ...AllRelations>
    bool BaseModel<Model, AllRelations...>::touch()
    {
        if (!usesTimestamps())
            return false;

        updateTimestamps();

        return save();
    }

    template<typename Model, typename ...AllRelations>
    void BaseModel<Model, AllRelations...>::updateTimestamps()
    {
        const auto time = freshTimestamp();

        const QString &updatedAtColumn = getUpdatedAtColumn();

        if (!updatedAtColumn.isEmpty() && !isDirty(updatedAtColumn))
            setUpdatedAt(time);

        const QString &createdAtColumn = getCreatedAtColumn();

        if (!exists && !createdAtColumn.isEmpty() && !isDirty(createdAtColumn))
            setCreatedAt(time);
    }

    template<typename Model, typename ...AllRelations>
    Model &BaseModel<Model, AllRelations...>::setCreatedAt(const QDateTime &value)
    {
        return setAttribute(getCreatedAtColumn(), value);
    }

    template<typename Model, typename ...AllRelations>
    Model &BaseModel<Model, AllRelations...>::setUpdatedAt(const QDateTime &value)
    {
        return setAttribute(getUpdatedAtColumn(), value);
    }

    template<typename Model, typename ...AllRelations>
    QString BaseModel<Model, AllRelations...>::freshTimestampString() const
    {
        return fromDateTime(freshTimestamp());
    }

    template<typename Model, typename ...AllRelations>
    QString BaseModel<Model, AllRelations...>::getQualifiedCreatedAtColumn() const
    {
        return qualifyColumn(getCreatedAtColumn());
    }

    template<typename Model, typename ...AllRelations>
    QString BaseModel<Model, AllRelations...>::getQualifiedUpdatedAtColumn() const
    {
        return qualifyColumn(getUpdatedAtColumn());
    }

    template<typename Model, typename ...AllRelations>
    template<typename ClassToCheck>
    bool BaseModel<Model, AllRelations...>::isIgnoringTouch()
    {
        if (!ClassToCheck().usesTimestamps()
            || ClassToCheck::getUpdatedAtColumn().isEmpty()
        )
            return true;

        // TODO future implement withoutTouching() and related data member $ignoreOnTouch silverqx

        return false;
    }

} // namespace Orm::Tiny
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // BASEMODEL_H
