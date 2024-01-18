#include "orm/drivers/constants_extern_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

namespace Orm::Drivers::Constants
{

    // Common chars
    const QChar SPACE = ' '_L1;
    const QChar DOT   = '.'_L1;
    const QChar QUOTE = '"'_L1;

    // Common strings
    const QString NEWLINE = u"\n"_s;
    const QString null_   = u"null"_s;

    // Database related
    const QString QMYSQL  = u"QMYSQL"_s;
//    const QString QPSQL   = u"QPSQL"_s;
//    const QString QSQLITE = u"QSQLITE"_s;

    // Others
    const QString NotImplemented = u"Not implemented :/."_s;

} // namespace Orm::Drivers::Constants

TINYORM_END_COMMON_NAMESPACE
