#include "orm/tiny/concerns/guardedmodel.hpp"

#include <atomic>

#include "orm/macros/threadlocal.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Concerns
{

/* Extracting these guard related methods to own class allows to call
   GuardedModel::unguarded() regardless of a template parameters needed on the Model
   class, before was needed to call Model<Torrent, Relations...>::unguarded().
   The g_unguarded variable has to live in the dll to work correctly, before it was
   inline static and it caused problems, it had different address if was called from
   the dll and from the application exe. */

namespace
{
    /*! Indicates if all mass assignment is enabled, atomic. */
    std::atomic<bool> &g_unguarded() noexcept
    {
        T_THREAD_LOCAL
        static std::atomic<bool> cached = false;

        return cached;
    }
} // namespace

/* public */

// NOTE api different, Eloquent returns whatever callback returns silverqx
void GuardedModel::unguarded(const std::function<void()> &callback)
{
    if (g_unguarded())
        return std::invoke(callback); // NOLINT(readability-avoid-return-with-void-value) clazy:exclude=returning-void-expression

    unguard();

    try {
        std::invoke(callback);

        reguard();

    } catch (...) {

        reguard();
        // Re-throw
        throw;
    }
}

void GuardedModel::unguard(const bool state) noexcept
{
    // NOTE api different, Eloquent use late static binding for unguarded, what means that it can be overridden in the user defined model silverqx
    g_unguarded() = state;
}

void GuardedModel::reguard() noexcept
{
    g_unguarded() = false;
}

bool GuardedModel::isUnguarded() noexcept
{
    return g_unguarded();
}

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE
