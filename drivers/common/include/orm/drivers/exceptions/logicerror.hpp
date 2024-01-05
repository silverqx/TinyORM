#pragma once
#ifndef ORM_DRIVERS_EXCEPTIONS_LOGICERROR_HPP
#define ORM_DRIVERS_EXCEPTIONS_LOGICERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <stdexcept>

#include "orm/drivers/exceptions/driverserror.hpp"
#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Exceptions
{

    /*! TinyDrivers Logic exception. */
    class TINYDRIVERS_EXPORT LogicError : public std::logic_error, // clazy:exclude=copyable-polymorphic
                                          public DriversError
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
        QString m_message = QString::fromUtf8(what());
    };

    /* public */

    const QString &LogicError::message() const noexcept
    {
        return m_message;
    }

} // namespace Orm::Drivers::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_EXCEPTIONS_LOGICERROR_HPP
