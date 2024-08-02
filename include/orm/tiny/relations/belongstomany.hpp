#pragma once
#ifndef ORM_TINY_RELATIONS_BELONGSTOMANY_HPP
#define ORM_TINY_RELATIONS_BELONGSTOMANY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/likely.hpp"
#include "orm/tiny/exceptions/modelnotfounderror.hpp"
#include "orm/tiny/relations/concerns/interactswithpivottable.hpp"
#include "orm/tiny/relations/relation.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{

    /*! Belongs to many relation. */
    template<class Model, class Related, class PivotType = Pivot>
    class BelongsToMany :
            public IsManyRelation,
            public Relation<Model, Related>,
            public IsPivotRelation,
            public Concerns::InteractsWithPivotTable<Model, Related, PivotType>
    {
        /*! To access a private data members m_xyz and a few other methods. */
        friend Concerns::InteractsWithPivotTable<Model, Related, PivotType>;

        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;
        /*! Alias for query utils. */
        using QueryUtils = Orm::Utils::Query;
        /*! Alias for the type utils. */
        using TypeUtils = Orm::Utils::Type;

    protected:
        /*! Protected constructor. */
        BelongsToMany(std::unique_ptr<Related> &&related, Model &parent,
                      const QString &table = "", const QString &foreignPivotKey = "", // NOLINT(modernize-pass-by-value)
                      const QString &relatedPivotKey = "", const QString &parentKey = "", // NOLINT(modernize-pass-by-value)
                      const QString &relatedKey = "", const QString &relationName = ""); // NOLINT(modernize-pass-by-value)

        /*! BelongsToMany's copy constructor (used by BelongsToMany::clone()). */
        BelongsToMany(const BelongsToMany &) = default;

    public:
        /*! Parent Model type. */
        using ModelType = Model;
        /*! Related type. */
        using RelatedType = Related;
        /*! Pivot type. */
        using PivotTypeType = PivotType;

        /*! Virtual destructor. */
        ~BelongsToMany() override = default;

        /*! BelongsToMany's move constructor. */
        BelongsToMany(BelongsToMany &&) = delete;

        /*! BelongsToMany's copy assignment operator. */
        BelongsToMany &operator=(const BelongsToMany &) = delete;
        /*! BelongsToMany's move assignment operator. */
        BelongsToMany &operator=(BelongsToMany &&) = delete;

        /*! Instantiate and initialize a new BelongsToMany instance. */
        static std::unique_ptr<BelongsToMany<Model, Related, PivotType>>
        instance(std::unique_ptr<Related> &&related, Model &parent,
                 const QString &table = "", const QString &foreignPivotKey = "",
                 const QString &relatedPivotKey = "", const QString &parentKey = "",
                 const QString &relatedKey = "", const QString &relation = "");

        /* Relation related operations */
        /*! Set the base constraints on the relation query. */
        void addConstraints() const override;

        /*! Set the constraints for an eager load of the relation. */
        inline void addEagerConstraints(const ModelsCollection<Model> &models) override;
        /*! Set the constraints for an eager load of the relation. */
        inline void addEagerConstraints(const ModelsCollection<Model *> &models) override;

        /*! Initialize the relation on a set of models. */
        inline ModelsCollection<Model> &
        initRelation(ModelsCollection<Model> &models,
                     const QString &relation) const override;
        /*! Initialize the relation on a set of models. */
        inline ModelsCollection<Model *> &
        initRelation(ModelsCollection<Model *> &models,
                     const QString &relation) const override;

        /*! Match the eagerly loaded results to their parents. */
        inline void
        match(ModelsCollection<Model> &models, ModelsCollection<Related> &&results,
              const QString &relation) const override;
        /*! Match the eagerly loaded results to their parents. */
        inline void
        match(ModelsCollection<Model *> &models, ModelsCollection<Related> &&results,
              const QString &relation) const override;

        /*! Get the results of the relationship. */
        std::variant<ModelsCollection<Related>, std::optional<Related>>
        getResults() const override;
        /*! Execute the query as a "select" statement. */
        ModelsCollection<Related>
        get(const QList<Column> &columns = {ASTERISK}) const override; // NOLINT(google-default-arguments)

        /* Getters / Setters */
        /*! Get the fully qualified foreign key for the relation. */
        inline QString getQualifiedForeignPivotKeyName() const;
        /*! Get the fully qualified "related key" for the relation. */
        inline QString getQualifiedRelatedPivotKeyName() const;
        /*! Get the fully qualified parent key name for the relation. */
        inline QString getQualifiedParentKeyName() const override;
        /*! Get the fully qualified related key name for the relation. */
        inline QString getQualifiedRelatedKeyName() const;
        /*! Get the key for comparing against the parent key in "has" query. */
        inline QString getExistenceCompareKey() const override;

        /*! Get the intermediate table for the relationship. */
        inline const QString &getTable() const noexcept;
        /*! Get the foreign key for the relation. */
        inline const QString &getForeignPivotKeyName() const noexcept;
        /*! Get the "related key" for the relation. */
        inline const QString &getRelatedPivotKeyName() const noexcept;
        /*! Get the parent key for the relationship. */
        inline const QString &getParentKeyName() const noexcept;
        /*! Get the relationship name for the relationship. */
        inline const QString &getRelationName() const noexcept;

        /*! Get the name of the pivot accessor for this relationship. */
        inline const QString &getPivotAccessor() const noexcept;
        /*! Specify the custom pivot accessor to use for the relationship. */
        inline BelongsToMany &as(const QString &accessor);
        /*! Specify the custom pivot accessor to use for the relationship. */
        inline BelongsToMany &as(QString &&accessor);

        /*! Get the pivot columns for this relationship. */
        inline const QStringList &getPivotColumns() const noexcept;

        /* Timestamps */
        /*! Determine if the 'pivot' model uses timestamps. */
        inline bool usesTimestamps() const noexcept;
        /*! Specify that the pivot table has creation and update timestamps. */
        BelongsToMany &withTimestamps(const QString &createdAt = Constants::CREATED_AT,
                                      const QString &updatedAt = Constants::UPDATED_AT);
        /*! Get the name of the "created at" column. */
        const QString &createdAt() const;
        /*! Get the name of the "updated at" column. */
        const QString &updatedAt() const;
        /*! If we're touching the parent model, touch. */
        void touchIfTouching() const;
        /*! Touch all of the related models for the relationship.
            E.g.: Touch all roles associated with this user. */
        void touch() const override;

        /* TinyBuilder proxy methods that need modifications */
        /*! Find a model by its primary key. */
        std::optional<Related>
        find(const QVariant &id, // NOLINT(google-default-arguments)
             const QList<Column> &columns = {ASTERISK}) const override;
        /*! Find a model by its primary key or return fresh model instance. */
        Related
        findOrNew(const QVariant &id, // NOLINT(google-default-arguments)
                  const QList<Column> &columns = {ASTERISK}) const override;
        /*! Find a model by its primary key or throw an exception. */
        Related
        findOrFail(const QVariant &id, // NOLINT(google-default-arguments)
                   const QList<Column> &columns = {ASTERISK}) const override;
        /*! Find multiple models by their primary keys. */
        ModelsCollection<Related>
        findMany(const QList<QVariant> &ids,
                 const QList<Column> &columns = {ASTERISK}) const;

        /*! Execute a query for a single record by ID or call a callback. */
        std::optional<Related>
        findOr(const QVariant &id, const QList<Column> &columns,
               const std::function<void()> &callback) const override;
        /*! Execute a query for a single record by ID or call a callback. */
        inline std::optional<Related>
        findOr(const QVariant &id, const std::function<void()> &callback) const override;

        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        std::pair<std::optional<Related>, R>
        findOr(const QVariant &id, const QList<Column> &columns,
               const std::function<R()> &callback) const;
        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        inline std::pair<std::optional<Related>, R>
        findOr(const QVariant &id, const std::function<R()> &callback) const;

        /*! Execute the query and get the first result. */
        std::optional<Related>
        first(const QList<Column> &columns = {ASTERISK}) const override; // NOLINT(google-default-arguments)
        /*! Get the first record matching the attributes or instantiate it. */
        Related firstOrNew(const QList<WhereItem> &attributes = {}, // NOLINT(google-default-arguments)
                           const QList<AttributeItem> &values = {}) const override;
        /*! Get the first record matching the attributes or create it. */
        Related firstOrCreate(const QList<WhereItem> &attributes = {},
                              const QList<AttributeItem> &values = {},
                              const QList<AttributeItem> &pivotValues = {},
                              bool touch = true) const;
        /*! Execute the query and get the first result or throw an exception. */
        Related firstOrFail(const QList<Column> &columns = {ASTERISK}) const override; // NOLINT(google-default-arguments)

        /*! Execute the query and get the first result or call a callback. */
        std::optional<Related>
        firstOr(const QList<Column> &columns, // NOLINT(google-default-arguments)
                const std::function<void()> &callback = nullptr) const override;
        /*! Execute the query and get the first result or call a callback. */
        inline std::optional<Related>
        firstOr(const std::function<void()> &callback = nullptr) const override; // NOLINT(google-default-arguments)

        /*! Execute the query and get the first result or call a callback. */
        template<typename R>
        std::pair<std::optional<Related>, R>
        firstOr(const QList<Column> &columns, const std::function<R()> &callback) const;
        /*! Execute the query and get the first result or call a callback. */
        template<typename R>
        inline std::pair<std::optional<Related>, R>
        firstOr(const std::function<R()> &callback) const;

        /*! Add a basic where clause to the query, and return the first result. */
        std::optional<Related>
        firstWhere(const Column &column, const QString &comparison, // NOLINT(google-default-arguments)
                   const QVariant &value,
                   const QString &condition = AND) const override;
        /*! Add a basic where clause to the query, and return the first result. */
        inline std::optional<Related>
        firstWhereEq(const Column &column, const QVariant &value, // NOLINT(google-default-arguments)
                     const QString &condition = AND) const override;

        /* Builds Queries */
        /*! Chunk the results of the query. */
        bool chunk(qint64 count,
                   const std::function<
                       bool(ModelsCollection<Related> &&models, qint64 page)> &callback
        ) const override;
        /*! Execute a callback over each item while chunking. */
        bool each(const std::function<bool(Related &&model, qint64 index)> &callback, // NOLINT(google-default-arguments)
                  qint64 count = 1000) const override;

        /*! Run a map over each item while chunking. */
        ModelsCollection<Related>
        chunkMap(const std::function<Related(Related &&model)> &callback, // NOLINT(google-default-arguments)
                 qint64 count = 1000) const override;
        /*! Run a map over each item while chunking. */
        template<typename T>
        QList<T>
        chunkMap(const std::function<T(Related &&model)> &callback,
                 qint64 count = 1000) const;

        /*! Chunk the results of a query by comparing IDs. */
        bool
        chunkById(qint64 count, // NOLINT(google-default-arguments)
                  const std::function<
                      bool(ModelsCollection<Related> &&models, qint64 page)> &callback,
                  const QString &column = "",
                  const QString &alias = "") const override;
        /*! Execute a callback over each item while chunking by ID. */
        bool eachById(const std::function<bool(Related &&model, qint64 index)> &callback, // NOLINT(google-default-arguments)
                      qint64 count = 1000, const QString &column = "",
                      const QString &alias = "") const override;

        /* Inserting/Updating operations on the relationship */
        /*! Attach a model instance to the parent model. */
        std::tuple<bool, Related &>
        save(Related &model,
             const QList<AttributeItem> &pivotValues = {},
             bool touch = true) const;
        /*! Attach a model instance to the parent model. */
        std::tuple<bool, Related>
        save(Related &&model,
             const QList<AttributeItem> &pivotValues = {},
             bool touch = true) const;
        /*! Attach a vector of models to the parent instance. */
        ModelsCollection<Related> &
        saveMany(ModelsCollection<Related> &models,
                 const QList<QList<AttributeItem>> &pivotValues = {}) const;
        /*! Attach a vector of models to the parent instance. */
        ModelsCollection<Related>
        saveMany(ModelsCollection<Related> &&models,
                 const QList<QList<AttributeItem>> &pivotValues = {}) const;

        /*! Create a new instance of the related model. */
        Related create(const QList<AttributeItem> &attributes = {},
                       const QList<AttributeItem> &pivotValues = {},
                       bool touch = true) const;
        /*! Create a new instance of the related model. */
        Related create(QList<AttributeItem> &&attributes = {},
                       const QList<AttributeItem> &pivotValues = {},
                       bool touch = true) const;
        /*! Create a vector of new instances of the related model. */
        ModelsCollection<Related>
        createMany(const QList<QList<AttributeItem>> &records,
                   const QList<QList<AttributeItem>> &pivotValues = {}) const;
        /*! Create a vector of new instances of the related model. */
        ModelsCollection<Related>
        createMany(QList<QList<AttributeItem>> &&records,
                   const QList<QList<AttributeItem>> &pivotValues = {}) const;

        /*! Create or update a related record matching the attributes, and fill it
            with values. */
        Related updateOrCreate(const QList<WhereItem> &attributes,
                               const QList<AttributeItem> &values = {},
                               const QList<AttributeItem> &pivotValues = {},
                               bool touch = true) const;

        /* Others */
        /*! Qualify the given column name by the pivot table. */
        QString qualifyPivotColumn(const QString &column) const;

        /*! Get all of the IDs for the related models. */
        QList<QVariant> allRelatedIds() const;

        /*! The textual representation of the Relation type. */
        inline const QString &relationTypeName() const override;

    protected:
        /* Relation related operations */
        /*! Attempt to resolve the intermediate table name from the given string. */
        static QString resolveTableName(const QString &table);

        /*! Set the join clause for the relation query. */
        inline const BelongsToMany &performJoin() const;
        /*! Set the join clause for the relation query. */
        const BelongsToMany &performJoin(Builder<Related> &query) const;
        /*! Set the where clause for the relation query. */
        const BelongsToMany &addWhereConstraints() const;

        /*! Build model dictionary keyed by the relation's foreign key. */
        QHash<typename Model::KeyType, ModelsCollection<Related>>
        buildDictionary(ModelsCollection<Related> &&results) const; // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)

        /*! Prepare the query builder for query execution. */
        Builder<Related> &prepareQueryBuilder() const;
        /*! Get the select columns for the relation query. */
        QList<Column> shouldSelect(QList<Column> columns = {ASTERISK}) const;
        /*! Get the pivot columns for the relation, "pivot_" is prefixed
            to each column for easy removal later. */
        QStringList aliasedPivotColumns() const;

        /*! Hydrate the pivot table relationship on the models. */
        void hydratePivotRelation(ModelsCollection<Related> &models) const;
        /*! Get the pivot attributes from a model. */
        QList<AttributeItem> migratePivotAttributes(Related &model) const;

        /* Inserting & Updating relationship */
        /*! Determine if we should touch the parent on sync. */
        bool touchingParent() const;
        /*! Attempt to guess the name of the inverse of the relation. */
        QString guessInverseRelation() const;

        /* Others */
        /*! Clone the belongs-to-many relation. */
        inline BelongsToMany<Model, Related, PivotType> clone() const;

        /* Querying Relationship Existence/Absence */
        /*! Add the constraints for a relationship query. */
        std::unique_ptr<Builder<Related>>
        getRelationExistenceQuery( // NOLINT(google-default-arguments)
                std::unique_ptr<Builder<Related>> &&query,
                const Builder<Model> &parentQuery,
                const QList<Column> &columns = {ASTERISK}) const override;

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
        QString m_accessor = pivot_;
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
        /* Relation related operations */
        /*! Set the constraints for an eager load of the relation, common code. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        void addEagerConstraintsInternal(const ModelsCollection<CollectionModel> &models);

        /*! Initialize the relation on a set of models, common code. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        ModelsCollection<CollectionModel> &
        initRelationInternal(ModelsCollection<CollectionModel> &models,
                             const QString &relation) const;
        /*! Match the eagerly loaded results to their parents. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        void matchInternal(
                ModelsCollection<CollectionModel> &models,
                ModelsCollection<Related> &&results, const QString &relation) const;
    };

    /* protected */

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType>::BelongsToMany(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &table, const QString &foreignPivotKey, // NOLINT(modernize-pass-by-value)
            const QString &relatedPivotKey, const QString &parentKey,  // NOLINT(modernize-pass-by-value)
            const QString &relatedKey, const QString &relationName  // NOLINT(modernize-pass-by-value)
    )
        : Relation<Model, Related>(std::move(related), parent, relatedKey)
        , m_table(resolveTableName(table))
        , m_foreignPivotKey(foreignPivotKey)
        , m_relatedPivotKey(relatedPivotKey)
        , m_parentKey(parentKey)
        , m_relationName(relationName)
    {}

    /* public */

    template<class Model, class Related, class PivotType>
    std::unique_ptr<BelongsToMany<Model, Related, PivotType>>
    BelongsToMany<Model, Related, PivotType>::instance(
            std::unique_ptr<Related> &&related, Model &parent,
            const QString &table, const QString &foreignPivotKey,
            const QString &relatedPivotKey, const QString &parentKey,
            const QString &relatedKey, const QString &relation)
    {
        // Relations have private ctors, std::make_unique() can't be used
        auto instance =
                std::unique_ptr<BelongsToMany<Model, Related, PivotType>>(
                    new BelongsToMany<Model, Related, PivotType>(
                        std::move(related), parent, table, foreignPivotKey,
                        relatedPivotKey, parentKey, relatedKey, relation));

        instance->init();

        return instance;
    }

    /* Relation related operations */

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::addConstraints() const
    {
        performJoin();

        if (this->constraints)
            addWhereConstraints();
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::addEagerConstraints(
            const ModelsCollection<Model> &models)
    {
        addEagerConstraintsInternal(models);
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::addEagerConstraints(
            const ModelsCollection<Model *> &models)
    {
        addEagerConstraintsInternal(models);
    }

    template<class Model, class Related, class PivotType>
    ModelsCollection<Model> &
    BelongsToMany<Model, Related, PivotType>::initRelation(
            ModelsCollection<Model> &models, const QString &relation) const
    {
        return initRelationInternal(models, relation);
    }

    template<class Model, class Related, class PivotType>
    ModelsCollection<Model *> &
    BelongsToMany<Model, Related, PivotType>::initRelation(
            ModelsCollection<Model *> &models, const QString &relation) const
    {
        return initRelationInternal(models, relation);
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::match(
            ModelsCollection<Model> &models, ModelsCollection<Related> &&results,
            const QString &relation) const
    {
        matchInternal(models, std::move(results), relation);
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::match(
            ModelsCollection<Model *> &models, ModelsCollection<Related> &&results,
            const QString &relation) const
    {
        matchInternal(models, std::move(results), relation);
    }

    template<class Model, class Related, class PivotType>
    QHash<typename Model::KeyType, ModelsCollection<Related>>
    BelongsToMany<Model, Related, PivotType>::buildDictionary(
            ModelsCollection<Related> &&results) const // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        /* First we will build a dictionary of child models keyed by the foreign key
           of the relation so that we will easily and quickly match them to their
           parents without having a possibly slow inner loops for every models. */
        QHash<typename Model::KeyType, ModelsCollection<Related>> dictionary;
        dictionary.reserve(results.size());

        // Build model dictionary keyed by the parent's primary key
        for (auto &&result : results) {

            const auto foreignPivotKey =
                    result.template getRelation<PivotType, Orm::One>(m_accessor)
                    ->getAttribute(m_foreignPivotKey)
                    .template value<typename Model::KeyType>();

            dictionary[foreignPivotKey] << std::move(result);
        }

        return dictionary;
    }

    template<class Model, class Related, class PivotType>
    std::variant<ModelsCollection<Related>, std::optional<Related>>
    BelongsToMany<Model, Related, PivotType>::getResults() const
    {
        // If a Model doesn't contain primary key (eg. empty Model instance)
        if (const auto parentKey = this->m_parent->getAttribute(m_parentKey);
            !parentKey.isValid() || parentKey.isNull()
        )
            return ModelsCollection<Related>();

        return get();
    }

    template<class Model, class Related, class PivotType>
    ModelsCollection<Related>
    BelongsToMany<Model, Related, PivotType>::get(const QList<Column> &columns) const // NOLINT(google-default-arguments)
    {
        /* First we'll add the proper select columns onto the query so it is run with
           the proper columns. Then, we will get the results and hydrate out pivot
           models with the result of those columns as a separate model relation. */
        // FEATURE scopes silverqx
//        $builder = $this->query->applyScopes();

        auto l_columns = this->getBaseQuery().getColumns().isEmpty()
                         ? columns
                         : QList<Column>();

        // Hydrated related models
        auto models = this->m_query->addSelect(shouldSelect(std::move(l_columns)))
                                    .getModels();

        hydratePivotRelation(models);

        /* If we actually found models, we will also eager load any relationships that
           have been specified as needing to be eager loaded. This will solve the
           n + 1 query problem for the developer and also increase performance. */
        if (!models.isEmpty())
            this->m_query->eagerLoadRelations(models);

        return models;
    }

    /* Getters / Setters */

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
    QString
    BelongsToMany<Model, Related, PivotType>::getQualifiedParentKeyName() const
    {
        return this->m_parent->qualifyColumn(m_parentKey);
    }

    template<class Model, class Related, class PivotType>
    QString
    BelongsToMany<Model, Related, PivotType>::getQualifiedRelatedKeyName() const
    {
        return this->m_related->qualifyColumn(this->m_relatedKey);
    }

    template<class Model, class Related, class PivotType>
    QString
    BelongsToMany<Model, Related, PivotType>::getExistenceCompareKey() const
    {
        return getQualifiedForeignPivotKeyName();
    }

    template<class Model, class Related, class PivotType>
    const QString &
    BelongsToMany<Model, Related, PivotType>::getTable() const noexcept
    {
        return m_table;
    }

    template<class Model, class Related, class PivotType>
    const QString &
    BelongsToMany<Model, Related, PivotType>::getForeignPivotKeyName() const noexcept
    {
        return m_foreignPivotKey;
    }

    template<class Model, class Related, class PivotType>
    const QString &
    BelongsToMany<Model, Related, PivotType>::getRelatedPivotKeyName() const noexcept
    {
        return m_relatedPivotKey;
    }

    template<class Model, class Related, class PivotType>
    const QString &
    BelongsToMany<Model, Related, PivotType>::getParentKeyName() const noexcept
    {
        return m_parentKey;
    }

    template<class Model, class Related, class PivotType>
    const QString &
    BelongsToMany<Model, Related, PivotType>::getRelationName() const noexcept
    {
        return m_relationName;
    }

    template<class Model, class Related, class PivotType>
    const QString &
    BelongsToMany<Model, Related, PivotType>::getPivotAccessor() const noexcept
    {
        return m_accessor;
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
    BelongsToMany<Model, Related, PivotType>::as(QString &&accessor)
    {
        m_accessor = std::move(accessor);

        return *this;
    }

    template<class Model, class Related, class PivotType>
    const QStringList &
    BelongsToMany<Model, Related, PivotType>::getPivotColumns() const noexcept
    {
        return m_pivotColumns;
    }

    /* Timestamps */

    template<class Model, class Related, class PivotType>
    bool BelongsToMany<Model, Related, PivotType>::usesTimestamps() const noexcept
    {
        return m_withTimestamps;
    }

    // NOTE api different, the createdAt and updatedAt parameters have default column values, so they are less confusing silverqx
    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::withTimestamps(
            const QString &createdAt, const QString &updatedAt)
    {
        m_withTimestamps = true;

        m_pivotCreatedAt = createdAt;
        m_pivotUpdatedAt = updatedAt;

        return this->withPivot({this->createdAt(), this->updatedAt()});
    }

    template<class Model, class Related, class PivotType>
    const QString &BelongsToMany<Model, Related, PivotType>::createdAt() const
    {
        // NOTE api different, checking the m_withTimestamps instead of m_pivotCreatedAt.isEmpty() silverqx
        return m_withTimestamps ? m_pivotCreatedAt
                                : this->m_parent->getCreatedAtColumn();
    }

    template<class Model, class Related, class PivotType>
    const QString &BelongsToMany<Model, Related, PivotType>::updatedAt() const
    {
        // NOTE api different, checking the m_withTimestamps instead of m_pivotCreatedAt.isEmpty() silverqx
        return m_withTimestamps ? m_pivotUpdatedAt
                                : this->m_parent->getUpdatedAtColumn();
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::touchIfTouching() const
    {
        // BUG circular dependency when both models (Model and Related) has set up u_touches silverqx
        if (touchingParent())
            this->m_parent->touch();

        if (this->m_parent->touches(m_relationName))
            touch();
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::touch() const
    {
        const auto &related = this->m_related.get();

        const auto &key = related->getKeyName();

        const QList<UpdateItem> record {
            {related->getUpdatedAtColumn(), related->freshTimestampString()},
        };

        const auto ids = allRelatedIds();

        if (ids.isEmpty())
            return;

        /* If we actually have IDs for the relation, we will run the query to update all
           the related model's timestamps, to make sure these all reflect the changes
           to the parent models. This will help us keep any caching synced up here. */
        related->newQueryWithoutRelationships()
               ->whereIn(key, ids)
                .update(record);
    }

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::find( // NOLINT(google-default-arguments)
            const QVariant &id, const QList<Column> &columns) const
    {
        return this->where(this->m_related->getQualifiedKeyName(), EQ, id)
                .first(columns);
    }

    template<class Model, class Related, class PivotType>
    Related BelongsToMany<Model, Related, PivotType>::findOrNew( // NOLINT(google-default-arguments)
            const QVariant &id, const QList<Column> &columns) const
    {
        // Found
        if (auto instance = find(id, columns); instance)
            return std::move(*instance);

        return this->m_related->newInstance();
    }

    template<class Model, class Related, class PivotType>
    Related BelongsToMany<Model, Related, PivotType>::findOrFail( // NOLINT(google-default-arguments)
            const QVariant &id, const QList<Column> &columns) const
    {
        auto model = find(id, columns);

        // Found
        if (model)
            return std::move(*model);

        throw Exceptions::ModelNotFoundError(
                    TypeUtils::classPureBasename<Related>(), {id});
    }

    template<class Model, class Related, class PivotType>
    ModelsCollection<Related>
    BelongsToMany<Model, Related, PivotType>::findMany(
            const QList<QVariant> &ids, const QList<Column> &columns) const
    {
        if (ids.isEmpty())
            return {};

        return this->whereIn(this->m_related->getQualifiedKeyName(), ids).get(columns);
    }

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::findOr(
            const QVariant &id, const QList<Column> &columns,
            const std::function<void()> &callback) const
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

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::findOr(
            const QVariant &id, const std::function<void()> &callback) const
    {
        return findOr(id, {ASTERISK}, callback);
    }

    template<class Model, class Related, class PivotType>
    template<typename R>
    std::pair<std::optional<Related>, R>
    BelongsToMany<Model, Related, PivotType>::findOr(
            const QVariant &id, const QList<Column> &columns,
            const std::function<R()> &callback) const
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

    template<class Model, class Related, class PivotType>
    template<typename R>
    std::pair<std::optional<Related>, R>
    BelongsToMany<Model, Related, PivotType>::findOr(
            const QVariant &id, const std::function<R()> &callback) const
    {
        return findOr<R>(id, {ASTERISK}, callback);
    }

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::first(const QList<Column> &columns) const // NOLINT(google-default-arguments)
    {
        auto results = this->take(1).get(columns);

        if (results.isEmpty())
            return std::nullopt;

        return std::move(results.first());
    }

    template<class Model, class Related, class PivotType>
    Related
    BelongsToMany<Model, Related, PivotType>::firstOrNew( // NOLINT(google-default-arguments)
            const QList<WhereItem> &attributes,
            const QList<AttributeItem> &values) const
    {
        // Model found in db
        if (auto instance = this->m_related->where(attributes)->first(); instance)
            return std::move(*instance);

        return this->m_related->newInstance(
                    AttributeUtils::joinAttributesForFirstOr(
                        attributes, values, this->m_relatedKey));
    }

    template<class Model, class Related, class PivotType>
    Related
    BelongsToMany<Model, Related, PivotType>::firstOrCreate(
            const QList<WhereItem> &attributes,
            const QList<AttributeItem> &values,
            const QList<AttributeItem> &pivotValues,
            const bool touch) const
    {
        // Related model is attached and attributes were found
        if (auto instance = clone().where(attributes).first(); instance)
            return std::move(*instance);

        // Attributes were not found (Related model doesn't exist or isn't attached)

        // Related model already exists so only attach (also updates pivot attributes)
        if (auto instance = this->m_related->where(attributes)->first();
            instance
         ) {
            this->attach(*instance, pivotValues, touch);

            return std::move(*instance);
        }

        // Related model doesn't exist so create it and also attach
        return create(AttributeUtils::joinAttributesForFirstOr(
                          attributes, values, this->m_relatedKey),
                      pivotValues, touch);
    }

    template<class Model, class Related, class PivotType>
    Related
    BelongsToMany<Model, Related, PivotType>::firstOrFail( // NOLINT(google-default-arguments)
            const QList<Column> &columns) const
    {
        if (auto model = first(columns); model)
            return std::move(*model);

        throw Exceptions::ModelNotFoundError(TypeUtils::classPureBasename<Related>());
    }

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::firstOr( // NOLINT(google-default-arguments)
            const QList<Column> &columns, const std::function<void()> &callback) const
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

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::firstOr( // NOLINT(google-default-arguments)
            const std::function<void()> &callback) const
    {
        return firstOr({ASTERISK}, callback);
    }

    template<class Model, class Related, class PivotType>
    template<typename R>
    std::pair<std::optional<Related>, R>
    BelongsToMany<Model, Related, PivotType>::firstOr(
            const QList<Column> &columns, const std::function<R()> &callback) const
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

    template<class Model, class Related, class PivotType>
    template<typename R>
    std::pair<std::optional<Related>, R>
    BelongsToMany<Model, Related, PivotType>::firstOr(
            const std::function<R()> &callback) const
    {
        return firstOr<R>({ASTERISK}, callback);
    }

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::firstWhere( // NOLINT(google-default-arguments)
            const Column &column, const QString &comparison,
            const QVariant &value, const QString &condition) const
    {
        return this->where(column, comparison, value, condition).first();
    }

    template<class Model, class Related, class PivotType>
    std::optional<Related>
    BelongsToMany<Model, Related, PivotType>::firstWhereEq( // NOLINT(google-default-arguments)
            const Column &column, const QVariant &value,
            const QString &condition) const
    {
        return firstWhere(column, EQ, value, condition);
    }

    /* Builds Queries */

    template<class Model, class Related, class PivotType>
    bool BelongsToMany<Model, Related, PivotType>::chunk(
            const qint64 count,
            const std::function<
                bool(ModelsCollection<Related> &&, qint64)> &callback) const
    {
        return prepareQueryBuilder()
                .chunk(count, [this, &callback]
                              (ModelsCollection<Related> &&models, const qint64 page)
        {
            hydratePivotRelation(models);

            return std::invoke(callback, std::move(models), page);
        });
    }

    template<class Model, class Related, class PivotType>
    bool BelongsToMany<Model, Related, PivotType>::each( // NOLINT(google-default-arguments)
            const std::function<bool(Related &&, qint64)> &callback,
            const qint64 count) const
    {
        return chunk(count, [&callback]
                            (ModelsCollection<Related> &&models, const qint64 /*unused*/) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        {
            qint64 index = 0;

            for (auto &&model : models)
                if (const auto result = std::invoke(callback, std::move(model), index++);
                    !result
                )
                    return false;

            return true;
        });
    }

    template<class Model, class Related, class PivotType>
    ModelsCollection<Related>
    BelongsToMany<Model, Related, PivotType>::chunkMap( // NOLINT(google-default-arguments)
            const std::function<Related(Related &&)> &callback, const qint64 count) const
    {
        ModelsCollection<Related> result;
        // Reserve the first page, it can help reallocations at the beginning
        result.reserve(count);

        chunk(count, [&result, &callback]
                     (ModelsCollection<Related> &&models, const qint64 /*unused*/) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        {
            for (auto &&model : models)
                result << std::invoke(callback, std::move(model));

            return true;
        });

        return result;
    }

    template<class Model, class Related, class PivotType>
    template<typename T>
    QList<T>
    BelongsToMany<Model, Related, PivotType>::chunkMap(
            const std::function<T(Related &&)> &callback, const qint64 count) const
    {
        QList<T> result;
        // Reserve the first page, it can help reallocations at the beginning
        result.reserve(count);

        chunk(count, [&result, &callback]
                     (ModelsCollection<Related> &&models, const qint64 /*unused*/) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        {
            for (auto &&model : models)
                result << std::invoke(callback, std::move(model));

            return true;
        });

        return result;
    }

    template<class Model, class Related, class PivotType>
    bool BelongsToMany<Model, Related, PivotType>::chunkById( // NOLINT(google-default-arguments)
            const qint64 count,
            const std::function<bool(ModelsCollection<Related> &&, qint64)> &callback,
            const QString &column, const QString &alias) const
    {
        const auto &relatedKeyName = this->getRelatedKeyName();

        const auto columnName = column.isEmpty()
                                ? this->m_related->qualifyColumn(relatedKeyName)
                                : column;
        const auto aliasName = alias.isEmpty() ? relatedKeyName : alias;

        return prepareQueryBuilder()
                .chunkById(count, [this, &callback]
                                  (ModelsCollection<Related> &&models, const qint64 page)
        {
            hydratePivotRelation(models);

            return std::invoke(callback, std::move(models), page);
        },
                columnName, aliasName);
    }

    template<class Model, class Related, class PivotType>
    bool BelongsToMany<Model, Related, PivotType>::eachById( // NOLINT(google-default-arguments)
            const std::function<bool(Related &&, qint64)> &callback,
            const qint64 count, const QString &column, const QString &alias) const
    {
        return chunkById(count, [&callback, count]
                                (ModelsCollection<Related> &&models, const qint64 page) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        {
            qint64 index = 0;

            for (auto &&model : models)
                if (const auto result = std::invoke(callback, std::move(model),
                                                    ((page - 1) * count) + index++);
                    !result
                )
                    return false;

            return true;
        },
            column, alias);
    }

    /* Inserting/Updating operations on the relationship */

    template<class Model, class Related, class PivotType>
    std::tuple<bool, Related &>
    BelongsToMany<Model, Related, PivotType>::save(
            Related &model,
            const QList<AttributeItem> &pivotValues, const bool touch) const
    {
        const auto result = model.save({.touch = false});

        this->attach(model, pivotValues, touch);

        return {result, model};
    }

    template<class Model, class Related, class PivotType>
    std::tuple<bool, Related>
    BelongsToMany<Model, Related, PivotType>::save(
            Related &&model,
            const QList<AttributeItem> &pivotValues, const bool touch) const
    {
        const auto result = model.save({.touch = false});

        this->attach(model, pivotValues, touch);

        return {result, std::move(model)};
    }

    template<class Model, class Related, class PivotType>
    ModelsCollection<Related> &
    BelongsToMany<Model, Related, PivotType>::saveMany(
            ModelsCollection<Related> &models,
            const QList<QList<AttributeItem>> &pivotValues) const
    {
        using SizeType = std::remove_cvref_t<decltype (pivotValues)>::size_type;

        const SizeType attributesSize = pivotValues.size();

        for (SizeType i = 0; i < models.size(); ++i)
            if (attributesSize > i) T_LIKELY
                save(models[i], pivotValues.at(i), false);
            else T_UNLIKELY
                save(models[i], {}, false);

        touchIfTouching();

        return models;
    }

    template<class Model, class Related, class PivotType>
    ModelsCollection<Related>
    BelongsToMany<Model, Related, PivotType>::saveMany(
            ModelsCollection<Related> &&models,
            const QList<QList<AttributeItem>> &pivotValues) const
    {
        using SizeType = std::remove_cvref_t<decltype (pivotValues)>::size_type;

        const SizeType attributesSize = pivotValues.size();

        for (SizeType i = 0; i < models.size(); ++i)
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
            const QList<AttributeItem> &attributes,
            const QList<AttributeItem> &pivotValues, const bool touch) const
    {
        auto instance = this->m_related->newInstance(attributes);

        /* Once we save the related model, we need to attach it to the base model via
           through intermediate table so we'll use the existing "attach" method to
           accomplish this which will insert the record and any more attributes. */
        instance.save({.touch = false});

        this->attach(instance, pivotValues, touch);

        return instance;
    }

    template<class Model, class Related, class PivotType>
    Related
    BelongsToMany<Model, Related, PivotType>::create(
            QList<AttributeItem> &&attributes,
            const QList<AttributeItem> &pivotValues, const bool touch) const
    {
        auto instance = this->m_related->newInstance(std::move(attributes));

        /* Once we save the related model, we need to attach it to the base model via
           through intermediate table so we'll use the existing "attach" method to
           accomplish this which will insert the record and any more attributes. */
        instance.save({.touch = false});

        this->attach(instance, pivotValues, touch);

        return instance;
    }

    template<class Model, class Related, class PivotType>
    ModelsCollection<Related>
    BelongsToMany<Model, Related, PivotType>::createMany(
            const QList<QList<AttributeItem>> &records,
            const QList<QList<AttributeItem>> &pivotValues) const
    {
        const auto recordsSize = records.size();

        ModelsCollection<Related> instances;
        instances.reserve(recordsSize);

        using SizeType = std::remove_cvref_t<decltype (pivotValues)>::size_type;
        const SizeType attributesSize = pivotValues.size();

        for (SizeType i = 0; i < recordsSize; ++i)
            if (attributesSize > i) T_LIKELY
                instances << create(records.at(i), pivotValues.at(i), false);
            else T_UNLIKELY
                instances << create(records.at(i), {}, false);

        touchIfTouching();

        return instances;
    }

    template<class Model, class Related, class PivotType>
    ModelsCollection<Related>
    BelongsToMany<Model, Related, PivotType>::createMany(
            QList<QList<AttributeItem>> &&records,
            const QList<QList<AttributeItem>> &pivotValues) const
    {
        const auto recordsSize = records.size();

        ModelsCollection<Related> instances;
        instances.reserve(recordsSize);

        using SizeType = std::remove_cvref_t<decltype (pivotValues)>::size_type;
        const SizeType attributesSize = pivotValues.size();

        for (SizeType i = 0; i < recordsSize; ++i)
            if (attributesSize > i) T_LIKELY
                instances << create(std::move(records[i]), pivotValues.at(i), false);
            else T_UNLIKELY
                instances << create(std::move(records[i]), {}, false);

        touchIfTouching();

        return instances;
    }

    template<class Model, class Related, class PivotType>
    Related BelongsToMany<Model, Related, PivotType>::updateOrCreate(
            const QList<WhereItem> &attributes,
            const QList<AttributeItem> &values,
            const QList<AttributeItem> &pivotValues,
            const bool touch) const
    {
        auto instance = clone().where(attributes).first();

        // if : attributes were not found (Related model doesn't exist or isn't attached)
        if (!instance) {
            instance = this->m_related->where(attributes)->first();

            /* Related model doesn't exist so create it and also attach (also updates
               pivot attributes). */
            if (!instance)
                return create(AttributeUtils::joinAttributesForFirstOr(
                                  attributes, values, this->m_relatedKey),
                              pivotValues, touch);

            // Related model already exists so only attach (also updates pivot attributes)

            /* Attach before an update because something may fail so even if an update
               fails it will be already attached. */
            this->attach(*instance, pivotValues, touch);
        }

        // else : related model is attached and attributes were found

        // Update the Related model
        instance->fill(values);

        // The save() method already updates timestamps during Model::performUpdate()
        instance->save({.touch = false});

        return std::move(*instance);
    }

    /* Others */

    template<class Model, class Related, class PivotType>
    QString
    BelongsToMany<Model, Related, PivotType>::qualifyPivotColumn(
            const QString &column) const
    {
        if (column.contains(DOT))
            return column;

        return DOT_IN.arg(m_table, column);
    }

    template<class Model, class Related, class PivotType>
    QList<QVariant>
    BelongsToMany<Model, Related, PivotType>::allRelatedIds() const
    {
        // Ownership of the std::shared_ptr<QueryBuilder>
        auto query = this->newPivotQuery()->get({m_relatedPivotKey});

        QList<QVariant> ids;
        ids.reserve(QueryUtils::queryResultSize(query));

        while (query.next())
            ids << query.value(m_relatedPivotKey);

        return ids;
    }

    template<class Model, class Related, class PivotType>
    const QString &
    BelongsToMany<Model, Related, PivotType>::relationTypeName() const
    {
        static const auto cached = QStringLiteral("BelongsToMany");
        return cached;
    }

    // NOTE api different, Eloquent always use the guessed table name if the table is null during the belongsToMany() call, the table property is used only when the Model::class (class name) is passed to the belongsToMany() silverqx
    template<class Model, class Related, class PivotType>
    QString
    BelongsToMany<Model, Related, PivotType>::resolveTableName(const QString &table)
    {
        // The u_table cannot be set on the Basic Pivot model
        if constexpr (!PivotType::isCustomPivot())
            return table;

        /* User can define the u_table on a Custom Pivot model, it has higher priority
           than the guessed table name in the HasRelationships::pivotTableName(). */
        else {
            auto pivotTable = PivotType().getTable();

            if (!pivotTable.isEmpty())
                return pivotTable;

            return table;
        }
    }

    /* protected */

    /* Relation related operations */

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
                             this->m_parent->getAttribute(m_parentKey));

        return *this;
    }

    template<class Model, class Related, class PivotType>
    Builder<Related> &
    BelongsToMany<Model, Related, PivotType>::prepareQueryBuilder() const
    {
        return this->m_query->addSelect(shouldSelect());
    }

    template<class Model, class Related, class PivotType>
    QList<Column>
    BelongsToMany<Model, Related, PivotType>::shouldSelect(QList<Column> columns) const
    {
        if (columns == QList<Column> {ASTERISK})
            columns = QList<Column> {QStringLiteral("%1.*")
                                     .arg(this->m_related->getTable())};

        // Obtain aliased pivot columns
        auto aliasedPivotColumns = this->aliasedPivotColumns();

        // Will contain final result
        QList<Column> mergedColumns;
        mergedColumns.reserve(aliasedPivotColumns.size() + columns.size());

        std::ranges::move(aliasedPivotColumns, std::back_inserter(mergedColumns));

        // Merge aliasedPivotColumns and columns
        for (auto &&column : columns) {
            // Avoid duplicates, expressions are not checked
            if (std::holds_alternative<QString>(column) &&
                mergedColumns.contains(column)
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
        columns.reserve(columns.size() + m_pivotColumns.size());

        columns += m_pivotColumns;

        columns.removeDuplicates();

        return columns
                | ranges::views::transform([this](const QString &column)
        {
            return QStringLiteral("%1 as pivot_%2").arg(qualifyPivotColumn(column),
                                                        column);
        })
                | ranges::to<QStringList>();
    }

    template<class Model, class Related, class PivotType>
    void BelongsToMany<Model, Related, PivotType>::hydratePivotRelation(
            ModelsCollection<Related> &models) const
    {
        /* To hydrate the pivot relationship, we will just gather the pivot attributes
           and create a new Pivot model, which is basically a dynamic model that we
           will set the attributes, table, and connections on it so it will work. */
        for (auto &model : models)
            model.template setRelation<PivotType>(
                        m_accessor,
                        std::optional<PivotType>(this->newExistingPivot(
                                                     migratePivotAttributes(model))));
    }

    template<class Model, class Related, class PivotType>
    QList<AttributeItem>
    BelongsToMany<Model, Related, PivotType>::migratePivotAttributes(
            Related &model) const
    {
        const auto &modelAttributes = model.getAttributes();

        QList<AttributeItem> values;
        values.reserve(modelAttributes.size());

        for (const auto &attribute : modelAttributes)
            /* To get the pivots attributes we will just take any of the attributes which
               begin with "pivot_" and add those to this vector, as well as unsetting
               them from the parent's models since they exist in a different table. */
            if (attribute.key.startsWith(QStringLiteral("pivot_"))) {
                // Remove the 'pivot_' part from an attribute key
                values.append({attribute.key.sliced(6), attribute.value});

                model.unsetAttribute(attribute);
            }

        return values;
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
        auto relation = TypeUtils::classPureBasename<Model>();

        relation[0] = relation[0].toLower();

        return TMPL_PLURAL.arg(relation);
    }

    /* Others */

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType>
    BelongsToMany<Model, Related, PivotType>::clone() const
    {
        return *this;
    }

    /* Querying Relationship Existence/Absence */

    template<class Model, class Related, class PivotType>
    std::unique_ptr<Builder<Related>>
    BelongsToMany<Model, Related, PivotType>::getRelationExistenceQuery( // NOLINT(google-default-arguments)
            std::unique_ptr<Builder<Related>> &&query,
            const Builder<Model> &parentQuery,
            const QList<Column> &columns) const
    {
        // CUR finish self query silverqx
//        if (query->getQuery()->from == parentQuery.getQuery()->from)
//            return this->getRelationExistenceQueryForSelfRelation(query, parentQuery,
//                                                                  columns);

        performJoin(*query);

        return Relation<Model, Related>::getRelationExistenceQuery(
                    std::move(query), parentQuery, columns);
    }

    /* private */

    /* Relation related operations */

    template<class Model, class Related, class PivotType>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    void BelongsToMany<Model, Related, PivotType>::addEagerConstraintsInternal(
            const ModelsCollection<CollectionModel> &models)
    {
        this->whereInEager(getQualifiedForeignPivotKeyName(),
                           this->getKeys(models, m_parentKey));
    }

    template<class Model, class Related, class PivotType>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    ModelsCollection<CollectionModel> &
    BelongsToMany<Model, Related, PivotType>::initRelationInternal(
            ModelsCollection<CollectionModel> &models,
            const QString &relation) const
    {
        /*! Model type used in the for-ranged loops. */
        using ModelLoopType = typename ModelsCollection<CollectionModel>::ModelLoopType;

        for (ModelLoopType model : models)
            Relation<Model,Related>::toPointer(model)
                    ->template setRelation<Related>(relation,
                                                    ModelsCollection<Related>());

        return models;
    }

    template<class Model, class Related, class PivotType>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    void BelongsToMany<Model, Related, PivotType>::matchInternal(
            ModelsCollection<CollectionModel> &models,
            ModelsCollection<Related> &&results, const QString &relation) const
    {
        auto dictionary = buildDictionary(std::move(results));

        /*! Model type used in the for-ranged loops. */
        using ModelLoopType = typename ModelsCollection<CollectionModel>::ModelLoopType;

        /* Once we have the dictionary of child objects, we can easily match the
           children back to their parent using the dictionary and the keys on the
           the parent models. Then we will return the hydrated models back out. */
        for (ModelLoopType model : models) {
            auto *const modelPointer = Relation<Model,Related>::toPointer(model);

            if (const auto parentKey = modelPointer->getAttribute(m_parentKey)
                .template value<typename Model::KeyType>();
                dictionary.contains(parentKey)
            )
                modelPointer->template setRelation<Related>(
                            /* Is safe to move as the hash is keyed by IDs so every ID
                               has its own related models prepared. */
                            relation, std::move(dictionary.find(parentKey).value()));
        }
    }

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_BELONGSTOMANY_HPP
