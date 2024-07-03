#pragma once
#ifndef ORM_TINY_CONCERNS_GUARDEDMODEL_HPP
#define ORM_TINY_CONCERNS_GUARDEDMODEL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <functional>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Concerns
{

    /*! Manages mass assignment restrictions for the entire Model class. */
    class TINYORM_EXPORT GuardedModel
    {
    public:
        /*! Run the given callable while being unguarded. */
        static void unguarded(const std::function<void()> &callback);

        /*! Disable all mass assignable restrictions. */
        static void unguard(bool state = true) noexcept;
        /*! Enable the mass assignment restrictions. */
        static void reguard() noexcept;

        /*! Determine if the current state is "unguarded". */
        static bool isUnguarded() noexcept;
    };

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_GUARDEDMODEL_HPP
