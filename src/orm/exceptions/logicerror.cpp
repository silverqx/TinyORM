#include "orm/exceptions/logicerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Exceptions
{

LogicError::LogicError(const char *message)
    : std::logic_error(message)
{}

LogicError::LogicError(const QString &message)
    : std::logic_error(message.toUtf8().constData())
{}

} // namespace Orm::Exceptions
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
