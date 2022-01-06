#pragma once
#ifndef ORM_SCHEMA_GRAMMARS_SQLITESCHEMAGRAMMAR_HPP
#define ORM_SCHEMA_GRAMMARS_SQLITESCHEMAGRAMMAR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/grammars/schemagrammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Schema::Grammars
{

    /*! SQLite schemma grammar. */
    class SHAREDLIB_EXPORT SQLiteSchemaGrammar : public SchemaGrammar
    {
        Q_DISABLE_COPY(SQLiteSchemaGrammar)

    public:
        /*! Default constructor. */
        inline SQLiteSchemaGrammar() = default;
        /*! Virtual destructor. */
        inline ~SQLiteSchemaGrammar() override = default;

        /*! Compile the query to determine the list of columns. */
        QString compileColumnListing(const QString &table = "") const override;
    };

} // namespace Orm::Schema::Grammars

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_GRAMMARS_SQLITESCHEMAGRAMMAR_HPP
