#pragma once
#ifndef RUNTIMEERROR_H
#define RUNTIMEERROR_H

#include <QString>

#include <stdexcept>

#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Exceptions
{

    class SHAREDLIB_EXPORT RuntimeError : public std::runtime_error
    {
    public:
        /*! const char * constructor. */
        explicit RuntimeError(const char *message);
        /*! QString constructor. */
        explicit RuntimeError(const QString &message);

        /*! Return exception message as a QString. */
        const QString &message() const;

    protected:
        /*! Exception message. */
        const QString m_message {what()};
    };

    inline RuntimeError::RuntimeError(const char *message)
        : std::runtime_error(message)
    {}

    inline RuntimeError::RuntimeError(const QString &message)
        : std::runtime_error(message.toUtf8().constData())
    {}

    inline const QString &RuntimeError::message() const
    {
        return m_message;
    }

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // RUNTIMEERROR_H
