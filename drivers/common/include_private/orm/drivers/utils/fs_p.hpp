#pragma once
#ifndef ORM_DRIVERS_UTILS_FS_P_HPP
#define ORM_DRIVERS_UTILS_FS_P_HPP

#include <QtGlobal>

#ifdef _WIN32
#  include <qt_windows.h>
#endif

#if defined(_WIN32) || defined(__linux__)
#  include <string>
#endif

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Utils
{

    /*! Filesystem library class. */
    class FsPrivate
    {
        Q_DISABLE_COPY_MOVE(FsPrivate)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        FsPrivate() = delete;
        /*! Deleted destructor. */
        ~FsPrivate() = delete;

#ifdef _WIN32
        /*! Get the fully qualified module path for the given module handle. */
        static std::wstring getModuleFileName(HMODULE handle);
#elif __linux__
        /*! Get the fully qualified module path by the given address. */
        static std::string getModuleFileName(void *address);
#else
#  error Unsupported OS or platform in Orm::Drivers::Utils::FsFsPrivate.
#endif
    };

} // namespace Orm::Drivers::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_UTILS_FS_P_HPP
