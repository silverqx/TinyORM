#include "orm/tiny/types/syncchanges.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Types
{

/* public */

SyncChanges::SyncChanges()
    : map {{QStringLiteral("attached"), {}}, {QStringLiteral("detached"), {}},
           {QStringLiteral("updated"), {}}}
{}

/* private */

bool SyncChanges::supportedKey(const QString &key) const
{
    return SyncKeys.contains(key);
}

} // namespace Orm::Tiny::Types

TINYORM_END_COMMON_NAMESPACE
