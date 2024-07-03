#pragma once
#ifndef ORM_QUERY_JOINCLAUSE_HPP
#define ORM_QUERY_JOINCLAUSE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/querybuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query
{

    /*! Join clause. */
    class TINYORM_EXPORT JoinClause final : public Builder // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Type for the Join table (FromClause). */
        using JoinTable = FromClause;

        /*! Copy constructor. */
        JoinClause(const JoinClause &) = default;
        /*! Deleted copy assignment operator (class contains reference and const). */
        JoinClause &operator=(const JoinClause &) = delete;

        /*! Move constructor. */
        JoinClause(JoinClause &&) noexcept = default;
        /*! Deleted move assignment operator (class contains reference and const). */
        JoinClause &operator=(JoinClause &&) = delete;

        /*! Virtual destructor. */
        ~JoinClause() final = default;

        /*! Constructor with the table as QString. */
        JoinClause(const Builder &query, const QString &type, const QString &table);
        /*! Constructor with the table as Expression. */
        JoinClause(const Builder &query, const QString &type, const Expression &table);
        /*! Constructor with the table as Expression. */
        JoinClause(const Builder &query, const QString &type, Expression &&table);
        /*! Constructor with the table as Expression. */
        JoinClause(const Builder &query, const QString &type, const JoinTable &table);

        /*! Add an "on" clause to the join. */
        JoinClause &on(const QString &first, const QString &comparison,
                       const QString &second, const QString &condition = AND);
        /*! Add an "or on" clause to the join. */
        JoinClause &orOn(const QString &first, const QString &comparison,
                         const QString &second);

        /*! Get the type of join being performed. */
        inline const QString &getType() const;
        /*! Get the table the join clause is joining to. */
        inline const std::variant<std::monostate, QString, Expression> &
        getTable() const;

        /*! Get a new instance of the join clause builder. */
        std::shared_ptr<Builder> newQuery() const final;

    protected:
        /*! Create a new query instance for a sub-query. */
        std::shared_ptr<Builder> forSubQuery() const final;

    private:
        /*! The type of join being performed. */
        /*const*/ QString m_type;
        /*! The table the join clause is joining to. */
        /*const*/ JoinTable m_table;
    };

    const QString &
    JoinClause::getType() const
    {
        return m_type;
    }

    const std::variant<std::monostate, QString, Expression> &
    JoinClause::getTable() const
    {
        return m_table;
    }

} // namespace Orm::Query

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_JOINCLAUSE_HPP
