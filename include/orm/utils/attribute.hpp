#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "export.hpp"
#include "orm/ormtypes.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::Attribute
{
    /*! Convert a AttributeItem QVector to QVariantMap. */
    SHAREDLIB_EXPORT QVariantMap
    convertVectorToMap(const QVector<AttributeItem> &attributes);
    /*! Convert a vector of AttributeItem QVectors to the vector of QVariantMaps. */
    SHAREDLIB_EXPORT QVector<QVariantMap>
    convertVectorsToMaps(const QVector<QVector<AttributeItem>> &attributesVector);

    /*! Convert a AttributeItem QVector to UpdateItem QVector. */
    SHAREDLIB_EXPORT QVector<UpdateItem>
    convertVectorToUpdateItem(const QVector<AttributeItem> &attributes);
    /*! Convert a AttributeItem QVector to UpdateItem QVector. */
    SHAREDLIB_EXPORT QVector<UpdateItem>
    convertVectorToUpdateItem(QVector<AttributeItem> &&attributes);

} // namespace Orm::Utils::Attribute
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ATTRIBUTE_H
