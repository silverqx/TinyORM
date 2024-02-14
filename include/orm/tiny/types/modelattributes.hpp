#pragma once
#ifndef ORM_TINY_TYPES_MODELATTRIBUTES_HPP
#define ORM_TINY_TYPES_MODELATTRIBUTES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include <unordered_map>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
namespace Types
{

    /*! Model attributes map (used only in the Attribute callback). */
    class ModelAttributes : public std::unordered_map<QString, QVariant> // NOLINT(bugprone-exception-escape)
    {
    public:
        /*! Inherit constructors. */
        using std::unordered_map<QString, QVariant>::unordered_map;

        /* Container related */
        /*! The base class type (used as the storage container). */
        using BaseType = std::unordered_map<QString, QVariant>;

        /* Redeclared overridden methods from the base class */
        /*! Access specified element with bounds checking. */
        inline const mapped_type &at(const key_type &key) const;
        /*! Access specified element with bounds checking. */
        inline mapped_type &at(const key_type &key);

        /* Lookup */
        /*! Access specified element with bounds checking and QVariant converting. */
        template<typename T>
        T at(const key_type &key) const;
    };

    /* public */

    /* Redeclared overridden methods from the base class */

    const ModelAttributes::mapped_type &ModelAttributes::at(const key_type &key) const
    {
        return BaseType::at(key);
    }

    ModelAttributes::mapped_type &ModelAttributes::at(const key_type &key)
    {
        return BaseType::at(key);
    }

    /* Lookup */

    template<typename T>
    T ModelAttributes::at(const key_type &key) const
    {
        return BaseType::at(key).template value<T>();
    }

    /* public */

} // namespace Types

    /*! Alias for the Types::ModelAttributes, shortcut alias. */
    using Tiny::Types::ModelAttributes; // NOLINT(misc-unused-using-decls)

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TYPES_MODELATTRIBUTES_HPP
