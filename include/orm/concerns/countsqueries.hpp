#pragma once
#ifndef ORM_CONCERNS_COUNTSQUERIES_HPP
#define ORM_CONCERNS_COUNTSQUERIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QElapsedTimer>

#include <optional>

#include "orm/macros/export.hpp"
#include "orm/types/statementscounter.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

class DatabaseConnection;

namespace Concerns
{

    class ManagesTransactions;

    /*! Counts the number of executed queries and the elapsed time of queries. */
    class TINYORM_EXPORT CountsQueries
    {
        Q_DISABLE_COPY_MOVE(CountsQueries)

        // To access hitTransactionalCounters() method
        friend ManagesTransactions;

    public:
        /*! Default constructor. */
        CountsQueries() = default;
        /*! Pure virtual destructor, to pass -Weffc++. */
        inline virtual ~CountsQueries() = 0;

        /* Queries execution time counter */
        /*! Determine whether we're counting queries execution time. */
        bool countingElapsed() const;
        /*! Enable counting queries execution time on the current connection. */
        DatabaseConnection &enableElapsedCounter();
        /*! Disable counting queries execution time on the current connection. */
        DatabaseConnection &disableElapsedCounter();
        /*! Obtain queries execution time, -1 when disabled. */
        qint64 getElapsedCounter() const;
        /*! Obtain and reset queries execution time. */
        qint64 takeElapsedCounter();
        /*! Reset queries execution time. */
        DatabaseConnection &resetElapsedCounter();

        /* Queries executed counter */
        /*! Determine whether we're counting the number of executed queries. */
        bool countingStatements() const;
        /*! Enable counting the number of executed queries on the current connection. */
        DatabaseConnection &enableStatementsCounter();
        /*! Disable counting the number of executed queries on the current connection. */
        DatabaseConnection &disableStatementsCounter();
        /*! Obtain the number of executed queries, all counters are -1 when disabled. */
        const StatementsCounter &getStatementsCounter() const;
        /*! Obtain and reset the number of executed queries. */
        StatementsCounter takeStatementsCounter();
        /*! Reset the number of executed queries. */
        DatabaseConnection &resetStatementsCounter();

    protected:
        /* Queries execution time counter */
        /*! Indicates whether queries elapsed time are being counted. */
        bool m_countingElapsed = false;
        /*! Queries elpased time counter. */
        qint64 m_elapsedCounter = -1;

        /* Queries executed counter */
        /*! Indicates whether executed queries are being counted. */
        bool m_countingStatements = false;
        /*! Counts executed statements on current connection. */
        StatementsCounter m_statementsCounter {};

    private:
        /*! Count transactional queries execution time and statements counter. */
        std::optional<qint64>
        hitTransactionalCounters(QElapsedTimer timer, bool countElapsed);

        /*! Dynamic cast *this to the DatabaseConnection & derived type. */
        DatabaseConnection &databaseConnection();
    };

    /* public */

    CountsQueries::~CountsQueries() = default;

} // namespace Concerns
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_COUNTSQUERIES_HPP
