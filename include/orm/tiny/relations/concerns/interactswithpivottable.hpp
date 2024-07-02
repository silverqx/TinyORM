#pragma once
#ifndef ORM_TINY_RELATIONS_CONCERNS_INTERACTSWITHPIVOTTABLE_HPP
#define ORM_TINY_RELATIONS_CONCERNS_INTERACTSWITHPIVOTTABLE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlRecord

#include <range/v3/view/set_algorithm.hpp>

#include "orm/exceptions/domainerror.hpp"
#include "orm/ormconcepts.hpp"
#include "orm/tiny/types/syncchanges.hpp"
#include "orm/tiny/utils/attribute.hpp"
#include "orm/utils/query.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{
    template<class Model, class Related, class PivotType>
    class BelongsToMany;

    class Pivot;

namespace Concerns
{

    /*! TinyORM's 'Pivot' class. */
    template<typename PivotType>
    concept OurPivot = std::same_as<PivotType, Pivot>;

    /*! Custom pivot class, not TinyORM's 'Pivot' class. */
    template<typename PivotType>
    concept CustomPivot = !std::same_as<PivotType, Pivot>;

    /*! Wraps methods that interact with the pivot table in belongs-to-many relation. */
    template<class Model, class Related, class PivotType>
    class InteractsWithPivotTable
    {
        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;
        /*! Alias for the query utils. */
        using QueryUtils = Orm::Utils::Query;

        /*! Model alias, helps to avoid conflict with the Model template parameter. */
        template<typename Derived>
        using BaseModel = Orm::Tiny::Model<Derived>;

    protected:
        /*! InteractsWithPivotTable's copy constructor
            (used by BelongsToMany::clone()). */
        InteractsWithPivotTable(const InteractsWithPivotTable &) = default;

    public:
        /*! Alias for the current BelongsToMany type (for shorter name). */
        using BelongsToManyType = BelongsToMany<Model, Related, PivotType>;
        /*! Alias for the parent model's key type (for shorter name). */
        using ParentKeyType = typename BaseModel<Model>::KeyType;
        /*! Alias for the related model's key type (for shorter name). */
        using RelatedKeyType = typename BaseModel<Related>::KeyType;

        /*! Default constructor. */
        InteractsWithPivotTable() = default;
        /*! Pure virtual destructor. */
        inline virtual ~InteractsWithPivotTable() = 0;

        /*! InteractsWithPivotTable's move constructor. */
        InteractsWithPivotTable(InteractsWithPivotTable &&) = delete;

        /*! InteractsWithPivotTable's copy assignment operator. */
        InteractsWithPivotTable &operator=(const InteractsWithPivotTable &) = delete;
        /*! InteractsWithPivotTable's move assignment operator. */
        InteractsWithPivotTable &operator=(InteractsWithPivotTable &&) = delete;

        /*! Set the columns on the pivot table to retrieve. */
        BelongsToManyType &withPivot(const QStringList &columns);
        /*! Set the columns on the pivot table to retrieve. */
        inline BelongsToManyType &withPivot(const QString &column);
        /*! Set the columns on the pivot table to retrieve. */
        BelongsToManyType &withPivot(QStringList &&columns); // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        /*! Set the columns on the pivot table to retrieve. */
        inline BelongsToManyType &withPivot(QString &&column);
        /*! Set the columns on the pivot table to retrieve. */
        template<QStringConcept ...Args>
        BelongsToManyType &withPivot(Args &&...columns);

        /*! Determine whether the given column is defined as a pivot column. */
        inline bool hasPivotColumn(const QString &column) const;

        /*! Attach models to the parent. */
        void attach(const QList<QVariant> &ids,
                    const QList<AttributeItem> &attributes = {},
                    bool touch = true) const;
        /*! Attach models to the parent. */
        inline void
        attach(const QList<std::reference_wrapper<Related>> &models,
               const QList<AttributeItem> &attributes = {},
               bool touch = true) const;
        /*! Attach a model to the parent. */
        inline void
        attach(const QVariant &id, const QList<AttributeItem> &attributes = {},
               bool touch = true) const;
        /*! Attach a model to the parent. */
        inline void
        attach(const Related &model, const QList<AttributeItem> &attributes = {},
               bool touch = true) const;
        /*! Attach models to the parent. */
        void attach(const std::map<RelatedKeyType,
                                   QList<AttributeItem>> &idsWithAttributes,
                    bool touch = true) const;

        /*! Sync the intermediate tables with a list of IDs. */
        SyncChanges sync(const std::map<RelatedKeyType,
                                        QList<AttributeItem>> &idsWithAttributes,
                         bool detaching = true) const;
        /*! Sync the intermediate tables with a vector of IDs. */
        inline SyncChanges sync(const QList<QVariant> &ids,
                                bool detaching = true) const;

        /*! Sync the intermediate tables with a vector of IDs without detaching. */
        inline SyncChanges syncWithoutDetaching(
                const std::map<RelatedKeyType,
                               QList<AttributeItem>> &idsWithAttributes) const;
        /*! Sync the intermediate tables with a vector of IDs without detaching. */
        inline SyncChanges syncWithoutDetaching(const QList<QVariant> &ids) const;

        /*! Detach models from the relationship. */
        inline int detach(const QList<QVariant> &ids, bool touch = true) const;
        /*! Detach models from the relationship. */
        inline int detach(const QList<std::reference_wrapper<Related>> &models,
                          bool touch = true) const;
        /*! Detach model from the relationship. */
        inline int detach(const QVariant &id, bool touch = true) const;
        /*! Detach model from the relationship. */
        inline int detach(const Related &model, bool touch = true) const;
        /*! Detach all models from the relationship. */
        inline int detachAll(bool touch = true) const;

        /*! Update an existing pivot record on the table. */
        int updateExistingPivot(const QVariant &id,
                                QList<AttributeItem> attributes,
                                bool touch = true) const;
        /*! Update an existing pivot record on the table. */
        int updateExistingPivot(const Related &model,
                                const QList<AttributeItem> &attributes,
                                bool touch = true) const;

        /*! Create a new existing pivot model instance. */
        PivotType newExistingPivot(const QList<AttributeItem> &attributes = {}) const;
        /*! Create a new pivot model instance. */
        PivotType newPivot(const QList<AttributeItem> &attributes = {},
                           bool exists = false) const;

        /*! Create a new query builder for the pivot table. */
        std::shared_ptr<QueryBuilder> newPivotQuery() const;
        /*! Get a new plain query builder for the pivot table. */
        std::shared_ptr<QueryBuilder> newPivotStatement() const;
        /*! Get a new pivot statement for a given "other" / "related" ID. */
        std::shared_ptr<QueryBuilder>
        newPivotStatementForId(const QList<QVariant> &ids) const;
        /*! Get a new pivot statement for a given "other" / "related" ID. */
        std::shared_ptr<QueryBuilder>
        inline newPivotStatementForId(const QVariant &id) const;

    protected:
        /*! Attach a model to the parent using a custom class. */
        void attachUsingCustomClass(const QList<QVariant> &ids,
                                    const QList<AttributeItem> &attributes) const;
        /*! Create the vector of records to insert into the pivot table. */
        QList<QList<AttributeItem>>
        formatAttachRecords(const QList<QVariant> &ids,
                            const QList<AttributeItem> &attributes) const;
        /*! Create a full attachment record payload. */
        QList<AttributeItem>
        formatAttachRecord(const QVariant &id, const QList<AttributeItem> &attributes,
                           bool hasTimestamps) const;

        /*! Attach a model to the parent using a custom class. */
        void attachUsingCustomClass(
                const std::map<RelatedKeyType,
                               QList<AttributeItem>> &idsWithAttributes) const;
        /*! Create the vector of records to insert into the pivot table. */
        QList<QList<AttributeItem>>
        formatAttachRecords(
                const std::map<RelatedKeyType,
                               QList<AttributeItem>> &idsWithAttributes) const;

        /*! Create a new pivot attachment record. */
        QList<AttributeItem>
        baseAttachRecord(const QVariant &id, bool timed) const;
        /*! Set the creation and update timestamps on an attach record. */
        QList<AttributeItem> &
        addTimestampsToAttachment(QList<AttributeItem> &record,
                                  bool exists = false) const;

        /*! Get the pivot models that are currently attached. */
        QList<PivotType> getCurrentlyAttachedPivots() const;
        /*! Get the attached pivot model by related model ID. */
        std::optional<PivotType> getAttachedPivot(const QVariant &id) const;
        /*! Convert a TSqlRecord to the QList<AttributeItem>. */
        QList<AttributeItem> attributesFromRecord(const TSqlRecord &record) const;

        /*! Cast the given key to the primary key type. */
        template<typename T>
        T castKey(const QVariant &key) const;

        /*! Update an existing pivot record on the table via a custom class. */
        int updateExistingPivotUsingCustomClass(
                const QVariant &id, const QList<AttributeItem> &attributes,
                bool touch = true) const;

        /*! Attach all of the records that aren't in the given current records. */
        SyncChanges
        attachNew(const std::map<RelatedKeyType,
                                 QList<AttributeItem>> &records,
                  const QList<QVariant> &current, bool touch = true) const;

        /*! Convert IDs vector to the map with attributes keyed by IDs. */
        std::map<RelatedKeyType, QList<AttributeItem>>
        recordsFromIds(const QList<QVariant> &ids) const;
        /*! Convert IDs vector to the map with attributes keyed by IDs. */
        QList<QVariant>
        idsFromRecords(const std::map<RelatedKeyType,
                                      QList<AttributeItem>> &idsWithAttributes) const;

        /*! Cast the given pivot attributes. */
        const QList<AttributeItem> &
        castAttributes(const QList<AttributeItem> &attributes) const
        requires OurPivot<PivotType>;
        /*! Cast the given pivot attributes. */
        QList<AttributeItem>
        castAttributes(const QList<AttributeItem> &attributes) const
        requires CustomPivot<PivotType>;
        /*! Cast the given pivot attributes. */
//        QList<AttributeItem>
//        castAttributes(QList<AttributeItem> &&attributes) const
//        requires OurPivot<PivotType>;

        /*! Detach models from the relationship. */
        int detach(bool detachAll, const QList<QVariant> &ids, bool touch) const;
        /*! Detach models from the relationship using a custom class. */
        int detachUsingCustomClass(const QList<QVariant> &ids) const;

        /*! Obtain IDs from the Related models. */
        QList<QVariant>
        getRelatedIds(const QList<std::reference_wrapper<Related>> &models) const;
        /*! Obtain IDs from the vector of pivot models. */
        QList<QVariant>
        getRelatedIds(const QList<PivotType> &pivots) const;

    private:
        /*! Throw domain exception, when a user tries to override ID key
            on the pivot table. */
        void validateAttachAttribute(const AttributeItem &attribute,
                                     const QVariant &id) const;
        /*! Throw domain exception, when a user tries to override ID key
            on the pivot table.  */
        template<typename KeyType = ParentKeyType>
        [[noreturn]]
        void throwOverwritingKeyError(const QString &key, const QVariant &original,
                                      const QVariant &overwrite) const;

        /* Getters proxy methods (for shorter code) */
        /* Relation proxy methods */
        /* Getters */
        /*! Get the base QueryBuilder driving the TinyBuilder. */
        inline QueryBuilder &getBaseQuery_() const noexcept;
        /*! Get the parent model of the relation. */
        inline const Model &getParent_() const noexcept;
        /*! Get the related model of the relation. */
        inline const Related &getRelated_() const noexcept;
        /*! Get the related key for the relationship. */
        inline const QString &getRelatedKeyName_() const noexcept;

        /* BelongsToMany proxy methods */
        /* Getters */
        /*! Get the intermediate table for the relationship. */
        inline const QString &getTable_() const noexcept;
        /*! Get the foreign key for the relation. */
        inline const QString &getForeignPivotKeyName_() const noexcept;
        /*! Get the "related key" for the relation. */
        inline const QString &getRelatedPivotKeyName_() const noexcept;
        /*! Get the parent key for the relationship. */
        inline const QString &getParentKeyName_() const noexcept;
        /*! Get the pivot columns for this relationship. */
        inline const QStringList &getPivotColumns_() const noexcept;

        /* Timestamps */
        /*! Get the name of the "created at" column. */
        inline const QString &createdAt_() const;
        /*! Get the name of the "updated at" column. */
        inline const QString &updatedAt_() const;
        /*! If we're touching the parent model, touch. */
        void touchIfTouching_() const;
        /*! Get the name of the "created at" column passed to the withTimestamps(). */
        inline const QString &pivotCreatedAt() const;
        /*! Get the name of the "updated at" column passed to the withTimestamps(). */
        inline const QString &pivotUpdatedAt() const;

        /* Others */
        /*! Qualify the given column name by the pivot table. */
        QString qualifyPivotColumn_(const QString &column) const;

        /* InteractsWithPivotTable */
        /*! Static cast *this to the Relation & derived type. */
        inline BelongsToManyType &relation() noexcept;
        /*! Static cast *this to the Relation & derived type, const version. */
        inline const BelongsToManyType &relation() const noexcept;
    };

    /* public */

    template<class Model, class Related, class PivotType>
    InteractsWithPivotTable<Model, Related, PivotType>::
    ~InteractsWithPivotTable() = default;

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    InteractsWithPivotTable<Model, Related, PivotType>::withPivot(
            const QStringList &columns)
    {
        std::ranges::copy_if(columns, std::back_inserter(relation().m_pivotColumns),
                             [this](const auto &column)
        {
            return !hasPivotColumn(column);
        });

        return relation();
    }

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    InteractsWithPivotTable<Model, Related, PivotType>::withPivot(const QString &column)
    {
        return withPivot(QStringList {column});
    }

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    InteractsWithPivotTable<Model, Related, PivotType>::withPivot(QStringList &&columns) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        for (auto &&column : columns)
            if (!hasPivotColumn(column))
                relation().m_pivotColumns << std::move(column);

        return relation();
    }

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    InteractsWithPivotTable<Model, Related, PivotType>::withPivot(QString &&column)
    {
        return withPivot(QStringList {std::move(column)});
    }

    template<class Model, class Related, class PivotType>
    template<QStringConcept ...Args>
    BelongsToMany<Model, Related, PivotType> &
    InteractsWithPivotTable<Model, Related, PivotType>::withPivot(Args &&...columns)
    {
        return withPivot(QStringList {std::forward<Args>(columns)...}); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

    template<class Model, class Related, class PivotType>
    bool InteractsWithPivotTable<Model, Related, PivotType>::hasPivotColumn(
            const QString &column) const
    {
        return getPivotColumns_().contains(column);
    }

    // FEATURE move semantics, for attributes silverqx
    template<class Model, class Related, class PivotType>
    void InteractsWithPivotTable<Model, Related, PivotType>::attach(
            const QList<QVariant> &ids, const QList<AttributeItem> &attributes,
            const bool touch) const
    {
        // FUTURE add the number of affected/attached models as return value silverqx
        if constexpr (std::is_same_v<PivotType, Pivot>)
            /* Here we will insert the attachment records into the pivot table. Once
               we have inserted the records, we will touch the relationships if
               necessary and the function will return. */
            newPivotStatement()->insert(
                    AttributeUtils::convertVectorsToMaps(
                        formatAttachRecords(ids, attributes)));
        else
            attachUsingCustomClass(ids, attributes);

        if (touch)
            touchIfTouching_();
    }

    template<class Model, class Related, class PivotType>
    void InteractsWithPivotTable<Model, Related, PivotType>::attach(
            const QList<std::reference_wrapper<Related>> &models,
            const QList<AttributeItem> &attributes, const bool touch) const
    {
        attach(getRelatedIds(models), attributes, touch);
    }

    template<class Model, class Related, class PivotType>
    void InteractsWithPivotTable<Model, Related, PivotType>::attach(
            const QVariant &id, const QList<AttributeItem> &attributes,
            const bool touch) const
    {
        attach(QList<QVariant> {id}, attributes, touch);
    }

    template<class Model, class Related, class PivotType>
    void InteractsWithPivotTable<Model, Related, PivotType>::attach(
            const Related &model, const QList<AttributeItem> &attributes,
            const bool touch) const
    {
        attach(QList<QVariant> {model.getAttribute(getRelatedKeyName_())},
               attributes, touch);
    }

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<class Model, class Related, class PivotType>
    void InteractsWithPivotTable<Model, Related, PivotType>::attach(
            const std::map<RelatedKeyType, QList<AttributeItem>> &idsWithAttributes,
            const bool touch) const
    {
        if constexpr (std::is_same_v<PivotType, Pivot>)
            /* Here we will insert the attachment records into the pivot table. Once
               we have inserted the records, we will touch the relationships if
               necessary and the function will return. */
            newPivotStatement()->insert(
                    AttributeUtils::convertVectorsToMaps(
                        formatAttachRecords(idsWithAttributes)));
        else
            attachUsingCustomClass(idsWithAttributes);

        if (touch)
            touchIfTouching_();
    }

    template<class Model, class Related, class PivotType>
    SyncChanges
    InteractsWithPivotTable<Model, Related, PivotType>::sync(
            const std::map<RelatedKeyType, QList<AttributeItem>> &idsWithAttributes,
            const bool detaching) const
    {
        /*! Cast the primary key to the Related::KeyType, for ranges::sort(). */
        const auto castKey = [this](const auto &id)
        {
            return this->template castKey<RelatedKeyType>(id);
        };

        SyncChanges changes;

        /* First we need to attach any of the associated models that are not currently
           in this joining table. We'll spin through the given IDs, checking to see
           if they exist in the vector of current ones, and if not we will insert. */
        auto current = getRelatedIds(getCurrentlyAttachedPivots());

        // Compute different keys, these keys will be detached
        auto ids = idsFromRecords(idsWithAttributes);

        std::ranges::sort(ids, {}, castKey);
        std::ranges::sort(current, {}, castKey);
        auto detach =
                ranges::views::set_difference(current, ids, {}, castKey, castKey) |
                ranges::to<QList<QVariant>>();

        /* Next, we will take the differences of the currents and given IDs and detach
           all of the entities that exist in the "current" vector but are not in the
           vector of the new IDs given to the method which will complete the sync. */
        if (detaching && !detach.isEmpty()) {
            this->detach(detach);

            changes.at(Detached) = std::move(detach);
        }

        /* Now we are finally ready to attach the new records. Note that we'll disable
           touching until after the entire operation is complete so we don't fire a
           ton of touch operations until we are totally done syncing the records. */
        changes.template merge<RelatedKeyType>(
                    attachNew(idsWithAttributes, current, false));

        /* Once we have finished attaching or detaching the records, we will see if we
           have done any attaching or detaching, and if we have we will touch these
           relationships if they are configured to touch on any database updates. */
        if (!changes.at(Attached).isEmpty() || !changes.at(Updated_).isEmpty())
            touchIfTouching_();

        return changes;
    }

    template<class Model, class Related, class PivotType>
    SyncChanges
    InteractsWithPivotTable<Model, Related, PivotType>::sync(
            const QList<QVariant> &ids, const bool detaching) const
    {
        return sync(recordsFromIds(ids), detaching);
    }

    template<class Model, class Related, class PivotType>
    SyncChanges
    InteractsWithPivotTable<Model, Related, PivotType>::syncWithoutDetaching(
            const std::map<RelatedKeyType,
                           QList<AttributeItem>> &idsWithAttributes) const
    {
        return sync(idsWithAttributes, false);
    }

    template<class Model, class Related, class PivotType>
    SyncChanges
    InteractsWithPivotTable<Model, Related, PivotType>::syncWithoutDetaching(
            const QList<QVariant> &ids) const
    {
        return sync(ids, false);
    }

    template<class Model, class Related, class PivotType>
    int InteractsWithPivotTable<Model, Related, PivotType>::detach(
            const QList<QVariant> &ids, const bool touch) const
    {
        return detach(false, ids, touch);
    }

    template<class Model, class Related, class PivotType>
    int InteractsWithPivotTable<Model, Related, PivotType>::detach(
            const QList<std::reference_wrapper<Related>> &models,
            const bool touch) const
    {
        return detach(getRelatedIds(models), touch);
    }

    template<class Model, class Related, class PivotType>
    int InteractsWithPivotTable<Model, Related, PivotType>::detach(
            const QVariant &id, const bool touch) const
    {
        return detach(QList<QVariant> {id}, touch);
    }

    template<class Model, class Related, class PivotType>
    int InteractsWithPivotTable<Model, Related, PivotType>::detach(
            const Related &model, const bool touch) const
    {
        return detach(QList<QVariant> {model.getAttribute(getRelatedKeyName_())},
                      touch);
    }

    template<class Model, class Related, class PivotType>
    int
    InteractsWithPivotTable<Model, Related, PivotType>::detachAll(const bool touch) const
    {
        return detach(true, {}, touch);
    }

    template<class Model, class Related, class PivotType>
    int InteractsWithPivotTable<Model, Related, PivotType>::updateExistingPivot(
            const QVariant &id, QList<AttributeItem> attributes,
            const bool touch) const
    {
        if (!std::is_same_v<PivotType, Pivot>
//            && m_pivotWheres.isEmpty()
//            && m_pivotWhereIns.isEmpty()
//            && m_pivotWhereNulls.isEmpty()
        )
            return updateExistingPivotUsingCustomClass(id, attributes, touch);

        if (hasPivotColumn(updatedAt_()))
            addTimestampsToAttachment(attributes, true);

        int updated = -1;
        std::tie(updated, std::ignore) =
                newPivotStatementForId(id)->update(
                    AttributeUtils::convertVectorToUpdateItem(
                        /* Cast attributes not needed, the if statement above will be
                           executed for the custom pivot and we don't need casting
                           for the non-custom normal pivots. */
                        std::move(attributes)));
//                        castAttributes(std::move(attributes))));
        // FEATURE relations, the castAttributes() will be needed when following task will be done : add support for BelongsToMany::where/whereIn/whereNull silverqx

        /* It will not touch if attributes size is 0, because this function is called
           only when attributes are not empty. */
        if (touch)
            touchIfTouching_();

        return updated;
    }

    template<class Model, class Related, class PivotType>
    int InteractsWithPivotTable<Model, Related, PivotType>::updateExistingPivot(
            const Related &model, const QList<AttributeItem> &attributes,
            const bool touch) const
    {
        return updateExistingPivot(model.getAttribute(getRelatedKeyName_()),
                                   attributes, touch);
    }

    template<class Model, class Related, class PivotType>
    PivotType InteractsWithPivotTable<Model, Related, PivotType>::newExistingPivot(
            const QList<AttributeItem> &attributes) const
    {
        return newPivot(attributes, true);
    }

    template<class Model, class Related, class PivotType>
    PivotType InteractsWithPivotTable<Model, Related, PivotType>::newPivot(
            const QList<AttributeItem> &attributes, const bool exists) const
    {
        return getRelated_().template newPivot<PivotType, Model>(
                    getParent_(), attributes, getTable_(), exists,
                    relation().usesTimestamps(),
                    pivotCreatedAt(), pivotUpdatedAt())

                .setPivotKeys(getForeignPivotKeyName_(), getRelatedPivotKeyName_());
    }

    template<class Model, class Related, class PivotType>
    std::shared_ptr<QueryBuilder>
    InteractsWithPivotTable<Model, Related, PivotType>::newPivotQuery() const
    {
        // Ownership of the std::shared_ptr<QueryBuilder>
        auto query = newPivotStatement();

        // FEATURE relations, add support for BelongsToMany::where/whereIn/whereNull silverqx
//        for (auto &[column, value, comparison, condition] : m_pivotWheres)
//            query->where(column, value, comparison, condition);

//        for (auto &[column, values, condition, nope] : m_pivotWhereIns)
//            query->whereIn(column, values, condition, nope);

//        for (auto &[😭, condition, nope] : m_pivotWhereNulls)
//            query->whereNull(columns, condition, nope);

        query->whereEq(getForeignPivotKeyName_(),
                       getParent_().getAttribute(getParentKeyName_()));

        return query;
    }

    template<class Model, class Related, class PivotType>
    std::shared_ptr<QueryBuilder>
    InteractsWithPivotTable<Model, Related, PivotType>::newPivotStatement() const
    {
        // Ownership of the std::shared_ptr<QueryBuilder>
        auto query = getBaseQuery_().newQuery();

        query->from(getTable_());

        return query;
    }

    template<class Model, class Related, class PivotType>
    std::shared_ptr<QueryBuilder>
    InteractsWithPivotTable<Model, Related, PivotType>::newPivotStatementForId(
            const QList<QVariant> &ids) const
    {
        // Ownership of the std::shared_ptr<QueryBuilder>
        auto query = newPivotQuery();

        query->whereIn(getRelatedPivotKeyName_(), ids);

        return query;
    }

    template<class Model, class Related, class PivotType>
    std::shared_ptr<QueryBuilder>
    InteractsWithPivotTable<Model, Related, PivotType>::newPivotStatementForId(
            const QVariant &id) const
    {
        return newPivotStatementForId(QList<QVariant> {id});
    }

    /* protected */

    template<class Model, class Related, class PivotType>
    void InteractsWithPivotTable<Model, Related, PivotType>::attachUsingCustomClass(
            const QList<QVariant> &ids,
            const QList<AttributeItem> &attributes) const
    {
        for (const auto &record : formatAttachRecords(ids, attributes))
            newPivot(record).save();
    }

    template<class Model, class Related, class PivotType>
    QList<QList<AttributeItem>>
    InteractsWithPivotTable<Model, Related, PivotType>::formatAttachRecords(
            const QList<QVariant> &ids,
            const QList<AttributeItem> &attributes) const
    {
        QList<QList<AttributeItem>> records;
        records.reserve(ids.size());

        const auto hasTimestamps = hasPivotColumn(createdAt_()) ||
                                   hasPivotColumn(updatedAt_());

        /* To create the attachment records, we will simply spin through the IDs given
           and create a new record to insert for each ID with extra attributes to be
           placed in other columns. */
        for (const auto &id : ids)
            records << formatAttachRecord(id, attributes, hasTimestamps);

        return records;
    }

    template<class Model, class Related, class PivotType>
    QList<AttributeItem>
    InteractsWithPivotTable<Model, Related, PivotType>::formatAttachRecord(
            const QVariant &id, const QList<AttributeItem> &attributes,
            const bool hasTimestamps) const
    {
        auto baseAttributes = baseAttachRecord(id, hasTimestamps);
        baseAttributes.reserve(baseAttributes.size() + attributes.size());

        /* I have to add a note here because this casting is really complex, for now,
           it doesn't matter if the castAttributes() is called here.
           First I'm going to describe why castAttributes() must be called,
           it has to be called because the BasePivot::fromRawAttributes() is called
           for the custom pivots and it internally calls the setRawAttributes(), so
           it doesn't call all the casting logic that is in the setAttribute() method.
           Now why is not needed in the TinyORM because we don't have any custom logic
           now in the setAttribute() like the isEnumCastable(), isClassCastable(), or
           isJsonCastable(), so at the end of the day it doesn't matter whether
           the castAttributes() will be called, it will matter when any of these
           will be implemented. 🤯
           Ok, currently it's needed only for the fromDateTime() in the setAttribute(),
           SO it matters and the castAttributes() must be called.
           I leave all the comment above because it nicely describes the whole problem. */
        for (auto &&attribute : castAttributes(attributes)) {
            // NOTE api different silverqx
            validateAttachAttribute(attribute, id);

            baseAttributes << std::move(attribute);
        }

        return baseAttributes;
    }

    template<class Model, class Related, class PivotType>
    void InteractsWithPivotTable<Model, Related, PivotType>::attachUsingCustomClass(
            const std::map<RelatedKeyType,
                           QList<AttributeItem>> &idsWithAttributes) const
    {
        for (const auto &record : formatAttachRecords(idsWithAttributes))
            newPivot(record).save();
    }

    template<class Model, class Related, class PivotType>
    QList<QList<AttributeItem>>
    InteractsWithPivotTable<Model, Related, PivotType>::formatAttachRecords(
            const std::map<RelatedKeyType,
                           QList<AttributeItem>> &idsWithAttributes) const
    {
        QList<QList<AttributeItem>> records;
        records.reserve(static_cast<decltype (records)::size_type>(
                            idsWithAttributes.size()));

        const auto hasTimestamps = hasPivotColumn(createdAt_()) ||
                                   hasPivotColumn(updatedAt_());

        /* To create the attachment records, we will simply spin through the IDs given
           and create a new record to insert for each ID with extra attributes to be
           placed in other columns. */
        for (const auto &[id, attributes] : idsWithAttributes)
            records << formatAttachRecord(id, attributes, hasTimestamps);

        return records;
    }

    template<class Model, class Related, class PivotType>
    QList<AttributeItem>
    InteractsWithPivotTable<Model, Related, PivotType>::baseAttachRecord(
            const QVariant &id, const bool timed) const
    {
        QList<AttributeItem> record;
        // 2 keys and 2 columns for timestamps
        record.reserve(4);

        record.append({getRelatedPivotKeyName_(), id});
        record.append({getForeignPivotKeyName_(),
                       getParent_().getAttribute(getParentKeyName_())});

        /* If the record needs to have creation and update timestamps, we will make
           them by calling the parent model's "freshTimestamp" method, which will
           provide us with a fresh timestamp in this model's preferred format. */
        if (timed)
            addTimestampsToAttachment(record);

        // FEATURE pivot, withPivotValues silverqx
//        for (auto &[column, value] as m_pivotValues)
//            record << column, value;

        return record;
    }

    template<class Model, class Related, class PivotType>
    QList<AttributeItem> &
    InteractsWithPivotTable<Model, Related, PivotType>::addTimestampsToAttachment(
            QList<AttributeItem> &record, const bool exists) const
    {
        // QDateTime
        QVariant fresh = getParent_().freshTimestamp();

        /* If a custom pivot is used, then 'fresh' will be QString, formatted
           datetime as defined in the custom pivot class. */
        if constexpr (!std::is_same_v<PivotType, Pivot>)
            fresh = fresh.toDateTime().toString(PivotType().getDateFormat());

        if (const auto &createdAt = this->createdAt_();
            !exists && hasPivotColumn(createdAt)
        )
            record.append({createdAt, fresh});

        if (const auto &updatedAt = this->updatedAt_();
            hasPivotColumn(updatedAt)
        )
            record.append({updatedAt, fresh});

        return record;
    }

    template<class Model, class Related, class PivotType>
    QList<PivotType>
    InteractsWithPivotTable<Model, Related, PivotType>::getCurrentlyAttachedPivots() const
    {
        auto query = newPivotQuery()->get();

        QList<PivotType> pivots;
        pivots.reserve(QueryUtils::queryResultSize(query));

        while (query.next())
            // std::move() is really needed here
            pivots << std::move(
                          PivotType::fromRawAttributes(
                              getParent_(), attributesFromRecord(query.record()),
                              getTable_(), true, relation().usesTimestamps(),
                              pivotCreatedAt(), pivotUpdatedAt())

                          .setPivotKeys(getForeignPivotKeyName_(),
                                        getRelatedPivotKeyName_()));

        return pivots;
    }

    template<class Model, class Related, class PivotType>
    std::optional<PivotType>
    InteractsWithPivotTable<Model, Related, PivotType>::getAttachedPivot(
            const QVariant &id) const
    {
        const auto query = newPivotStatementForId(id)->first();

        return std::move(
                    PivotType::fromRawAttributes(
                        getParent_(), attributesFromRecord(query.record()),
                        getTable_(), true, relation().usesTimestamps(),
                        pivotCreatedAt(), pivotUpdatedAt())

                    .setPivotKeys(getForeignPivotKeyName_(),
                                  getRelatedPivotKeyName_()));
    }

    template<class Model, class Related, class PivotType>
    QList<AttributeItem>
    InteractsWithPivotTable<Model, Related, PivotType>::attributesFromRecord(
            const TSqlRecord &record) const
    {
        const auto recordsCount = record.count();

        QList<AttributeItem> attributes;
        attributes.reserve(recordsCount);

        for (int i = 0; i < recordsCount; ++i)
            attributes.append({record.fieldName(i), record.value(i)});

        return attributes;
    }

    template<class Model, class Related, class PivotType>
    template<typename T>
    T InteractsWithPivotTable<Model, Related, PivotType>::castKey(
            const QVariant &key) const
    {
        return key.template value<T>();
    }

    template<class Model, class Related, class PivotType>
    int
    InteractsWithPivotTable<Model, Related, PivotType>::
    updateExistingPivotUsingCustomClass(
            const QVariant &id, const QList<AttributeItem> &attributes,
            const bool touch) const
    {
        auto pivot = getAttachedPivot(id);

        const auto updated = pivot ? pivot->fill(attributes).isDirty() : false;

        if (updated)
            pivot->save();

        /* It will not touch if attributes size is 0, this function is only called when
           attributes are not empty, this condition is in the attachNew() method. */
        if (touch)
            touchIfTouching_();

        return updated ? 1 : 0;
    }

    template<class Model, class Related, class PivotType>
    SyncChanges
    InteractsWithPivotTable<Model, Related, PivotType>::attachNew(
            const std::map<RelatedKeyType, QList<AttributeItem>> &records,
            const QList<QVariant> &current, const bool touch) const
    {
        SyncChanges changes;

        for (const auto &[id, attributes] : records) {
            /* If the ID is not in the list of existing pivot IDs, we will insert
               a new pivot record, otherwise, we will just update this existing record
               on this joining table, so that the developers will easily update these
               records pain free. */
            if (!current.contains(id)) {
                attach(id, attributes, touch);

                changes.at(Attached) << id;
            }

            /* If the pivot record already exists, we'll try to update the attributes
               that were given to the method. If the model is actually updated, we will
               add it to the list of updated pivot records, so we return them back
               out to the consumer. */
            else if (!attributes.isEmpty() &&
                     updateExistingPivot(id, attributes, touch)
            )
                changes.at(Updated_) << id;
        }

        return changes;
    }

    template<class Model, class Related, class PivotType>
    std::map<typename InteractsWithPivotTable<Model, Related, PivotType>::RelatedKeyType,
             QList<AttributeItem>>
    InteractsWithPivotTable<Model, Related, PivotType>::recordsFromIds(
            const QList<QVariant> &ids) const
    {
        std::map<RelatedKeyType, QList<AttributeItem>> records;

        for (const auto &id : ids)
            records.try_emplace(castKey<RelatedKeyType>(id));

        return records;
    }

    template<class Model, class Related, class PivotType>
    QList<QVariant>
    InteractsWithPivotTable<Model, Related, PivotType>::idsFromRecords(
            const std::map<RelatedKeyType,
                           QList<AttributeItem>> &idsWithAttributes) const
    {
        QList<QVariant> ids;
        ids.reserve(static_cast<decltype (ids)::size_type>(idsWithAttributes.size()));

        for (const auto &record : idsWithAttributes)
            ids << record.first;

        return ids;
    }

    /* These castAttributes() has very weird params/return values, but they are ok and
       as effective as can be, I'm going to describe every overload to be more clear. */

    /* The following two examples are called from the formatAttachRecord(), the problem
       here is that we don't have the formatAttachRecord() overload that accepts
       the attributes as a rvalue reference. */
    template<class Model, class Related, class PivotType>
    const QList<AttributeItem> &
    InteractsWithPivotTable<Model, Related, PivotType>::castAttributes(
            const QList<AttributeItem> &attributes) const
    requires OurPivot<PivotType>
    {
        static_assert (std::is_same_v<PivotType, Pivot>,
                "Bad castAttribute overload selected, PivotType != Pivot.");

        /* We don't cast attributes for the non-custom pivot because the u_cast data
           member can not be set, it can be set only on the custom pivot. */
        return attributes;
    }

    template<class Model, class Related, class PivotType>
    QList<AttributeItem>
    InteractsWithPivotTable<Model, Related, PivotType>::castAttributes(
            const QList<AttributeItem> &attributes) const
    requires CustomPivot<PivotType>
    {
        static_assert (!std::is_same_v<PivotType, Pivot>,
                "Bad castAttribute overload selected, should be overload "
                "for the custom pivot.");

        return newPivot().fill(attributes).getAttributes();
    }

    /* This overload is called from the updateExistingPivot() and currently is not
       needed, it will be needed when the pivotWheres/pivotWhereIns/pivotWhereNulls
       will be implemented, so I'm currently disabling this overload. */
//    template<class Model, class Related, class PivotType>
//    QList<AttributeItem>
//    InteractsWithPivotTable<Model, Related, PivotType>::castAttributes(
//            QList<AttributeItem> &&attributes) const
//    requires OurPivot<PivotType>
//    {
//        static_assert (std::is_same_v<PivotType, Pivot>,
//                "Bad castAttribute overload selected, PivotType != Pivot.");

//        return std::move(attributes);
//    }

    template<class Model, class Related, class PivotType>
    int InteractsWithPivotTable<Model, Related, PivotType>::detach(
            const bool detachAll, const QList<QVariant> &ids,
            const bool touch) const
    {
        int affected = 0;
        const auto idsAreEmpty = ids.isEmpty();

        if (!detachAll && !idsAreEmpty && !std::is_same_v<PivotType, Pivot>
//            && m_pivotWheres.isEmpty()
//            && m_pivotWhereIns.isEmpty()
//            && m_pivotWhereNulls.isEmpty()
        )
            affected = detachUsingCustomClass(ids);

        else {
            // Ownership of the std::shared_ptr<QueryBuilder>
            auto query = newPivotQuery();

            /* If associated IDs were passed to the method we will only delete those
               associations, otherwise all of the association ties will be broken.
               We'll return the numbers of affected rows when we do the deletes. */
            if (!detachAll) {
                // Nothing to delete/detach
                if (idsAreEmpty)
                    return 0;

                query->whereIn(getRelatedPivotKeyName_(), ids);
            }

            /* Once we have all of the conditions set on the statement, we are ready
               to run the delete on the pivot table. Then, if the touch parameter
               is true, we will go ahead and touch all related models to sync. */
            std::tie(affected, std::ignore) = query->remove();
        }

        if (touch)
            touchIfTouching_();

        return affected;
    }

    template<class Model, class Related, class PivotType>
    int InteractsWithPivotTable<Model, Related, PivotType>::detachUsingCustomClass(
            const QList<QVariant> &ids) const
    {
        int affected = 0;

        for (const auto &id : ids)
            affected += newPivot({
                {getForeignPivotKeyName_(),
                 getParent_().getAttribute(getParentKeyName_())},

                {getRelatedPivotKeyName_(), id},
            }, true).remove();

        return affected;
    }

    template<class Model, class Related, class PivotType>
    QList<QVariant>
    InteractsWithPivotTable<Model, Related, PivotType>::getRelatedIds(
            const QList<std::reference_wrapper<Related>> &models) const
    {
        QList<QVariant> ids;
        ids.reserve(models.size());

        for (const auto &model : models)
            ids << model.get().getAttribute(getRelatedKeyName_());

        return ids;
    }

    template<class Model, class Related, class PivotType>
    QList<QVariant>
    InteractsWithPivotTable<Model, Related, PivotType>::getRelatedIds(
            const QList<PivotType> &pivots) const
    {
        QList<QVariant> ids;
        ids.reserve(pivots.size());

        for (const auto &pivot : pivots)
            ids << pivot.getAttribute(getRelatedPivotKeyName_());

        return ids;
    }

    /* private */

    template<class Model, class Related, class PivotType>
    void InteractsWithPivotTable<Model, Related, PivotType>::validateAttachAttribute(
            const AttributeItem &attribute, const QVariant &id) const
    {
        // Don't overwrite ID keys, throw domain exception
        if (attribute.key == getForeignPivotKeyName_())
            // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
            throwOverwritingKeyError/*<ParentKeyType>*/(
                        attribute.key,
                        getParent_().getAttribute(getParentKeyName_()),
                        attribute.value);

        else if (attribute.key == getRelatedPivotKeyName_())
            throwOverwritingKeyError/*<RelatedKeyType>*/(attribute.key, id,
                                                         attribute.value);
    }

    template<class Model, class Related, class PivotType>
    template<typename KeyType>
    void InteractsWithPivotTable<Model, Related, PivotType>::throwOverwritingKeyError(
            const QString &key, const QVariant &original,
            const QVariant &overwrite) const
    {
        static const auto overwriteMessage =
                QStringLiteral("You can not overwrite '%1' ID key; "
                               "original value : %2, your value : %3.");

        throw Orm::Exceptions::DomainError(
                    overwriteMessage.arg(
                        qualifyPivotColumn_(key),
                        QString::number(original.template value<KeyType>()),
                        QString::number(overwrite.template value<KeyType>())));
    }

    /* Getters proxy methods (for shorter code) */

    /* Relation proxy methods */

    /* Getters */

    template<class Model, class Related, class PivotType>
    QueryBuilder &
    InteractsWithPivotTable<Model, Related, PivotType>::getBaseQuery_() const noexcept
    {
        return relation().getBaseQuery();
    }

    template<class Model, class Related, class PivotType>
    const Model &
    InteractsWithPivotTable<Model, Related, PivotType>::getParent_() const noexcept
    {
        return relation().getParent();
    }

    template<class Model, class Related, class PivotType>
    const Related &
    InteractsWithPivotTable<Model, Related, PivotType>::getRelated_() const noexcept
    {
        return relation().getRelated();
    }

    template<class Model, class Related, class PivotType>
    const QString &
    InteractsWithPivotTable<Model, Related, PivotType>::
    getRelatedKeyName_() const noexcept
    {
        return relation().getRelatedKeyName();
    }

    /* BelongsToMany proxy methods */

    /* Getters */

    template<class Model, class Related, class PivotType>
    const QString &
    InteractsWithPivotTable<Model, Related, PivotType>::getTable_() const noexcept
    {
        return relation().getTable();
    }

    template<class Model, class Related, class PivotType>
    const QString &
    InteractsWithPivotTable<Model, Related, PivotType>::
    getForeignPivotKeyName_() const noexcept
    {
        return relation().getForeignPivotKeyName();
    }

    template<class Model, class Related, class PivotType>
    const QString &
    InteractsWithPivotTable<Model, Related, PivotType>::
    getRelatedPivotKeyName_() const noexcept
    {
        return relation().getRelatedPivotKeyName();
    }

    template<class Model, class Related, class PivotType>
    const QString &
    InteractsWithPivotTable<Model, Related, PivotType>::getParentKeyName_() const noexcept
    {
        return relation().getParentKeyName();
    }

    template<class Model, class Related, class PivotType>
    const QStringList &
    InteractsWithPivotTable<Model, Related, PivotType>::getPivotColumns_() const noexcept
    {
        return relation().getPivotColumns();
    }

    /* Timestamps */

    template<class Model, class Related, class PivotType>
    const QString &
    InteractsWithPivotTable<Model, Related, PivotType>::createdAt_() const
    {
        return relation().createdAt();
    }

    template<class Model, class Related, class PivotType>
    const QString &
    InteractsWithPivotTable<Model, Related, PivotType>::updatedAt_() const
    {
        return relation().updatedAt();
    }

    template<class Model, class Related, class PivotType>
    void InteractsWithPivotTable<Model, Related, PivotType>::touchIfTouching_() const
    {
        return relation().touchIfTouching();
    }

    template<class Model, class Related, class PivotType>
    const QString &
    InteractsWithPivotTable<Model, Related, PivotType>::pivotCreatedAt() const
    {
        return relation().m_pivotCreatedAt;
    }

    template<class Model, class Related, class PivotType>
    const QString &
    InteractsWithPivotTable<Model, Related, PivotType>::pivotUpdatedAt() const
    {
        return relation().m_pivotUpdatedAt;
    }

    /* Others */

    template<class Model, class Related, class PivotType>
    QString
    InteractsWithPivotTable<Model, Related, PivotType>::qualifyPivotColumn_(
            const QString &column) const
    {
        return relation().qualifyPivotColumn(column);
    }

    /* InteractsWithPivotTable */

    template<class Model, class Related, class PivotType>
    BelongsToMany<Model, Related, PivotType> &
    InteractsWithPivotTable<Model, Related, PivotType>::relation() noexcept
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
        return static_cast<BelongsToManyType &>(*this);
    }

    template<class Model, class Related, class PivotType>
    const BelongsToMany<Model, Related, PivotType> &
    InteractsWithPivotTable<Model, Related, PivotType>::relation() const noexcept
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
        return static_cast<const BelongsToManyType &>(*this);
    }

} // namespace Concerns
} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_CONCERNS_INTERACTSWITHPIVOTTABLE_HPP
