#include "orm/utils/fs.hpp"

#include <QDir>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

QString Fs::resolveHome(QString filepath)
{
    using Qt::StringLiterals::operator""_L1;

    if (filepath == u'~' || filepath.startsWith("~/"_L1))
        filepath.replace (0, 1, QDir::homePath());

    return QDir::cleanPath(filepath);
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
