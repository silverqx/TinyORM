#include "orm/constants_extern.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Constants
{

    // Common chars
    const QChar SPACE      = QChar(' ');
    const QChar DOT        = QChar('.');
    const QChar SEMICOLON  = QChar(';');
    const QChar COLON      = QChar(':');
    const QChar UNDERSCORE = QChar('_');
    const QChar DASH       = QChar('-');
    const QChar MINUS      = QChar('-');
    const QChar PLUS       = QChar('+');
    const QChar ASTERISK_C = QChar('*');
    const QChar COMMA_C    = QChar(',');
    const QChar EQ_C       = QChar('=');
    const QChar NOT_C      = QChar('!');
    const QChar LT_C       = QChar('<');
    const QChar GT_C       = QChar('>');

    // Common strings
    const QString ASTERISK    = QStringLiteral("*");
    const QString COMMA       = QStringLiteral(", ");
    const QString INNER       = QStringLiteral("inner");
    const QString LEFT        = QStringLiteral("left");
    const QString RIGHT       = QStringLiteral("right");
    const QString CROSS       = QStringLiteral("cross");
    const QString ASC         = QStringLiteral("asc");
    const QString DESC        = QStringLiteral("desc");
    const QString ID          = QStringLiteral("id");
    const QString NAME        = QStringLiteral("name");
    const QString CREATED_AT  = QStringLiteral("created_at");
    const QString UPDATED_AT  = QStringLiteral("updated_at");
    const QString PARENTH_ONE = QStringLiteral("(%1)");
    const QString QMYSQL      = QStringLiteral("QMYSQL");
    const QString QPSQL       = QStringLiteral("QPSQL");
    const QString QSQLITE     = QStringLiteral("QSQLITE");
    const QString MYSQL_      = QStringLiteral("MySQL");
    const QString POSTGRESQL  = QStringLiteral("PostgreSQL");
    const QString SQLITE      = QStringLiteral("SQLite");

    // Comparison/logical/search operators
    const QString EQ       = QStringLiteral("=");
    const QString NE       = QStringLiteral("!=");
    const QString LT       = QStringLiteral("<");
    const QString LE       = QStringLiteral("<=");
    const QString GT       = QStringLiteral(">");
    const QString GE       = QStringLiteral(">=");
    const QString OR       = QStringLiteral("or");
    const QString AND      = QStringLiteral("and");
    const QString NOT      = QStringLiteral("not");
    const QString LIKE     = QStringLiteral("like");
    const QString NLIKE    = QStringLiteral("not like");
    const QString ILIKE    = QStringLiteral("ilike");

    // Alternatives
    const QString NE_      = QStringLiteral("<>");
    const QString OR_      = QStringLiteral("||");
    const QString AND_     = QStringLiteral("&&");
    const QString NOT_     = QStringLiteral("!");

    // Bitewise operators
    const QString B_OR     = QStringLiteral("|");
    const QString B_AND    = QStringLiteral("&");

} // namespace Orm::Constants

TINYORM_END_COMMON_NAMESPACE
