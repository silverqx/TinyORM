#pragma once
#ifndef ORM_SCHEMA_GRAMMARS_POSTGRESSCHEMAGRAMMAR_HPP
#define ORM_SCHEMA_GRAMMARS_POSTGRESSCHEMAGRAMMAR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/grammars/schemagrammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Schema::Grammars
{

    /*! PostgreSql schemma grammar. */
    class SHAREDLIB_EXPORT PostgresSchemaGrammar : public SchemaGrammar
    {
        Q_DISABLE_COPY(PostgresSchemaGrammar)

    public:
        /*! Default constructor. */
        inline PostgresSchemaGrammar() = default;
        /*! Virtual destructor. */
        inline ~PostgresSchemaGrammar() override = default;

        /*! Compile the query to determine the list of columns. */
        QString compileColumnListing(const QString &table = "") const override;
    };

} // namespace Orm::Schema::Grammars

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_GRAMMARS_POSTGRESSCHEMAGRAMMAR_HPP
