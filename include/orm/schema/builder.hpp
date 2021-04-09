#ifndef SCHEMABUILDER_H
#define SCHEMABUILDER_H

#include "export.hpp"

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
    class Grammar;
}

    class SHAREDLIB_EXPORT Builder
    {
        Q_DISABLE_COPY(Builder)

    public:
        explicit Builder(DatabaseConnection &connection);

        /*! Get the column listing for a given table. */
        virtual QStringList getColumnListing(const QString &table) const;

    protected:
        using SchemaGrammar = Grammars::Grammar;

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
