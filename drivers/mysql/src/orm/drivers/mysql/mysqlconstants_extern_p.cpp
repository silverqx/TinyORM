#include "orm/drivers/mysql/mysqlconstants_extern_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::MySql::Constants
{

    // Common chars - QChar(u'') is faster than ''_L1
    const QChar DOT       = QChar(u'.');
    const QChar SEMICOLON = QChar(u';');
    const QChar COLON     = QChar(u':');
    const QChar DASH      = QChar(u'-');
    const QChar EQ_C      = QChar(u'=');
    const QChar BACKTICK  = QChar(u'`');

    // Common strings
    const QString EMPTY   = u""_s; // This is fastest
    const QString COMMA   = u", "_s;

    // Database related
    const QString QMYSQL  = u"QMYSQL"_s;
//    const QString QPSQL   = u"QPSQL"_s;
//    const QString QSQLITE = u"QSQLITE"_s;

} // namespace Orm::Drivers::MySql::Constants

TINYORM_END_COMMON_NAMESPACE
