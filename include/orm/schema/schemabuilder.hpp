#pragma once
#ifndef ORM_SCHEMABUILDER_HPP
#define ORM_SCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QStringList>
#include <QtGlobal>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class DatabaseConnection;

namespace Schema
{
namespace Grammars
{
    class SchemaGrammar;
}

    /*! Database schema repository base class. */
    class SHAREDLIB_EXPORT SchemaBuilder
    {
        Q_DISABLE_COPY(SchemaBuilder)

    public:
        /*! Constructor. */
        explicit SchemaBuilder(DatabaseConnection &connection);
        /*! Virtual destructor. */
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

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMABUILDER_HPP
