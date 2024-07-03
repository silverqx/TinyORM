#pragma once
#ifndef ORM_UTILS_FS_HPP
#define ORM_UTILS_FS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! Filesystem library class. */
    class TINYORM_EXPORT Fs
    {
        Q_DISABLE_COPY_MOVE(Fs)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Fs() = delete;
        /*! Deleted destructor. */
        ~Fs() = delete;

        /*! Resolve ~ home prefix to the full filepath. */
        [[maybe_unused]]
        static QString resolveHome(QString filepath);
    };

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_FS_HPP
