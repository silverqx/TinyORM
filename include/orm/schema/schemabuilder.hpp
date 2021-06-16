#pragma once
#ifndef SCHEMABUILDER_H
#define SCHEMABUILDER_H

#include <QStringList>
#include <QtGlobal>

#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
    class DatabaseConnection;

namespace Schema
{
namespace Grammars
{
    class SchemaGrammar;
}

    class SHAREDLIB_EXPORT SchemaBuilder
    {
        Q_DISABLE_COPY(SchemaBuilder)

    public:
        explicit SchemaBuilder(DatabaseConnection &connection);
        inline virtual ~SchemaBuilder() = default;

        /*! Get the column listing for a given table. */
        virtual QStringList getColumnListing(const QString &table) const;

    protected:
        using SchemaGrammar = Grammars::SchemaGrammar;

        /*! The database connection instance. */
        DatabaseConnection &m_connection;
        /*! The schema grammar instance. */
        const SchemaGrammar &m_grammar;
    };

} // namespace Orm::Schema
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SCHEMABUILDER_H
