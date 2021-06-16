#pragma once
#ifndef BOUNDVALUES_HPP
#define BOUNDVALUES_HPP

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QList>
#else
#include <QMap>
#endif

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Types
{

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using BoundValues = QList<QVariant>;
#else
    using BoundValues = QMap<QString, QVariant>;
#endif

} // namespace Orm::Types
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif


#endif // BOUNDVALUES_HPP
