#include "string.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
namespace Orm
{

QString Utils::String::toSnake(const QString &string)
{
    static const QRegularExpression regExp1 {"(.)([A-Z][a-z]+)"};
    static const QRegularExpression regExp2 {"([a-z0-9])([A-Z])"};

    QString result = string;

    result.replace(regExp1, "\\1_\\2");
    result.replace(regExp2, "\\1_\\2");

    return result.toLower();
}

} // namespace Orm
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif
