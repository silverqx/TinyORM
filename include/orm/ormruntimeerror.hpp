#ifndef ORMRUNTIMEERROR_H
#define ORMRUNTIMEERROR_H

#include <QString>

#include <stdexcept>

#include "export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT OrmRuntimeError : public std::runtime_error
    {
    public:
        explicit inline OrmRuntimeError(const char *message)
            : std::runtime_error(message)
        {}
        explicit inline OrmRuntimeError(const QString &message)
            : std::runtime_error(message.toUtf8().constData())
        {}

        /*! Return exception message as a QString. */
        inline const QString &message() const
        { return m_message; }

    protected:
        /*! Exception message. */
        const QString m_message {what()};
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ORMRUNTIMEERROR_H
