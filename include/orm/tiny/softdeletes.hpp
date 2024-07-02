#pragma once
#ifndef ORM_TINY_SOFTDELETES_HPP
#define ORM_TINY_SOFTDELETES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include "orm/ormtypes.hpp"
#include "orm/utils/helpers.hpp"
#include "orm/utils/nullvariant.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{

    /*! Model Soft Deleting. */
    template<typename Derived>
    class SoftDeletes
    {
        /*! Alias for the helper utils. */
        using Helpers = Orm::Utils::Helpers;
        /*! Alias for the null QVariant-s utils. */
        using NullVariant = Orm::Utils::NullVariant;

    public:
        /*! Default constructor. */
        SoftDeletes() = default;
        /*! Default destructor. */
        ~SoftDeletes() = default;

        /*! Copy constructor. */
        SoftDeletes(const SoftDeletes &) = default;
        /*! Copy assignment operator. */
        SoftDeletes &operator=(const SoftDeletes &) = default;

        /*! Move constructor. */
        SoftDeletes(SoftDeletes &&) noexcept = default;
        /*! Move assignment operator. */
        SoftDeletes &operator=(SoftDeletes &&) noexcept = default;

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
        inline QString getQualifiedDeletedAtColumn() const;

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
        inline Derived &model() noexcept;
        /*! Static cast this to a child's instance type (CRTP), const version. */
        inline const Derived &model() const noexcept;
    };

    /* We can't use the basemodel() in SoftDeletes concern as it's templated only by
       the Derived template parameter so the model() is used everywhere instead.
       It's not a big deal as everything works as expected and I'm not going to refactor
       it to add the AllRelationsConcept ...AllRelations template parameter just because
       of the basemodel(), it's good enough as it is. */

    /* public */

    template<typename Derived>
    void SoftDeletes<Derived>::initializeSoftDeletes() const
    {
        const auto &deletedAtColumn = getDeletedAtColumn();

        if (model().getDates().contains(deletedAtColumn))
            return;

        Derived::appendToUserDates(deletedAtColumn);
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
        model.setAttribute(getDeletedAtColumn(), NullVariant::QDateTime());

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
        if (!m_forceDeleting)
            return runSoftDelete(); // clazy:exclude=returning-void-expression

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

        const auto timestamp = model.freshTimestamp();
        auto timestampString = model.fromDateTime(timestamp);

        QList<UpdateItem> columns;
        columns.reserve(2);

        columns.append({getDeletedAtColumn(), timestampString});

        model.setAttribute(getDeletedAtColumn(), timestamp);

        // Update also the updated_at column
        if (const auto &updatedAtColumn = Derived::getUpdatedAtColumn();
            model.usesTimestamps() && !updatedAtColumn.isEmpty()
        ) {
            model.setUpdatedAt(timestamp);

            columns.append({updatedAtColumn, std::move(timestampString)});
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
    Derived &SoftDeletes<Derived>::model() noexcept
    {
        return static_cast<Derived &>(*this);
    }

    template<typename Derived>
    const Derived &SoftDeletes<Derived>::model() const noexcept
    {
        return static_cast<const Derived &>(*this);
    }

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_SOFTDELETES_HPP
