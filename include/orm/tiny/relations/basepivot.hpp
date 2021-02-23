#ifndef BASEPIVOT_HPP
#define BASEPIVOT_HPP

#include "orm/tiny/basemodel.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Relations
{

    /*! The tag for the Pivot model. */
    class IsPivotModel
    {};

    /*! Base class for Pivot models. */
    template<typename PivotModel>
    class BasePivot : public BaseModel<PivotModel>, public IsPivotModel
    {
    public:
        friend BaseModel<PivotModel>;

        using BaseModel<PivotModel>::BaseModel;

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

        // TODO fuckup, timestamps in pivot, I will solve it when I will have to use timestamps in the code, anyway may be I will not need it, because I can pass to the method right away what I will need silverqx
        /*! The parent model of the relationship. */
//        inline static const Parent *pivotParent = nullptr;

    protected:
        /*! Indicates if the ID is auto-incrementing. */
        bool u_incrementing = false;
        // TODO guarded silverqx
        /*! The attributes that aren't mass assignable. */
//        QStringList u_guarded;

        /*! The name of the foreign key column. */
        QString m_foreignKey;
        /*! The name of the "other key" column. */
        QString m_relatedKey;
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
        return ranges::contains(attributes, true,
                                [&createdAtColumn = this->getCreatedAtColumn()]
                                (const auto &attribute)
       {
           return attribute.key == createdAtColumn;
       });;
    }

    template<typename PivotModel>
    bool BasePivot<PivotModel>::hasTimestampAttributes() const
    {
        return hasTimestampAttributes(this->m_attributes);
    }

} // namespace Orm::Tiny::Relations
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // BASEPIVOT_HPP
