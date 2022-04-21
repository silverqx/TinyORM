#pragma once
#ifndef ORM_TINY_UTILS_ATTRIBUTE_HPP
#define ORM_TINY_UTILS_ATTRIBUTE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/tinytypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Utils
{

    /*! Library class for the database attribute. */
    class SHAREDLIB_EXPORT Attribute
    {
        Q_DISABLE_COPY(Attribute)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Attribute() = delete;
        /*! Deleted destructor. */
        ~Attribute() = delete;

        /*! Convert a AttributeItem QVector to QVariantMap. */
        static QVariantMap
        convertVectorToMap(const QVector<AttributeItem> &attributes);
        /*! Convert a vector of AttributeItem QVectors to the vector of QVariantMaps. */
        static QVector<QVariantMap>
        convertVectorsToMaps(const QVector<QVector<AttributeItem>> &attributesVector);

        /*! Convert a AttributeItem QVector to UpdateItem QVector. */
        static QVector<UpdateItem>
        convertVectorToUpdateItem(const QVector<AttributeItem> &attributes);
        /*! Convert a AttributeItem QVector to UpdateItem QVector. */
        static QVector<UpdateItem>
        convertVectorToUpdateItem(QVector<AttributeItem> &&attributes);

        /*! Remove attributes which have duplicite keys and leave only the last one. */
        static QVector<AttributeItem>
        removeDuplicitKeys(const QVector<AttributeItem> &attributes);

        /*! Join attributes and values for firstOrXx methods. */
        static QVector<AttributeItem>
        joinAttributesForFirstOr(const QVector<WhereItem> &attributes,
                                 const QVector<AttributeItem> &values,
                                 const QString &keyName);
    };

} // namespace Orm::Tiny::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_UTILS_ATTRIBUTE_HPP
