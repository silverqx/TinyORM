#include "orm/drivers/mysql/mysqlconstants_extern_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

namespace Orm::Drivers::MySql::Constants
{

    // Common chars
    const QChar SEMICOLON = ';'_L1;
    const QChar COLON     = ':'_L1;
    const QChar DASH      = '-'_L1;
    const QChar EQ_C      = '='_L1;

    // Common strings
    const QString COMMA   = u", "_s;

    // Database related
    const QString QMYSQL  = u"QMYSQL"_s;
//    const QString QPSQL   = u"QPSQL"_s;
//    const QString QSQLITE = u"QSQLITE"_s;

} // namespace Orm::Drivers::MySql::Constants

TINYORM_END_COMMON_NAMESPACE
