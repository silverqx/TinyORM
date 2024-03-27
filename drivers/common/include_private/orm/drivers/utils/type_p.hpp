#pragma once
#ifndef ORM_DRIVERS_UTILS_TYPE_P_HPP
#define ORM_DRIVERS_UTILS_TYPE_P_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

#if defined(__GNUG__) || defined(__clang__)
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#  define __tiny_func__                                            \
    Orm::Drivers::Utils::TypePrivate::prettyFunction(              \
            static_cast<const char *>(__PRETTY_FUNCTION__))
#elif defined(_MSC_VER)
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#  define __tiny_func__                                            \
    Orm::Drivers::Utils::TypePrivate::prettyFunction(              \
            static_cast<const char *>(__FUNCTION__))
#else
#  define __tiny_func__ QString::fromUtf8(__FUNCTION__)
#endif

namespace Orm::Drivers::Utils
{

    /*! Types library class for TinyDrivers. */
    class TINYDRIVERS_EXPORT TypePrivate
    {
        Q_DISABLE_COPY_MOVE(TypePrivate)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        TypePrivate() = delete;
        /*! Deleted destructor. */
        ~TypePrivate() = delete;

        /*! Return a pretty function name in the following format: Xyz::function. */
        static QString prettyFunction(const QString &function);
        /*! Return a pretty function name in the following format: Xyz::function. */
        inline static QString prettyFunction(const char *function);
    };

    /* public */

    QString TypePrivate::prettyFunction(const char *const function)
    {
        return prettyFunction(QString::fromUtf8(function));
    }

} // namespace Orm::Drivers::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_UTILS_TYPE_P_HPP
