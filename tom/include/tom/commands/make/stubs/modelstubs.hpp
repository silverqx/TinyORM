#pragma once
#ifndef TOM_COMMANDS_MAKE_STUBS_MODELSTUBS_HPP
#define TOM_COMMANDS_MAKE_STUBS_MODELSTUBS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Stubs
{

using Qt::StringLiterals::operator""_s;

/*! Model stub. */
inline const auto ModelStub =
uR"TTT(#pragma once
#ifndef MODELS_{{ macroguard }}_HPP
#define MODELS_{{ macroguard }}_HPP

{{ includesOrmSection }}{{ includesSection }}

namespace Models
{

{{ usingsSection }}
{{ forwardsSection }}
class {{ class }} final : public Model<{{ class }}{{ relationsList }}{{ pivotsList }}> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    using Model::Model;{{ publicSection }}{{ protectedSection }}{{ privateSection }}
};

} // namespace Models

#endif // MODELS_{{ macroguard }}_HPP
)TTT"_s;

/*! Custom pivot model stub. */
inline const auto PivotModelStub =
uR"TTT(#pragma once
#ifndef MODELS_{{ macroguard }}_HPP
#define MODELS_{{ macroguard }}_HPP

{{ includesOrmSection }}{{ includesSection }}

namespace Models
{

{{ usingsSection }}
{{ forwardsSection }}
class {{ class }} final : public BasePivot<{{ class }}{{ relationsList }}{{ pivotsList }}> // NOLINT(bugprone-exception-escape, misc-no-recursion)
{
    friend Model;
    friend BasePivot;

    using BasePivot::BasePivot;{{ publicSection }}{{ protectedSection }}{{ privateSection }}
};

} // namespace Models

#endif // MODELS_{{ macroguard }}_HPP
)TTT"_s;

/*! TinyORM include item stub. */
inline const auto ModelIncludeOrmItemStub =
uR"(#include <orm/%1>)"_s;

/*! Include item stub. */
inline const auto ModelIncludeItemStub =
uR"(#include "models/%1.hpp")"_s;

/*! Using item stub. */
inline const auto ModelUsingItemStub =
uR"(using Orm::Tiny::Relations::%1;)"_s;

/*! Forward class stub. */
inline const auto ModelForwardItemStub =
uR"(class %1;)"_s;

/*! Model public section stub. */
inline const auto ModelPublicStub =
uR"(

public:)"_s;

/*! One-to-one type relation stub. */
inline const auto OneToOneStub =
uR"(
    /*! Get a {{ relatedComment }} associated with the {{ parentComment }}. */
    std::unique_ptr<HasOne<{{ parentClass }}, {{ relatedClass }}>>
    {{ relationName }}()
    {
        return hasOne<{{ relatedClass }}>({{ relationArguments }});
    })"_s;

/*! One-to-many type relation stub. */
inline const auto OneToManyStub =
uR"(
    /*! Get a {{ relatedComment }} associated with the {{ parentComment }}. */
    std::unique_ptr<HasMany<{{ parentClass }}, {{ relatedClass }}>>
    {{ relationName }}()
    {
        return hasMany<{{ relatedClass }}>({{ relationArguments }});
    })"_s;

/*! Belongs-to type relation stub (inverse for oto and otm). */
inline const auto BelongsToStub =
uR"(
    /*! Get a {{ relatedComment }} that owns the {{ parentComment }}. */
    std::unique_ptr<BelongsTo<{{ parentClass }}, {{ relatedClass }}>>
    {{ relationName }}()
    {
        return belongsTo<{{ relatedClass }}>({{ relationArguments }});
    })"_s;

/*! Belongs-to-many type relation stub (it's the many-to-many and also inverse). */
inline const auto BelongsToManyStub =
uR"(
    /*! Get {{ relatedComment }} that belong to the {{ parentComment }}. */
    std::unique_ptr<BelongsToMany<{{ parentClass }}, {{ relatedClass }}{{ pivotClass }}>>
    {{ relationName }}()
    {
        return belongsToMany<{{ relatedClass }}{{ pivotClass }}>({{ relationArguments }});
    })"_s;

/*! Belongs-to-many type relation stub v2 (it's the many-to-many and also inverse). */
inline const auto BelongsToManyStub2 =
uR"(
    /*! Get {{ relatedComment }} that belong to the {{ parentComment }}. */
    std::unique_ptr<BelongsToMany<{{ parentClass }}, {{ relatedClass }}{{ pivotClass }}>>
    {{ relationName }}()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<{{ relatedClass }}{{ pivotClass }}>({{ relationArguments }});

        relation->{{ relationCalls }};

        return relation;
    })"_s;

/*! Model protected section stub. */
inline const auto ModelProtectedStub =
uR"(

protected:)"_s;

/*! Model private section stub. */
inline const auto ModelPrivateStub =
uR"(

private:)"_s;

/*! Model u_relations hash stub. */
inline const auto ModelRelationsStub =
uR"(
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
{{ relationItems }}
    };)"_s;

/*! Relation mapping item for model u_relations hash stub. */
inline const auto ModelRelationItemStub =
uR"(        {"{{ relationName }}", {{ spaceAlign }}[](auto &v) { v(&{{ parentClass }}::{{ relationName }}); }},)"_s;

/*! Model u_table stub. */
inline const auto ModelTableStub =
uR"(
    /*! The table associated with the model. */
    QString u_table {"%1"};)"_s;

/*! Model u_primaryKey stub. */
inline const auto ModelPrimaryKeyStub =
uR"(
    /*! The primary key associated with the table. */
    QString u_primaryKey {"%1"};)"_s;

/*! Model enable auto-incrementing stub. */
inline const auto ModelIncrementingStub =
uR"(
    /*! Indicates if the model's ID is auto-incrementing. */
    bool u_incrementing = %1;)"_s;

/*! Model u_connection stub. */
inline const auto ModelConnectionStub =
uR"(
    /*! The connection name for the model. */
    QString u_connection {"%1"};)"_s;

/*! Model eager load u_with stub. */
inline const auto ModelWithStub =
uR"(
    /*! The relations to eager load on every query. */
    QList<QString> u_with {%1};)"_s;

/*! Model u_fillable stub. */
inline const auto ModelFillableStub =
uR"(
    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable {%1};)"_s;

/*! Model u_guarded stub. */
inline const auto ModelGuardedStub =
uR"(
    /*! The attributes that aren't mass assignable. */
    inline static QStringList u_guarded {%1};)"_s;

/*! Model disable u_timestamps stub. */
inline const auto ModelDisableTimestampsStub =
uR"(
    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;)"_s;

/*! Model u_casts example stub. */
inline const auto ModelCastsExampleStub =
uR"(
    /*! The attributes that should be cast. */
    inline static std::unordered_map<QString, CastItem> u_casts {
        {"added_on",   {CastType::CustomQDateTime, "yyyy-MM-ddTHH:mm:ssZ"}}, // the custom format used during serialization only
        {"created_at", CastType::QDateTime},
        {"decimal",    {CastType::Decimal, 2}},
        {"is_admin",   CastType::Boolean},
        {"name",       CastType::QString},
        {"progress",   CastType::UShort},
        {"size",       CastType::ULongLong},
    };)"_s;

/*! Model u_dateFormat stub. */
inline const auto ModelDateFormatStub =
uR"(
    /*! The storage format of the model's date columns. */
    inline static QString u_dateFormat {"%1"};)"_s;

/*! Model u_dates stub. */
inline const auto ModelDatesStub =
uR"(
    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {%1};)"_s;

/*! Model u_touches stub. */
inline const auto ModelTouchesStub =
uR"(
    /*! All of the relationships to be touched. */
    QStringList u_touches {%1};)"_s;

/*! Model enable/disable snake_cased attributes during serialization stub. */
inline const auto ModelSnakeAttributesStub =
uR"(
    /*! Indicates whether attributes are snake_cased during serialization. */
    inline static const bool u_snakeAttributes = %1;)"_s;

/*! Model u_visible stub. */
inline const auto ModelVisibleStub =
uR"(
    /*! The attributes that should be visible during serialization. */
    inline static std::set<QString> u_visible {%1};)"_s;

/*! Model u_hidden stub. */
inline const auto ModelHiddenStub =
uR"(
    /*! The attributes that should be hidden during serialization. */
    inline static std::set<QString> u_hidden {%1};)"_s;

/*! Model u_mutators stub. */
inline const auto ModelMutatorsStub =
uR"(
    /*! Map of mutator names to methods. */
    inline static const QHash<QString, MutatorFunction> u_mutators {{{ mutatorItems }}};)"_s;

/*! Mutator mapping item for model's u_mutators hash stub. */
inline const auto ModelMutatorItemStub =
uR"(        {"{{ mutatorNameSnake }}", {{ spaceAlign }}&{{ class }}::{{ mutatorNameCamel }}},)"_s;

/*! Model u_appends stub. */
inline const auto ModelAppendsStub =
uR"(
    /*! The attributes that should be appended during serialization. */
    std::set<QString> u_appends {%1};)"_s;

/*! Accessor method stub. */
inline const auto AccessorMethodStub =
uR"(
    /*! The {{ accessorNameSnake }} accessor. */
    Attribute {{ accessorNameCamel }}() const noexcept
    {
        return Attribute::make(/* get */ [this]() -> QVariant
        {
            return getAttribute<QString>("example_attribute");
        });
    })"_s;

} // namespace Tom::Commands::Make::Stubs

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_STUBS_MODELSTUBS_HPP
