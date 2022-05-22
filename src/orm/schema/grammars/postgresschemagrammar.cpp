#include "orm/schema/grammars/postgresschemagrammar.hpp"

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs::Grammars
{

/* public */

/* Compile methods for the SchemaBuilder */

QString PostgresSchemaGrammar::compileEnableForeignKeyConstraints() const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString PostgresSchemaGrammar::compileDisableForeignKeyConstraints() const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString PostgresSchemaGrammar::compileColumnListing(const QString &/*unused*/) const
{
    return QStringLiteral("select column_name "
                          "from information_schema.columns "
                          "where table_schema = ? and table_name = ?");
}

/* Compile methods for commands */

QVector<QString>
PostgresSchemaGrammar::compileForeign(const Blueprint &/*unused*/,
                                      const ForeignKeyCommand &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QVector<QString>
PostgresSchemaGrammar::invokeCompileMethod(const CommandDefinition &/*unused*/,
                                           const DatabaseConnection &/*unused*/,
                                           const Blueprint &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString PostgresSchemaGrammar::addModifiers(QString &&/*unused*/,
                                            const ColumnDefinition &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString PostgresSchemaGrammar::getType(const ColumnDefinition &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

} // namespace Orm::SchemaNs::Grammars

TINYORM_END_COMMON_NAMESPACE
