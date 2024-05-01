#pragma once
#ifndef ORM_TYPES_ABOUTVALUE_HPP
#define ORM_TYPES_ABOUTVALUE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QStringList>

#include <optional>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Types
{

    /*! About value type (used by the tom about command and LibraryInfo). */
    struct AboutValue
    {
        /*! Converting constructor from the QString. */
        inline AboutValue(QString value) noexcept; // NOLINT(google-explicit-constructor)
        /*! Converting constructor from the QString and QStringList (components). */
        inline AboutValue(QString value, QStringList components) noexcept;
        /*! Converting constructor from the null-terminated character string. */
        inline AboutValue(const char *value); // NOLINT(google-explicit-constructor)

        /*! About item value. */
        QString value;
        /*! About item components (used in the Dependencies section). */
        std::optional<QStringList> components = std::nullopt;
    };

    /* public */

    AboutValue::AboutValue(QString value_) noexcept
        : value(std::move(value_))
    {}

    AboutValue::AboutValue(QString value_, QStringList components_) noexcept
        : value(std::move(value_))
        , components(std::move(components_))
    {}

    AboutValue::AboutValue(const char *value_)
        : value(QString::fromUtf8(value_))
    {}

} // namespace Orm::Types

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TYPES_ABOUTVALUE_HPP
