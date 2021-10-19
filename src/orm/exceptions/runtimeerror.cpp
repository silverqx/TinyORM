#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

RuntimeError::RuntimeError(const char *message)
    : std::runtime_error(message)
{}

RuntimeError::RuntimeError(const QString &message)
    : std::runtime_error(message.toUtf8().constData())
{}

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE
