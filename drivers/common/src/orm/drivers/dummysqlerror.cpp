#include "orm/drivers/dummysqlerror.hpp"

#ifndef QT_NO_DEBUG_STREAM
#  include <QDebug>
#endif

#ifndef QT_NO_DEBUG_STREAM
QDebug
operator<<(QDebug debug,
           const TINYORM_PREPEND_NAMESPACE(Orm::Drivers::DummySqlError) &/*unused*/)
{
    debug << "DummySqlError(errorType: NoError)";
    return debug;
}
#endif
