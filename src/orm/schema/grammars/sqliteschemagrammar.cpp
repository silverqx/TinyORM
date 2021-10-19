#include "orm/schema/grammars/sqliteschemagrammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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

TINYORM_END_COMMON_NAMESPACE
