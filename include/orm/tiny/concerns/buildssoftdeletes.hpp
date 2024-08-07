#pragma once
#ifndef ORM_TINY_CONCERNS_BUILDSSOFTDELETES_HPP
#define ORM_TINY_CONCERNS_BUILDSSOFTDELETES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlQuery

#include "orm/tiny/tinyconcepts.hpp" // IWYU pragma: keep
#include "orm/utils/nullvariant.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
    template<typename Model>
    class Builder;

namespace Concerns
{

    /*! SoftDeletes constraint types. */
    enum struct TrashedType : quint8
    {
        WITHOUT_TRASHED,
        WITH_TRASHED,
        ONLY_TRASHED,
    };

    /*! Builds queries for the SoftDeleting feature. */
    template<ModelConcept Model, bool T = true>
    class BuildsSoftDeletes
    {
        /*! Alias for the null QVariant-s utils. */
        using NullVariant = Orm::Utils::NullVariant;

    public:
        /*! Default constructor. */
        BuildsSoftDeletes() = default;
        /*! Default destructor. */
        ~BuildsSoftDeletes() = default;

        /*! Copy constructor. */
        BuildsSoftDeletes(const BuildsSoftDeletes &) = default;
        /*! Deleted copy assignment operator (not needed). */
        BuildsSoftDeletes &operator=(const BuildsSoftDeletes &) = delete;

        /*! Move constructor. */
        BuildsSoftDeletes(BuildsSoftDeletes &&) noexcept = default;
        /*! Deleted move assignment operator (not needed). */
        BuildsSoftDeletes &operator=(BuildsSoftDeletes &&) = delete;

        /*! Enable the default soft deletes constraint on the TinyBuilder
            (withoutTrashed). */
        inline Builder<Model> &enableSoftDeletes();
        /*! Disable the default soft deletes constraint on the TinyBuilder
            (disabled the default withoutTrashed). */
        inline Builder<Model> &disableSoftDeletes();

        /*! Initialize the BuildsSoftDeletes concern (registers onDelete callback). */
        void initializeBuildsSoftDeletes();

        /*! Constraint the TinyBuilder query to exclude trashed models
            (where deleted_at IS NULL). */
        inline Builder<Model> &withoutTrashed();
        /*! Constraint the TinyBuilder query to include trashed models
            (no where clause added). */
        inline Builder<Model> &withTrashed(bool withTrashed = true);
        /*! Constraint the TinyBuilder query to select only trashed models
            (where deleted_at IS NOT NULL). */
        inline Builder<Model> &onlyTrashed();

        /*! Restore all trashed models (call update on deleted_at column, set to null). */
        std::tuple<int, TSqlQuery> restore();

        /*! Get the currently applied soft deletes constraint on the TinyBuilder. */
        inline TrashedType currentSoftDeletes() const noexcept;

    protected:
        /*! Expose the TrashedType enum. */
        using enum TrashedType;

        /*! Get the "deleted at" column for the builder (fully qualified if joins
            are defined). */
        QString getDeletedAtColumn(Builder<Model> &builder) const;

        /*! Apply the default SoftDeletes constraint (withoutTrashed). */
        Builder<Model> &applySoftDeletes();

        /*! Stores the currently applied soft deletes constraint on the TinyBuilder. */
        TrashedType m_trashed = WITHOUT_TRASHED;
        /*! Is the default soft deletes constraint on the TinyBuilder enabled? */
        bool m_withSoftDeletes = false;

    private:
        /*! Static cast *this to the TinyBuilder & derived type. */
        inline Builder<Model> &builder() noexcept;
    };

    /*! Builds queries for the SoftDeleting feature (specialization when the SoftDeletes
        feature is disabled). */
    template<ModelConcept Model>
    class BuildsSoftDeletes<Model, false>
    {
    public:
        /*! Default constructor. */
        BuildsSoftDeletes() = default;
        /*! Default destructor. */
        ~BuildsSoftDeletes() = default;

        /*! Copy constructor. */
        BuildsSoftDeletes(const BuildsSoftDeletes &) = default;
        /*! Deleted copy assignment operator (not needed). */
        BuildsSoftDeletes &operator=(const BuildsSoftDeletes &) = delete;

        /*! Move constructor. */
        BuildsSoftDeletes(BuildsSoftDeletes &&) noexcept = default;
        /*! Deleted move assignment operator (not needed). */
        BuildsSoftDeletes &operator=(BuildsSoftDeletes &&) = delete;
    };

    /* BuildsSoftDeletes */

    /* public */

    template<ModelConcept Model, bool T>
    Builder<Model> &BuildsSoftDeletes<Model, T>::enableSoftDeletes()
    {
        // Controlled by the Model::newQueryXyz() methods
        m_withSoftDeletes = true;

        return builder();
    }

    template<ModelConcept Model, bool T>
    Builder<Model> &BuildsSoftDeletes<Model, T>::disableSoftDeletes()
    {
        m_withSoftDeletes = false;

        return builder();
    }

    template<ModelConcept Model, bool T>
    void BuildsSoftDeletes<Model, T>::initializeBuildsSoftDeletes()
    {
        builder().onDelete([this](Builder<Model> &builder)
        {
            return builder.update({{getDeletedAtColumn(builder),
                                    builder.getModel().freshTimestampString()}});
        });
    }

    template<ModelConcept Model, bool T>
    Builder<Model> &
    BuildsSoftDeletes<Model, T>::withoutTrashed()
    {
        m_trashed = WITHOUT_TRASHED;

        /* Disable the default soft deletes constraint on the TinyBuilder because
           manually overridden and we are applying another whereNull clause here. */
        disableSoftDeletes();

        auto &builder = this->builder();

        return builder.whereNull(builder.getModel().getQualifiedDeletedAtColumn());
    }

    template<ModelConcept Model, bool T>
    Builder<Model> &
    BuildsSoftDeletes<Model, T>::withTrashed(const bool withTrashed)
    {
        /* This may be appropriate in very few cases, eg. when a user wants to
           control it at runtime. */
        if (!withTrashed)
            return withoutTrashed();

        m_trashed = WITH_TRASHED;

        // Disable the default soft deletes constraint on the TinyBuilder (withoutTrashed)
        disableSoftDeletes();

        return builder();
    }

    template<ModelConcept Model, bool T>
    Builder<Model> &
    BuildsSoftDeletes<Model, T>::onlyTrashed()
    {
        m_trashed = ONLY_TRASHED;

        /* Disable the default soft deletes constraint on the TinyBuilder because
           manually overridden and we are applying another whereNotNull clause here. */
        disableSoftDeletes();

        auto &builder = this->builder();

        return builder.whereNotNull(builder.getModel().getQualifiedDeletedAtColumn());
    }

    template<ModelConcept Model, bool T>
    std::tuple<int, TSqlQuery>
    BuildsSoftDeletes<Model, T>::restore()
    {
        withTrashed();

        return builder().update({{builder().getModel().getDeletedAtColumn(),
                                  // Null QDateTime value
                                  NullVariant::QDateTime()}});
    }

    template<ModelConcept Model, bool T>
    TrashedType
    BuildsSoftDeletes<Model, T>::currentSoftDeletes() const noexcept
    {
        return m_trashed;
    }

    /* protected */

    template<ModelConcept Model, bool T>
    QString
    BuildsSoftDeletes<Model, T>::getDeletedAtColumn(Builder<Model> &builder) const
    {
        if (builder.getQuery().getJoins().isEmpty())
            return builder.getModel().getDeletedAtColumn();

        // Use a qualified deleted_at column if joins are in the game
        return builder.getModel().getQualifiedDeletedAtColumn();
    }

    template<ModelConcept Model, bool T>
    Builder<Model> &
    BuildsSoftDeletes<Model, T>::applySoftDeletes()
    {
        auto &builder = this->builder();

        // Controlled by the Model::newQueryXyz() methods
        if (!m_withSoftDeletes)
            return builder;

        return builder.whereNull(builder.getModel().getQualifiedDeletedAtColumn());
    }

    /* private */

    template<ModelConcept Model, bool T>
    Builder<Model> &BuildsSoftDeletes<Model, T>::builder() noexcept
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
        return static_cast<Builder<Model> &>(*this);
    }

} // namespace Concerns
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_BUILDSSOFTDELETES_HPP
