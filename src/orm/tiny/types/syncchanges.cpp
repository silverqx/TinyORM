#include "orm/tiny/types/syncchanges.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Types
{

SyncChanges::SyncChanges()
    : map {{"attached", {}}, {"detached", {}}, {"updated", {}}}
{}

bool SyncChanges::supportedKey(const QString &key) const
{
    return SyncKeys.contains(key);
}

} // namespace Orm::Tiny::Types

TINYORM_END_COMMON_NAMESPACE
