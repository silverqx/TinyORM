#include "orm/schema/grammars/postgresschemagrammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Schema::Grammars
{

QString PostgresSchemaGrammar::compileColumnListing(const QString &/*unused*/) const
{
    return QStringLiteral("select column_name "
                          "from information_schema.columns "
                          "where table_schema = ? and table_name = ?");
}

} // namespace Orm::Schema::Grammars

TINYORM_END_COMMON_NAMESPACE
