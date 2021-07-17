#include "orm/constants.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Constants
{

    // CUR search for common join chars, strings silverqx
    const QString ASTERISK   = QStringLiteral("*");
    const QString COMMA      = QStringLiteral(", ");
    const QString INNER      = QStringLiteral("inner");
    const QString LEFT       = QStringLiteral("left");
    const QString RIGHT      = QStringLiteral("right");
    const QString CROSS      = QStringLiteral("cross");
    // CUR aggregates silverqx
    const QString ASC        = QStringLiteral("asc");
    const QString DESC       = QStringLiteral("desc");
    const QString ID         = QStringLiteral("id");
    const QString NAME       = QStringLiteral("name");
    const QString CREATED_AT = QStringLiteral("created_at");
    const QString UPDATED_AT = QStringLiteral("updated_at");

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

    const QString NE_      = QStringLiteral("<>");
    const QString OR_      = QStringLiteral("||");
    const QString AND_     = QStringLiteral("&&");
    const QString NOT_     = QStringLiteral("!");

    const QString B_OR     = QStringLiteral("|");
    const QString B_AND    = QStringLiteral("&");

} // namespace Orm::Constants
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
