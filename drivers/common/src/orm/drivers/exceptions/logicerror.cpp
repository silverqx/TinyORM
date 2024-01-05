#include "orm/drivers/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Exceptions
{

/* public */

LogicError::LogicError(const char *message)
    : std::logic_error(message)
{}

LogicError::LogicError(const QString &message)
    : std::logic_error(message.toUtf8().constData())
{}

LogicError::LogicError(const std::string &message)
    : std::logic_error(message)
{}

} // namespace Orm::Drivers::Exceptions

TINYORM_END_COMMON_NAMESPACE
