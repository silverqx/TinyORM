#pragma once
#ifndef TOM_EXCEPTIONS_RUNTIMEERROR_HPP
#define TOM_EXCEPTIONS_RUNTIMEERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <stdexcept>

#include "tom/exceptions/tomerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Exceptions
{

    /*! Tom Runtime exception. */
    class RuntimeError : public std::runtime_error,
                         public TomError
    {
    public:
        /*! const char * constructor. */
        explicit RuntimeError(const char *message);
        /*! QString constructor. */
        explicit RuntimeError(const QString &message);
        /*! std::string constructor. */
        explicit RuntimeError(const std::string &message);

        /*! Return exception message as a QString. */
        inline const QString &message() const noexcept;

    protected:
        /*! Exception message. */
        QString m_message = what();
    };

    const QString &RuntimeError::message() const noexcept
    {
        return m_message;
    }

} // namespace Tom::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_EXCEPTIONS_RUNTIMEERROR_HPP
