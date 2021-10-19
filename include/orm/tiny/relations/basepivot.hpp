#pragma once
#ifndef BASEPIVOT_HPP
#define BASEPIVOT_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/model.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Relations
{

    /*! The tag for the Pivot model. */
    class IsPivotModel
    {};

    /*! Base class for Pivot models. */
    template<typename PivotModel>
    class BasePivot : public Model<PivotModel>, public IsPivotModel
    {
    public:
        friend Model<PivotModel>;

        /*! Inherit constructors. */
        using Model<PivotModel>::Model;

        /* AsPivot */
        /*! Create a new pivot model instance. */
        template<typename Parent>
        static PivotModel
        fromAttributes(const Parent &parent, const QVector<AttributeItem> &attributes,
                       const QString &table, bool exists = false);
        template<typename Parent>
        /*! Create a new pivot model from raw values returned from a query. */
        static PivotModel
        fromRawAttributes(const Parent &parent, const QVector<AttributeItem> &attributes,
                          const QString &table, bool exists = false);

        /*! Set the key names for the pivot model instance. */
        PivotModel &setPivotKeys(const QString &foreignKey, const QString &relatedKey);
        /*! Determine if the pivot model or given attributes has timestamp attributes. */
        bool hasTimestampAttributes(const QVector<AttributeItem> &attributes) const;
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
        bool deleteModel();

        /*! Set the keys for a save update query. */
        TinyBuilder<PivotModel> &
        setKeysForSaveQuery(TinyBuilder<PivotModel> &query);
        /*! Set the keys for a select query. */
        TinyBuilder<PivotModel> &
        setKeysForSelectQuery(TinyBuilder<PivotModel> &query);

        /*! Get the table associated with the model. */
        QString getTable() const;
        /*! Get the foreign key column name. */
        QString getForeignKey() const;
        /*!  Get the "related key" column name. */
        QString getRelatedKey() const;


        // TODO fuckup, timestamps in pivot, I will solve it when I will have to use timestamps in the code, anyway may be I will not need it, because I can pass to the method right away what I will need silverqx
        // TODO also don't forget unsetRelations() if pivotParent will be implemented silverqx
        /*! The parent model of the relationship. */
//        inline static const Parent *pivotParent = nullptr;

    protected:
        /* AsPivot */
        /*! Get the query builder for a delete operation on the pivot. */
        std::unique_ptr<TinyBuilder<PivotModel>> getDeleteQuery();

        /* BasePivot */
        /*! Indicates if the ID is auto-incrementing. */
        bool u_incrementing = false;
        /*! The attributes that aren't mass assignable. */
        inline static QStringList u_guarded = {};

        /* AsPivot */
        /*! The name of the foreign key column. */
        QString m_foreignKey = {};
        /*! The name of the "other key" column. */
        QString m_relatedKey = {};
    };

    template<typename PivotModel>
    template<typename Parent>
    PivotModel
    BasePivot<PivotModel>::fromAttributes(
            const Parent &parent, const QVector<AttributeItem> &attributes,
            const QString &table, const bool exists)
    {
        PivotModel instance;

        instance.setUseTimestamps(instance.hasTimestampAttributes(attributes));

        /* The pivot model is a "dynamic" model since we will set the tables dynamically
           for the instance. This allows it work for any intermediate tables for the
           many to many relationship that are defined by this developer's classes. */
        instance.setConnection(parent.getConnectionName())
            .setTable(table)
            .forceFill(attributes)
            .syncOriginal();

        /* We store off the parent instance so we will access the timestamp column names
           for the model, since the pivot model timestamps aren't easily configurable
           from the developer's point of view. We can use the parents to get these. */
//        PivotModel::template pivotParent = &parent;

        instance.exists = exists;

        return instance;
    }

    template<typename PivotModel>
    template<typename Parent>
    PivotModel
    BasePivot<PivotModel>::fromRawAttributes(
            const Parent &parent, const QVector<AttributeItem> &attributes,
            const QString &table, const bool exists)
    {
        auto instance = fromAttributes(parent, {}, table, exists);

        instance.setUseTimestamps(instance.hasTimestampAttributes(attributes));

        instance.setRawAttributes(attributes, exists);

        return instance;
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
    bool BasePivot<PivotModel>::hasTimestampAttributes(
            const QVector<AttributeItem> &attributes) const
    {
        const auto &createdAtColumn = this->getCreatedAtColumn();

        for (const auto &attribute : attributes)
            if (attribute.key == createdAtColumn)
                return true;

        return false;
    }

    template<typename PivotModel>
    bool BasePivot<PivotModel>::hasTimestampAttributes() const
    {
        return hasTimestampAttributes(this->m_attributes);
    }

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

        return affected > 0 ? true : false;
    }

    template<typename PivotModel>
    inline bool BasePivot<PivotModel>::deleteModel()
    {
        return this->model().remove();
    }

    template<typename PivotModel>
    TinyBuilder<PivotModel> &
    BasePivot<PivotModel>::setKeysForSaveQuery(TinyBuilder<PivotModel> &query)
    {
        return this->model().setKeysForSelectQuery(query);
    }

    template<typename PivotModel>
    TinyBuilder<PivotModel> &
    BasePivot<PivotModel>::setKeysForSelectQuery(TinyBuilder<PivotModel> &query)
    {
        // TODO now isset also check for NULL, so I have to check for QVariant::isNull/isValid too silverqx
        if (this->m_attributesHash.contains(this->getKeyName()))
            return Model<PivotModel>::setKeysForSelectQuery(query);

        query.whereEq(m_foreignKey,
                      this->getOriginal(m_foreignKey, this->getAttribute(m_foreignKey)));

        return query.whereEq(m_relatedKey,
                             this->getOriginal(m_relatedKey,
                                               this->getAttribute(m_relatedKey)));
    }

    template<typename PivotModel>
    QString BasePivot<PivotModel>::getTable() const
    {
        const auto &table = this->model().u_table;

        // Get singularizes snake-case table name
        if (table.isEmpty())
            return Utils::String::singular(
                        Utils::String::toSnake(
                            Utils::Type::classPureBasename<PivotModel>()));

        return table;
    }

    template<typename PivotModel>
    QString BasePivot<PivotModel>::getForeignKey() const
    {
        return m_foreignKey;
    }

    template<typename PivotModel>
    QString BasePivot<PivotModel>::getRelatedKey() const
    {
        return m_relatedKey;
    }

    template<typename PivotModel>
    std::unique_ptr<TinyBuilder<PivotModel>>
    BasePivot<PivotModel>::getDeleteQuery()
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

} // namespace Orm::Tiny::Relations

TINYORM_END_COMMON_NAMESPACE

#endif // BASEPIVOT_HPP
