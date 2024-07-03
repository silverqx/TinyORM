#pragma once
#ifndef ORM_EXCEPTIONS_RUNTIMEERROR_HPP
#define ORM_EXCEPTIONS_RUNTIMEERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include <stdexcept>

#include "orm/exceptions/ormerror.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! TinyORM Runtime exception. */
    class TINYORM_EXPORT RuntimeError : public std::runtime_error, // clazy:exclude=copyable-polymorphic
                                        public OrmError
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

    /* public */

    const QString &RuntimeError::message() const noexcept
    {
        return m_message;
    }

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_RUNTIMEERROR_HPP
