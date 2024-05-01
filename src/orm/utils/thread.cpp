#include "orm/utils/thread.hpp"

#include <QString>

#include "orm/config.hpp" // IWYU pragma: keep

#if !defined(__clang__) && \
    !defined(TINYORM_NO_DEBUG) && defined(_MSC_VER) && !defined(Q_OS_WINRT)
#  include <qt_windows.h>
#endif

#if defined(Q_OS_LINUX) && !defined(QT_LINUXBASE)
#  include <sys/prctl.h>
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

namespace
{
#if !defined(__clang__) && \
    !defined(TINYORM_NO_DEBUG) && defined(_MSC_VER) && !defined(Q_OS_WINRT)

#ifndef Q_OS_WIN64
#  define ULONG_PTR DWORD
#endif

    struct tagTHREADNAME_INFO
    {
        DWORD dwType;      // must be 0x1000
        LPCSTR szName;     // pointer to name (in user addr space)
        HANDLE dwThreadID; // thread ID (-1=caller thread)
        DWORD dwFlags;     // reserved for future use, must be zero
    };
    using THREADNAME_INFO = tagTHREADNAME_INFO;

    void setCurrentThreadName(HANDLE threadId, LPCSTR threadName)
    {
        // Process is not debugged, so no debugger connected
        if (!IsDebuggerPresent())
            return;

        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = threadName;
        info.dwThreadID = threadId;
        info.dwFlags = 0;

        __try
        {
            RaiseException(0x406D1388, 0, sizeof (info) / sizeof (DWORD),
                           reinterpret_cast<const ULONG_PTR *>(&info));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {}
    }
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC) || defined(Q_OS_QNX)
    void setCurrentThreadName(const char *name)
    {
#  if defined(Q_OS_LINUX) && !defined(QT_LINUXBASE)
        prctl(PR_SET_NAME, reinterpret_cast<ulong>(name), 0, 0, 0); // NOLINT(cppcoreguidelines-pro-type-vararg, cppcoreguidelines-pro-type-reinterpret-cast)
#  elif defined(Q_OS_MAC)
        pthread_setname_np(name);
#  elif defined(Q_OS_QNX)
        pthread_setname_np(pthread_self(), name);
#  endif
    }
#endif

} // namespace

void Thread::nameThreadForDebugging([[maybe_unused]] const QString &threadName,
                                    [[maybe_unused]] const quint64 threadId)
{
#if !defined(__clang__) && \
    !defined(TINYORM_NO_DEBUG) && defined(Q_CC_MSVC) && !defined(Q_OS_WINRT)
    setCurrentThreadName(reinterpret_cast<HANDLE>(threadId), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                         threadName.toLocal8Bit().constData());
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC) || defined(Q_OS_QNX)
    setCurrentThreadName(threadName.toLocal8Bit().constData());
#endif
}

void Thread::nameThreadForDebugging([[maybe_unused]] const char *threadName,
                                    [[maybe_unused]] const quint64 threadId)
{
#if !defined(__clang__) && \
    !defined(TINYORM_NO_DEBUG) && defined(Q_CC_MSVC) && !defined(Q_OS_WINRT)
    setCurrentThreadName(reinterpret_cast<HANDLE>(threadId), threadName); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC) || defined(Q_OS_QNX)
    setCurrentThreadName(threadName);
#endif
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
