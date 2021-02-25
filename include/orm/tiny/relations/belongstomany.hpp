#ifndef BELONGSTOMANY_H
#define BELONGSTOMANY_H

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include "orm/tiny/relations/relation.hpp"

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
        /*! The pivot table columns to retrieve. */
        QStringList m_pivotColumns;

        /*! Indicates if timestamps are available on the pivot table. */
        bool m_withTimestamps = false;
        /*! The custom pivot table column for the created_at timestamp. */
        QString m_pivotCreatedAt;
        /*! The custom pivot table column for the updated_at timestamp. */
        QString m_pivotUpdatedAt;
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

    // TODO perf for all similar methods make rvalue variants, or what if all this methods would be rvalue only, so if it is possible then move and if not then copy silverqx
    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    BelongsToMany<Model, Related, PivotType>::withPivot(const QStringList &columns)
    {
        ranges::copy(columns, ranges::back_inserter(m_pivotColumns));

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

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // BELONGSTOMANY_H
