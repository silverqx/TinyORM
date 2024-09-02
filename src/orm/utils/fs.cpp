#include "orm/utils/fs.hpp"

#include <QDir>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

QString Fs::resolveHome(QString filepath)
{
    using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

    if (filepath == '~'_L1 || filepath.startsWith(u"~/"_s))
        filepath.replace (0, 1, QDir::homePath());

    return QDir::cleanPath(filepath);
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
