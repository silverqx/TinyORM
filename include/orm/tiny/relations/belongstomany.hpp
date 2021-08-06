#pragma once
#ifndef BELONGSTOMANY_H
#define BELONGSTOMANY_H

#include <QDateTime>
#include <QtSql/QSqlRecord>

#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/set_algorithm.hpp>
#include <range/v3/view/transform.hpp>

#include "orm/exceptions/domainerror.hpp"
#include "orm/macros.hpp"
#include "orm/tiny/exceptions/modelnotfounderror.hpp"
#include "orm/tiny/relations/relation.hpp"
#include "orm/utils/attribute.hpp"
#include "orm/utils/type.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{
    class Pivot;

    /*! TinyORM's 'Pivot' class. */
    template<typename PivotType>
    concept OurPivot = std::same_as<PivotType, Pivot>;

    /*! Custom pivot class, not TinyORM's 'Pivot' class. */
    template<typename PivotType>
    concept CustomPivot = !std::same_as<PivotType, Pivot>;

    template<class Model, class Related, class PivotType = Pivot>
    class BelongsToMany :
            public ManyRelation,
            public Relation<Model, Related>,
            public PivotRelation
    {
        /*! Model alias. */
        template<typename Derived>
        using BaseModel = Orm::Tiny::Model<Derived>;

    protected:
        /*! Protected constructor. */
        BelongsToMany(std::unique_ptr<Related> &&related, Model &parent,
                      const QString &table = "", const QString &foreignPivotKey = "",
                      const QString &relatedPivotKey = "", const QString &parentKey = "",
                      const QString &relatedKey = "", const QString &relationName = "");

    public:
        /*! Instantiate and initialize a new BelongsToMany instance. */
        static std::unique_ptr<BelongsToMany<Model, Related, PivotType>>
        instance(std::unique_ptr<Related> &&related, Model &parent,
                 const QString &table = "", const QString &foreignPivotKey = "",
                 const QString &relatedPivotKey = "", const QString &parentKey = "",
                 const QString &relatedKey = "", const QString &relation = "");

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
        /*! Execute the query as a "select" statement. */
        QVector<Related> get(const QVector<Column> &columns = {ASTERISK}) const override;

        /* Getters / Setters */
        /*! Qualify the given column name by the pivot table. */
        QString qualifyPivotColumn(const QString &column) const;
        /*! Get the fully qualified foreign key for the relation. */
        QString getQualifiedForeignPivotKeyName() const;
        /*! Get the fully qualified "related key" for the relation. */
        QString getQualifiedRelatedPivotKeyName() const;
        /*! Get the fully qualified parent key name for the relation. */
        QString getQualifiedParentKeyName() const override;
        /*! Get the fully qualified related key name for the relation. */
        QString getQualifiedRelatedKeyName() const;
        /*! Get the key for comparing against the parent key in "has" query. */
        QString getExistenceCompareKey() const override;

        /*! Get the intermediate table for the relationship. */
        const QString &getTable() const;
        /*! Get the foreign key for the relation. */
        const QString &getForeignPivotKeyName() const;
        /*! Get the "related key" for the relation. */
        const QString &getRelatedPivotKeyName() const;
        /*! Get the parent key for the relationship. */
        const QString &getParentKeyName() const;
        /*! Get the relationship name for the relationship. */
        const QString &getRelationName() const;

        /*! Get the name of the pivot accessor for this relationship. */
        const QString &getPivotAccessor() const;
        /*! Specify the custom pivot accessor to use for the relationship. */
        BelongsToMany &as(const QString &accessor);

        /*! Get the pivot columns for this relationship. */
        const QStringList &getPivotColumns() const;

        /*! Determine if the 'pivot' model uses timestamps. */
        bool usesTimestamps() const;
        /*! Specify that the pivot table has creation and update timestamps. */
        BelongsToMany &withTimestamps(const QString &createdAt = "",
                                      const QString &updatedAt = "");
        /*! Get the name of the "created at" column. */
        const QString &createdAt() const;
        /*! Get the name of the "updated at" column. */
        const QString &updatedAt() const;
        /*! If we're touching the parent model, touch. */
        void touchIfTouching() const;
        /*! Touch all of the related models for the relationship.
            E.g.: Touch all roles associated with this user. */
        void touch() const override;

        /* InteractsWithPivotTable */
        /*! Set the columns on the pivot table to retrieve. */
        BelongsToMany &withPivot(const QStringList &columns);
        /*! Set the columns on the pivot table to retrieve. */
        BelongsToMany &withPivot(const QString &column);

        /*! Determine whether the given column is defined as a pivot column. */
        bool hasPivotColumn(const QString &column) const;

        /*! Create a new existing pivot model instance. */
        PivotType newExistingPivot(const QVector<AttributeItem> &attributes = {}) const;
        /*! Create a new pivot model instance. */
        PivotType newPivot(const QVector<AttributeItem> &attributes = {},
                           bool exists = false) const;

        /*! Create a new query builder for the pivot table. */
        QSharedPointer<QueryBuilder> newPivotQuery() const;
        /*! Get a new plain query builder for the pivot table. */
        QSharedPointer<QueryBuilder> newPivotStatement() const;
        /*! Get a new pivot statement for a given "other" / "related" ID. */
        QSharedPointer<QueryBuilder>
        newPivotStatementForId(const QVector<QVariant> &ids) const;
        /*! Get a new pivot statement for a given "other" / "related" ID. */
        QSharedPointer<QueryBuilder>
        newPivotStatementForId(const QVariant &id) const;

        /* TinyBuilder proxy methods */
        /*! Find a model by its primary key. */
        std::optional<Related>
        find(const QVariant &id,
             const QVector<Column> &columns = {ASTERISK}) const override;
        /*! Find a model by its primary key or return fresh model instance. */
        Related
        findOrNew(const QVariant &id,
                  const QVector<Column> &columns = {ASTERISK}) const override;
        /*! Find a model by its primary key or throw an exception. */
        Related
        findOrFail(const QVariant &id,
                   const QVector<Column> &columns = {ASTERISK}) const override;
        /*! Find multiple models by their primary keys. */
        QVector<Related>
        findMany(const QVector<QVariant> &ids,
                 const QVector<Column> &columns = {ASTERISK}) const;

        /*! Execute the query and get the first result. */
        std::optional<Related>
        first(const QVector<Column> &columns = {ASTERISK}) const override;
        /*! Get the first record matching the attributes or instantiate it. */
        Related firstOrNew(const QVector<WhereItem> &attributes = {},
                           const QVector<AttributeItem> &values = {}) const override;
        /*! Get the first record matching the attributes or create it. */
        Related firstOrCreate(const QVector<WhereItem> &attributes = {},
                              const QVector<AttributeItem> &values = {},
                              const QVector<AttributeItem> &pivotValues = {},
                              bool touch = true) const;
        /*! Execute the query and get the first result or throw an exception. */
        Related firstOrFail(const QVector<Column> &columns = {ASTERISK}) const override;

        /*! Add a basic where clause to the query, and return the first result. */
        std::optional<Related>
        firstWhere(const Column &column, const QString &comparison,
                   const QVariant &value,
                   const QString &condition = AND) const override;
        /*! Add a basic where clause to the query, and return the first result. */
        std::optional<Related>
        firstWhereEq(const Column &column, const QVariant &value,
                     const QString &condition = AND) const override;

        /* Inserting operations on the relationship */
        /*! Attach a model instance to the parent model. */
        std::tuple<bool, Related &>
        save(Related &model,
             const QVector<AttributeItem> &pivotValues = {},
             bool touch = true) const;
        /*! Attach a model instance to the parent model. */
        std::tuple<bool, Related>
        save(Related &&model,
             const QVector<AttributeItem> &pivotValues = {},
             bool touch = true) const;
        /*! Attach a vector of models to the parent instance. */
        QVector<Related> &
        saveMany(QVector<Related> &models,
                 const QVector<QVector<AttributeItem>> &pivotValues = {}) const;
        /*! Attach a vector of models to the parent instance. */
        QVector<Related>
        saveMany(QVector<Related> &&models,
                 const QVector<QVector<AttributeItem>> &pivotValues = {}) const;

        /*! Create a new instance of the related model. */
        Related create(const QVector<AttributeItem> &attributes = {},
                       const QVector<AttributeItem> &pivotValues = {},
                       bool touch = true) const;
        /*! Create a new instance of the related model. */
        Related create(QVector<AttributeItem> &&attributes = {},
                       const QVector<AttributeItem> &pivotValues = {},
                       bool touch = true) const;
        /*! Create a vector of new instances of the related model. */
        QVector<Related>
        createMany(const QVector<QVector<AttributeItem>> &records,
                   const QVector<QVector<AttributeItem>> &pivotValues = {}) const;
        /*! Create a vector of new instances of the related model. */
        QVector<Related>
        createMany(QVector<QVector<AttributeItem>> &&records,
                   const QVector<QVector<AttributeItem>> &pivotValues = {}) const;

        /* Inserting & Updating relationship */
        /*! Attach models to the parent. */
        void attach(const QVector<QVariant> &ids,
                    const QVector<AttributeItem> &attributes = {},
                    bool touch = true) const;
        /*! Attach models to the parent. */
        void attach(const QVector<std::reference_wrapper<Related>> &models,
                    const QVector<AttributeItem> &attributes = {},
                    bool touch = true) const;
        /*! Attach a model to the parent. */
        void
        attach(const QVariant &id, const QVector<AttributeItem> &attributes = {},
               bool touch = true) const;
        /*! Attach a model to the parent. */
        void
        attach(const Related &model, const QVector<AttributeItem> &attributes = {},
               bool touch = true) const;
        /*! Attach models to the parent. */
        void attach(const std::map<typename BaseModel<Related>::KeyType,
                                   QVector<AttributeItem>> &idsWithAttributes,
                    bool touch = true) const;

        /*! Sync the intermediate tables with a list of IDs. */
        SyncChanges sync(const std::map<typename BaseModel<Related>::KeyType,
                                        QVector<AttributeItem>> &idsWithAttributes,
                         bool detaching = true) const;
        /*! Sync the intermediate tables with a vector of IDs. */
        SyncChanges sync(const QVector<QVariant> &ids,
                         bool detaching = true) const;

        /*! Sync the intermediate tables with a vector of IDs without detaching. */
        SyncChanges syncWithoutDetaching(
                const std::map<typename BaseModel<Related>::KeyType,
                QVector<AttributeItem>> &idsWithAttributes) const;
        /*! Sync the intermediate tables with a vector of IDs without detaching. */
        SyncChanges syncWithoutDetaching(const QVector<QVariant> &ids) const;

        /*! Detach models from the relationship. */
        int detach(const QVector<QVariant> &ids, bool touch = true) const;
        /*! Detach models from the relationship. */
        int detach(bool touch = true) const;
        /*! Detach models from the relationship. */
        int detach(const QVector<std::reference_wrapper<Related>> &models,
                   bool touch = true) const;
        /*! Detach model from the relationship. */
        int detach(const QVariant &id, bool touch = true) const;
        /*! Detach model from the relationship. */
        int detach(const Related &model, bool touch = true) const;

        /*! Update an existing pivot record on the table. */
        int updateExistingPivot(const QVariant &id,
                                QVector<AttributeItem> attributes,
                                bool touch = true) const;
        /*! Update an existing pivot record on the table. */
        int updateExistingPivot(const Related &model,
                                const QVector<AttributeItem> &attributes,
                                bool touch = true) const;

        /*! Create or update a related record matching the attributes, and fill it
            with values. */
        Related updateOrCreate(const QVector<WhereItem> &attributes,
                               const QVector<AttributeItem> &values = {},
                               const QVector<AttributeItem> &pivotValues = {},
                               bool touch = true) const;

        /* Others */
        /*! Get all of the IDs for the related models. */
        QVector<QVariant> allRelatedIds() const;

        /*! The textual representation of the Relation type. */
        QString relationTypeName() const override;

    protected:
        /*! Set the join clause for the relation query. */
        const BelongsToMany &performJoin() const;
        /*! Set the join clause for the relation query. */
        const BelongsToMany &performJoin(Builder<Related> &query) const;
        /*! Set the where clause for the relation query. */
        const BelongsToMany &addWhereConstraints() const;

        /*! Build model dictionary keyed by the relation's foreign key. */
        QHash<typename Model::KeyType, QVector<Related>>
        buildDictionary(QVector<Related> &results) const;

        /*! Get the select columns for the relation query. */
        QVector<Column> shouldSelect(QVector<Column> columns = {ASTERISK}) const;
        /*! Get the pivot columns for the relation, "pivot_" is prefixed
            to each column for easy removal later. */
        QStringList aliasedPivotColumns() const;

        /*! Hydrate the pivot table relationship on the models. */
        void hydratePivotRelation(QVector<Related> &models) const;
        /*! Get the pivot attributes from a model. */
        QVector<AttributeItem> migratePivotAttributes(Related &model) const;

        /* Inserting & Updating relationship */
        /* InteractsWithPivotTable */
        /*! Attach a model to the parent using a custom class. */
        void attachUsingCustomClass(const QVector<QVariant> &ids,
                                    const QVector<AttributeItem> &attributes) const;
        /*! Create the vector of records to insert into the pivot table. */
        QVector<QVector<AttributeItem>>
        formatAttachRecords(const QVector<QVariant> &ids,
                            const QVector<AttributeItem> &attributes) const;
        /*! Create a full attachment record payload. */
        QVector<AttributeItem>
        formatAttachRecord(const QVariant &id, const QVector<AttributeItem> &attributes,
                           bool hasTimestamps) const;
        /*! Attach a model to the parent using a custom class. */
        void attachUsingCustomClass(
                const std::map<typename BaseModel<Related>::KeyType,
                               QVector<AttributeItem>> &idsWithAttributes) const;
        /*! Create the vector of records to insert into the pivot table. */
        QVector<QVector<AttributeItem>>
        formatAttachRecords(
                const std::map<typename BaseModel<Related>::KeyType,
                               QVector<AttributeItem>> &idsWithAttributes) const;
        /*! Create a new pivot attachment record. */
        QVector<AttributeItem>
        baseAttachRecord(const QVariant &id, bool timed) const;
        /*! Set the creation and update timestamps on an attach record. */
        QVector<AttributeItem> &
        addTimestampsToAttachment(QVector<AttributeItem> &record,
                                  bool exists = false) const;

        /*! Get the pivot models that are currently attached. */
        QVector<PivotType> getCurrentlyAttachedPivots() const;
        /*! Get the attached pivot model by related model ID. */
        std::optional<PivotType> getAttachedPivot(const QVariant &id) const;
        /*! Convert a QSqlRecord to the QVector<AttributeItem>. */
        QVector<AttributeItem>
        attributesFromRecord(const QSqlRecord &record) const;
        /*! Cast the given key to primary key type. */
        template<typename T>
        T castKey(const QVariant &key) const;
        /*! Update an existing pivot record on the table via a custom class. */
        int updateExistingPivotUsingCustomClass(
                const QVariant &id, const QVector<AttributeItem> &attributes,
                bool touch = true) const;
        /*! Attach all of the records that aren't in the given current records. */
        SyncChanges
        attachNew(const std::map<typename BaseModel<Related>::KeyType,
                                 QVector<AttributeItem>> &records,
                  const QVector<QVariant> &current, bool touch = true) const;
        /*! Convert IDs vector to the map with attributes keyed by IDs. */
        std::map<typename BaseModel<Related>::KeyType, QVector<AttributeItem>>
        recordsFromIds(const QVector<QVariant> &ids) const;
        /*! Convert IDs vector to the map with attributes keyed by IDs. */
        QVector<QVariant>
        idsFromRecords(const std::map<typename BaseModel<Related>::KeyType,
                                      QVector<AttributeItem>> &idsWithAttributes) const;
        /*! Cast the given pivot attributes. */
        QVector<AttributeItem> &
        castAttributes(QVector<AttributeItem> &attributes) const
        requires OurPivot<PivotType>;
        /*! Cast the given pivot attributes. */
        QVector<AttributeItem>
        castAttributes(const QVector<AttributeItem> &attributes) const
        requires CustomPivot<PivotType>;

        /*! Determine if we should touch the parent on sync. */
        bool touchingParent() const;
        /*! Attempt to guess the name of the inverse of the relation. */
        QString guessInverseRelation() const;

        /*! Detach models from the relationship. */
        int detach(bool detachAll, const QVector<QVariant> &ids, bool touch) const;
        /*! Detach models from the relationship using a custom class. */
        int detachUsingCustomClass(const QVector<QVariant> &ids) const;

        /*! Obtain ids from the Related models. */
        QVector<QVariant>
        getRelatedIds(const QVector<std::reference_wrapper<Related>> &models) const;
        /*! Obtain ids from the QVector<AttributeItem>. */
        QVector<QVariant>
        getRelatedIds(const QVector<PivotType> &pivots) const;
        /*! Obtain ids from the QVector<AttributeItem>. */
        QVector<QVariant>
        getRelatedIds(QVector<PivotType> &&pivots) const;

        /* Querying Relationship Existence/Absence */
        /*! Add the constraints for a relationship query. */
        std::unique_ptr<Builder<Related>>
        getRelationExistenceQuery(
                std::unique_ptr<Builder<Related>> &&query,
                const Builder<Model> &parentQuery,
                const QVector<Column> &columns = {ASTERISK}) const override;

        /*! The intermediate table for the relation. */
        QString m_table;
        /*! The foreign key of the parent model. */
        QString m_foreignPivotKey;
        /*! The associated key of the relation. */
        QString m_relatedPivotKey;
        /*! The key name of the parent model. */
        QString m_parentKey;
        /*! The name of the relationship. */
        QString m_relationName;

        /*! The name of the accessor to use for the "pivot" relationship. */
        QString m_accessor {"pivot"};
        // BUG should be QSet, duplicates are not allowed, check all the containers 😭 and use proper containers where I did mistake, from the point of view of duplicates silverqx
        /*! The pivot table columns to retrieve. */
        QStringList m_pivotColumns;

        /*! Indicates if timestamps are available on the pivot table. */
        bool m_withTimestamps = false;
        /*! The custom pivot table column for the created_at timestamp. */
        QString m_pivotCreatedAt;
        /*! The custom pivot table column for the updated_at timestamp. */
        QString m_pivotUpdatedAt;

    private:
        /*! Throw domain exception, when a user tries to override ID key
            on the pivot table. */
        void validateAttachAttribute(const AttributeItem &attribute,
                                     const QVariant &id) const;
        /*! Throw domain exception, when a user tries to override ID key
            on the pivot table.  */
        template<typename KeyType = typename BaseModel<Model>::KeyType>
        void throwOverwritingKeyError(const QString &key, const QVariant &original,
                                      const QVariant &overwrite) const;
    };

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType>::BelongsToMany(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &table, const QString &foreignPivotKey,
            const QString &relatedPivotKey, const QString &parentKey,
            const QString &relatedKey, const QString &relationName
    )
        : Relation<Model, Related>(std::move(related), parent, relatedKey)
        , m_table(table)
        , m_foreignPivotKey(foreignPivotKey)
        , m_relatedPivotKey(relatedPivotKey)
        , m_parentKey(parentKey)
        , m_relationName(relationName)
    {}

    template<class Model, class Related, class PivotType>
    std::unique_ptr<BelongsToMany<Model, Related, PivotType>>
    BelongsToMany<Model, Related, PivotType>::instance(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &table, const QString &foreignPivotKey,
            const QString &relatedPivotKey, const QString &parentKey,
            const QString &relatedKey, const QString &relation)
    {
        auto instance = std::unique_ptr<BelongsToMany<Model, Related, PivotType>>(
                    new BelongsToMany<Model, Related, PivotType>(
                        std::move(related), parent, table, foreignPivotKey,
                        relatedPivotKey, parentKey, relatedKey, relation));

        instance->init();

        return instance;
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::addConstraints() const
    {
        performJoin();

        if (this->constraints)
            addWhereConstraints();
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::addEagerConstraints(
            const QVector<Model> &models) const
    {
        this->m_query->whereIn(getQualifiedForeignPivotKeyName(),
                               this->getKeys(models, m_parentKey));
    }

    template<class Model, class Related, class PivotType>
    QVector<Model> &
    BelongsToMany<Model, Related, PivotType>::initRelation(
            QVector<Model> &models, const QString &relation) const
    {
        for (auto &model : models)
            model.template setRelation<Related>(relation, QVector<Related>());

        return models;
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::match(
            QVector<Model> &models, QVector<Related> results,
            const QString &relation) const
    {
        auto dictionary = buildDictionary(results);

        /* Once we have the dictionary of child objects, we can easily match the
           children back to their parent using the dictionary and the keys on the
           the parent models. Then we will return the hydrated models back out. */
        for (auto &model : models)
            if (const auto key = model.getAttribute(m_parentKey)
                .template value<typename Model::KeyType>();
                dictionary.contains(key)
            )
                model.template setRelation<Related>(
                            relation,
                            std::move(dictionary.find(key).value()));
    }

    template<class Model, class Related, class PivotType>
    QHash<typename Model::KeyType, QVector<Related>>
    BelongsToMany<Model, Related, PivotType>::buildDictionary(
            QVector<Related> &results) const
    {
        /* First we will build a dictionary of child models keyed by the foreign key
           of the relation so that we will easily and quickly match them to their
           parents without having a possibly slow inner loops for every models. */
        QHash<typename Model::KeyType, QVector<Related>> dictionary;

        /*! Build model dictionary keyed by the parent's primary key. */
        for (auto &result : results) {

            const auto foreignPivotKey =
                    result.template getRelation<PivotType, Orm::One>(m_accessor)
                    ->getAttribute(m_foreignPivotKey)
                    .template value<typename Model::KeyType>();

            dictionary[foreignPivotKey].append(std::move(result));
        }

        return dictionary;
    }

    template<class Model, class Related, class PivotType>
    std::variant<QVector<Related>, std::optional<Related>>
    BelongsToMany<Model, Related, PivotType>::getResults() const
    {
        // Model doesn't contain primary key ( eg empty Model instance )
        if (const auto key = this->m_parent.getAttribute(m_parentKey);
            !key.isValid() || key.isNull()
        )
            return QVector<Related>();

        return get();
    }

    template<class Model, class Related, class PivotType>
    QVector<Related>
    BelongsToMany<Model, Related, PivotType>::get(const QVector<Column> &columns) const
    {
        /* First we'll add the proper select columns onto the query so it is run with
           the proper columns. Then, we will get the results and hydrate out pivot
           models with the result of those columns as a separate model relation. */
        // FEATURE scopes silverqx
//        $builder = $this->query->applyScopes();

        auto l_columns = this->m_query->getQuery().getColumns().isEmpty()
                         ? columns
                         : QVector<Column>();

        // Hydrated related models
        auto models = this->m_query
                      ->addSelect(shouldSelect(l_columns))
                      .getModels();

        hydratePivotRelation(models);

        /* If we actually found models, we will also eager load any relationships that
           have been specified as needing to be eager loaded. This will solve the
           n + 1 query problem for the developer and also increase performance. */
        if (!models.isEmpty())
            this->m_query->eagerLoadRelations(models);

        return models;
    }

    template<class Model, class Related, class PivotType>
    QString
    BelongsToMany<Model, Related, PivotType>::qualifyPivotColumn(
            const QString &column) const
    {
        if (column.contains(DOT))
            return column;

        return QStringLiteral("%1.%2").arg(m_table, column);
    }

    template<class Model, class Related, class PivotType>
    inline QString
    BelongsToMany<Model, Related, PivotType>::getQualifiedForeignPivotKeyName() const
    {
        return qualifyPivotColumn(m_foreignPivotKey);
    }

    template<class Model, class Related, class PivotType>
    inline QString
    BelongsToMany<Model, Related, PivotType>::getQualifiedRelatedPivotKeyName() const
    {
        return qualifyPivotColumn(m_relatedPivotKey);
    }

    template<class Model, class Related, class PivotType>
    inline QString
    BelongsToMany<Model, Related, PivotType>::getQualifiedParentKeyName() const
    {
        return this->m_parent.qualifyColumn(m_parentKey);
    }

    template<class Model, class Related, class PivotType>
    inline QString
    BelongsToMany<Model, Related, PivotType>::getQualifiedRelatedKeyName() const
    {
        return this->m_related->qualifyColumn(this->m_relatedKey);
    }

    template<class Model, class Related, class PivotType>
    inline QString
    BelongsToMany<Model, Related, PivotType>::getExistenceCompareKey() const
    {
        return getQualifiedForeignPivotKeyName();
    }

    template<class Model, class Related, class PivotType>
    inline const QString &
    BelongsToMany<Model, Related, PivotType>::getTable() const
    {
        return m_table;
    }

    template<class Model, class Related, class PivotType>
    inline const QString &
    BelongsToMany<Model, Related, PivotType>::getForeignPivotKeyName() const
    {
        return m_foreignPivotKey;
    }

    template<class Model, class Related, class PivotType>
    inline const QString &
    BelongsToMany<Model, Related, PivotType>::getRelatedPivotKeyName() const
    {
        return m_relatedPivotKey;
    }

    template<class Model, class Related, class PivotType>
    inline const QString &
    BelongsToMany<Model, Related, PivotType>::getParentKeyName() const
    {
        return m_parentKey;
    }

    template<class Model, class Related, class PivotType>
    inline const QString &
    BelongsToMany<Model, Related, PivotType>::getRelationName() const
    {
        return m_relationName;
    }

    template<class Model, class Related, class PivotType>
    const QString &
    BelongsToMany<Model, Related, PivotType>::getPivotAccessor() const
    {
        return m_accessor;
    }

    template<class Model, class Related, class PivotType>
    inline BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::as(const QString &accessor)
    {
        m_accessor = accessor;

        return *this;
    }

    template<class Model, class Related, class PivotType>
    inline const QStringList &
    BelongsToMany<Model, Related, PivotType>::getPivotColumns() const
    {
        return m_pivotColumns;
    }

    template<class Model, class Related, class PivotType>
    inline bool
    BelongsToMany<Model, Related, PivotType>::usesTimestamps() const
    {
        return m_withTimestamps;
    }

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::withTimestamps(
            const QString &createdAt, const QString &updatedAt)
    {
        m_withTimestamps = true;

        m_pivotCreatedAt = createdAt;
        m_pivotUpdatedAt = updatedAt;

        return withPivot({this->createdAt(), this->updatedAt()});
    }

    template<class Model, class Related, class PivotType>
    const QString &BelongsToMany<Model, Related, PivotType>::createdAt() const
    {
        if (m_pivotCreatedAt.isEmpty())
            return this->m_parent.getCreatedAtColumn();

        return m_pivotCreatedAt;
    }

    template<class Model, class Related, class PivotType>
    const QString &BelongsToMany<Model, Related, PivotType>::updatedAt() const
    {
        if (m_pivotUpdatedAt.isEmpty())
            return this->m_parent.getUpdatedAtColumn();

        return m_pivotUpdatedAt;
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::touchIfTouching() const
    {
        // BUG circular dependency when both models (Model and Related) has set up u_touches silverqx
        if (touchingParent())
            this->m_parent.touch();

        if (this->m_parent.touches(m_relationName))
            touch();
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::touch() const
    {
        const auto &related = this->m_related;

        const auto &key = related->getKeyName();

        const QVector<UpdateItem> record {
            {related->getUpdatedAtColumn(), related->freshTimestampString()},
        };

        const auto ids = allRelatedIds();

        /* If we actually have IDs for the relation, we will run the query to update all
           the related model's timestamps, to make sure these all reflect the changes
           to the parent models. This will help us keep any caching synced up here. */
        if (!ids.isEmpty())
            related->newQueryWithoutRelationships()
                ->whereIn(key, ids)
                .update(record);
    }

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::withPivot(const QStringList &columns)
    {
        std::ranges::copy_if(columns, std::back_inserter(m_pivotColumns),
                             [this](const auto &column)
        {
            return !m_pivotColumns.contains(column);
        });

        return *this;
    }

    template<class Model, class Related, class PivotType>
    inline BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::withPivot(const QString &column)
    {
        return withPivot(QStringList {column});
    }

    template<class Model, class Related, class PivotType>
    inline bool
    BelongsToMany<Model, Related, PivotType>::hasPivotColumn(
            const QString &column) const
    {
        return m_pivotColumns.contains(column);
    }

    template<class Model, class Related, class PivotType>
    PivotType BelongsToMany<Model, Related, PivotType>::newExistingPivot(
            const QVector<AttributeItem> &attributes) const
    {
        return newPivot(attributes, true);
    }

    template<class Model, class Related, class PivotType>
    PivotType BelongsToMany<Model, Related, PivotType>::newPivot(
            const QVector<AttributeItem> &attributes, const bool exists) const
    {
        return this->m_related->template newPivot<PivotType, Model>(
                    this->m_parent, attributes, m_table, exists)

                .setPivotKeys(m_foreignPivotKey, m_relatedPivotKey);
    }

    template<class Model, class Related, class PivotType>
    QSharedPointer<QueryBuilder>
    BelongsToMany<Model, Related, PivotType>::newPivotQuery() const
    {
        // Ownership of the QSharedPointer<QueryBuilder>
        auto query = newPivotStatement();

        // FEATURE relations, add support for BelongsToMany::where/whereIn/whereNull silverqx
//        for (auto &[column, value, comparison, condition] : m_pivotWheres)
//            query->where(column, value, comparison, condition);

//        for (auto &[column, values, condition, nope] : m_pivotWhereIns)
//            query->whereIn(column, values, condition, nope);

//        for (auto &[😭, condition, nope] : m_pivotWhereNulls)
//            query->whereNull(columns, condition, nope);

        query->whereEq(m_foreignPivotKey, this->m_parent[m_parentKey]);

        return query;
    }

    template<class Model, class Related, class PivotType>
    QSharedPointer<QueryBuilder>
    BelongsToMany<Model, Related, PivotType>::newPivotStatement() const
    {
        auto query = this->m_query->getQuery().newQuery();

        query->from(m_table);

        return query;
    }

    template<class Model, class Related, class PivotType>
    QSharedPointer<QueryBuilder>
    BelongsToMany<Model, Related, PivotType>::newPivotStatementForId(
            const QVector<QVariant> &ids) const
    {
        // Ownership of the QSharedPointer<QueryBuilder>
        auto query = newPivotQuery();

        query->whereIn(m_relatedPivotKey, ids);

        return query;
    }

    template<class Model, class Related, class PivotType>
    inline QSharedPointer<QueryBuilder>
    BelongsToMany<Model, Related, PivotType>::newPivotStatementForId(
            const QVariant &id) const
    {
        return newPivotStatementForId(QVector<QVariant> {id});
    }

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::find(
            const QVariant &id, const QVector<Column> &columns) const
    {
        return this->where(this->m_related->getQualifiedKeyName(), EQ, id)
                .first(columns);
    }

    template<class Model, class Related, class PivotType>
    Related BelongsToMany<Model, Related, PivotType>::findOrNew(
            const QVariant &id, const QVector<Column> &columns) const
    {
        // Found
        if (auto instance = find(id, columns); instance)
            return *instance;

        return this->m_related->newInstance();
    }

    template<class Model, class Related, class PivotType>
    Related BelongsToMany<Model, Related, PivotType>::findOrFail(
            const QVariant &id, const QVector<Column> &columns) const
    {
        auto model = find(id, columns);

        // Found
        if (model)
            return *model;

        throw Exceptions::ModelNotFoundError(
                    Utils::Type::classPureBasename<Related>(), {id});
    }

    template<class Model, class Related, class PivotType>
    QVector<Related>
    BelongsToMany<Model, Related, PivotType>::findMany(
            const QVector<QVariant> &ids, const QVector<Column> &columns) const
    {
        if (ids.isEmpty())
            return {};

        return this->whereIn(this->m_related->getQualifiedKeyName(), ids).get(columns);
    }

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::first(const QVector<Column> &columns) const
    {
        auto results = this->take(1).get(columns);

        if (results.isEmpty())
            return std::nullopt;

        return std::move(results.first());
    }

    template<class Model, class Related, class PivotType>
    Related
    BelongsToMany<Model, Related, PivotType>::firstOrNew(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values) const
    {
        // Model found in db
        if (auto instance = this->where(attributes).first(); instance)
            return *instance;

        return this->m_related->newInstance(
                    Utils::Attribute::joinAttributesForFirstOr(attributes, values,
                                                               this->m_relatedKey));
    }

    template<class Model, class Related, class PivotType>
    Related
    BelongsToMany<Model, Related, PivotType>::firstOrCreate(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values,
            const QVector<AttributeItem> &pivotValues,
            const bool touch) const
    {
        if (auto instance = this->where(attributes).first(); instance)
            return *instance;

        // NOTE api different, Eloquent doen't use values argument silverqx
        return create(Utils::Attribute::joinAttributesForFirstOr(
                          attributes, values, this->m_relatedKey),
                      pivotValues, touch);
    }

    template<class Model, class Related, class PivotType>
    Related
    BelongsToMany<Model, Related, PivotType>::firstOrFail(
            const QVector<Column> &columns) const
    {
        if (auto model = first(columns); model)
            return *model;

        throw Exceptions::ModelNotFoundError(
                    Utils::Type::classPureBasename<Related>());
    }

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::firstWhere(
            const Column &column, const QString &comparison,
            const QVariant &value, const QString &condition) const
    {
        return this->where(column, comparison, value, condition).first();
    }

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::firstWhereEq(
            const Column &column, const QVariant &value,
            const QString &condition) const
    {
        return firstWhere(column, EQ, value, condition);
    }

    template<class Model, class Related, class PivotType>
    std::tuple<bool, Related &>
    BelongsToMany<Model, Related, PivotType>::save(
            Related &model,
            const QVector<AttributeItem> &pivotValues, const bool touch) const
    {
        const auto result = model.save({.touch = false});

        attach(model, pivotValues, touch);

        return {result, model};
    }

    template<class Model, class Related, class PivotType>
    std::tuple<bool, Related>
    BelongsToMany<Model, Related, PivotType>::save(
            Related &&model,
            const QVector<AttributeItem> &pivotValues, const bool touch) const
    {
        const auto result = model.save({.touch = false});

        attach(model, pivotValues, touch);

        return {result, std::move(model)};
    }

    template<class Model, class Related, class PivotType>
    QVector<Related> &
    BelongsToMany<Model, Related, PivotType>::saveMany(
            QVector<Related> &models,
            const QVector<QVector<AttributeItem>> &pivotValues) const
    {
        for (int i = 0, attributesSize = pivotValues.size(); i < models.size(); ++i)
            if (attributesSize > i) T_LIKELY
                save(models[i], pivotValues.at(i), false);
            else T_UNLIKELY
                save(models[i], {}, false);

        touchIfTouching();

        return models;
    }

    template<class Model, class Related, class PivotType>
    QVector<Related>
    BelongsToMany<Model, Related, PivotType>::saveMany(
            QVector<Related> &&models,
            const QVector<QVector<AttributeItem>> &pivotValues) const
    {
        for (int i = 0, attributesSize = pivotValues.size(); i < models.size(); ++i)
            if (attributesSize > i) T_LIKELY
                save(models[i], pivotValues.at(i), false);
            else T_UNLIKELY
                save(models[i], {}, false);

        touchIfTouching();

        return std::move(models);
    }

    template<class Model, class Related, class PivotType>
    Related
    BelongsToMany<Model, Related, PivotType>::create(
            const QVector<AttributeItem> &attributes,
            const QVector<AttributeItem> &pivotValues, const bool touch) const
    {
        auto instance = this->m_related->newInstance(attributes);

        /* Once we save the related model, we need to attach it to the base model via
           through intermediate table so we'll use the existing "attach" method to
           accomplish this which will insert the record and any more attributes. */
        instance.save({.touch = false});

        attach(instance, pivotValues, touch);

        return instance;
    }

    template<class Model, class Related, class PivotType>
    Related
    BelongsToMany<Model, Related, PivotType>::create(
            QVector<AttributeItem> &&attributes,
            const QVector<AttributeItem> &pivotValues, const bool touch) const
    {
        auto instance = this->m_related->newInstance(std::move(attributes));

        /* Once we save the related model, we need to attach it to the base model via
           through intermediate table so we'll use the existing "attach" method to
           accomplish this which will insert the record and any more attributes. */
        instance.save({.touch = false});

        attach(instance, pivotValues, touch);

        return instance;
    }

    template<class Model, class Related, class PivotType>
    QVector<Related>
    BelongsToMany<Model, Related, PivotType>::createMany(
            const QVector<QVector<AttributeItem>> &records,
            const QVector<QVector<AttributeItem>> &pivotValues) const
    {
        QVector<Related> instances;
        const auto recordsSize = records.size();
        instances.reserve(recordsSize);

        for (int i = 0, attributesSize = pivotValues.size(); i < recordsSize; ++i)
            if (attributesSize > i) T_LIKELY
                instances << create(records.at(i), pivotValues.at(i), false);
            else T_UNLIKELY
                instances << create(records.at(i), {}, false);

        touchIfTouching();

        return instances;
    }

    template<class Model, class Related, class PivotType>
    QVector<Related>
    BelongsToMany<Model, Related, PivotType>::createMany(
            QVector<QVector<AttributeItem>> &&records,
            const QVector<QVector<AttributeItem>> &pivotValues) const
    {
        QVector<Related> instances;
        const auto recordsSize = records.size();
        instances.reserve(recordsSize);

        for (int i = 0, attributesSize = pivotValues.size(); i < recordsSize; ++i)
            if (attributesSize > i) T_LIKELY
                instances << create(std::move(records[i]), pivotValues.at(i), false);
            else T_UNLIKELY
                instances << create(std::move(records[i]), {}, false);

        touchIfTouching();

        return instances;
    }

    // FEATURE move semantics, for attributes silverqx
    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::attach(
            const QVector<QVariant> &ids, const QVector<AttributeItem> &attributes,
            const bool touch) const
    {
        // FUTURE add the number of affected/attached models as return value silverqx
        if constexpr (std::is_same_v<PivotType, Pivot>)
            /* Here we will insert the attachment records into the pivot table. Once
               we have inserted the records, we will touch the relationships if
               necessary and the function will return. */
            newPivotStatement()->insert(Utils::Attribute::convertVectorsToMaps(
                                            formatAttachRecords(ids, attributes)));
        else
            attachUsingCustomClass(ids, attributes);

        if (touch)
            touchIfTouching();
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::attach(
            const QVector<std::reference_wrapper<Related>> &models,
            const QVector<AttributeItem> &attributes, const bool touch) const
    {
        attach(getRelatedIds(models), attributes, touch);
    }

    template<class Model, class Related, class PivotType>
    inline void BelongsToMany<Model, Related, PivotType>::attach(
            const QVariant &id, const QVector<AttributeItem> &attributes,
            const bool touch) const
    {
        attach(QVector {id}, attributes, touch);
    }

    template<class Model, class Related, class PivotType>
    inline void BelongsToMany<Model, Related, PivotType>::attach(
            const Related &model, const QVector<AttributeItem> &attributes,
            const bool touch) const
    {
        attach(QVector {model.getAttribute(this->m_relatedKey)}, attributes, touch);
    }

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::attach(
            const std::map<typename BaseModel<Related>::KeyType,
                           QVector<AttributeItem>> &idsWithAttributes,
            const bool touch) const
    {
        if constexpr (std::is_same_v<PivotType, Pivot>)
            /* Here we will insert the attachment records into the pivot table. Once
               we have inserted the records, we will touch the relationships if
               necessary and the function will return. */
            newPivotStatement()->insert(Utils::Attribute::convertVectorsToMaps(
                                            formatAttachRecords(idsWithAttributes)));
        else
            attachUsingCustomClass(idsWithAttributes);

        if (touch)
            touchIfTouching();
    }

    template<class Model, class Related, class PivotType>
    SyncChanges
    BelongsToMany<Model, Related, PivotType>::sync(
            const std::map<typename BaseModel<Related>::KeyType,
                           QVector<AttributeItem>> &idsWithAttributes,
            const bool detaching) const
    {
        const auto castKey = [this](const auto &id)
        {
            return this->castKey<typename Related::KeyType>(id);
        };
        SyncChanges changes;

        /* First we need to attach any of the associated models that are not currently
           in this joining table. We'll spin through the given IDs, checking to see
           if they exist in the vector of current ones, and if not we will insert. */
        auto current = getRelatedIds(getCurrentlyAttachedPivots());

        // Compute different keys, these keys will be detached
        auto ids = idsFromRecords(idsWithAttributes);

        ranges::sort(ids, {}, castKey);
        ranges::sort(current, {}, castKey);
        auto detach = ranges::views::set_difference(current, ids, {}, castKey, castKey)
                      | ranges::to<QVector<QVariant>>();

        /* Next, we will take the differences of the currents and given IDs and detach
           all of the entities that exist in the "current" vector but are not in the
           vector of the new IDs given to the method which will complete the sync. */
        if (detaching && !detach.isEmpty()) {
            this->detach(detach);

            changes["detached"] = std::move(detach);
        }

        /* Now we are finally ready to attach the new records. Note that we'll disable
           touching until after the entire operation is complete so we don't fire a
           ton of touch operations until we are totally done syncing the records. */
        changes.merge<typename Related::KeyType>(
                    attachNew(idsWithAttributes, current, false));

        /* Once we have finished attaching or detaching the records, we will see if we
           have done any attaching or detaching, and if we have we will touch these
           relationships if they are configured to touch on any database updates. */
        if (!changes["attached"].isEmpty() || !changes["updated"].isEmpty())
            touchIfTouching();

        return changes;
    }

    template<class Model, class Related, class PivotType>
    SyncChanges BelongsToMany<Model, Related, PivotType>::sync(
            const QVector<QVariant> &ids, const bool detaching) const
    {
        return sync(recordsFromIds(ids), detaching);
    }

    template<class Model, class Related, class PivotType>
    SyncChanges BelongsToMany<Model, Related, PivotType>::syncWithoutDetaching(
            const std::map<typename BaseModel<Related>::KeyType,
                           QVector<AttributeItem>> &idsWithAttributes) const
    {
        return sync(idsWithAttributes, false);
    }

    template<class Model, class Related, class PivotType>
    SyncChanges BelongsToMany<Model, Related, PivotType>::syncWithoutDetaching(
            const QVector<QVariant> &ids) const
    {
        return sync(ids, false);
    }

    template<class Model, class Related, class PivotType>
    int BelongsToMany<Model, Related, PivotType>::detach(
            const QVector<QVariant> &ids, const bool touch) const
    {
        return detach(false, ids, touch);
    }

    template<class Model, class Related, class PivotType>
    int BelongsToMany<Model, Related, PivotType>::detach(const bool touch) const
    {
        return detach(true, {}, touch);
    }

    template<class Model, class Related, class PivotType>
    inline int BelongsToMany<Model, Related, PivotType>::detach(
            const QVector<std::reference_wrapper<Related>> &models,
            const bool touch) const
    {
        return detach(getRelatedIds(models), touch);
    }

    template<class Model, class Related, class PivotType>
    int BelongsToMany<Model, Related, PivotType>::detach(
            const QVariant &id, const bool touch) const
    {
        return detach(QVector {id}, touch);
    }

    template<class Model, class Related, class PivotType>
    inline int BelongsToMany<Model, Related, PivotType>::detach(
            const Related &model, const bool touch) const
    {
        return detach(QVector {model.getAttribute(this->m_relatedKey)}, touch);
    }

    template<class Model, class Related, class PivotType>
    int BelongsToMany<Model, Related, PivotType>::updateExistingPivot(
            const QVariant &id, QVector<AttributeItem> attributes,
            const bool touch) const
    {
        if (!std::is_same_v<PivotType, Pivot>
//            && m_pivotWheres.isEmpty()
//            && m_pivotWhereIns.isEmpty()
//            && m_pivotWhereNulls.isEmpty()
        )
            return updateExistingPivotUsingCustomClass(id, attributes, touch);

        if (hasPivotColumn(updatedAt()))
            addTimestampsToAttachment(attributes, true);

        int updated;
        std::tie(updated, std::ignore) =
                newPivotStatementForId(id)->update(
                    Utils::Attribute::convertVectorToUpdateItem(
                        castAttributes(attributes)));

        /* It will not touch if attributes size is 0, because this function is called
           only when attributes are not empty. */
        if (touch)
            touchIfTouching();

        return updated;
    }

    template<class Model, class Related, class PivotType>
    int BelongsToMany<Model, Related, PivotType>::updateExistingPivot(
            const Related &model, const QVector<AttributeItem> &attributes,
            const bool touch) const
    {
        return updateExistingPivot(model.getAttribute(this->m_relatedKey), attributes,
                                   touch);
    }

    template<class Model, class Related, class PivotType>
    Related BelongsToMany<Model, Related, PivotType>::updateOrCreate(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values,
            const QVector<AttributeItem> &pivotValues,
            const bool touch) const
    {
        auto instance = this->where(attributes).first();

        if (!instance)
            return create(values, pivotValues, touch);

        instance->fill(values);

        instance->save({.touch = touch});

        // TODO update also pivot attributes silverqx

        return *instance;
    }

    template<class Model, class Related, class PivotType>
    QVector<QVariant>
    BelongsToMany<Model, Related, PivotType>::allRelatedIds() const
    {
        QVector<QVariant> ids;

        // Ownership of the QSharedPointer<QueryBuilder>
        auto query = newPivotQuery()->get({m_relatedPivotKey});

        while (query.next())
            ids << query.value(m_relatedPivotKey);

        return ids;
    }

    template<class Model, class Related, class PivotType>
    inline QString
    BelongsToMany<Model, Related, PivotType>::relationTypeName() const
    {
        return "BelongsToMany";
    }

    template<class Model, class Related, class PivotType>
    const BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::performJoin() const
    {
        return performJoin(*this->m_query);
    }

    template<class Model, class Related, class PivotType>
    const BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::performJoin(Builder<Related> &query) const
    {
        /* We need to join to the intermediate table on the related model's primary
           key column with the intermediate table's foreign key for the related
           model instance. Then we can set the "where" for the parent models. */
        query.join(m_table, getQualifiedRelatedKeyName(), EQ,
                   getQualifiedRelatedPivotKeyName());

        return *this;
    }

    template<class Model, class Related, class PivotType>
    const BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::addWhereConstraints() const
    {
        this->m_query->where(getQualifiedForeignPivotKeyName(), EQ,
                             this->m_parent.getAttribute(m_parentKey));

        return *this;
    }

    template<class Model, class Related, class PivotType>
    QVector<Column>
    BelongsToMany<Model, Related, PivotType>::shouldSelect(QVector<Column> columns) const
    {
        if (columns == QVector<Column> {ASTERISK})
            columns = QVector<Column> {QStringLiteral("%1.*")
                                       .arg(this->m_related->getTable())};

        // Obtain aliased pivot columns
        QVector<Column> mergedColumns;
        std::ranges::move(aliasedPivotColumns(), std::back_inserter(mergedColumns));

        // Merge aliasedPivotColumns into columns
        for (auto &&column : columns) {
            // Avoid duplicates, expressions are not checked
            if (std::holds_alternative<QString>(column))
                if (const auto &column_ = std::get<QString>(column);
                    /* Here can be aliasedPivotColumns_.contains(), but I use
                       result.contains(), which also prevents duplicates in columns
                       variable. */
                    mergedColumns.contains(column_)
                )
                    continue;

            mergedColumns << Column(std::move(column));
        }

        return mergedColumns;
    }

    template<class Model, class Related, class PivotType>
    QStringList BelongsToMany<Model, Related, PivotType>::aliasedPivotColumns() const
    {
        QStringList columns {
            // Default columns
            m_foreignPivotKey,
            m_relatedPivotKey,
        };

        columns += m_pivotColumns;

        columns.removeDuplicates();

        using namespace ranges;
        return columns | views::transform([this](const auto &column)
        {
            return QString("%1 as pivot_%2").arg(qualifyPivotColumn(column), column);
        })
                | ranges::to<QStringList>();
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::hydratePivotRelation(
            QVector<Related> &models) const
    {
        /* To hydrate the pivot relationship, we will just gather the pivot attributes
           and create a new Pivot model, which is basically a dynamic model that we
           will set the attributes, table, and connections on it so it will work. */
        for (auto &model : models)
            model.template setRelation<PivotType>(
                        m_accessor,
                        std::optional<PivotType>(newExistingPivot(
                                                     migratePivotAttributes(model))));
    }

    template<class Model, class Related, class PivotType>
    QVector<AttributeItem>
    BelongsToMany<Model, Related, PivotType>::migratePivotAttributes(Related &model) const
    {
        QVector<AttributeItem> values;

        for (const auto &attribute : model.getAttributes())
            /* To get the pivots attributes we will just take any of the attributes which
               begin with "pivot_" and add those to this vector, as well as unsetting
               them from the parent's models since they exist in a different table. */
            if (attribute.key.startsWith("pivot_")) {

                // Remove the 'pivot_' part from an attribute key
                values.append({attribute.key.mid(6), attribute.value});

                model.unsetAttribute(attribute);
            }

        return values;
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::attachUsingCustomClass(
            const QVector<QVariant> &ids,
            const QVector<AttributeItem> &attributes) const
    {
        for (const auto &record : formatAttachRecords(ids, attributes))
            newPivot(record).save();
    }

    template<class Model, class Related, class PivotType>
    QVector<QVector<AttributeItem>>
    BelongsToMany<Model, Related, PivotType>::formatAttachRecords(
            const QVector<QVariant> &ids,
            const QVector<AttributeItem> &attributes) const
    {
        QVector<QVector<AttributeItem>> records;

        const auto hasTimestamps = hasPivotColumn(createdAt()) ||
                                   hasPivotColumn(updatedAt());

        /* To create the attachment records, we will simply spin through the IDs given
           and create a new record to insert for each ID with extra attributes to be
           placed in other columns. */
        for (const auto &id : ids)
            records << formatAttachRecord(id, attributes, hasTimestamps);

        return records;
    }

    template<class Model, class Related, class PivotType>
    QVector<AttributeItem>
    BelongsToMany<Model, Related, PivotType>::formatAttachRecord(
            const QVariant &id, const QVector<AttributeItem> &attributes,
            const bool hasTimestamps) const
    {
        auto baseAttributes = baseAttachRecord(id, hasTimestamps);

        for (const auto &attribute : attributes) {
            // NOTE api different silverqx
            validateAttachAttribute(attribute, id);

            baseAttributes << attribute;
        }

        return baseAttributes;
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::attachUsingCustomClass(
            const std::map<typename BaseModel<Related>::KeyType,
                           QVector<AttributeItem>> &idsWithAttributes) const
    {
        for (const auto &record : formatAttachRecords(idsWithAttributes))
            newPivot(record).save();
    }

    template<class Model, class Related, class PivotType>
    QVector<QVector<AttributeItem>>
    BelongsToMany<Model, Related, PivotType>::formatAttachRecords(
            const std::map<typename BaseModel<Related>::KeyType,
                           QVector<AttributeItem>> &idsWithAttributes) const
    {
        QVector<QVector<AttributeItem>> records;

        const auto hasTimestamps = hasPivotColumn(createdAt()) ||
                                   hasPivotColumn(updatedAt());

        /* To create the attachment records, we will simply spin through the IDs given
           and create a new record to insert for each ID with extra attributes to be
           placed in other columns. */
        for (const auto &[id, attributes] : idsWithAttributes)
            records << formatAttachRecord(id, attributes, hasTimestamps);

        return records;
    }

    template<class Model, class Related, class PivotType>
    QVector<AttributeItem>
    BelongsToMany<Model, Related, PivotType>::baseAttachRecord(
            const QVariant &id, const bool timed) const
    {
        QVector<AttributeItem> record;

        record.append({m_relatedPivotKey, id});
        record.append({m_foreignPivotKey, this->m_parent[m_parentKey]});

        /* If the record needs to have creation and update timestamps, we will make
           them by calling the parent model's "freshTimestamp" method, which will
           provide us with a fresh timestamp in this model's preferred format. */
        if (timed)
            addTimestampsToAttachment(record);

        // FEATURE pivot, withPivotValues silverqx
//        for (auto &[column, value] as m_pivotValues)
//            record.append(column, value);

        return record;
    }

    template<class Model, class Related, class PivotType>
    QVector<AttributeItem> &
    BelongsToMany<Model, Related, PivotType>::addTimestampsToAttachment(
            QVector<AttributeItem> &record, const bool exists) const
    {
        // QDateTime
        QVariant fresh = this->m_parent.freshTimestamp();

        /* If a custom pivot is used, then 'fresh' will be QString, formatted
           datetime as defined in the custom pivot class. */
        if constexpr (!std::is_same_v<PivotType, Pivot>)
            fresh = fresh.toDateTime().toString(PivotType().getDateFormat());

        if (!exists && hasPivotColumn(createdAt()))
            record.append({createdAt(), fresh});

        if (hasPivotColumn(updatedAt()))
            record.append({updatedAt(), fresh});

        return record;
    }

    template<class Model, class Related, class PivotType>
    QVector<PivotType>
    BelongsToMany<Model, Related, PivotType>::getCurrentlyAttachedPivots() const
    {
        auto query = newPivotQuery()->get();

        QVector<PivotType> pivots;

        while (query.next())
            pivots << std::move(PivotType::fromRawAttributes(
                          this->m_parent, attributesFromRecord(query.record()),
                          this->m_table, true)
                      .setPivotKeys(m_foreignPivotKey, m_relatedPivotKey));

        return pivots;
    }

    template<class Model, class Related, class PivotType>
    std::optional<PivotType>
    BelongsToMany<Model, Related, PivotType>::getAttachedPivot(const QVariant &id) const
    {
        auto query = newPivotStatementForId(id)->first();

        return PivotType::fromRawAttributes(this->m_parent,
                                            attributesFromRecord(query.record()),
                                            this->m_table, true)
                .setPivotKeys(m_foreignPivotKey, m_relatedPivotKey);
    }

    template<class Model, class Related, class PivotType>
    QVector<AttributeItem>
    BelongsToMany<Model, Related, PivotType>::attributesFromRecord(
            const QSqlRecord &record) const
    {
        QVector<AttributeItem> attributes;

        const auto recordsCount = record.count();
        attributes.reserve(recordsCount);

        for (auto i = 0; i < recordsCount; ++i)
            attributes.append({record.fieldName(i), record.value(i)});

        return attributes;
    }

    template<class Model, class Related, class PivotType>
    template<typename T>
    T BelongsToMany<Model, Related, PivotType>::castKey(const QVariant &key) const
    {
        return key.value<T>();
    }

    template<class Model, class Related, class PivotType>
    int BelongsToMany<Model, Related, PivotType>::updateExistingPivotUsingCustomClass(
            const QVariant &id, const QVector<AttributeItem> &attributes,
            const bool touch) const
    {
        auto pivot = getAttachedPivot(id);

        const auto updated = pivot ? pivot->fill(attributes).isDirty() : false;

        if (updated)
            pivot->save();

        /* It will not touch if attributes size is 0, because this function is called
           only when attributes are not empty. */
        if (touch)
            touchIfTouching();

        return static_cast<int>(updated);
    }

    template<class Model, class Related, class PivotType>
    SyncChanges
    BelongsToMany<Model, Related, PivotType>::attachNew(
                const std::map<typename BaseModel<Related>::KeyType,
                               QVector<AttributeItem>> &records,
                const QVector<QVariant> &current, const bool touch) const
    {
        SyncChanges changes;

        for (const auto &[id, attributes] : records) {
            /* If the ID is not in the list of existing pivot IDs, we will insert
               a new pivot record, otherwise, we will just update this existing record
               on this joining table, so that the developers will easily update these
               records pain free. */
            if (!current.contains(id)) {
                attach(id, attributes, touch);

                changes["attached"] << id;
            }

            /* If the pivot record already exists, we'll try to update the attributes
               that were given to the method. If the model is actually updated, we will
               add it to the list of updated pivot records, so we return them back
               out to the consumer. */
            else if (!attributes.isEmpty()
                     && updateExistingPivot(id, attributes, touch)
            )
                changes["updated"] << id;
        }

        return changes;
    }

    template<class Model, class Related, class PivotType>
    std::map<typename BelongsToMany<Model, Related, PivotType>
            ::template BaseModel<Related>::KeyType, QVector<AttributeItem>>
    BelongsToMany<Model, Related, PivotType>::recordsFromIds(
                const QVector<QVariant> &ids) const
    {
        // FEATURE dilemma primarykey, when I solve this dilema, then add using for ModelKeyType and RelatedKeyType silverqx
        std::map<typename BaseModel<Related>::KeyType, QVector<AttributeItem>> records;

        for (const auto &id : ids)
            records.emplace(castKey<typename Related::KeyType>(id),
                            QVector<AttributeItem>());

        return records;
    }

    template<class Model, class Related, class PivotType>
    QVector<QVariant>
    BelongsToMany<Model, Related, PivotType>::idsFromRecords(
                const std::map<typename BaseModel<Related>::KeyType,
                               QVector<AttributeItem>> &idsWithAttributes) const
    {
        QVector<QVariant> ids;

        for (const auto &record : idsWithAttributes)
            ids << record.first;

        return ids;
    }

    template<class Model, class Related, class PivotType>
    QVector<AttributeItem> &
    BelongsToMany<Model, Related, PivotType>::castAttributes(
                QVector<AttributeItem> &attributes) const
    requires OurPivot<PivotType>
    {
        static_assert (std::is_same_v<PivotType, Pivot>,
                "Bad castAttribute overload selected, PivotType != Pivot.");

        return attributes;
    }

    template<class Model, class Related, class PivotType>
    QVector<AttributeItem>
    BelongsToMany<Model, Related, PivotType>::castAttributes(
                const QVector<AttributeItem> &attributes) const
    requires CustomPivot<PivotType>
    {
        static_assert (!std::is_same_v<PivotType, Pivot>,
                "Bad castAttribute overload selected, should be overload "
                "for the custom pivot.");

        return newPivot().fill(attributes).getAttributes();
    }

    template<class Model, class Related, class PivotType>
    bool BelongsToMany<Model, Related, PivotType>::touchingParent() const
    {
        return this->m_related->touches(guessInverseRelation());
    }

    template<class Model, class Related, class PivotType>
    QString BelongsToMany<Model, Related, PivotType>::guessInverseRelation() const
    {
        // FEATURE relations, add parent touches (eg parentTouchesName) to the Model::belongsToMany factory method silverqx
        auto relation = Utils::Type::classPureBasename<Model>();

        relation[0] = relation[0].toLower();

        return QStringLiteral("%1s").arg(relation);
    }

    template<class Model, class Related, class PivotType>
    int BelongsToMany<Model, Related, PivotType>::detach(
                const bool detachAll, const QVector<QVariant> &ids,
                const bool touch) const
    {
        int affected = 0;

        if (!std::is_same_v<PivotType, Pivot>
            && !detachAll && !ids.isEmpty()
//            && m_pivotWheres.isEmpty()
//            && m_pivotWhereIns.isEmpty()
//            && m_pivotWhereNulls.isEmpty()
        )
            affected = detachUsingCustomClass(ids);

        else {
            // Ownership of the QSharedPointer<QueryBuilder>
            auto query = newPivotQuery();

            /* If associated IDs were passed to the method we will only delete those
               associations, otherwise all of the association ties will be broken.
               We'll return the numbers of affected rows when we do the deletes. */
            if (!detachAll) {
                // Nothing to delete/detach
                if (ids.isEmpty())
                    return 0;

                query->whereIn(m_relatedPivotKey, ids);
            }

            /* Once we have all of the conditions set on the statement, we are ready
               to run the delete on the pivot table. Then, if the touch parameter
               is true, we will go ahead and touch all related models to sync. */
            std::tie(affected, std::ignore) = query->remove();
        }

        if (touch)
            touchIfTouching();

        return affected;
    }

    template<class Model, class Related, class PivotType>
    int BelongsToMany<Model, Related, PivotType>::detachUsingCustomClass(
            const QVector<QVariant> &ids) const
    {
        int affected = 0;

        for (const auto &id : ids)
            affected += newPivot({
                {m_foreignPivotKey, this->m_parent[this->m_parentKey]},
                {m_relatedPivotKey, id},
            }, true).remove();

        return affected;
    }

    template<class Model, class Related, class PivotType>
    QVector<QVariant>
    BelongsToMany<Model, Related, PivotType>::getRelatedIds(
            const QVector<std::reference_wrapper<Related>> &models) const
    {
        QVector<QVariant> ids;
        ids.reserve(models.size());

        for (const auto &model : models)
            ids << model.get().getAttribute(this->m_relatedKey);

        return ids;
    }

    template<class Model, class Related, class PivotType>
    QVector<QVariant>
    BelongsToMany<Model, Related, PivotType>::getRelatedIds(
                const QVector<PivotType> &pivots) const
    {
        QVector<QVariant> ids;
        ids.reserve(pivots.size());

        for (const auto &pivot : pivots)
            ids << pivot[m_relatedPivotKey];

        return ids;
    }

    template<class Model, class Related, class PivotType>
    QVector<QVariant>
    BelongsToMany<Model, Related, PivotType>::getRelatedIds(
                QVector<PivotType> &&pivots) const
    {
        QVector<QVariant> ids;
        ids.reserve(pivots.size());

        for (auto &pivot : pivots)
            ids << std::move(pivot[m_relatedPivotKey]);

        return ids;
    }

    template<class Model, class Related, class PivotType>
    std::unique_ptr<Builder<Related>>
    BelongsToMany<Model, Related, PivotType>::getRelationExistenceQuery(
            std::unique_ptr<Builder<Related>> &&query,
            const Builder<Model> &parentQuery,
            const QVector<Column> &columns) const
    {
        // CUR finish self query silverqx
//        if (query->getQuery()->from == parentQuery.getQuery()->from)
//            return this->getRelationExistenceQueryForSelfRelation(query, parentQuery,
//                                                                  columns);

        performJoin(*query);

        return Relation<Model, Related>::getRelationExistenceQuery(
                    std::move(query), parentQuery, columns);
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::validateAttachAttribute(
            const AttributeItem &attribute, const QVariant &id) const
    {
        // Don't overwrite ID keys, throw domain exception
        if (attribute.key == m_foreignPivotKey)
            // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
            throwOverwritingKeyError/*<Model::KeyType>*/(attribute.key,
                                                         this->m_parent[m_parentKey],
                                                         attribute.value);
        else if (attribute.key == m_relatedPivotKey)
            throwOverwritingKeyError/*<Related::KeyType>*/(attribute.key, id,
                                                           attribute.value);
    }

    template<class Model, class Related, class PivotType>
    template<typename KeyType>
    void BelongsToMany<Model, Related, PivotType>::throwOverwritingKeyError(
            const QString &key, const QVariant &original,
            const QVariant &overwrite) const
    {
        static const auto overwriteMessage =
                QStringLiteral("You can not overwrite '%1' ID key; "
                               "original value : %2, your value : %3.");

        throw Orm::Exceptions::DomainError(
                    overwriteMessage.arg(qualifyPivotColumn(key),
                                         QString::number(original.value<KeyType>()),
                                         QString::number(overwrite.value<KeyType>())));
    }

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // BELONGSTOMANY_H
