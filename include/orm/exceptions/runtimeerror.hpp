#pragma once
#ifndef RUNTIMEERROR_HPP
#define RUNTIMEERROR_HPP

#include <QString>

#include <stdexcept>

#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
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
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // RUNTIMEERROR_HPP
