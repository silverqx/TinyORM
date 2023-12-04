#pragma once
#ifndef ORM_DRIVERS_UTILS_HELPERS_P_HPP
#define ORM_DRIVERS_UTILS_HELPERS_P_HPP

#include <QVariant>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Utils
{

    /*! Helpers library class for TinyDrivers. */
    class Helpers
    {
        Q_DISABLE_COPY_MOVE(Helpers)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Helpers() = delete;
        /*! Deleted destructor. */
        ~Helpers() = delete;

        /*! Get the storage type of the value stored in the QVariant. */
        inline static int qVariantTypeId(const QVariant &value);
    };

    /* public */

    int Helpers::qVariantTypeId(const QVariant &value)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        return value.typeId();
#else
        return value.userType();
#endif
    }

} // namespace Orm::Drivers::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_UTILS_HELPERS_P_HPP
