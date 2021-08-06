#pragma once
#ifndef POSTGRESSCHEMAGRAMMAR_H
#define POSTGRESSCHEMAGRAMMAR_H

#include "orm/schema/grammars/schemagrammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema::Grammars
{

    /*! PostgreSql schemma grammar. */
    class SHAREDLIB_EXPORT PostgresSchemaGrammar : public SchemaGrammar
    {
        Q_DISABLE_COPY(PostgresSchemaGrammar)

    public:
        /*! Default constructor. */
        PostgresSchemaGrammar() = default;

        /*! Compile the query to determine the list of columns. */
        QString compileColumnListing(const QString &table = "") const override;
    };

} // namespace Orm::Schema::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // POSTGRESSCHEMAGRAMMAR_H
