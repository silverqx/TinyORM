#pragma once
#ifndef ORM_SCHEMAGRAMMAR_HPP
#define ORM_SCHEMAGRAMMAR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/basegrammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMAGRAMMAR_HPP
