#include "orm/schema/grammars/sqliteschemagrammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema::Grammars
{

QString SQLiteSchemaGrammar::compileColumnListing(const QString &table) const
{
    auto table_ = table;

    // TODO study, wtf is this 🤔 silverqx
    table_.replace(DOT, "__");

    return QStringLiteral("pragma table_info(%1)").arg(wrap(table_));
}

} // namespace Orm::Schema::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
