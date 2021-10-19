#pragma once
#ifndef RUNTIMEERROR_HPP
#define RUNTIMEERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include <stdexcept>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! Runtime exception. */
    class SHAREDLIB_EXPORT RuntimeError : public std::runtime_error
    {
    public:
        // CUR make all this ctors char * noexcept silverqx
        /*! const char * constructor. */
        explicit RuntimeError(const char *message);
        /*! QString constructor. */
        explicit RuntimeError(const QString &message);

        /*! Return exception message as a QString. */
        const QString &message() const;

    protected:
        // BUG FIXIT, this will be pain, verify all the const data member, they prevent generation of default copy/move assignment operators, you can not assign or move to the const data member silverqx
        /*! Exception message. */
        const QString m_message = what();
    };

    inline const QString &RuntimeError::message() const
    {
        return m_message;
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // RUNTIMEERROR_HPP
