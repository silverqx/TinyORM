#pragma once
#ifndef ORM_TINY_RELATIONS_BASEPIVOT_HPP
#define ORM_TINY_RELATIONS_BASEPIVOT_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/model.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{

    /*! Tag for the Pivot model. */
    class IsPivotModel
    {};

    /*! Base class for Pivot models. */
    template<typename PivotModel>
    class BasePivot : public Model<PivotModel>, // NOLINT(bugprone-exception-escape, misc-no-recursion)
                      public IsPivotModel
    {
        friend Model<PivotModel>;

        /*! Alias for the string utils. */
        using StringUtils = Orm::Utils::String;
        /*! Alias for the type utils. */
        using TypeUtils = Orm::Utils::Type;

    public:
        /*! Inherit constructors. */
        using Model<PivotModel>::Model;

        /* AsPivot */
        /*! Create a new pivot model instance. */
        template<typename Parent>
        static PivotModel
        fromAttributes(const Parent &parent, const QList<AttributeItem> &attributes,
                       const QString &table, bool exists = false,
                       bool withTimestamps = false,
                       const QString &createdAt = Constants::CREATED_AT,
                       const QString &updatedAt = Constants::UPDATED_AT);
        template<typename Parent>
        /*! Create a new pivot model from raw values returned from a query. */
        static PivotModel
        fromRawAttributes(const Parent &parent, const QList<AttributeItem> &attributes,
                          const QString &table, bool exists = false,
                          bool withTimestamps = false,
                          const QString &createdAt = Constants::CREATED_AT,
                          const QString &updatedAt = Constants::UPDATED_AT);

        /*! Determine if the pivot model or given attributes has timestamp attributes. */
        static bool hasTimestampAttributes(const QList<AttributeItem> &attributes);
        /*! Determine if the pivot model or given attributes has timestamp attributes. */
        bool hasTimestampAttributes() const;

        /* Hide methods from a Model<PivotModel> */
        /* Different implementation than the Model and instead of make them virtual
           I used the CRTP pattern in the Model to properly call them, I can not use
           virtual here, because of the Virtual Friend Function Idiom, it would
           be very disarranged. */
        /*! Delete the pivot model record from the database. */
        bool remove();
        /*! Delete the pivot model record from the database (alias). */
        inline bool deleteModel();

        /* Getters / Setters */
        /*! Get the table associated with the model. */
        inline QString getTable() const;
        /*! Get the foreign key column name. */
        const QString &getForeignKey() const noexcept;
        /*!  Get the "related key" column name. */
        const QString &getRelatedKey() const noexcept;

        /*! Set the key names for the pivot model instance. */
        PivotModel &setPivotKeys(const QString &foreignKey, const QString &relatedKey);

        /*! Determine whether the PivotType is a custom pivot. */
        constexpr static bool isCustomPivot();

    protected:
        /* AsPivot */
        /*! Set the keys for a select query. */
        TinyBuilder<PivotModel> &
        setKeysForSelectQuery(TinyBuilder<PivotModel> &query) const;
        /*! Set the keys for a save update query. */
        inline TinyBuilder<PivotModel> &
        setKeysForSaveQuery(TinyBuilder<PivotModel> &query) const;

        /*! Get the query builder for a delete operation on the pivot. */
        std::unique_ptr<TinyBuilder<PivotModel>> getDeleteQuery() const;

        /*! Merge attributes from the database to the original (default) attributes. */
        static QList<AttributeItem>
        mergeAttributes(const QList<AttributeItem> &rawOriginals,
                        const QList<AttributeItem> &attributes);

        /* BasePivot */
        /*! Indicates if the ID is auto-incrementing. */
        bool u_incrementing = false;
        /*! The attributes that aren't mass assignable. */
        T_THREAD_LOCAL
        inline static QStringList u_guarded;

        /* AsPivot */
        /*! The name of the foreign key column. */
        QString m_foreignKey;
        /*! The name of the "other key" column. */
        QString m_relatedKey;

    private:
        /* AsPivot */
        /*! Set timestamp column names from a parent if they are not the same. */
        template<typename Parent>
        static void
        syncTimestampsFromParent(bool withTimestamps, const QString &createdAt,
                                 const QString &updatedAt) noexcept;
    };

    /* public */

    /* AsPivot */

    /* Custom Pivot models only:
       Below is true, only when obtaining pivot records from the database during
       the lazy or eager loading.
       It's not true eg. if you call Tagged::create()/save()/update()/..., in all this
       cases the Ignored u_xyz data members are taken into account normally❗

       Ignored  : u_connection, u_timestamps, CREATED_AT(), UPDATED_AT()
       Accepted : u_attributes, u_dates, u_dateFormat, u_fillable, u_guarded,
                  u_incrementing, u_table

       Notes : u_connection - inferred from the parent model
               u_timestamps - true if obtained pivot attributes contain both CREATED_AT
                              and UPDATED_AT
               CREATED/UPDATED_AT - inferred from the parent model, can be overridden
                                    using the withTimestamps() method
    */

    // NOTE api different, passing down a pivot timestamps data silverqx
    template<typename PivotModel>
    template<typename Parent>
    PivotModel
    BasePivot<PivotModel>::fromAttributes(
            const Parent &parent, const QList<AttributeItem> &attributes,
            const QString &table, const bool exists, const bool withTimestamps,
            const QString &createdAt, const QString &updatedAt)
    {
        // Set timestamp column names from the parent if they are not the same
        syncTimestampsFromParent<Parent>(withTimestamps, createdAt, updatedAt);

        auto instance = PivotModel::instance();

        /* I will not store a pointer to the parent model because we don't have
           the reference to it, the parent argument is the reference to a copy and
           this copy will be destroyed before the BTM relationship method returns. */

        // Guess whether the pivot uses timestamps (u_timestamps)
        instance.setUseTimestamps(instance.hasTimestampAttributes(attributes));

        /* The pivot model is a "dynamic" model since we will set the tables dynamically
           for the instance. This allows it work for any intermediate tables for the
           many to many relationship that are defined by this developer's classes. */
        instance.setConnection(parent.getConnectionName())
            .setTable(table)
            .forceFill(attributes)
            .syncOriginal();

        instance.exists = exists;

        return instance;
    }

    // NOTE api different, passing down a pivot timestamps data silverqx
    template<typename PivotModel>
    template<typename Parent>
    PivotModel
    BasePivot<PivotModel>::fromRawAttributes(
            const Parent &parent, const QList<AttributeItem> &attributes,
            const QString &table, const bool exists, const bool withTimestamps,
            const QString &createdAt, const QString &updatedAt)
    {
        auto instance = fromAttributes(parent, {}, table, exists, withTimestamps,
                                       createdAt, updatedAt);

        // Guess whether the pivot uses timestamps (u_timestamps)
        instance.setUseTimestamps(instance.hasTimestampAttributes(attributes));

        // Support Default Attribute Values
        // No merge needed, default attributes are empty
        if (const auto &rawOriginals = instance.getRawOriginals();
            rawOriginals.isEmpty()
        )
            instance.setRawAttributes(attributes, exists);

        // Merge a new attributes from the database to the default attributes
        else
            instance.setRawAttributes(mergeAttributes(rawOriginals, attributes), exists);

        return instance;
    }

    template<typename PivotModel>
    bool BasePivot<PivotModel>::hasTimestampAttributes(
            const QList<AttributeItem> &attributes)
    {
        // NOTE api different, has to contain both timestamp columns silverqx
        auto hasCreatedAt = false;
        auto hasUpdatedAt = false;

        for (const auto &attribute : attributes) {
            const auto &attributeKey = attribute.key;

            if (attributeKey == PivotModel::CREATED_AT())
                hasCreatedAt = true;
            else if (attributeKey == PivotModel::UPDATED_AT())
                hasUpdatedAt = true;
        }

        return hasCreatedAt && hasUpdatedAt;
    }

    template<typename PivotModel>
    bool BasePivot<PivotModel>::hasTimestampAttributes() const
    {
        return hasTimestampAttributes(this->m_attributes);
    }

    /* Hide methods from a Model<PivotModel> */

    // NOTE api different silverqx
    template<typename PivotModel>
    bool BasePivot<PivotModel>::remove()
    {
        /* If a primary key is defined on the current Pivot model, we can use
           Model's 'remove' method, otherwise we have to build a query with
           the help of QueryBuilder's 'where' method. */
        if (this->m_attributesHash.contains(this->getKeyName()))
            return Model<PivotModel>::remove();

        // FEATURE events silverqx
//        if (fireModelEvent("deleting") == false)
//            return false;

        this->touchOwners();

        // Ownership of a unique_ptr()
        int affected = 0;
        std::tie(affected, std::ignore) = getDeleteQuery()->remove();

        this->exists = false;

        // FEATURE events silverqx
//        fireModelEvent("deleted", false);

        return affected > 0;
    }

    template<typename PivotModel>
    bool BasePivot<PivotModel>::deleteModel()
    {
        return remove();
    }

    /* Getters / Setters */

    template<typename PivotModel>
    QString BasePivot<PivotModel>::getTable() const
    {
        // NOTE api different, if the u_table is not defined on the Custom Pivot model, then return the guessed table name from the HasRelationships::pivotTableName() silverqx
        return this->model().u_table;
    }

    template<typename PivotModel>
    const QString &BasePivot<PivotModel>::getForeignKey() const noexcept
    {
        return m_foreignKey;
    }

    template<typename PivotModel>
    const QString &BasePivot<PivotModel>::getRelatedKey() const noexcept
    {
        return m_relatedKey;
    }

    template<typename PivotModel>
    PivotModel &
    BasePivot<PivotModel>::setPivotKeys(const QString &foreignKey,
                                        const QString &relatedKey)
    {
        m_foreignKey = foreignKey;

        m_relatedKey = relatedKey;

        return this->model();
    }

    template<typename PivotModel>
    constexpr bool BasePivot<PivotModel>::isCustomPivot()
    {
        return !std::is_same_v<PivotModel, Relations::Pivot>;
    }

    /* protected */

    /* AsPivot */

    template<typename PivotModel>
    TinyBuilder<PivotModel> &
    BasePivot<PivotModel>::setKeysForSelectQuery(TinyBuilder<PivotModel> &query) const
    {
        /* If the pivot table contains a primary key then use this primary key
           in the where clause. */
        if (const auto &primaryKeyName = this->getKeyName();
            this->m_attributesHash.contains(primaryKeyName)
        )
            // Also check if this primary key is valid
            if (const auto id = this->getKeyForSelectQuery();
                id.isValid() && !id.isNull()
            )
                return Model<PivotModel>::setKeysForSelectQuery(query);

        // NOTE api different, we are using parenthesis around the following keys, I think it's a little safer silverqx
        return query.where({
            {m_foreignKey, this->getOriginal(m_foreignKey,
                                             this->getAttribute(m_foreignKey))},
            {m_relatedKey, this->getOriginal(m_relatedKey,
                                             this->getAttribute(m_relatedKey))},
        });
    }

    template<typename PivotModel>
    TinyBuilder<PivotModel> &
    BasePivot<PivotModel>::setKeysForSaveQuery(TinyBuilder<PivotModel> &query) const
    {
        return setKeysForSelectQuery(query);
    }

    template<typename PivotModel>
    std::unique_ptr<TinyBuilder<PivotModel>>
    BasePivot<PivotModel>::getDeleteQuery() const
    {
        // Ownership of a unique_ptr()
        auto builder = this->newQueryWithoutRelationships();

        builder->where({
            {m_foreignKey, this->getOriginal(m_foreignKey,
                                             this->getAttribute(m_foreignKey))},
            {m_relatedKey, this->getOriginal(m_relatedKey,
                                             this->getAttribute(m_relatedKey))},
        });

        return builder;
    }

    template<typename PivotModel>
    QList<AttributeItem>
    BasePivot<PivotModel>::mergeAttributes(const QList<AttributeItem> &rawOriginals,
                                           const QList<AttributeItem> &attributes)
    {
        QList<AttributeItem> mergedAttributes;
        mergedAttributes.reserve(rawOriginals.size() + attributes.size());

        mergedAttributes = rawOriginals;

        const auto keyProj = [](const auto &attribute)
        {
            return attribute.key;
        };

        for (const auto &attribute : attributes) {
            const auto it = std::ranges::find(mergedAttributes, attribute.key, keyProj);

            // Doesn't contain, so append
            if (it == mergedAttributes.cend())
                mergedAttributes << attribute;
            // Already contains, update value
            else
                it->value = attribute.value;
        }

        return mergedAttributes;
    }

    /* AsPivot */

    /* private */

    template<typename PivotModel>
    template<typename Parent>
    void BasePivot<PivotModel>::syncTimestampsFromParent(
            const bool withTimestamps, const QString &createdAt,
            const QString &updatedAt) noexcept
    {
        // NOTE api different, timestamp column names passed to the withTimestamps() are also considered silverqx
        /* Pivot timestamp column names are configured/set using the withTimestamps()
           method or they will be inferred from the parent model if they are not set.
           The logic has to be the same as in the BelongsToMany::createdAt/updatedAt
           methods! */

        // Configured using the withTimestamps() method
        if (withTimestamps) {
            if (createdAt != CREATED_AT)
                const_cast<QString &>(CREATED_AT) = createdAt; // NOLINT(cppcoreguidelines-pro-type-const-cast)

            if (updatedAt != UPDATED_AT)
                const_cast<QString &>(UPDATED_AT) = updatedAt; // NOLINT(cppcoreguidelines-pro-type-const-cast)
        }

        // Inferre from the parent model
        else {
            if (const auto &parentCreatedAt = Parent::getCreatedAtColumn();
                parentCreatedAt != CREATED_AT
            )
                const_cast<QString &>(CREATED_AT) = parentCreatedAt; // NOLINT(cppcoreguidelines-pro-type-const-cast)

            if (const auto &parentUpdatedAt = Parent::getUpdatedAtColumn();
                parentUpdatedAt != UPDATED_AT
            )
                const_cast<QString &>(UPDATED_AT) = parentUpdatedAt; // NOLINT(cppcoreguidelines-pro-type-const-cast)
        }
    }

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_BASEPIVOT_HPP
