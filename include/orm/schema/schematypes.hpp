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
    enum struct ColumnType : qint8
    {
        Undefined = -1,
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
        TinyBinary,
        Binary,
        MediumBinary,
        LongBinary,
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

    /* Common for the invokeCompileMethod() related methods */

    /* Revisited and it's correct, can't use decltype (auto) as the return value
       for argumentType/s(), and I have removed the M && and std::decay_t<M> to be more
       strict/explicit because the argumentType/s() can only be called with a pointer
       to the member function. */

    /*! Concept for a member function. */
    template<typename M>
    concept IsMemFun = std::is_member_function_pointer_v<M>;

    /*! Function signature, primary template. */
    template<typename Sig>
    struct FunctionSignature;

    /*! Function signature, a const member function explicit specialization. */
    template<typename R, typename C, typename ...Args>
    struct FunctionSignature<R(C::*)(Args...) const>
    {
        using type = std::tuple<Args...>;
    };

    /*! Helper function to get function parameter types as std::tuple. */
    template<IsMemFun M>
    auto argumentTypes(M) -> FunctionSignature<M>::type;

    /*! Helper function to get function parameter type at I position
        from the std::tuple. */
    template<std::size_t I, IsMemFun M>
    auto argumentType(M method) -> decltype (std::get<I>(argumentTypes(method)));

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMATYPES_HPP
