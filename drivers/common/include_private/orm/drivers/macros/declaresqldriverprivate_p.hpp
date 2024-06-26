#pragma once
#ifndef ORM_DRIVERS_MACROS_DECLARESQLDRIVERPRIVATE_P_HPP
#define ORM_DRIVERS_MACROS_DECLARESQLDRIVERPRIVATE_P_HPP

#include <QString> // IWYU pragma: export

#include <orm/macros/likely.hpp>

#include "orm/drivers/exceptions/logicerror.hpp" // IWYU pragma: export
#include "orm/drivers/utils/type_p.hpp"

/*! The XyzResultPrivate::drv_d_func() method returns a pointer to the private SqlDriver.
    Compare to the Q_DECLARE_PRIVATE in qglobal.h to understand it better.
    Also, the std::dynamic_pointer_cast<> is not needed here (to perform better),
    is the developer's responsibility to declare the correct SqlDriver type. */
#define TINY_DECLARE_SQLDRIVER_PRIVATE(Class)                                            \
    /*! Get a pointer to the ClassSqlDriver private implementation, const version. */    \
    inline const Class##Private *drv_d_func() const                                      \
    {                                                                                    \
        using namespace Qt::StringLiterals; /* NOLINT(google-build-using-namespace) */   \
                                                                                         \
        if (const auto driver = sqldriver.lock(); !driver) T_UNLIKELY                    \
            throw Exceptions::LogicError(                                                \
                        u"The SqlResultPrivate::sqldriver smart pointer is nullptr. "    \
                         "The SqlDatabase instance is invalid after calling "            \
                         "removeDatabase(), for '%1' database connection in %2()."_s     \
                        .arg(connectionName, __tiny_func__));                            \
                                                                                         \
        else T_LIKELY                                                                    \
            return std::static_pointer_cast<const Class>(driver)->d_func();              \
    }                                                                                    \
                                                                                         \
    /*! Get a pointer to the ClassSqlDriver private implementation. */                   \
    inline Class##Private *drv_d_func()                                                  \
    {                                                                                    \
        using namespace Qt::StringLiterals; /* NOLINT(google-build-using-namespace) */   \
                                                                                         \
        if (const auto driver = sqldriver.lock(); !driver) T_UNLIKELY                    \
            throw Exceptions::LogicError(                                                \
                        u"The SqlResultPrivate::sqldriver smart pointer is nullptr. "    \
                         "The SqlDatabase instance is invalid after calling "            \
                         "removeDatabase(), for '%1' database connection in %2()."_s     \
                        .arg(connectionName, __tiny_func__));                            \
                                                                                         \
        else T_LIKELY                                                                    \
            return std::static_pointer_cast<Class>(driver)->d_func();                    \
    }                                                                                    \
                                                                                         \
    /*! Get a pointer to the ClassSqlDriver private implementation, const version. */    \
    inline const Class##Private *drv_d_func_noexcept() const noexcept                    \
    {                                                                                    \
        if (const auto driver = sqldriver.lock(); !driver) T_UNLIKELY                    \
            return nullptr;                                                              \
                                                                                         \
        else T_LIKELY                                                                    \
            return std::static_pointer_cast<const Class>(driver)->d_func();              \
    }

#endif // ORM_DRIVERS_MACROS_DECLARESQLDRIVERPRIVATE_P_HPP
