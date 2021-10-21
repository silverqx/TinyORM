#pragma once
#ifndef ORM_LIBRARYINFO_HPP
#define ORM_LIBRARYINFO_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVersionNumber>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    class SHAREDLIB_EXPORT LibraryInfo
    {
        Q_DISABLE_COPY(LibraryInfo)

    public:
        /*! Deleted DB's default constructor, this is a pure library class. */
        LibraryInfo() = delete;
        /*! Deleted DB's destructor. */
        ~LibraryInfo() = delete;

        /*! Return a string describing how this version of TinyORM library was built. */
        static const char *build() noexcept;
        /*! Return true if this build of TinyORM was built with debugging enabled, or
            false if it was built in release mode. */
        static bool isDebugBuild();

        /*! Return the version of the TinyORM library. */
        static QVersionNumber version() noexcept Q_DECL_CONST_FUNCTION;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_LIBRARYINFO_HPP
