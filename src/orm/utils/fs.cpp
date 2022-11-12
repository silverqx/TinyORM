#include "orm/utils/fs.hpp"

#include <QDir>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

QString Fs::resolveHome(QString filepath)
{
    if (filepath == QLatin1Char('~') || filepath.startsWith(QStringLiteral("~/")))
        filepath.replace (0, 1, QDir::homePath());

    return QDir::cleanPath(filepath);
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
