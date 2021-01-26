#ifndef ORMLOGICERROR_H
#define ORMLOGICERROR_H

#include <stdexcept>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class OrmLogicError : public std::logic_error
    {
    public:
        explicit inline OrmLogicError(const char *message)
            : std::logic_error(message)
        {}
        explicit inline OrmLogicError(const QString &message)
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

#endif // ORMLOGICERROR_H
