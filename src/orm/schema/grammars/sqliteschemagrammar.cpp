#include "orm/schema/grammars/sqliteschemagrammar.hpp"

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs::Grammars
{

/* Compile methods for the SchemaBuilder */

QString SQLiteSchemaGrammar::compileEnableForeignKeyConstraints() const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString SQLiteSchemaGrammar::compileDisableForeignKeyConstraints() const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString SQLiteSchemaGrammar::compileColumnListing(const QString &table) const
{
    auto table_ = table;

    // TODO study, wtf is this 🤔 silverqx
    table_.replace(DOT, "__");

    return QStringLiteral("pragma table_info(%1)").arg(BaseGrammar::wrap(table_));
}

/* Compile methods for commands */

QVector<QString>
SQLiteSchemaGrammar::compileForeign(const Blueprint &/*unused*/,
                                    const ForeignKeyCommand &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QVector<QString>
SQLiteSchemaGrammar::invokeCompileMethod(const CommandDefinition &/*unused*/,
                                         const DatabaseConnection &/*unused*/,
                                         const Blueprint &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString SQLiteSchemaGrammar::addModifiers(QString &&/*unused*/,
                                          const ColumnDefinition &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString SQLiteSchemaGrammar::escapeString(QString /*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

QString SQLiteSchemaGrammar::getType(const ColumnDefinition &/*unused*/) const
{
    throw Exceptions::RuntimeError(NotImplemented);
}

} // namespace Orm::SchemaNs::Grammars

TINYORM_END_COMMON_NAMESPACE
