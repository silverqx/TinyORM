#ifndef SQLITESCHEMAGRAMMAR_H
#define SQLITESCHEMAGRAMMAR_H

#include "orm/schema/grammars/grammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema::Grammars
{

    class SHAREDLIB_EXPORT SQLiteGrammar : public Grammar
    {
        Q_DISABLE_COPY(SQLiteGrammar)

    public:
        SQLiteGrammar() = default;

        /*! Compile the query to determine the list of columns. */
        QString compileColumnListing(const QString &table = "") const override;
    };

} // namespace Orm::Schema::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SQLITESCHEMAGRAMMAR_H
