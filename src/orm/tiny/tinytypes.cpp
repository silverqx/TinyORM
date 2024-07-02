#include "orm/tiny/tinytypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{

/* AttributeItem */

/* public */

AttributeItem::operator UpdateItem() const
{
    return {key, value};
}

bool operator==(const AttributeItem &left, const AttributeItem &right)
{
    return left.key == right.key && left.value == right.value;
}

/* WithItem */

/* public */

QList<WithItem>
WithItem::fromStringVector(const QList<QString> &relations)
{
    QList<WithItem> relationsConverted;
    relationsConverted.reserve(relations.size());

    for (const auto &relation : relations)
        relationsConverted.append({relation});

    return relationsConverted;
}

QList<WithItem>
WithItem::fromStringVector(QList<QString> &&relations) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    QList<WithItem> relationsConverted;
    relationsConverted.reserve(relations.size());

    for (auto &&relation : relations)
        relationsConverted.append({std::move(relation)});

    return relationsConverted;
}

bool operator==(const WithItem &left, const WithItem &right)
{
    return left.name == right.name;
}

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE
