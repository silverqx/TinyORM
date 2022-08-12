#pragma once
#ifndef ORM_UTILS_CONFIG_HPP
#define ORM_UTILS_CONFIG_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! Database configuration related library class. */
    class SHAREDLIB_EXPORT Configuration
    {
        Q_DISABLE_COPY(Configuration)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Configuration() = delete;
        /*! Deleted destructor. */
        ~Configuration() = delete;

        /*! Determine whether the database config. contains a valid version value. */
        static bool hasValidConfigVersion(const QVariantHash &config);
        /*! Get a valid config. version value. */
        static QString getValidConfigVersion(const QVariantHash &config);
    };

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_CONFIG_HPP
