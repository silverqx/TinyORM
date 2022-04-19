#pragma once
#ifndef TOM_EXCEPTIONS_LOGICERROR_HPP
#define TOM_EXCEPTIONS_LOGICERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <stdexcept>

#include "tom/exceptions/tomerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Exceptions
{

    /*! Tom Logic exception. */
    class LogicError : public std::logic_error,
                       public TomError
    {
    public:
        /*! const char * constructor. */
        explicit LogicError(const char *message);
        /*! QString constructor. */
        explicit LogicError(const QString &message);
        /*! std::string constructor. */
        explicit LogicError(const std::string &message);

        /*! Return exception message as a QString. */
        inline const QString &message() const noexcept;

    protected:
        /*! Exception message. */
        QString m_message = what();
    };

    const QString &LogicError::message() const noexcept
    {
        return m_message;
    }

} // namespace Tom::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_EXCEPTIONS_LOGICERROR_HPP
