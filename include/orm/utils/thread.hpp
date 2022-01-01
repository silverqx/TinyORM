#pragma once
#ifndef ORM_UTILS_THREAD_HPP
#define ORM_UTILS_THREAD_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! Library class for Thread specific code. */
    class SHAREDLIB_EXPORT Thread
    {
        Q_DISABLE_COPY(Thread)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Thread() = delete;
        /*! Deleted destructor. */
        ~Thread() = delete;

        /*! Set thread name for debugger, -1 for a current thread. */
        [[maybe_unused]]
        static void nameThreadForDebugging(
                const QString &threadName,
                quint64 threadId = static_cast<quint64>(-1));
        /*! Set thread name for debugger, -1 for a current thread. */
        [[maybe_unused]]
        static void nameThreadForDebugging(
                const char *threadName,
                quint64 threadId = static_cast<quint64>(-1));
    };

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_THREAD_HPP
