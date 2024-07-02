#pragma once
#ifndef ORM_TINY_CONCERNS_GUARDSATTRIBUTES_HPP
#define ORM_TINY_CONCERNS_GUARDSATTRIBUTES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <unordered_set>

#include "orm/macros/threadlocal.hpp"
#include "orm/tiny/concerns/guardedmodel.hpp"
#include "orm/tiny/macros/crtpmodelwithbase.hpp"
#include "orm/tiny/tinytypes.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Concerns
{

    /*! Guards attributes. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class GuardsAttributes : public Concerns::GuardedModel
    {
        /*! Alias for the type utils. */
        using TypeUtils = Orm::Utils::Type;

    public:
        /*! Get the fillable attributes for the model. */
        inline const QStringList &getFillable() const;
        /*! Set the fillable attributes for the model. */
        Derived &fillable(const QStringList &fillable);
        /*! Set the fillable attributes for the model. */
        Derived &fillable(QStringList &&fillable);
        /*! Merge new fillable attributes with existing fillable attributes
            on the model. */
        Derived &mergeFillable(const QStringList &fillable);
        /*! Merge new fillable attributes with existing fillable attributes
            on the model. */
        Derived &mergeFillable(QStringList &&fillable);

        /*! Get the guarded attributes for the model. */
        inline const QStringList &getGuarded() const;
        /*! Set the guarded attributes for the model. */
        Derived &guard(const QStringList &guarded);
        /*! Set the guarded attributes for the model. */
        Derived &guard(QStringList &&guarded);
        /*! Merge new guarded attributes with existing guarded attributes
            on the model. */
        Derived &mergeGuarded(const QStringList &guarded);
        /*! Merge new guarded attributes with existing guarded attributes
            on the model. */
        Derived &mergeGuarded(QStringList &&guarded);

        /*! Determine if the given attribute may be mass assigned. */
        bool isFillable(const QString &key) const;
        /*! Determine if the given key is guarded. */
        bool isGuarded(const QString &key) const;

        /*! Determine if the model is totally guarded. */
        bool totallyGuarded() const;

    protected:
        /*! Determine if the given column is a valid, guardable column. */
        bool isGuardableColumn(const QString &key) const;
        /*! The key for guardable columns hash cache. */
        QString getKeyForGuardableHash() const;
        /*! Move columns to the std::unordered_set for guardable columns hash cache. */
        static std::unordered_set<QString>
        moveToSetForGuardableHash(QStringList &&columns); // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        /*! Get the fillable attributes of a given vector. */
        QList<AttributeItem>
        fillableFromArray(const QList<AttributeItem> &attributes) const;
        /*! Get the fillable attributes of a given vector. */
        QList<AttributeItem>
        fillableFromArray(QList<AttributeItem> &&attributes) const;

        /*! The attributes that are mass assignable. */
        T_THREAD_LOCAL
        inline static QStringList u_fillable;
        /*! The attributes that aren't mass assignable. */
        T_THREAD_LOCAL
        inline static QStringList u_guarded {ASTERISK}; // NOLINT(cppcoreguidelines-interfaces-global-init)
        /*! The actual columns that exist on the database and can be guarded. */
        T_THREAD_LOCAL
        inline static
        std::unordered_map<QString, std::unordered_set<QString>> m_guardableColumns;

    private:
        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_WITH_BASE_DECLARATIONS
    };

    /* These methods may look a little strange because they are non-static, but it is
       intentional because I want to preserve the same API as Eloquent and return
       a Model &, but because of the CRTP pattern and the need of calling fill() method
       from the Model::ctor all of the u_xx mass asignment related data members have
       to be static. ✌ */

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    GuardsAttributes<Derived, AllRelations...>::getFillable() const
    {
        return basemodel().getUserFillable();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::fillable(const QStringList &fillable)
    {
        basemodel().getUserFillable() = fillable;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::fillable(QStringList &&fillable)
    {
        basemodel().getUserFillable() = std::move(fillable);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::mergeFillable(
            const QStringList &fillable)
    {
        auto &fillable_ = basemodel().getUserFillable();

        for (const auto &value : fillable)
            if (!fillable_.contains(value))
                fillable_ << value;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::mergeFillable(QStringList &&fillable) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        auto &fillable_ = basemodel().getUserFillable();

        for (auto &&value : fillable)
            if (!fillable_.contains(value))
                fillable_ << std::move(value);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    GuardsAttributes<Derived, AllRelations...>::getGuarded() const
    {
        return basemodel().getUserGuarded();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::guard(const QStringList &guarded)
    {
        basemodel().getUserGuarded() = guarded;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::guard(QStringList &&guarded)
    {
        basemodel().getUserGuarded() = std::move(guarded);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::mergeGuarded(const QStringList &guarded)
    {
        auto &guarded_ = basemodel().getUserGuarded();

        for (const auto &value : guarded)
            if (!guarded_.contains(value))
                guarded_ << value;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::mergeGuarded(QStringList &&guarded) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        auto &guarded_ = basemodel().getUserGuarded();

        for (auto &&value : guarded)
            if (!guarded_.contains(value))
                guarded_ << std::move(value);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    GuardsAttributes<Derived, AllRelations...>::isFillable(const QString &key) const
    {
        if (isUnguarded())
            return true;

        const auto &fillable = basemodel().getUserFillable();

        /* If the key is in the "fillable" vector, we can of course assume that it's
           a fillable attribute. Otherwise, we will check the guarded vector when
           we need to determine if the attribute is black-listed on the model. */
        if (fillable.contains(key))
            return true;

        /* If the attribute is explicitly listed in the "guarded" vector then we can
           return false immediately. This means this attribute is definitely not
           fillable and there is no point in going any further in this method. */
        if (isGuarded(key))
            return false;

        return fillable.isEmpty() &&
               // Don't allow mass filling with table names
               !key.contains(DOT);
               // NOTE api different, isFillable() !key.startsWith(), what is this good for? silverqx
//               && !key.startsWith(UNDERSCORE);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    GuardsAttributes<Derived, AllRelations...>::isGuarded(const QString &key) const
    {
        const auto &guarded = basemodel().getUserGuarded();

        if (guarded.isEmpty())
            return false;

        return guarded == QStringList {ASTERISK} ||
               guarded.contains(key) ||
               /* Not a VALID guardable column is guarded, so it is not possible to fill
                  a column that is not in the database. */
               !isGuardableColumn(key);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool GuardsAttributes<Derived, AllRelations...>::totallyGuarded() const
    {
        const auto &basemodel = this->basemodel();

        return basemodel.getUserFillable().isEmpty() &&
               basemodel.getUserGuarded() == QStringList {ASTERISK};
    }

    /* protected */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    GuardsAttributes<Derived, AllRelations...>::isGuardableColumn(
            const QString &key) const
    {
        // NOTE api different, Eloquent caches it only by the model name silverqx
        // Cache columns by the connection and model name
        const auto [it, _] = m_guardableColumns.try_emplace(
                                 getKeyForGuardableHash(),
                                 moveToSetForGuardableHash(
                                     basemodel().getConnection()
                                                .getSchemaBuilder()
                                                .getColumnListing(model().getTable()))); // model() needed as it's overridden in the BasePivot

        return it->second.contains(key);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    GuardsAttributes<Derived, AllRelations...>::getKeyForGuardableHash() const
    {
        return QStringLiteral("%1-%2").arg(model().getConnectionName(),
                                           TypeUtils::classPureBasename<Derived>());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unordered_set<QString>
    GuardsAttributes<Derived, AllRelations...>::moveToSetForGuardableHash(
            QStringList &&columns) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        std::unordered_set<QString> columnsSet;

        columnsSet.reserve(static_cast<decltype (columnsSet)::size_type>(
                               columns.size()));

        std::ranges::move(columns, std::inserter(columnsSet, columnsSet.end()));

        return columnsSet;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QList<AttributeItem>
    GuardsAttributes<Derived, AllRelations...>::fillableFromArray(
            const QList<AttributeItem> &attributes) const
    {
        const auto &fillable = basemodel().getUserFillable();

        if (fillable.isEmpty() || isUnguarded())
            return attributes;

        QList<AttributeItem> result;
        result.reserve(attributes.size());

        for (const auto &attribute : attributes)
            if (fillable.contains(attribute.key))
                result << attribute;

        return result;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QList<AttributeItem>
    GuardsAttributes<Derived, AllRelations...>::fillableFromArray(
            QList<AttributeItem> &&attributes) const
    {
        const auto &fillable = basemodel().getUserFillable();

        if (fillable.isEmpty() || isUnguarded())
            return std::move(attributes);

        QList<AttributeItem> result;
        result.reserve(attributes.size());

        for (auto &attribute : attributes)
            if (fillable.contains(attribute.key))
                result << std::move(attribute);

        return result;
    }

    /* private */

    /* Static cast this to a child's instance type (CRTP) */

    TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(GuardsAttributes)

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_GUARDSATTRIBUTES_HPP
