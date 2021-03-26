#ifndef LOGICERROR_H
#define LOGICERROR_H

#include <stdexcept>

#include "export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT LogicError : public std::logic_error
    {
    public:
        explicit inline LogicError(const char *message)
            : std::logic_error(message)
        {}
        explicit inline LogicError(const QString &message)
            : std::logic_error(message.toUtf8().constData())
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

#endif // LOGICERROR_H
