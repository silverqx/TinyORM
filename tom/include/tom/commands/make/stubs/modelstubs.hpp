#pragma once
#ifndef TOM_COMMANDS_MAKE_STUBS_MODELSTUBS_HPP
#define TOM_COMMANDS_MAKE_STUBS_MODELSTUBS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Stubs
{

/*! Model stub. */
inline const auto *const ModelStub =
R"TTT(#pragma once
#ifndef MODELS_{{ macroguard }}_HPP
#define MODELS_{{ macroguard }}_HPP

{{ includesOrmSection }}{{ includesSection }}

namespace Models
{

{{ usingsSection }}
{{ forwardsSection }}
// NOLINTNEXTLINE(misc-no-recursion)
class {{ class }} final : public Model<{{ class }}{{ relationsList }}{{ pivotsList }}>
{
    friend Model;
    using Model::Model;{{ publicSection }}{{ protectedSection }}{{ privateSection }}
};

} // namespace Models

#endif // MODELS_{{ macroguard }}_HPP
)TTT";

/*! Custom pivot model stub. */
inline const auto *const PivotModelStub =
R"TTT(#pragma once
#ifndef MODELS_{{ macroguard }}_HPP
#define MODELS_{{ macroguard }}_HPP

{{ includesOrmSection }}{{ includesSection }}

namespace Models
{

{{ usingsSection }}
{{ forwardsSection }}
class {{ class }} final : public BasePivot<{{ class }}{{ relationsList }}{{ pivotsList }}>
{
    friend Model;
    friend BasePivot;

    using BasePivot::BasePivot;{{ publicSection }}{{ protectedSection }}{{ privateSection }}
};

} // namespace Models

#endif // MODELS_{{ macroguard }}_HPP
)TTT";

/*! TinyORM include item stub. */
inline const auto *const ModelIncludeOrmItemStub =
R"(#include <orm/%1>)";

/*! Include item stub. */
inline const auto *const ModelIncludeItemStub =
R"(#include "models/%1.hpp")";

/*! Using item stub. */
inline const auto *const ModelUsingItemStub =
R"(using Orm::Tiny::Relations::%1;)";

/*! Forward class stub. */
inline const auto *const ModelForwardItemStub =
R"(class %1;)";

/*! Model public section stub. */
inline const auto *const ModelPublicStub =
R"(

public:)";

/*! One-to-one type relation stub. */
inline const auto *const OneToOneStub =
R"(
    /*! Get a {{ relatedComment }} associated with the {{ parentComment }}. */
    std::unique_ptr<HasOne<{{ parentClass }}, {{ relatedClass }}>>
    {{ relationName }}()
    {
        return hasOne<{{ relatedClass }}>({{ relationArguments }});
    })";

/*! One-to-many type relation stub. */
inline const auto *const OneToManyStub =
R"(
    /*! Get a {{ relatedComment }} associated with the {{ parentComment }}. */
    std::unique_ptr<HasMany<{{ parentClass }}, {{ relatedClass }}>>
    {{ relationName }}()
    {
        return hasMany<{{ relatedClass }}>({{ relationArguments }});
    })";

/*! Belongs-to type relation stub (inverse for oto and otm). */
inline const auto *const BelongsToStub =
R"(
    /*! Get a {{ relatedComment }} that owns the {{ parentComment }}. */
    std::unique_ptr<BelongsTo<{{ parentClass }}, {{ relatedClass }}>>
    {{ relationName }}()
    {
        return belongsTo<{{ relatedClass }}>({{ relationArguments }});
    })";

/*! Belongs-to-many type relation stub (it's the many-to-many and also inverse). */
inline const auto *const BelongsToManyStub =
R"(
    /*! Get {{ relatedComment }} that belong to the {{ parentComment }}. */
    std::unique_ptr<BelongsToMany<{{ parentClass }}, {{ relatedClass }}{{ pivotClass }}>>
    {{ relationName }}()
    {
        return belongsToMany<{{ relatedClass }}{{ pivotClass }}>({{ relationArguments }});
    })";

/*! Belongs-to-many type relation stub v2 (it's the many-to-many and also inverse). */
inline const auto *const BelongsToManyStub2 =
R"(
    /*! Get {{ relatedComment }} that belong to the {{ parentComment }}. */
    std::unique_ptr<BelongsToMany<{{ parentClass }}, {{ relatedClass }}{{ pivotClass }}>>
    {{ relationName }}()
    {
        // Ownership of a unique_ptr()
        auto relation = belongsToMany<{{ relatedClass }}{{ pivotClass }}>({{ relationArguments }});

        relation->{{ relationCalls }};

        return relation;
    })";

/*! Model protected section stub. */
inline const auto *const ModelProtectedStub =
R"(

protected:)";

/*! Model private section stub. */
inline const auto *const ModelPrivateStub =
R"(

private:)";

/*! Model u_relations hash stub. */
inline const auto *const ModelRelationsStub =
R"(
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
{{ relationItems }}
    };)";

/*! Relation mapping item for model u_relations hash stub. */
inline const auto *const ModelRelationItemStub =
R"(        {"{{ relationName }}", {{ spaceAlign }}[](auto &v) { v(&{{ parentClass }}::{{ relationName }}); }},)";

/*! Model u_table stub. */
inline const auto *const ModelTableStub =
R"(
    /*! The table associated with the model. */
    QString u_table {"%1"};)";

/*! Model u_primaryKey stub. */
inline const auto *const ModelPrimaryKeyStub =
R"(
    /*! The primary key associated with the table. */
    QString u_primaryKey {"%1"};)";

/*! Model enable auto-incrementing stub. */
inline const auto *const ModelIncrementingStub =
R"(
    /*! Indicates if the model's ID is auto-incrementing. */
    bool u_incrementing = %1;)";

/*! Model u_connection stub. */
inline const auto *const ModelConnectionStub =
R"(
    /*! The connection name for the model. */
    QString u_connection {"%1"};)";

/*! Model eager load u_with stub. */
inline const auto *const ModelWithStub =
R"(
    /*! The relations to eager load on every query. */
    QVector<QString> u_with {%1};)";

/*! Model u_fillable stub. */
inline const auto *const ModelFillableStub =
R"(
    /*! The attributes that are mass assignable. */
    inline static const QStringList u_fillable {%1};)";

/*! Model u_guarded stub. */
inline const auto *const ModelGuardedStub =
R"(
    /*! The attributes that aren't mass assignable. */
    inline static QStringList u_guarded {%1};)";

/*! Model disable u_timestamps stub. */
inline const auto *const ModelDisableTimestampsStub =
R"(
    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;)";

/*! Model u_casts example stub. */
inline const auto *const ModelCastsExampleStub =
R"(
    /*! The attributes that should be cast. */
    inline static std::unordered_map<QString, CastItem> u_casts {
        {"added_on",   {CastType::CustomQDateTime, "yyyy-MM-ddTHH:mm:ssZ"}}, // the custom format used during serialization only
        {"created_at", CastType::QDateTime},
        {"decimal",    {CastType::Decimal, 2}},
        {"is_admin",   CastType::Boolean},
        {"name",       CastType::QString},
        {"progress",   CastType::UShort},
        {"size",       CastType::ULongLong},
    };)";

/*! Model u_dateFormat stub. */
inline const auto *const ModelDateFormatStub =
R"(
    /*! The storage format of the model's date columns. */
    inline static QString u_dateFormat {"%1"};)";

/*! Model u_dates stub. */
inline const auto *const ModelDatesStub =
R"(
    /*! The attributes that should be mutated to dates. */
    inline static const QStringList u_dates {%1};)";

/*! Model u_touches stub. */
inline const auto *const ModelTouchesStub =
R"(
    /*! All of the relationships to be touched. */
    QStringList u_touches {%1};)";

/*! Model enable/disable snake_cased attributes during serialization stub. */
inline const auto *const ModelSnakeAttributesStub =
R"(
    /*! Indicates whether attributes are snake_cased during serialization. */
    inline static const bool u_snakeAttributes = %1;)";

/*! Model u_visible stub. */
inline const auto *const ModelVisibleStub =
R"(
    /*! The attributes that should be visible during serialization. */
    inline static std::set<QString> u_visible {%1};)";

/*! Model u_hidden stub. */
inline const auto *const ModelHiddenStub =
R"(
    /*! The attributes that should be hidden during serialization. */
    inline static std::set<QString> u_hidden {%1};)";

/*! Model u_mutators stub. */
inline const auto *const ModelMutatorsStub =
R"(
    /*! Map of mutator names to methods. */
    inline static const QHash<QString, MutatorFunction> u_mutators {{{ mutatorItems }}};)";

/*! Mutator mapping item for model's u_mutators hash stub. */
inline const auto *const ModelMutatorItemStub =
R"(        {"{{ mutatorNameSnake }}", {{ spaceAlign }}&{{ class }}::{{ mutatorNameCamel }}},)";

/*! Model u_appends stub. */
inline const auto *const ModelAppendsStub =
R"(
    /*! The attributes that should be appended during serialization. */
    std::set<QString> u_appends {%1};)";

/*! Accessor method stub. */
inline const auto *const AccessorMethodStub =
R"(
    /*! The {{ accessorNameSnake }} accessor. */
    Attribute {{ accessorNameCamel }}() const noexcept
    {
        return Attribute::make(/* get */ [this]() -> QVariant
        {
            return getAttribute<QString>("example_attribute");
        });
    })";

} // namespace Tom::Commands::Make::Stubs

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_STUBS_MODELSTUBS_HPP
