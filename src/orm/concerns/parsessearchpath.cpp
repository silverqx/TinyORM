#include "orm/concerns/parsessearchpath.hpp"

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include "orm/schema/schemaconstants.hpp"
#include "orm/utils/string.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::COMMA_C;

using Orm::SchemaNs::Constants::TRIM_QUOTES;

using StringUtils = Orm::Utils::String;

namespace Orm::Concerns
{

/* public */

bool ParsesSearchPath::isSearchPathEmpty(const QStringList &searchPath)
{
    return searchPath.isEmpty() ||
            (searchPath.size() == 1 &&
             // set search_path = ''
             (searchPath.constFirst() == QStringLiteral("''") ||
              // During set search_path it set it to ''
              searchPath.constFirst().isEmpty() ||
              // Returned by the 'show search_path' query
              searchPath.constFirst() == QStringLiteral("\"\"")));
}

/* protected */

QStringList ParsesSearchPath::parseSearchPath(const QString &searchPath)
{
    // Nothing to parse
    if (isSearchPathEmpty(searchPath))
        return {};

    QStringList list;
    list.reserve(searchPath.count(COMMA_C));

    for (auto &&path : searchPath.split(COMMA_C, Qt::SkipEmptyParts))
        // Trim also spaces
        list << StringUtils::trim(path, QStringLiteral(" '\""));

    return list;
}

QStringList ParsesSearchPath::parseSearchPath(const QStringList &searchPath)
{
    // Nothing to parse
    if (isSearchPathEmpty(searchPath))
        return searchPath;

    return searchPath
            | ranges::views::transform([](const QString &path)
    {
        // Don't trim spaces
        return StringUtils::trim(path, TRIM_QUOTES);
    })
            | ranges::to<QStringList>();
}

QStringList ParsesSearchPath::parseSearchPath(const QVariant &searchPath)
{
    /* The type is already validated in the PostgreSQL's configuration parser, so
       we can be sure here that it's the QString or QStringList. Also don't use
       the QVariant::canConvert() here as there are no benefits from it, only pitfalls,
       eg. an empty QString if converting from the QStringList. */
    if (searchPath.typeId() == QMetaType::QString)
        return parseSearchPath(searchPath.value<QString>());

    return parseSearchPath(searchPath.value<QStringList>());
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
