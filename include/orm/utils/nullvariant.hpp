#pragma once
#ifndef ORM_UTILS_NULLVARIANT_HPP
#define ORM_UTILS_NULLVARIANT_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

class QVariant; // clazy:exclude=qt6-fwd-fixes

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! Null QVariant-s library class. */
    class TINYORM_EXPORT NullVariant
    {
        Q_DISABLE_COPY_MOVE(NullVariant)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        NullVariant() = delete;
        /*! Deleted destructor. */
        ~NullVariant() = delete;

        /*! Null QVariant(QMetaType(QMetaType::Bool)). */
        static const QVariant &Bool();
        /*! Null QVariant(QMetaType(QMetaType::Short)). */
        static const QVariant &Short();
        /*! Null QVariant(QMetaType(QMetaType::UShort)). */
        static const QVariant &UShort();
        /*! Null QVariant(QMetaType(QMetaType::Int)). */
        static const QVariant &Int();
        /*! Null QVariant(QMetaType(QMetaType::UInt)). */
        static const QVariant &UInt();
        /*! Null QVariant(QMetaType(QMetaType::LongLong)). */
        static const QVariant &LongLong();
        /*! Null QVariant(QMetaType(QMetaType::ULongLong)). */
        static const QVariant &ULongLong();
        /*! Null QVariant(QMetaType(QMetaType::Double)). */
        static const QVariant &Double();
        /*! Null QVariant(QMetaType(QMetaType::QByteArray)). */
        static const QVariant &QByteArray();
        /*! Null QVariant(QMetaType(QMetaType::QDate)). */
        static const QVariant &QDate();
        /*! Null QVariant(QMetaType(QMetaType::QDateTime)). */
        static const QVariant &QDateTime();
        /*! Null QVariant(QMetaType(QMetaType::QTime)). */
        static const QVariant &QTime();
        /*! Null QVariant(QMetaType(QMetaType::QString)). */
        static const QVariant &QString();
    };

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_NULLVARIANT_HPP
