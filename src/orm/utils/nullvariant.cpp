#include "orm/utils/nullvariant.hpp"

#include <QVariant>

/* Is much faster to instantiate the QVariant only once and cache it like returning
   by value, even if the recipient of the QVariant value is expecting the rvalue.
   I have tested it with the for loop using 1'000'000 iterations.
   It's like 5ms vs 180ms or 84ms, the 5ms is of course with the cached QVariant and
   the higher elapsed times are for returning the QVariant by value with different
   combinations of rvalue or passing by value on the recipient side (eg. to some
   function).
   I wanted to refactor this to returning by value because practically all NullVariant
   method calls are used as factory methods but I denied it after I tested it.

   Also, the primary reason this class exists was to support the older Qt v5.15,
   which means this class isn't needed anymore, but I have decided to leave it because of
   the above-described performance boost. 😎 */

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

/* public */

const QVariant &NullVariant::Bool()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::Bool));
    return Cached;
}

const QVariant &NullVariant::Short()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::Short));
    return Cached;
}

const QVariant &NullVariant::UShort()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::UShort));
    return Cached;
}

const QVariant &NullVariant::Int()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::Int));
    return Cached;
}

const QVariant &NullVariant::UInt()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::UInt));
    return Cached;
}

const QVariant &NullVariant::LongLong()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::LongLong));
    return Cached;
}

const QVariant &NullVariant::ULongLong()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::ULongLong));
    return Cached;
}

const QVariant &NullVariant::Double()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::Double));
    return Cached;
}

const QVariant &NullVariant::QByteArray()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::QByteArray));
    return Cached;
}

const QVariant &NullVariant::QDate()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::QDate));
    return Cached;
}

const QVariant &NullVariant::QDateTime()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::QDateTime));
    return Cached;
}

const QVariant &NullVariant::QTime()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::QTime));
    return Cached;
}

const QVariant &NullVariant::QString()
{
    static const QVariant Cached = QVariant(QMetaType(QMetaType::QString));
    return Cached;
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
