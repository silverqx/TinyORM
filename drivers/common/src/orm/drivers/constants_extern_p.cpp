#include "orm/drivers/constants_extern_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Constants
{

    // Common chars - QChar(u'') is faster than ''_L1
    const QChar SPACE = QChar(u' ');
    const QChar DOT   = QChar(u'.');
    const QChar QUOTE = QChar(u'"');

    // Common strings
    const QString NEWLINE = u"\n"_s;
    const QString COMMA   = u", "_s;
    const QString null_   = u"null"_s;

    // Database related
    const QString QMYSQL  = u"QMYSQL"_s;
//    const QString QPSQL   = u"QPSQL"_s;
//    const QString QSQLITE = u"QSQLITE"_s;

    // Others
    const QString NotImplemented = u"Not implemented :/."_s;

} // namespace Orm::Drivers::Constants

TINYORM_END_COMMON_NAMESPACE
