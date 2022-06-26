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

#include <orm/tiny/model.hpp>{{ includesSection }}

namespace Models
{

using Orm::Tiny::Model;{{ usingsSection }}

// NOLINTNEXTLINE(misc-no-recursion)
class {{ class }} final : public Model<{{ class }}>
{
    friend Model;
    using Model::Model;{{ publicSection }}{{ privateSection }}
};

} // namespace Models

#endif // MODELS_{{ macroguard }}_HPP
)TTT";

/*! Include item stub. */
inline const auto *const ModelIncludeItemStub =
R"(#include "models/%1.hpp")";

/*! Using item stub. */
inline const auto *const ModelUsingItemStub =
R"(using Orm::Tiny::Relations::%1;)";

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
        return hasOne<{{ relatedClass }}>();
    })";

/*! One-to-many type relation stub. */
inline const auto *const OneToManyStub =
R"(
    /*! Get a {{ relatedComment }} associated with the {{ parentComment }}. */
    std::unique_ptr<HasMany<{{ parentClass }}, {{ relatedClass }}>>
    {{ relationName }}()
    {
        return hasOne<{{ relatedClass }}>();
    })";

/*! Belongs-to type relation stub (inverse for oto and otm). */
inline const auto *const BelongsToStub =
R"(
    /*! Get a {{ relatedComment }} that owns the {{ parentComment }}. */
    std::unique_ptr<BelongsTo<{{ parentClass }}, {{ relatedClass }}>>
    {{ relationName }}()
    {
        return belongsTo<{{ relatedClass }}>();
    })";

/*! Belongs-to-many type relation stub (it's the many-to-many and also inverse). */
inline const auto *const BelongsToManyStub =
R"(
    /*! Get {{ relatedComment }} that belong to the {{ parentComment }}. */
    std::unique_ptr<BelongsToMany<{{ parentClass }}, {{ relatedClass }}>>
    {{ relationName }}()
    {
        return belongsToMany<{{ relatedClass }}>();
    })";

/*! Model private section stub. */
inline const auto *const ModelPrivateStub =
R"(

private:)";

/*! Model relations hash stub. */
inline const auto *const ModelRelationsStub =
R"(
    /*! Map of relation names to methods. */
    QHash<QString, RelationVisitor> u_relations {
{{ relationItems }}
    };)";

/*! Relation item for model relations hash stub. */
inline const auto *const ModelRelationItemStub =
R"(        {"{{ relationName }}", {{ spaceAlign }}[](auto &v) { v(&{{ parentClass }}::{{ relationName }}); }},)";

/*! Model table stub. */
inline const auto *const ModelTableStub =
R"(
    /*! The table associated with the model. */
    QString u_table {"{{ table }}"};)";

/*! Model connection stub. */
inline const auto *const ModelConnectionStub =
R"(
    /*! The connection name for the model. */
    QString u_connection {"{{ connection }}"};)";

/*! Model disable timestamps stub. */
inline const auto *const ModelDisableTimestampsStub =
R"(
    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;)";

} // namespace Tom::Commands::Make::Stubs

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_STUBS_MODELSTUBS_HPP
