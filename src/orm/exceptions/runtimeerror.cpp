#include "orm/exceptions/runtimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Exceptions
{

RuntimeError::RuntimeError(const char *message)
    : std::runtime_error(message)
{}

RuntimeError::RuntimeError(const QString &message)
    : std::runtime_error(message.toUtf8().constData())
{}

} // namespace Orm::Exceptions
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
