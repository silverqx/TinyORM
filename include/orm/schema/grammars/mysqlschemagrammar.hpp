#pragma once
#ifndef MYSQLSCHEMAGRAMMAR_HPP
#define MYSQLSCHEMAGRAMMAR_HPP

#include "orm/schema/grammars/schemagrammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema::Grammars
{

    /*! MySql schema grammar. */
    class SHAREDLIB_EXPORT MySqlSchemaGrammar : public SchemaGrammar
    {
        Q_DISABLE_COPY(MySqlSchemaGrammar)

    public:
        /*! Default constructor. */
        MySqlSchemaGrammar() = default;

        /*! Compile the query to determine the list of columns. */
        QString compileColumnListing(const QString &table = "") const override;
    };

} // namespace Orm::Schema::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MYSQLSCHEMAGRAMMAR_HPP
