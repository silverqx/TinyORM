#include "orm/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

LogicError::LogicError(const char *message)
    : std::logic_error(message)
{}

LogicError::LogicError(const QString &message)
    : std::logic_error(message.toUtf8().constData())
{}

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE
