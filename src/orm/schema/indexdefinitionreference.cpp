#include "orm/schema/indexdefinitionreference.hpp"

#include "orm/schema/columndefinition.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

/* public */

IndexDefinitionReference::IndexDefinitionReference(IndexCommand &indexCommand)
    : m_indexCommand(indexCommand)
{}

IndexDefinitionReference &
IndexDefinitionReference::algorithm(const QString &algorithm)
{
    m_indexCommand.get().algorithm = algorithm;

    return *this;
}

IndexDefinitionReference &
IndexDefinitionReference::language(const QString &language)
{
    m_indexCommand.get().language = language;

    return *this;
}

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE
