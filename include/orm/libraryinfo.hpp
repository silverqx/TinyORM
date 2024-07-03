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

    /*! Library class that provides information about TinyORM library. */
    class TINYORM_EXPORT LibraryInfo
    {
        Q_DISABLE_COPY_MOVE(LibraryInfo)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        LibraryInfo() = delete;
        /*! Deleted destructor. */
        ~LibraryInfo() = delete;

        /*! Get a string describing how this version of TinyORM library was built. */
        static const char *build() noexcept Q_DECL_CONST_FUNCTION;
        /*! Determine if this build of TinyORM was built with debugging enabled or
            if it was built in release mode. */
        static bool isDebugBuild() noexcept Q_DECL_CONST_FUNCTION;

        /*! Get the version of the TinyORM library. */
        static QVersionNumber version() noexcept Q_DECL_CONST_FUNCTION;

        /*! Gather all C preprocessor macros (used by tom about). */
        inline static std::map<QString, QString> allCMacrosMap();
        /*! Gather TinyOrm library C preprocessor macros. */
        static std::map<QString, QString> ormCMacrosMap();
        /*! Gather tom C preprocessor macros. */
        static std::map<QString, QString> tomCMacrosMap();
    };

    /* public */

    std::map<QString, QString> LibraryInfo::allCMacrosMap()
    {
        auto result = ormCMacrosMap();
        result.merge(tomCMacrosMap());

        return result;
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_LIBRARYINFO_HPP
