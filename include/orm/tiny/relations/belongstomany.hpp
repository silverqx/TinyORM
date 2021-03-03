#ifndef BELONGSTOMANY_H
#define BELONGSTOMANY_H

#include <QDateTime>

#include <range/v3/algorithm/copy_if.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include "orm/ormdomainerror.hpp"
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

    template<class Model, class Related, class PivotType = Pivot>
    class BelongsToMany :
            public Relation<Model, Related>,
            public ManyRelation,
            public PivotRelation
    {
    protected:
        BelongsToMany(std::unique_ptr<Related> &&related, Model &parent,
                      const QString &table = "", const QString &foreignPivotKey = "",
                      const QString &relatedPivotKey = "", const QString &parentKey = "",
                      const QString &relatedKey = "", const QString &relationName = "");

    public:
        /*! Instantiate and initialize a new BelongsToMany instance. */
        static std::unique_ptr<Relation<Model, Related>>
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
        QVector<Related> get(const QStringList &columns = {"*"}) const override;

        /* Getters / Setters */
        /*! Qualify the given column name by the pivot table. */
        QString qualifyPivotColumn(const QString &column) const;
        /*! Get the fully qualified foreign key for the relation. */
        QString getQualifiedForeignPivotKeyName() const;
        /*! Get the fully qualified "related key" for the relation. */
        QString getQualifiedRelatedPivotKeyName() const;
        /*! Get the fully qualified parent key name for the relation. */
        QString getQualifiedParentKeyName() const;
        /*! Get the fully qualified related key name for the relation. */
        QString getQualifiedRelatedKeyName() const;

        /*! Get the intermediate table for the relationship. */
        inline const QString &getTable() const
        { return m_table; }
        /*! Get the foreign key for the relation. */
        inline const QString &getForeignPivotKeyName() const
        { return m_foreignPivotKey; }
        /*! Get the "related key" for the relation. */
        inline const QString &getRelatedPivotKeyName() const
        { return m_relatedPivotKey; }
        /*! Get the parent key for the relationship. */
        inline const QString &getParentKeyName() const
        { return m_parentKey; }
        /*! Get the related key for the relationship. */
        inline const QString &getRelatedKeyName() const
        { return m_relatedKey; }
        /*! Get the relationship name for the relationship. */
        inline const QString &getRelationName() const
        { return m_relationName; }

        /*! Get the name of the pivot accessor for this relationship. */
        inline const QString &getPivotAccessor() const
        { return m_accessor; }
        /*! Specify the custom pivot accessor to use for the relationship. */
        BelongsToMany &as(const QString &accessor);

        /*! Get the pivot columns for this relationship. */
        inline const QStringList &getPivotColumns() const
        { return m_pivotColumns; }

        /*! Determine if the 'pivot' model uses timestamps. */
        inline bool usesTimestamps() const
        { return m_withTimestamps; }
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
        inline BelongsToMany &withPivot(const QString &column)
        { return withPivot(QStringList {column}); }

        /*! Determine whether the given column is defined as a pivot column. */
        inline bool hasPivotColumn(const QString &column) const
        { return m_pivotColumns.contains(column); }

        /*! Create a new existing pivot model instance. */
        PivotType newExistingPivot(const QVector<AttributeItem> &attributes = {}) const;
        /*! Create a new pivot model instance. */
        PivotType newPivot(const QVector<AttributeItem> &attributes = {},
                           bool exists = false) const;

        /*! Create a new query builder for the pivot table. */
        QSharedPointer<QueryBuilder> newPivotQuery() const;
        /*! Get a new plain query builder for the pivot table. */
        QSharedPointer<QueryBuilder> newPivotStatement() const;

        /* Inserting & Updating relationship */
        /*! Attach models to the parent. */
        void attach(const QVector<QVariant> &ids,
                    const QVector<AttributeItem> &attributes = {},
                    bool touch = true) const override;
        /*! Attach models to the parent. */
        void attach(const QVector<std::reference_wrapper<Related>> &models,
                    const QVector<AttributeItem> &attributes = {},
                    bool touch = true) const override;
        /*! Attach a model to the parent. */
        inline void
        attach(const QVariant &id, const QVector<AttributeItem> &attributes = {},
               bool touch = true) const override;
        /*! Attach a model to the parent. */
        inline void
        attach(const Related &model, const QVector<AttributeItem> &attributes = {},
               bool touch = true) const override;
        /*! Attach models to the parent. */
        void
        attach(const std::map<typename BaseModel<Related>::KeyType,
                              QVector<AttributeItem>> &idsWithAttributes,
               bool touch = true) const override;

        /*! Detach models from the relationship. */
        int detach(const QVector<QVariant> &ids, bool touch = true) const override;
        /*! Detach models from the relationship. */
        int detach(const QVector<std::reference_wrapper<Related>> &models,
                   bool touch = true) const override;
        /*! Detach model from the relationship. */
        inline int detach(const QVariant &id, bool touch = true) const override;
        /*! Detach model from the relationship. */
        inline int detach(const Related &model, bool touch = true) const override;

        /*! Get all of the IDs for the related models. */
        QVector<QVariant> allRelatedIds() const;

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
        QStringList shouldSelect(QStringList columns = {"*"}) const;
        /*! Get the pivot columns for the relation, "pivot_" is prefixed
            to each column for easy removal later. */
        QStringList aliasedPivotColumns() const;

        /*! Hydrate the pivot table relationship on the models. */
        void hydratePivotRelation(QVector<Related> &models) const;
        /*! Get the pivot attributes from a model. */
        QVector<AttributeItem> migratePivotAttributes(Related &model) const;

        /* Inserting & Updating relationship */
        /*! Attach a model to the parent using a custom class. */
        void attachUsingCustomClass(const QVector<QVariant> &ids,
                                    const QVector<AttributeItem> &attributes) const;
        /*! Create an array of records to insert into the pivot table. */
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
        /*! Create an array of records to insert into the pivot table. */
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

        /*! Determine if we should touch the parent on sync. */
        bool touchingParent() const;
        /*! Attempt to guess the name of the inverse of the relation. */
        QString guessInverseRelation() const;

        /*! Detach models from the relationship using a custom class. */
        int detachUsingCustomClass(const QVector<QVariant> &ids) const;

        /*! Obtain ids from the Related models. */
        QVector<QVariant>
        getRelatedIds(const QVector<std::reference_wrapper<Related>> &models) const;

        /*! The intermediate table for the relation. */
        QString m_table;
        /*! The foreign key of the parent model. */
        QString m_foreignPivotKey;
        /*! The associated key of the relation. */
        QString m_relatedPivotKey;
        /*! The key name of the parent model. */
        QString m_parentKey;
        /*! The key name of the related model. */
        QString m_relatedKey;
        /*! The name of the relationship. */
        QString m_relationName;

        /*! The name of the accessor to use for the "pivot" relationship. */
        QString m_accessor = QStringLiteral("pivot");
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
        : Relation<Model, Related>(std::move(related), parent)
        , m_table(table)
        , m_foreignPivotKey(foreignPivotKey)
        , m_relatedPivotKey(relatedPivotKey)
        , m_parentKey(parentKey)
        , m_relatedKey(relatedKey)
        , m_relationName(relationName)
    {}

    template<class Model, class Related, class PivotType>
    std::unique_ptr<Relation<Model, Related>>
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
    BelongsToMany<Model, Related, PivotType>::get(const QStringList &columns) const
    {
        /* First we'll add the proper select columns onto the query so it is run with
           the proper columns. Then, we will get the results and hydrate out pivot
           models with the result of those columns as a separate model relation. */
        // TODO scopes silverqx
//        $builder = $this->query->applyScopes();

        auto l_columns = this->m_query->getQuery().getColumns().isEmpty()
                         ? columns
                         : QStringList();

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
        if (column.contains(QChar('.')))
            return column;

        return m_table + QChar('.') + column;
    }

    template<class Model, class Related, class PivotType>
    QString
    BelongsToMany<Model, Related, PivotType>::getQualifiedForeignPivotKeyName() const
    {
        return qualifyPivotColumn(m_foreignPivotKey);
    }

    template<class Model, class Related, class PivotType>
    QString
    BelongsToMany<Model, Related, PivotType>::getQualifiedRelatedPivotKeyName() const
    {
        return qualifyPivotColumn(m_relatedPivotKey);
    }

    template<class Model, class Related, class PivotType>
    QString BelongsToMany<Model, Related, PivotType>::getQualifiedParentKeyName() const
    {
        return this->m_parent.qualifyColumn(m_parentKey);
    }

    template<class Model, class Related, class PivotType>
    QString BelongsToMany<Model, Related, PivotType>::getQualifiedRelatedKeyName() const
    {
        return this->m_related->qualifyColumn(m_relatedKey);
    }

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::as(const QString &accessor)
    {
        m_accessor = accessor;

        return *this;
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

    // TODO perf for all similar methods make rvalue variants, or what if all this methods would be rvalue only, so if it is possible then move and if not then copy silverqx
    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::withPivot(const QStringList &columns)
    {
        ranges::copy_if(columns, ranges::back_inserter(m_pivotColumns),
                        [this](const auto &column)
        {
            return !m_pivotColumns.contains(column);
        });

        return *this;
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

        // TODO relations, add support for BelongsToMany::where/whereIn/whereNull silverqx
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
    void BelongsToMany<Model, Related, PivotType>::attach(
            const QVector<QVariant> &ids, const QVector<AttributeItem> &attributes,
            const bool touch) const
    {
        // TODO future add the number of affected/attached models as return value silverqx
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
    void BelongsToMany<Model, Related, PivotType>::attach(
            const QVariant &id, const QVector<AttributeItem> &attributes,
            const bool touch) const
    {
        attach(QVector {id}, attributes, touch);
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::attach(
            const Related &model, const QVector<AttributeItem> &attributes,
            const bool touch) const
    {
        attach(QVector {model.getAttribute(m_relatedKey)}, attributes, touch);
    }

    // TODO dilemma primarykey, Model::KeyType vs QVariant silverqx
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
    int BelongsToMany<Model, Related, PivotType>::detach(
            const QVector<QVariant> &ids, const bool touch) const
    {
        int affected = 0;

        if (!std::is_same_v<PivotType, Pivot>
            && !ids.isEmpty()
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
            // Nothing to delete/detach
            if (ids.isEmpty())
                return 0;

            query->whereIn(m_relatedPivotKey, ids);

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
    int BelongsToMany<Model, Related, PivotType>::detach(
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
    int BelongsToMany<Model, Related, PivotType>::detach(
            const Related &model, const bool touch) const
    {
        return detach(QVector {model.getAttribute(m_relatedKey)}, touch);
    }

    template<class Model, class Related, class PivotType>
    QVector<QVariant>
    BelongsToMany<Model, Related, PivotType>::allRelatedIds() const
    {
        QVector<QVariant> ids;

        // Ownership of the QSharedPointer<QueryBuilder>
        auto [ok, query] = newPivotQuery()->get({m_relatedPivotKey});

        while (query.next())
            ids << query.value(m_relatedPivotKey);

        return ids;
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
        query.join(m_table, getQualifiedRelatedKeyName(), QStringLiteral("="),
                   getQualifiedRelatedPivotKeyName());

        return *this;
    }

    template<class Model, class Related, class PivotType>
    const BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::addWhereConstraints() const
    {
        this->m_query->where(getQualifiedForeignPivotKeyName(), QStringLiteral("="),
                             this->m_parent.getAttribute(m_parentKey));

        return *this;
    }

    template<class Model, class Related, class PivotType>
    QStringList
    BelongsToMany<Model, Related, PivotType>::shouldSelect(QStringList columns) const
    {
        if (columns == QStringList {"*"})
            columns = QStringList {this->m_related->getTable() + QStringLiteral(".*")};

        columns += aliasedPivotColumns();

        columns.removeDuplicates();

        return columns;
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
            return QStringLiteral("%1 as pivot_%2").arg(qualifyPivotColumn(column),
                                                        column);
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

        // TODO pivot, withPivotValues silverqx
//        for (auto &[column, value] as m_pivotValues)
//            record.append(column, value);

        return record;
    }

    template<class Model, class Related, class PivotType>
    QVector<AttributeItem> &
    BelongsToMany<Model, Related, PivotType>::addTimestampsToAttachment(
            QVector<AttributeItem> &record, const bool exists) const
    {
        QVariant fresh = this->m_parent.freshTimestamp();

        // If custom pivot is used
        if constexpr (!std::is_same_v<PivotType, Pivot>)
            fresh = fresh.toDateTime().toString(PivotType().getDateFormat());

        if (!exists && hasPivotColumn(createdAt()))
            record.append({createdAt(), fresh});

        if (hasPivotColumn(updatedAt()))
            record.append({updatedAt(), fresh});

        return record;
    }

    template<class Model, class Related, class PivotType>
    bool BelongsToMany<Model, Related, PivotType>::touchingParent() const
    {
        return this->m_related->touches(guessInverseRelation());
    }

    template<class Model, class Related, class PivotType>
    QString BelongsToMany<Model, Related, PivotType>::guessInverseRelation() const
    {
        // TODO relations, add parent touches (eg parentTouchesName) to the BaseModel::belongsToMany factory method silverqx
        auto relation = Utils::Type::classPureBasename<Model>();

        relation[0] = relation[0].toLower();

        return relation + QChar('s');
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
            ids << model.get().getAttribute(m_relatedKey);

        return ids;
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::validateAttachAttribute(
            const AttributeItem &attribute, const QVariant &id) const
    {
        // Don't overwrite ID keys, throw domain exception
        if (attribute.key == m_foreignPivotKey)
            // TODO dilemma primarykey, Model::KeyType vs QVariant silverqx
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

        throw OrmDomainError(overwriteMessage.arg(
                                 qualifyPivotColumn(key),
                                 QString::number(original.value<KeyType>()),
                                 QString::number(overwrite.value<KeyType>())));
    }

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // BELONGSTOMANY_H
