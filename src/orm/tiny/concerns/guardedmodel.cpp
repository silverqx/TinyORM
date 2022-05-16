#include "orm/tiny/concerns/guardedmodel.hpp"

#include <atomic>

#include "orm/macros/threadlocal.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Concerns
{

/* Extracting these guard related methods to own class allows to call
   GuardedModel::unguarded() regardless of a template parameters needed on the Model
   class, before was needed to call Model<Torrent, Relations..>::unguarded().
   The g_unguarded variable has to live in the dll to work correctly, before it was
   inline static and it caused problems, it had different address if was called from
   the dll and from the application exe. */

// BUG multithread, all writable integral/bool types should be of std::atomic<> type silverqx
/*! Indicates if all mass assignment is enabled. */
T_THREAD_LOCAL
static std::atomic<bool> g_unguarded = false;

/* public */

// NOTE api different, Eloquent returns whatever callback returns and catches all exceptions around the std::invoke() silverqx
void GuardedModel::unguarded(std::function<void()> callback)
{
    if (g_unguarded.load()) {
        std::invoke(std::move(callback));
        return;
    }

    unguard();

    try {
        std::invoke(std::move(callback));

    }  catch (...) {

        reguard();
        // Re-throw
        throw;
    }
}

void GuardedModel::unguard(const bool state) noexcept
{
    // NOTE api different, Eloquent use late static binding for unguarded, what means that it can be overriden in the user defined model silverqx
    g_unguarded = state;
}

void GuardedModel::reguard() noexcept
{
    g_unguarded = false;
}

bool GuardedModel::isUnguarded() noexcept
{
    return g_unguarded;
}

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE
