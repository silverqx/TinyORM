#ifndef MYSQLSCHEMAGRAMMAR_H
#define MYSQLSCHEMAGRAMMAR_H

#include "orm/schema/grammars/grammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema::Grammars
{

    class SHAREDLIB_EXPORT MySqlGrammar : public Grammar
    {
        Q_DISABLE_COPY(MySqlGrammar)

    public:
        MySqlGrammar() = default;

        /*! Compile the query to determine the list of columns. */
        QString compileColumnListing(const QString &table = "") const override;
    };

} // namespace Orm::Schema::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MYSQLSCHEMAGRAMMAR_H
