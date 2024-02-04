#pragma once
#ifndef ORM_DRIVERS_LIBRARYINFO_HPP
#define ORM_DRIVERS_LIBRARYINFO_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVersionNumber>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    /*! Library class that provides information about TinyDrivers library. */
    class TINYDRIVERS_EXPORT LibraryInfo
    {
        Q_DISABLE_COPY_MOVE(LibraryInfo)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        LibraryInfo() = delete;
        /*! Deleted destructor. */
        ~LibraryInfo() = delete;

        /*! Return a string describing how this version of TinyDrivers library was
            built. */
        static const char *build() noexcept;
#ifdef TINYDRIVERS_MYSQL_DRIVER
        /*! Return a string describing how this version of TinyMySql library was
            built. */
        static const char *mysqlBuild() noexcept;
#endif

        /*! Return true if this build of TinyDrivers was built with debugging enabled, or
            false if it was built in release mode. */
        static bool isDebugBuild() noexcept;

        /*! Return the version of the TinyDrivers library. */
        static QVersionNumber version() noexcept Q_DECL_CONST_FUNCTION;

        /*! Gather all C preprocessor macros (used by tom about). */
        static std::map<QString, QString> allCMacrosMap();
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_LIBRARYINFO_HPP
