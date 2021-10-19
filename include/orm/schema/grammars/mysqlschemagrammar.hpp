#pragma once
#ifndef MYSQLSCHEMAGRAMMAR_HPP
#define MYSQLSCHEMAGRAMMAR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/grammars/schemagrammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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

TINYORM_END_COMMON_NAMESPACE

#endif // MYSQLSCHEMAGRAMMAR_HPP
