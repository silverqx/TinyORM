#pragma once
#ifndef LOGICERROR_HPP
#define LOGICERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include <stdexcept>

#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
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

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // LOGICERROR_HPP
