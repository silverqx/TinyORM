#include "tom/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Exceptions
{

RuntimeError::RuntimeError(const char *message)
    : std::runtime_error(message)
{}

RuntimeError::RuntimeError(const QString &message)
    : std::runtime_error(message.toUtf8().constData())
{}

RuntimeError::RuntimeError(const std::string &message)
    : std::runtime_error(message)
{}

} // namespace Tom::Exceptions

TINYORM_END_COMMON_NAMESPACE
