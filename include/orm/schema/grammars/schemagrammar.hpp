#ifndef SCHEMAGRAMMAR_H
#define SCHEMAGRAMMAR_H

#include "orm/basegrammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema::Grammars
{

    class SHAREDLIB_EXPORT SchemaGrammar : public BaseGrammar
    {
        Q_DISABLE_COPY(SchemaGrammar)

    public:
        SchemaGrammar() = default;
        virtual ~SchemaGrammar() = default;

        /*! Compile the query to determine the list of columns. */
        virtual QString compileColumnListing(const QString &table = "") const = 0;
    };

} // namespace Orm::Schema::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SCHEMAGRAMMAR_H
