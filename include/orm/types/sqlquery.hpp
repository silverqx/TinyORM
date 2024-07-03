#pragma once
#ifndef ORM_TYPES_SQLQUERY_HPP
#define ORM_TYPES_SQLQUERY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlQuery

#include <optional>

#include "orm/macros/export.hpp"
#include "orm/ormtypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Query::Grammars
{
    class Grammar;
}

namespace Types
{

    /*! Wrapper around the QSqlQuery to fix QDateTime time zones. */
    class TINYORM_EXPORT SqlQuery : public TSqlQuery
    {
        /*! Alias for the QueryGrammar. */
        using QueryGrammar = Query::Grammars::Grammar;

    public:
        /*! Deleted default constructor (not needed). */
        SqlQuery() = delete;
        /*! Default destructor. */
        ~SqlQuery() = default;

        /*! Deleted copy constructor (follow the base class). */
        SqlQuery(const SqlQuery &other) = delete;
        /*! Deleted copy assignment operator (follow the base class). */
        SqlQuery &operator=(const SqlQuery &other) = delete;

        /*! Move constructor. */
        SqlQuery(SqlQuery &&other) noexcept = default;
        /*! Move assignment operator. */
        SqlQuery &operator=(SqlQuery &&other) noexcept = default;

        /*! Constructor from the QSqlQuery type and time zone from the configuration. */
        SqlQuery(TSqlQuery &&other, const QtTimeZoneConfig &qtTimeZone,
                 const QueryGrammar &queryGrammar,
                 std::optional<bool> returnQDateTime);

        /*! Return the value of field index in the current record. */
        inline QVariant value(int index) const;
        /*! Return the value of the field called name in the current record. */
        inline QVariant value(const QString &name) const;

    private:
        /*! Common value() method that correctly handles QDateTime's time zone. */
        QVariant valueInternal(QVariant &&value) const;

        /*! Determine whether it should try to convert to the QDateTime/QDate. */
        bool shouldPrepareDateTime(const QVariant &value) const;

        /*! Prepare a value as QDateTime or QDate. */
        std::optional<std::variant<QDateTime, QDate>>
        prepareDateTime(const QVariant &value) const;
        /*! Return a value as QDate object. */
        static std::optional<QDate> asDate(const QString &value);
        /*! Return a value as QDateTime object. */
        std::optional<QDateTime> asDateTime(const QString &value) const;

        /*! Determine how the QDateTime time zone will be converted. */
        QtTimeZoneConfig m_qtTimeZone;
        /*! Determine whether the QDateTime time zone should be converted. */
        bool m_isConvertingTimeZone;

        /*! Determine whether the database type used for the current query is SQLite. */
        bool m_isSQLiteDb;
        /*! Date format for database stored dates (SQLite only). */
        std::optional<QString> m_dateFormat;
        /*! Determine whether to return the QDateTime/QDate or QString (SQLite only). */
        std::optional<bool> m_returnQDateTime;
    };

    /* public */

    QVariant SqlQuery::value(const int index) const
    {
        return valueInternal(TSqlQuery::value(index));
    }

    QVariant SqlQuery::value(const QString &name) const
    {
        return valueInternal(TSqlQuery::value(name));
    }

} // namespace Types

    /*! Alias for the Types::SqlQuery, shortcut alias. */
    using Types::SqlQuery; // NOLINT(misc-unused-using-decls)

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TYPES_SQLQUERY_HPP
