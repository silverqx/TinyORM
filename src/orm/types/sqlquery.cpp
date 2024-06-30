#include "orm/types/sqlquery.hpp"

#include TINY_INCLUDE_TSqlDriver

#include "orm/query/grammars/grammar.hpp" // IWYU pragma: keep
#include "orm/utils/helpers.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Utils::Helpers;

namespace Orm::Types
{

namespace
{
    /*! Determine whether the underlying SQL driver for the given SqlQuery is SQLite. */
    const auto isSQLiteDbmsType = [](const SqlQuery &query) -> bool
    {
#ifdef TINYORM_USING_QTSQLDRIVERS
        return query.driver()->dbmsType() == TSqlDriver::DbmsType::SQLite;
#elif defined(TINYORM_USING_TINYDRIVERS)
        return query.driverWeak().lock()->dbmsType() == TSqlDriver::DbmsType::SQLite;
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
    };
} // namespace

/* public */

SqlQuery::SqlQuery(TSqlQuery &&other, const QtTimeZoneConfig &qtTimeZone, // NOLINT(modernize-pass-by-value, cppcoreguidelines-rvalue-reference-param-not-moved)
                   const QueryGrammar &queryGrammar,
                   const std::optional<bool> returnQDateTime
)
    : TSqlQuery(std::move(other))
    , m_qtTimeZone(qtTimeZone)
    , m_isConvertingTimeZone(m_qtTimeZone.type != QtTimeZoneType::DontConvert)
    , m_isSQLiteDb(isSQLiteDbmsType(*this))
    // Following two are need by SQLite only
    , m_dateFormat(m_isSQLiteDb ? std::make_optional(queryGrammar.getDateFormat())
                                : std::nullopt)
    , m_returnQDateTime(returnQDateTime)
{}

/* private */

QVariant SqlQuery::valueInternal(QVariant &&value) const
{
    /* Nothing to convert, no qt_timezone given in config. or a value not valid/null OR
       not the QDateTime or don't return the QDateTime for QSQLITE. */
    if (!shouldPrepareDateTime(value))
        return std::move(value);

    // Prepare a value as QDateTime or QDate
    auto datetime = prepareDateTime(value);

    // Invalid datetime value (can't convert to QString -> QDateTime/QDate)
    if (!datetime)
        return std::move(value);

    // It still can contain a simple QDate object (standard date format)
    if (std::holds_alternative<QDate>(*datetime))
        return std::get<QDate>(*datetime);

    /* Set the correct time zone provided through the qt_timezone connection config.
       option for QDateTime values. It fixes buggy behavior of all QtSql database
       drivers. */
    return Helpers::setTimeZone(std::get<QDateTime>(*datetime), m_qtTimeZone);
}

bool SqlQuery::shouldPrepareDateTime(const QVariant &value) const
{
    // Nothing to convert, no qt_timezone given in config. or not valid/null
    if (!m_isConvertingTimeZone || !value.isValid() || value.isNull())
        return false;

    const auto typeId = value.typeId();

    // If it's the QDateTime or if should return the QDateTime/QDate for QSQLITE
    return typeId == QMetaType::QDateTime ||
            (m_isSQLiteDb && m_returnQDateTime && *m_returnQDateTime &&
             typeId == QMetaType::QString);
}

std::optional<std::variant<QDateTime, QDate>>
SqlQuery::prepareDateTime(const QVariant &value) const
{
    // QMYSQL and QPSQL drivers already return the QVariant(QDateTime)
    if (!m_isSQLiteDb)
        return value.value<QDateTime>();

    // The value has to be convertible to the QString so we can work with it
    if (!value.canConvert<QString>())
        return std::nullopt;

    const auto valueString = value.value<QString>();

    /* If the value is in the so-called simple format (year-month-day), then we will
       instantiate the QDate instances from this format. This is only done for date
       fields/columns, while still supporting QDateTime conversion. */
    if (Helpers::isStandardDateFormat(valueString))
        return asDate(valueString);

    return asDateTime(valueString);
}

std::optional<QDate> SqlQuery::asDate(const QString &value)
{
    /* If the value is in simply year, month, day format, we will instantiate the
       QDate instances from that format. Again, this provides for simple date
       fields on the database, while still supporting QDateTime conversion. */
    auto date = QDateTime::fromString(value, QStringLiteral("yyyy-M-d"));

    if (!date.isValid())
        return std::nullopt;

    return date.date();
}

std::optional<QDateTime> SqlQuery::asDateTime(const QString &value) const
{
    // FUTURE datetime, apply this only if the return_qdatetime will support column names, so apply it only for the defined columns silverqx
    /* If this value is an integer, we will assume it is a Unix timestamp's value
       and format a QDateTime object from this timestamp. This allows flexibility
       when defining your date fields as they might be Unix timestamps here. */
//    if (StringUtils::isNumber(valueString))
//        // TODO switch ms accuracy? For the u_dateFormat too? silverqx
//        if (auto date = QDateTime::fromSecsSinceEpoch(value.value<qint64>());
//            date.isValid()
//        )
//            return date;

    Q_ASSERT(m_dateFormat);

    /* Finally, we will just assume this date is in the format used by default on
       the database connection and use that format to create the QDateTime object
       that is returned back out to the developers after we convert it here. */
    if (auto date = QDateTime::fromString(value, *m_dateFormat); // NOLINT(bugprone-unchecked-optional-access)
        date.isValid()
    )
        return date;

    return std::nullopt;
}

} // namespace Orm::Types

TINYORM_END_COMMON_NAMESPACE
