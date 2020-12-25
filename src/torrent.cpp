#include "torrent.hpp"

Torrent::Torrent(const QVector<Orm::AttributeItem> &attributes)
    : BaseModel(attributes)
{}

//QDebug operator<<(QDebug debug, const Torrent &c)
//{
//    QDebugStateSaver saver(debug);
//    debug.nospace() << c.getAttribute("id") << ", "
//                    << c.getAttribute("name");

//    return debug;
//}
