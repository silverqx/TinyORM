#include "fs.hpp"

#include <QDir>
#include <QString>

namespace TestUtils
{

QString Fs::cleanPath(const QString &path)
{
    auto pathTrimmed = path.trimmed();

    if (pathTrimmed.isEmpty())
        return pathTrimmed;

    return QDir::cleanPath(pathTrimmed);
}

QString Fs::absolutePath(const QString &path)
{
    return QDir(cleanPath(path)).absolutePath();
}

} // namespace TestUtils
