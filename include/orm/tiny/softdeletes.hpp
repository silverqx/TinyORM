#pragma once
#ifndef ORM_TINY_SOFTDELETES_HPP
#define ORM_TINY_SOFTDELETES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include "orm/ormtypes.hpp"
#include "orm/tiny/tinyconcepts.hpp"
#include "orm/utils/helpers.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{

    /*! Model Soft Deleting. */
    template<typename Derived>
    class SoftDeletes
    {
        /*! Alias for the helper utils. */
        using Helpers = Orm::Utils::Helpers;

    public:
        /*! Default constructor. */
        inline SoftDeletes() = default;
        /*! Default destructor. */
        inline ~SoftDeletes() = default;

        /*! Initialize the SoftDeletes (add the deleted_at column to the u_dates). */
        void initializeSoftDeletes() const;

        /*! Force a hard delete on a soft deletable model. */
        bool forceDelete();
        /*! Force a hard delete on a soft deletable model, alias. */
        inline bool forceRemove();

        /*! Restore a soft-deleted model instance. */
        bool restore();

        /*! Determine if the model instance has been soft-deleted. */
        bool trashed() const;

        /*! Determine if the model is currently force deleting. */
        inline bool isForceDeleting() const noexcept;

        /*! Get the name of the "deleted at" column. */
        inline static const QString &getDeletedAtColumn() noexcept;
        /*! Get the fully qualified name of the "deleted at" column. */
        QString getQualifiedDeletedAtColumn() const;

    protected:
        /*! The name of the "created at" column. */
        inline static const QString &DELETED_AT = Constants::DELETED_AT; // NOLINT(cppcoreguidelines-interfaces-global-init)

        /*! Perform the actual delete query on this model instance. */
        void performDeleteOnModel();
        /*! Perform the actual delete query on this model instance. */
        void runSoftDelete();

        /*! Indicates if the model is currently force deleting. */
        bool m_forceDeleting = false;

    private:
        /*! Static cast this to a child's instance type (CRTP). */
        inline Derived &model();
        /*! Static cast this to a child's instance type (CRTP), const version. */
        inline const Derived &model() const;
    };

    /* public */

    template<typename Derived>
    void SoftDeletes<Derived>::initializeSoftDeletes() const
    {
        const auto &deletedAtColumn = getDeletedAtColumn();

        if (model().getDates().contains(deletedAtColumn))
            return;

        model().appendToUserDates(deletedAtColumn);
    }

    template<typename Derived>
    bool SoftDeletes<Derived>::forceDelete()
    {
        m_forceDeleting = true;

        return Helpers::tap<bool>(model().remove(), [this]
        {
            m_forceDeleting = false;
        });
    }

    template<typename Derived>
    bool SoftDeletes<Derived>::forceRemove()
    {
        return forceDelete();
    }

    template<typename Derived>
    bool SoftDeletes<Derived>::restore()
    {
        auto &model = this->model();

        // NOTE api different silverqx
        if (!model.exists)
            return false;

        // Clear the deleted timestamp
        model.setAttribute(getDeletedAtColumn(),
                           // Null QDateTime value
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                           QVariant(QMetaType(QMetaType::QDateTime)));
#else
                           QVariant(QVariant::DateTime));
#endif

        return model.save();
    }

    template<typename Derived>
    bool SoftDeletes<Derived>::trashed() const
    {
        const auto deletedAt = model().getAttribute(getDeletedAtColumn());

        return deletedAt.isValid() && !deletedAt.isNull() &&
               deletedAt.template canConvert<QDateTime>();
    }

    template<typename Derived>
    bool SoftDeletes<Derived>::isForceDeleting() const noexcept
    {
        return m_forceDeleting;
    }

    template<typename Derived>
    const QString &
    SoftDeletes<Derived>::getDeletedAtColumn() noexcept
    {
        return Derived::DELETED_AT;
    }

    template<typename Derived>
    QString SoftDeletes<Derived>::getQualifiedDeletedAtColumn() const
    {
        return model().qualifyColumn(getDeletedAtColumn());
    }

    /* protected */

    template<typename Derived>
    void SoftDeletes<Derived>::performDeleteOnModel()
    {
        // Perform soft delete
        if (!m_forceDeleting) {
            runSoftDelete();
            return;
        }

        // Perform force delete
        auto &model = this->model();

        /* Ownership of a unique_ptr(), dereferenced and passed down, will be
           destroyed right after this command. */
        model.setKeysForSaveQuery(*model.newModelQuery()).forceDelete();

        model.exists = false;
    }

    template<typename Derived>
    void SoftDeletes<Derived>::runSoftDelete()
    {
        auto &model = this->model();

        // Ownership of a unique_ptr()
        auto query = model.newModelQuery();
        model.setKeysForSaveQuery(*query);

        const auto time = model.freshTimestamp();
        auto timeString = model.fromDateTime(time);

        QVector<UpdateItem> columns;
        columns.reserve(2);

        columns.append({getDeletedAtColumn(), timeString});

        model.setAttribute(getDeletedAtColumn(), time);

        // Update also the updated_at column
        if (const auto &updatedAtColumn = model.getUpdatedAtColumn();
            model.usesTimestamps() && !updatedAtColumn.isEmpty()
        ) {
            model.setUpdatedAt(time);

            columns.append({updatedAtColumn, std::move(timeString)});
        }

        query->update(columns);

        // Sync original attributes with their current values
        const auto columnNames = columns
                | ranges::views::transform([](const auto &updateItem)
        {
            return updateItem.column;
        })
                | ranges::to<QStringList>();

        model.syncOriginalAttributes(columnNames);
    }

    /* private */

    template<typename Derived>
    Derived &SoftDeletes<Derived>::model()
    {
        return static_cast<Derived &>(*this);
    }

    template<typename Derived>
    const Derived &SoftDeletes<Derived>::model() const
    {
        return static_cast<const Derived &>(*this);
    }

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_SOFTDELETES_HPP
