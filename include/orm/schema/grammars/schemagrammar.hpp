#pragma once
#ifndef SCHEMAGRAMMAR_H
#define SCHEMAGRAMMAR_H

#include "orm/basegrammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema::Grammars
{

    /*! Database schema grammar base class. */
    class SHAREDLIB_EXPORT SchemaGrammar : public BaseGrammar
    {
        Q_DISABLE_COPY(SchemaGrammar)

    public:
        /*! Default constructor. */
        SchemaGrammar() = default;
        /*! Pure virtual destructor. */
        virtual ~SchemaGrammar() = 0;

        /*! Compile the query to determine the list of columns. */
        virtual QString compileColumnListing(const QString &table = "") const = 0;
    };

    inline SchemaGrammar::~SchemaGrammar() = default;

} // namespace Orm::Schema::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SCHEMAGRAMMAR_H
