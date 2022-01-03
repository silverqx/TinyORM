#pragma once
#ifndef ORM_TINY_CONCERNS_HASATTRIBUTES_HPP
#define ORM_TINY_CONCERNS_HASATTRIBUTES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/ormtypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Concerns
{

    /*! Model attributes. */
    template<typename Model>
    class HasAttributes
    {
    public:
        /*! Set a given attribute on the model. */
        Model &setAttribute(const QString &key, const QVariant &value);
        /*! Set the vector of model attributes. No checking is done. */
//        Model &setRawAttributes(const QVector<AttributeItem> &attributes,
//                                bool sync = false);
        /*! Get all of the current attributes on the model. */
        const QVector<AttributeItem> &getAttributes() const;

    protected:
        /*! The model's attributes. */
        QVector<AttributeItem> m_attributes;
    };

    template<typename Model>
    Model &
    HasAttributes<Model>::setAttribute(const QString &key, const QVariant &value)
    {
        m_attributes.append({key, value});

        return static_cast<Model &>(*this);
    }

//    template<typename Model>
//    Model &
//    HasAttributes<Model>::setRawAttributes(const QVector<AttributeItem> &attributes,
//                                           const bool sync)
//    {
//        m_attributes = attributes;

//        if (sync)
//            syncOriginal();

//        return static_cast<Model &>(*this);
//    }

    template<typename Model>
    const QVector<AttributeItem> &HasAttributes<Model>::getAttributes() const
    { return m_attributes; }

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_HASATTRIBUTES_HPP
