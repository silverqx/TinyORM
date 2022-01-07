#pragma once
#ifndef ORM_TINY_CONCERNS_HASTIMESTAMPS_HPP
#define ORM_TINY_CONCERNS_HASTIMESTAMPS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QDateTime>
#include <QStringList>

#include "orm/tiny/macros/crtpmodelwithbase.hpp"
#include "orm/tiny/tinyconcepts.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{

    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model;

namespace Concerns
{

    /*! Model timestamps. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HasTimestamps
    {
    public:
        /*! Update the model's update timestamp. */
        bool touch();
        /*! Update the creation and update timestamps. */
        void updateTimestamps();

        /*! Set the value of the "created at" attribute. */
        Derived &setCreatedAt(const QDateTime &value);
        /*! Set the value of the "updated at" attribute. */
        Derived &setUpdatedAt(const QDateTime &value);

        /*! Get a fresh timestamp for the model. */
        inline QDateTime freshTimestamp() const;
        /*! Get a fresh timestamp for the model. */
        inline QString freshTimestampString() const;

        /*! Determine if the model uses timestamps. */
        inline bool usesTimestamps() const;
        /*! Set the value of the u_timestamps attribute. */
        inline Derived &setUseTimestamps(bool value);

        /*! Get the name of the "created at" column. */
        inline static const QString &getCreatedAtColumn();
        /*! Get the name of the "updated at" column. */
        inline static const QString &getUpdatedAtColumn();

        /*! Obtain timestamp column names. */
        inline static const QStringList &timestampColumnNames();

        /*! Get the fully qualified "created at" column. */
        QString getQualifiedCreatedAtColumn() const;
        /*! Get the fully qualified "updated at" column. */
        QString getQualifiedUpdatedAtColumn() const;

        /*! Indicates whether the model should be timestamped. */
        bool u_timestamps = true;

    private:
        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_WITH_BASE_DECLARATIONS
    };

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasTimestamps<Derived, AllRelations...>::touch()
    {
        if (!usesTimestamps())
            return false;

        updateTimestamps();

        return basemodel().save();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasTimestamps<Derived, AllRelations...>::updateTimestamps()
    {
        const auto time = freshTimestamp();

        const QString &updatedAtColumn = getUpdatedAtColumn();

        if (!updatedAtColumn.isEmpty() && !basemodel().isDirty(updatedAtColumn))
            setUpdatedAt(time);

        const QString &createdAtColumn = getCreatedAtColumn();

        if (!basemodel().exists && !createdAtColumn.isEmpty() &&
            !basemodel().isDirty(createdAtColumn)
        )
            setCreatedAt(time);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasTimestamps<Derived, AllRelations...>::setCreatedAt(const QDateTime &value)
    {
        return basemodel().setAttribute(getCreatedAtColumn(), value);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasTimestamps<Derived, AllRelations...>::setUpdatedAt(const QDateTime &value)
    {
        return basemodel().setAttribute(getUpdatedAtColumn(), value);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QDateTime
    HasTimestamps<Derived, AllRelations...>::freshTimestamp() const
    {
        return QDateTime::currentDateTime();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    HasTimestamps<Derived, AllRelations...>::freshTimestampString() const
    {
        return basemodel().fromDateTime(freshTimestamp());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasTimestamps<Derived, AllRelations...>::usesTimestamps() const
    {
        return basemodel().getUserTimestamps();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasTimestamps<Derived, AllRelations...>::setUseTimestamps(const bool value)
    {
        basemodel().getUserTimestamps() = value;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    HasTimestamps<Derived, AllRelations...>::getCreatedAtColumn()
    {
        return Model<Derived, AllRelations...>::getUserCreatedAtColumn();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    HasTimestamps<Derived, AllRelations...>::getUpdatedAtColumn()
    {
        return Model<Derived, AllRelations...>::getUserUpdatedAtColumn();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    HasTimestamps<Derived, AllRelations...>::timestampColumnNames()
    {
        /* Fuckin static, it works like is described here:
           https://stackoverflow.com/questions/2737013/static-variables-in-static-method-in-base-class-and-inheritance. */
        static const QStringList cached {
            getCreatedAtColumn(),
            getUpdatedAtColumn(),
        };

        return cached;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    HasTimestamps<Derived, AllRelations...>::getQualifiedCreatedAtColumn() const
    {
        return basemodel().qualifyColumn(getCreatedAtColumn());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    HasTimestamps<Derived, AllRelations...>::getQualifiedUpdatedAtColumn() const
    {
        return basemodel().qualifyColumn(getUpdatedAtColumn());
    }

    /* Static cast this to a child's instance type (CRTP) */
    TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(HasTimestamps)

} // namespace Concerns
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_HASTIMESTAMPS_HPP
