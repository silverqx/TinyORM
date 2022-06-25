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

#include "orm/tiny/model.hpp"

namespace Models
{

using Orm::Tiny::Model;

// NOLINTNEXTLINE(misc-no-recursion)
class {{ class }} final : public Model<{{ class }}>
{
    friend Model;
    using Model::Model;{{ privateSection }}
};

} // namespace Models

#endif // MODELS_{{ macroguard }}_HPP
)TTT";

/*! Model private section stub. */
inline const auto *const ModelPrivateStub =
R"(
private:)";

/*! Model connection stub. */
inline const auto *const ModelConnectionStub =
R"(
    /*! The connection name for the model. */
    QString u_connection {"{{ connection }}"};)";

/*! Model table stub. */
inline const auto *const ModelTableStub =
R"(
    /*! The table associated with the model. */
    QString u_table {"{{ table }}"};)";

/*! Model disable timestamps stub. */
inline const auto *const ModelDisableTimestampsStub =
R"(
    /*! Indicates whether the model should be timestamped. */
    bool u_timestamps = false;)";

} // namespace Tom::Commands::Make::Stubs

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_STUBS_MODELSTUBS_HPP
