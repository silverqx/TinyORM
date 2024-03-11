#include "orm/drivers/utils/fs_p.hpp"

#ifdef _WIN32
#  include <array>
#elif __linux__
#  include <dlfcn.h>
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Utils
{

#ifdef _WIN32
std::wstring FsPrivate::getModuleFileName(const HMODULE handle)
{
    // Don't initialize to 0 as it's ERROR_SUCCESS
    DWORD lastError = ERROR_NOT_ENOUGH_MEMORY;

    // First, try to simply obtain using classic method using the MAX_PATH size
    {
        std::array<wchar_t, MAX_PATH> moduleFilepath {}; // {} is need to 0 initialize (list initialize all)

        ::GetModuleFileNameW(handle, moduleFilepath.data(), MAX_PATH);

        if (lastError = ::GetLastError(); lastError == ERROR_SUCCESS)
            return moduleFilepath.data();
    }

    /* If it failed due to a small buffer, try with a larger buffer (Long Paths
       support). The Windows API has many functions that also have Unicode versions
       to permit an extended-length path for a maximum total path length of 32,767
       characters. MAX_PATH * 1024 = 266'240 that is far beyond this limit, this
       1024 value is from the boost::dll::fs::path path_from_handle() so if it's
       good for boost then it's good for TinyORM. I will not decrease this. */
    for (quint16 i = 2; i <= 1024 && lastError == ERROR_INSUFFICIENT_BUFFER; i *= 2) {
        const DWORD bufferSize = MAX_PATH * i;
        std::wstring moduleFilepath(bufferSize, L'\0');

        const auto realSize = ::GetModuleFileNameW(handle, moduleFilepath.data(),
                                                   bufferSize);

        // Failed, increase/double the buffer and try again
        if (lastError = ::GetLastError(); lastError != ERROR_SUCCESS)
            continue;

        // Success, reduce the size of the string to its actual size
        moduleFilepath.resize(realSize);
        return moduleFilepath;
    }

    /* An error other than ERROR_INSUFFICIENT_BUFFER occurred or failed to allocate
       a large enough buffer. */
    return {};
}

#elif __linux__
std::string FsPrivate::getModuleFileName(void *const address)
{
    Q_ASSERT(address != nullptr);

    Dl_info info {};

    const auto result = dladdr(address, &info);

    // Nothing to do, dladdr() failed
    if (result == 0 || info.dli_fname == nullptr)
        return {};

    return info.dli_fname;
}
#endif // _WIN32

} // namespace Orm::Drivers::Utils

TINYORM_END_COMMON_NAMESPACE
