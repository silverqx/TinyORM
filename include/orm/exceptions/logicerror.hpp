#pragma once
#ifndef ORM_EXCEPTIONS_LOGICERROR_HPP
#define ORM_EXCEPTIONS_LOGICERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include <stdexcept>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! Logic exception. */
    class SHAREDLIB_EXPORT LogicError : public std::logic_error
    {
    public:
        /*! const char * constructor. */
        explicit LogicError(const char *message);
        /*! QString constructor. */
        explicit LogicError(const QString &message);

        /*! Return exception message as a QString. */
        const QString &message() const;

    protected:
        /*! Exception message. */
        const QString m_message = what();
    };

    inline const QString &LogicError::message() const
    {
        return m_message;
    }

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_LOGICERROR_HPP
