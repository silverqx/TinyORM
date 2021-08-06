#pragma once
#ifndef SQLITESCHEMAGRAMMAR_H
#define SQLITESCHEMAGRAMMAR_H

#include "orm/schema/grammars/schemagrammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema::Grammars
{

    /*! SQLite schemma grammar. */
    class SHAREDLIB_EXPORT SQLiteSchemaGrammar : public SchemaGrammar
    {
        Q_DISABLE_COPY(SQLiteSchemaGrammar)

    public:
        /*! Default constructor. */
        SQLiteSchemaGrammar() = default;

        /*! Compile the query to determine the list of columns. */
        QString compileColumnListing(const QString &table = "") const override;
    };

} // namespace Orm::Schema::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SQLITESCHEMAGRAMMAR_H
