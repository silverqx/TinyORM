#pragma once
#ifndef ORM_GUARDSATTRIBUTES_HPP
#define ORM_GUARDSATTRIBUTES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/concepts.hpp"
#include "orm/macros/threadlocal.hpp"
#include "orm/tiny/macros/crtpmodelwithbase.hpp"
#include "orm/tiny/tinytypes.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model;

namespace Concerns
{

    /*! Guards attributes. */
    template<typename Derived, typename ...AllRelations>
    class GuardsAttributes
    {
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

        /*! Disable all mass assignable restrictions. */
        static void unguard(bool state = true);
        /*! Enable the mass assignment restrictions. */
        static void reguard();
        /*! Determine if the current state is "unguarded". */
        inline static bool isUnguarded();
        /*! Run the given callable while being unguarded. */
        static void unguarded(const std::function<void()> &callback);

        /*! Determine if the given attribute may be mass assigned. */
        bool isFillable(const QString &key) const;
        /*! Determine if the given key is guarded. */
        bool isGuarded(const QString &key) const;

        /*! Determine if the model is totally guarded. */
        bool totallyGuarded() const;

    protected:
        /*! Determine if the given column is a valid, guardable column. */
        bool isGuardableColumn(const QString &key) const;
        /*! Th key for guardable columns hash cache. */
        QString getKeyForGuardableHash() const;
        /*! Get the fillable attributes of a given vector. */
        QVector<AttributeItem>
        fillableFromArray(const QVector<AttributeItem> &attributes) const;
        /*! Get the fillable attributes of a given vector. */
        QVector<AttributeItem>
        fillableFromArray(QVector<AttributeItem> &&attributes) const;

        /*! The attributes that are mass assignable. */
        T_THREAD_LOCAL
        inline static QStringList u_fillable;
        /*! The attributes that aren't mass assignable. */
        T_THREAD_LOCAL
        inline static QStringList u_guarded {ASTERISK}; // NOLINT(cppcoreguidelines-interfaces-global-init)
        /*! Indicates if all mass assignment is enabled. */
        T_THREAD_LOCAL
        inline static bool m_unguarded = false;
        /*! The actual columns that exist on the database and can be guarded. */
        T_THREAD_LOCAL
        inline static QHash<QString, QStringList> m_guardableColumns;

    private:
        /* Static cast this to a child's instance type (CRTP). */
        TINY_CRTP_MODEL_WITH_BASE
    };

    /* These methods may look a little strange because they are non-static, but it is
       intentional because I want to preserve the same API as Eloquent and return
       a Model &, but because of the CRTP pattern and the need of calling fill() method
       from the Model::ctor all of the u_xx mass asignment related data members have
       to be static. ✌ */

    template<typename Derived, typename ...AllRelations>
    const QStringList &
    GuardsAttributes<Derived, AllRelations...>::getFillable() const
    {
        return basemodel().getUserFillable();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::fillable(const QStringList &fillable)
    {
        basemodel().getUserFillable() = fillable;

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::fillable(QStringList &&fillable)
    {
        basemodel().getUserFillable() = std::move(fillable);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::mergeFillable(
            const QStringList &fillable)
    {
        auto &fillable_ = basemodel().getUserFillable();

        for (const auto &value : fillable)
            if (!fillable_.contains(value))
                fillable_.append(value);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::mergeFillable(QStringList &&fillable)
    {
        auto &fillable_ = basemodel().getUserFillable();

        for (auto &value : fillable)
            if (!fillable_.contains(value))
                fillable_.append(std::move(value));

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    const QStringList &
    GuardsAttributes<Derived, AllRelations...>::getGuarded() const
    {
        return basemodel().getUserGuarded();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::guard(const QStringList &guarded)
    {
        basemodel().getUserGuarded() = guarded;

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::guard(QStringList &&guarded)
    {
        basemodel().getUserGuarded() = std::move(guarded);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::mergeGuarded(const QStringList &guarded)
    {
        auto &guarded_ = basemodel().getUserGuarded();

        for (const auto &value : guarded)
            if (!guarded_.contains(value))
                guarded_.append(value);

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::mergeGuarded(QStringList &&guarded)
    {
        auto &guarded_ = basemodel().getUserGuarded();

        for (auto &value : guarded)
            if (!guarded_.contains(value))
                guarded_.append(std::move(value));

        return model();
    }

    template<typename Derived, typename ...AllRelations>
    void GuardsAttributes<Derived, AllRelations...>::unguard(const bool state)
    {
        // NOTE api different, Eloquent use late static binding for unguarded silverqx
        m_unguarded = state;
    }

    template<typename Derived, typename ...AllRelations>
    void GuardsAttributes<Derived, AllRelations...>::reguard()
    {
        m_unguarded = false;
    }

    template<typename Derived, typename ...AllRelations>
    bool GuardsAttributes<Derived, AllRelations...>::isUnguarded()
    {
        return m_unguarded;
    }

    // NOTE api different, Eloquent returns whatever callback returns silverqx
    template<typename Derived, typename ...AllRelations>
    void GuardsAttributes<Derived, AllRelations...>::unguarded(
            const std::function<void()> &callback)
    {
        if (m_unguarded) {
            std::invoke(callback);
            return;
        }

        unguard();

        try {
            std::invoke(callback);
        } catch (...) {
        }

        reguard();
    }

    template<typename Derived, typename ...AllRelations>
    bool
    GuardsAttributes<Derived, AllRelations...>::isFillable(const QString &key) const
    {
        if (m_unguarded)
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

        return fillable.isEmpty()
                // Don't allow mass filling with table names
                && !key.contains(DOT);
                // NOTE api different, isFillable() !key.startsWith(), what is this good for? silverqx
//                && !key.startsWith(UNDERSCORE);
    }

    template<typename Derived, typename ...AllRelations>
    bool
    GuardsAttributes<Derived, AllRelations...>::isGuarded(const QString &key) const
    {
        const auto &guarded = basemodel().getUserGuarded();

        if (guarded.isEmpty())
            return false;

        return guarded == QStringList {ASTERISK}
                // NOTE api different, Eloquent uses CaseInsensitive compare, silverqx
                || guarded.contains(key)
                /* Not a VALID guardable column is guarded, so it is not possible to fill
                   a column that is not in the database. */
                || !isGuardableColumn(key);
    }

    template<typename Derived, typename ...AllRelations>
    bool GuardsAttributes<Derived, AllRelations...>::totallyGuarded() const
    {
        return basemodel().getUserFillable().isEmpty()
                && basemodel().getUserGuarded() == QStringList {ASTERISK};
    }

    template<typename Derived, typename ...AllRelations>
    bool
    GuardsAttributes<Derived, AllRelations...>::isGuardableColumn(
            const QString &key) const
    {
        // NOTE api different, Eloquent caches it only by the model name silverqx
        // Cache columns by the connection and model name
        const auto guardableKey = getKeyForGuardableHash();

        if (!m_guardableColumns.contains(guardableKey))
            m_guardableColumns[guardableKey] = model().getConnection()
                                          .getSchemaBuilder()
                                          ->getColumnListing(model().getTable());

        return m_guardableColumns[guardableKey].contains(key);
    }

    template<typename Derived, typename ...AllRelations>
    QString
    GuardsAttributes<Derived, AllRelations...>::getKeyForGuardableHash() const
    {
        return QStringLiteral("%1-%2").arg(
                    model().getConnectionName(),
                    Orm::Utils::Type::classPureBasename<Derived>());
    }

    template<typename Derived, typename ...AllRelations>
    QVector<AttributeItem>
    GuardsAttributes<Derived, AllRelations...>::fillableFromArray(
            const QVector<AttributeItem> &attributes) const
    {
        const auto &fillable = basemodel().getUserFillable();

        if (fillable.isEmpty() || m_unguarded)
            return attributes;

        QVector<AttributeItem> result;

        for (const auto &attribute : attributes)
            if (fillable.contains(attribute.key))
                result.append(attribute);

        return result;
    }

    template<typename Derived, typename ...AllRelations>
    QVector<AttributeItem>
    GuardsAttributes<Derived, AllRelations...>::fillableFromArray(
            QVector<AttributeItem> &&attributes) const
    {
        const auto &fillable = basemodel().getUserFillable();

        if (fillable.isEmpty() || m_unguarded)
            return std::move(attributes);

        QVector<AttributeItem> result;

        for (auto &attribute : attributes)
            if (fillable.contains(attribute.key))
                result.append(std::move(attribute));

        return result;
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    GuardsAttributes<Derived, AllRelations...>::model()
    {
        // Can not be cached with static because a copy can be made
        // TODO cache it as class data member std::optional<std::reference_wrapper<Derived>> m_model = std::nullopt, but I will have to create copy ctor to set m_model {std::nullopt}, the same for other similar model() methods like Model::model(), then I can to check if (m_model) and return right away and I will call static_cast or dynamic_cast only once for every instance, it is casted everytime now 😟 silverqx
        return static_cast<Derived &>(*this);
    }

    template<typename Derived, typename ...AllRelations>
    const Derived &
    GuardsAttributes<Derived, AllRelations...>::model() const
    {
        return static_cast<const Derived &>(*this);
    }

    template<typename Derived, typename ...AllRelations>
    Model<Derived, AllRelations...> &
    GuardsAttributes<Derived, AllRelations...>::basemodel()
    {
        // Can not be cached with static because a copy can be made
        return static_cast<Model<Derived, AllRelations...> &>(*this);
    }

    template<typename Derived, typename ...AllRelations>
    const Model<Derived, AllRelations...> &
    GuardsAttributes<Derived, AllRelations...>::basemodel() const
    {
        return static_cast<const Model<Derived, AllRelations...> &>(*this);
    }

} // namespace Concerns
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_GUARDSATTRIBUTES_HPP
