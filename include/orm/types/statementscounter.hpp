#pragma once
#ifndef STATEMENTSCOUNTER_HPP
#define STATEMENTSCOUNTER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Types
{

    /*! Counts executed statements in a current connection. */
    struct StatementsCounter
    {
        /*! Normal select statements. */
        int normal = -1;
        /*! Affecting statements (UPDATE, INSERT, DELETE). */
        int affecting = -1;
        /*! Transactional statements (START TRANSACTION, ROLLBACK, COMMIT, SAVEPOINT). */
        int transactional = -1;
    };

} // namespace Orm::Types

    using StatementsCounter = Types::StatementsCounter;

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // STATEMENTSCOUNTER_HPP
