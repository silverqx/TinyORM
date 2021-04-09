#include "orm/schema/grammars/sqlitegrammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema::Grammars
{

QString SQLiteGrammar::compileColumnListing(const QString &table) const
{
    auto table_ = table;

    // TODO study, wtf is this 🤔 silverqx
    table_.replace(QChar('.'), "__");

    return QStringLiteral("pragma table_info(%1)").arg(wrap(table_));
}

} // namespace Orm::Schema::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
