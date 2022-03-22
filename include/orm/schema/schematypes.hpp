#pragma once
#ifndef ORM_SCHEMATYPES_HPP
#define ORM_SCHEMATYPES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include <optional>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    /*! Default string/char length. */
    inline constexpr int DefaultStringLength = 255;

    /*! Column types. */
    enum struct ColumnType
    {
        Char,
        String,
        TinyText,
        Text,
        MediumText,
        LongText,
        BigInteger,
        Integer,
        MediumInteger,
        TinyInteger,
        SmallInteger,
        Float,
        Double,
        Decimal,
        Real, // PostgreSQL only
        Boolean,
        Enum,
        Set, // MySQL only
        Json,
        Jsonb,
        Date,
        DateTime,
        DateTimeTz,
        Time,
        TimeTz,
        Timestamp,
        TimestampTz,
        Year,
        Binary,
        Uuid,
        IpAddress,
        MacAddress,
        Geometry,
        Point,
        LineString,
        Polygon,
        GeometryCollection,
        MultiPoint,
        MultiLineString,
        MultiPolygon,
        MultiPolygonZ, // PostgreSQL only
        Computed, // SqlServer only
    };

    /*! Auto increment value for a column. */
    struct AutoIncrementColumnValue
    {
        QString                columnName;
        std::optional<quint64> value;
    };

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMATYPES_HPP
