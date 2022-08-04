#include "orm/tiny/types/syncchanges.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Types
{

/* public */

SyncChanges::SyncChanges()
    : map {{QStringLiteral("attached"), {}},
           {QStringLiteral("detached"), {}},
           {QStringLiteral("updated"),  {}}}
{}

bool SyncChanges::isValidKey(const QString &key)
{
    // FUTURE simply use contains(key) and remove syncKeys(), but this needs to = delete all methods that can modify this map as it's derived from the std::map because now an user can simply add another xyz key to the map and this method would return incorrect results, even better will be to define std::map as data member and not deriving from it silverqx
    return syncKeys().contains(key);
}

/* private */

const std::unordered_set<QString> &Types::SyncChanges::syncKeys()
{
    static const std::unordered_set<QString> cached {
        QStringLiteral("attached"),
        QStringLiteral("detached"),
        QStringLiteral("updated")
    };

    return cached;
}

} // namespace Orm::Tiny::Types

TINYORM_END_COMMON_NAMESPACE
