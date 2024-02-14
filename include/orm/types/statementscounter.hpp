#pragma once
#ifndef ORM_TYPES_STATEMENTSCOUNTER_HPP
#define ORM_TYPES_STATEMENTSCOUNTER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Types
{

    /*! Executed statements counter. */
    struct StatementsCounter
    {
        // CUR qint64 silverqx
        /*! Normal select statements. */
        int normal = -1;
        /*! Affecting statements (UPDATE, INSERT, DELETE). */
        int affecting = -1;
        /*! Transactional statements (START TRANSACTION, ROLLBACK, COMMIT, SAVEPOINT). */
        int transactional = -1;
    };

} // namespace Types

    /*! Alias for the Types::StatementsCounter, shortcut alias. */
    using Types::StatementsCounter; // NOLINT(misc-unused-using-decls)

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TYPES_STATEMENTSCOUNTER_HPP
