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
   method calls are used as factory methods but I denied it after I tested it. */

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

/* public */

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
const QVariant &NullVariant::Bool()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::Bool));
    return cached;
}

const QVariant &NullVariant::Short()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::Short));
    return cached;
}

const QVariant &NullVariant::UShort()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::UShort));
    return cached;
}

const QVariant &NullVariant::Int()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::Int));
    return cached;
}

const QVariant &NullVariant::UInt()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::UInt));
    return cached;
}

const QVariant &NullVariant::LongLong()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::LongLong));
    return cached;
}

const QVariant &NullVariant::ULongLong()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::ULongLong));
    return cached;
}

const QVariant &NullVariant::Double()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::Double));
    return cached;
}

const QVariant &NullVariant::QByteArray()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::QByteArray));
    return cached;
}

const QVariant &NullVariant::QDate()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::QDate));
    return cached;
}

const QVariant &NullVariant::QDateTime()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::QDateTime));
    return cached;
}

const QVariant &NullVariant::QTime()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::QTime));
    return cached;
}

const QVariant &NullVariant::QString()
{
    static const QVariant cached = QVariant(QMetaType(QMetaType::QString));
    return cached;
}
#else
const QVariant &NullVariant::Bool()
{
    static const QVariant cached(QVariant::Bool);
    return cached;
}

const QVariant &NullVariant::Short()
{
    // Qt5 QVariant doesn't support unsigned/ short int
    static const QVariant cached(QVariant::Int);
    return cached;
}

const QVariant &NullVariant::UShort()
{
    // Qt5 QVariant doesn't support unsigned/ short int
    static const QVariant cached(QVariant::UInt);
    return cached;
}

const QVariant &NullVariant::Int()
{
    static const QVariant cached(QVariant::Int);
    return cached;
}

const QVariant &NullVariant::UInt()
{
    static const QVariant cached(QVariant::UInt);
    return cached;
}

const QVariant &NullVariant::LongLong()
{
    static const QVariant cached(QVariant::LongLong);
    return cached;
}

const QVariant &NullVariant::ULongLong()
{
    static const QVariant cached(QVariant::ULongLong);
    return cached;
}

const QVariant &NullVariant::Double()
{
    static const QVariant cached(QVariant::Double);
    return cached;
}

const QVariant &NullVariant::QByteArray()
{
    static const QVariant cached(QVariant::ByteArray);
    return cached;
}

const QVariant &NullVariant::QDate()
{
    static const QVariant cached(QVariant::Date);
    return cached;
}

const QVariant &NullVariant::QDateTime()
{
    static const QVariant cached(QVariant::DateTime);
    return cached;
}

const QVariant &NullVariant::QString()
{
    static const QVariant cached(QVariant::String);
    return cached;
}
#endif

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
