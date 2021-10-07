#include "fs.hpp"

#include <QDir>
#include <QString>

namespace TestUtils
{

QString Fs::cleanPath(const QString &path)
{
    const auto ret = path.trimmed();

    if (ret.isEmpty())
        return ret;

    return QDir::cleanPath(ret);
}

QString Fs::absolutePath(const QString &path)
{
    return QDir(cleanPath(path)).absolutePath();
}

} // namespace TestUtils
