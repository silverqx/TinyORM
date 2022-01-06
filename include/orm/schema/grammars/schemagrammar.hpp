#pragma once
#ifndef ORM_SCHEMA_GRAMMARS_SCHEMAGRAMMAR_HPP
#define ORM_SCHEMA_GRAMMARS_SCHEMAGRAMMAR_HPP

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
        inline SchemaGrammar() = default;
        /*! Pure virtual destructor. */
        inline ~SchemaGrammar() override = 0;

        /*! Compile the query to determine the list of columns. */
        virtual QString compileColumnListing(const QString &table = "") const = 0;
    };

    SchemaGrammar::~SchemaGrammar() = default;

} // namespace Orm::Schema::Grammars

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_GRAMMARS_SCHEMAGRAMMAR_HPP
