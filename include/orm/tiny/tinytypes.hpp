#pragma once
#ifndef ORM_TINYTYPES_HPP
#define ORM_TINYTYPES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/ormtypes.hpp"

// CUR after commit move all common types here and remove all unneded forward declarations in Tiny ns silverqx
// CUR also replace ormtypes.hpp by tinytypes.hpp where appropriate and remove unneeded ormtypes.hpp silverqx
namespace Orm::Tiny
{
    template<typename Model>
    class Builder;

    template<typename Model>
    using TinyBuilder = Builder<Model>;

namespace Concerns
{

    /*! QueriesRelationships builder type passed to the callback. */
    template<typename Related>
    using QueriesRelationshipsCallback =
            std::conditional_t<std::is_void_v<Related>, QueryBuilder,
                               TinyBuilder<Related>>;

} // namespace Concerns
} // namespace Orm::Tiny

#endif // ORM_TINYTYPES_HPP
