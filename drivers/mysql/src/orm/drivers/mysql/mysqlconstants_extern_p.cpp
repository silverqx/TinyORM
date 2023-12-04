#include "orm/drivers/mysql/mysqlconstants_extern_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals;

namespace Orm::Drivers::MySql::Constants
{

    // Common chars
    const QChar SEMICOLON = ';'_L1;
    const QChar COLON     = ':'_L1;
    const QChar DASH      = '-'_L1;
    const QChar EQ_C      = '='_L1;

} // namespace Orm::Drivers::MySql::Constants

TINYORM_END_COMMON_NAMESPACE
