#ifndef ORMERROR_H
#define ORMERROR_H

#include <stdexcept>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    // TODO investigate and rework all orm exception classes silverqx
    class OrmError : public std::runtime_error
    {
    public:
        explicit inline OrmError(const char *message)
            : std::runtime_error(message)
        {}
        explicit inline OrmError(const QString &message)
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

#endif // ORMERROR_H
