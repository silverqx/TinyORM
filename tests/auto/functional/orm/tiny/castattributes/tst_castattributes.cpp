#include <QCoreApplication>
#include <QTest>

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TSqlDriver

#include "orm/db.hpp"
#include "orm/sqliteconnection.hpp"

#include "databases.hpp"
#include "macros.hpp"

#include "models/type.hpp"

using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::dummy_NONEXISTENT;

using Orm::DB;
using Orm::Exceptions::InvalidArgumentError;
using Orm::Exceptions::InvalidFormatError;
using Orm::SQLiteConnection;
using Orm::TTimeZone;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::CastItem;
using Orm::Tiny::CastType;

using TestUtils::Databases;

using Models::Type;

/* Helper types for the model cache */
/*! Row type to obtain from the DB. */
enum struct RowType : quint8
{
    /*! All columns all filled with values. */
    AllFilled = 1,
    /*! Number columns all filled negative values. */
    Negative,
    /*! All columns all filled with NULL values. */
    Null,
};
#if defined(__cpp_using_enum) && __cpp_using_enum >= 201907
/*! Introduce the RowType enumerator names. */
using enum RowType;
#else
/*! All columns all filled with values. */
constexpr auto AllFilled = RowType::AllFilled;
/*! Number columns all filled negative values. */
constexpr auto Negative  = RowType::Negative;
/*! All columns all filled with NULL values. */
constexpr auto Null      = RowType::Null;
#endif

/*! Type for the Connection name (QString). */
using ConnectionName = QString;

/*! Model cache key for the std::unordered_map. */
struct ModelCacheKey
{
    /*! Connection name. */
    ConnectionName connection;
    /*! Row type. */
    RowType rowType;

    /*! Equality comparison operator for the ModelCacheKey. */
    bool operator==(const ModelCacheKey &) const noexcept = default;
};

/*! The std::hash specialization for the ModelCacheKey. */
template<>
class std::hash<ModelCacheKey>
{
    /*! Alias for the helper utils. */
    using Helpers = Orm::Utils::Helpers;

public:
    /*! Generate hash for the given ModelCacheKey. */
    std::size_t operator()(const ModelCacheKey &cacheKey) const noexcept
    {
        /*! RowType underlying type. */
        using RowTypeUnderlying = std::underlying_type_t<RowType>;

        std::size_t resultHash = 0;

        const auto rowType = static_cast<RowTypeUnderlying>(cacheKey.rowType);

        Helpers::hashCombine<QString>(resultHash, cacheKey.connection);
        Helpers::hashCombine<RowTypeUnderlying>(resultHash, rowType);

        return resultHash;
    }
};

/* tst_CastAttributes */
/*! tst_CastAttributes test case. */
class tst_CastAttributes : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase_data() const;

    /* Others */
    void mergeCasts_const_lvalue() const;
    void mergeCasts_lvalue() const;
    void mergeCasts_rvalue() const;

    void withCasts_OnTinyBuilder() const;
    void withCasts_OnModel() const;
    /* The withCasts_OnRelation() test moved to the tst_Model_Relations to avoid
       increasing compilation times, after #include "models/torrent.hpp" added and
       Torrent::find(), the compilation time increased around a 12 seconds. */

    /* Cast Exceptions */
    void cast_QByteArray_to_QDateTime_ThrowException() const;
    void cast_QDateTime_to_QByteArray_ThrowException() const;

    /* Default casts */
    void defaultCast_bool_true() const;
    void defaultCast_bool_false() const;

    /* Look at NOTES.txt how integer types are handled by QtSql drivers, section:
       Handling integer values */
    void defaultCast_smallint() const;
    void defaultCast_smallint_u() const;
    void defaultCast_int() const;
    void defaultCast_int_u() const;
    void defaultCast_bigint() const;
    void defaultCast_bigint_u() const;

    void defaultCast_smallint_Negative() const;
    void defaultCast_int_Negative() const;
    void defaultCast_bigint_Negative() const;

    void defaultCast_double() const;
    void defaultCast_double_Negative() const;
    void defaultCast_double_NaN() const;
    void defaultCast_double_Infinity() const;

    void defaultCast_decimal() const;
    void defaultCast_decimal_Negative() const;
    void defaultCast_decimal_NaN() const;
    void defaultCast_decimal_Infinity() const;

    void defaultCast_string() const;
    void defaultCast_text() const;
    void defaultCast_mediumtext() const;

    void defaultCast_timestamp() const;
    void defaultCast_datetime() const;
    void defaultCast_date() const;
    void defaultCast_time() const;

    void defaultCast_timestamp_QSQLITE_OffReturnQDateTime() const;
    void defaultCast_datetime_QSQLITE_OffReturnQDateTime() const;
    void defaultCast_date_QSQLITE_OffReturnQDateTime() const;
    // Test for time column types not needed because return_qdatetime doesn't affect them

    void defaultCast_blob() const;

    /* Default Null casts */
    void defaultCast_Null_bool_true() const;

    void defaultCast_Null_smallint() const;
    void defaultCast_Null_smallint_u() const;
    void defaultCast_Null_int() const;
    void defaultCast_Null_int_u() const;
    void defaultCast_Null_bigint() const;
    void defaultCast_Null_bigint_u() const;

    void defaultCast_Null_double() const;
    void defaultCast_Null_double_NaN() const;
    void defaultCast_Null_double_Infinity() const;

    void defaultCast_Null_decimal() const;
    void defaultCast_Null_decimal_NaN() const;
    void defaultCast_Null_decimal_Infinity() const;

    void defaultCast_Null_string() const;
    void defaultCast_Null_text() const;

    void defaultCast_Null_timestamp() const;
    void defaultCast_Null_datetime() const;
    void defaultCast_Null_date() const;
    void defaultCast_Null_time() const;

    void defaultCast_Null_blob() const;

    /* Custom casts */
    void cast_bool_true_to_bool() const;
    void cast_bool_true_to_int() const;
    void cast_bool_false_to_bool() const;
    void cast_bool_false_to_int() const;

    void cast_smallint_to_uint() const;
    void cast_smallint_u_to_ulonglong() const;
    void cast_int_to_ulonglong() const;
    void cast_int_u_to_ulonglong() const;
    void cast_bigint_to_QString() const;
    void cast_bigint_u_to_QString() const;

    void cast_smallint_Negative_to_short() const;
    void cast_smallint_Negative_to_int() const;
    void cast_smallint_Negative_to_QString() const;
    void cast_int_Negative_to_longlong() const;
    void cast_int_Negative_to_QString() const;
    void cast_bigint_Negative_to_longlong() const;
    void cast_bigint_Negative_to_QString() const;

    void cast_double_to_longlong() const;
    void cast_double_to_QString() const;
    void cast_double_Negative_to_longlong() const;
    void cast_double_Negative_to_QString() const;

    void cast_decimal_to_Decimal() const;
    void cast_decimal_to_longlong() const;
    void cast_decimal_to_QString() const;
    void cast_decimal_Negative_to_Decimal() const;
    void cast_decimal_Negative_to_longlong() const;
    void cast_decimal_Negative_to_QString() const;

    void cast_decimal_to_Decimal_With_Modifier_2_Down() const;
    void cast_decimal_to_Decimal_With_Modifier_2_Up() const;
    void cast_decimal_to_Decimal_With_Modifier_2_Down_Negative() const;
    void cast_decimal_to_Decimal_With_Modifier_2_Up_Negative() const;

    void cast_timestamp_to_QDateTime() const;
    void cast_timestamp_to_QString() const;
    void cast_timestamp_to_Timestamp() const;

    void cast_datetime_to_QDateTime() const;
    void cast_datetime_to_QString() const;
    void cast_date_to_QDate() const;
    void cast_date_to_QString() const;
    void cast_time_to_QTime() const;
    void cast_time_to_QString() const;

    void cast_blob_to_QByteArray() const;
    void cast_blob_to_QString() const;

    /* Custom Null and Invalid casts */
    void cast_Null_bool_true_to_bool() const;
    void cast_Null_bool_true_to_int() const;

    void cast_Existing_Invalid_to_int() const;
    void cast_NonExisting_Invalid_to_int() const;

    void cast_Null_smallint_to_Short() const;
    void cast_Null_smallint_u_to_UShort() const;
    void cast_Null_int_to_Int() const;
    void cast_Null_int_u_to_UInt() const;
    void cast_Null_bigint_to_LongLong() const;
    void cast_Null_bigint_u_to_ULongLong() const;

    void cast_Null_double_to_Double() const;
    void cast_Null_double_NaN() const;
    void cast_Null_double_Infinity() const;

    void cast_Null_decimal_to_Decimal() const;
    void cast_Null_decimal_NaN() const;
    void cast_Null_decimal_Infinity() const;

    void cast_Null_string_to_QString() const;
    void cast_Null_text_to_QString() const;

    void cast_Null_timestamp_to_Timestamp() const;
    void cast_Null_datetime_to_QDateTime() const;
    void cast_Null_date_to_QDate() const;
    void cast_Null_time_to_QTime() const;

    void cast_Null_blob_to_QByteArray() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Get a Type model instance for the given connection. */
    Type &model(const QString &connection, RowType rowType = AllFilled) const;
    /*! Get a Type model instance with negative numbers for the given connection. */
    inline Type &modelNegative(const QString &connection) const;
    /*! Get a Type model instance with all attributes NULL for the given connection. */
    inline Type &modelNull(const QString &connection) const;

    /*! Reset the Type::u_casts. */
    inline static Type &resetCasts(Type &type);

    /*! Disable the return_qdatetime for the current connection. */
    static void disableReturnQDateTime(const QString &connection);
    /*! Enable the return_qdatetime for the current connection. */
    static void enableReturnQDateTime(const QString &connection);

    /*! Get the default numerical precision policy for the current driver. */
    inline static TPrecisionPolicy
    defaultNumericalPrecisionPolicy(const QString &connection);

    /*! Cache a Type model by a connection name and row type */
    mutable std::unordered_map<ModelCacheKey, Type> m_modelsCache;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_CastAttributes::initTestCase_data() const
{
    const auto connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

/* Others */

void tst_CastAttributes::mergeCasts_const_lvalue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    // Original casts
    {
        std::unordered_map<QString, CastItem> expected {
            {type.getKeyName(), CastType::ULongLong},
        };
        QCOMPARE(type.getCasts(), expected);
    }

    // Merge
    const std::unordered_map<QString, CastItem> toMerge {
        {"binary",                 CastType::QDateTime},
        {"decimal",                CastType::Decimal},
        {"decimal_with_modifier", {CastType::Decimal, 3}},
    };

    type.mergeCasts(toMerge);

    // Verify merge
    {
        std::unordered_map<QString, CastItem> expected {
            {type.getKeyName(),        CastType::ULongLong},
            {"binary",                 CastType::QDateTime},
            {"decimal",                CastType::Decimal},
            {"decimal_with_modifier", {CastType::Decimal, 3}},
        };
        QCOMPARE(type.getCasts(), expected);
    }

    // Our own merge algorithm, nothing will be extracted because const lvalue
    QCOMPARE(toMerge, toMerge);
}

void tst_CastAttributes::mergeCasts_lvalue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    // Original casts
    {
        std::unordered_map<QString, CastItem> expected {
            {type.getKeyName(), CastType::ULongLong},
        };
        QCOMPARE(type.getCasts(), expected);
    }

    // Merge
    std::unordered_map<QString, CastItem> toMerge {
        {type.getKeyName(),        CastType::ULongLong},
        {"binary",                 CastType::QDateTime},
        {"decimal",                CastType::Decimal},
        {"decimal_with_modifier", {CastType::Decimal, 3}},
    };

    type.mergeCasts(toMerge);

    // Verify merge
    {
        std::unordered_map<QString, CastItem> expected {
            {type.getKeyName(),        CastType::ULongLong},
            {"binary",                 CastType::QDateTime},
            {"decimal",                CastType::Decimal},
            {"decimal_with_modifier", {CastType::Decimal, 3}},
        };
        QCOMPARE(type.getCasts(), expected);
    }

    /* Original casts was empty before the mergeCasts() method call, so all casts
       will be extracted. The 'id' cast in the getCasts() is emplaced on the fly
       on the user casts copy, so the 'id' cast will be extracted too. */
    QVERIFY(toMerge.empty());
}

void tst_CastAttributes::mergeCasts_rvalue() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);
    const auto &primaryKey = type.getKeyName();

    // Original casts
    {
        std::unordered_map<QString, CastItem> expected {
            {primaryKey, CastType::ULongLong},
        };
        QCOMPARE(type.getCasts(), expected);
    }

    // Merge
    std::unordered_map<QString, CastItem> toMerge {
        {primaryKey,               CastType::ULongLong},
        {"binary",                 CastType::QDateTime},
        {"decimal",                CastType::Decimal},
        {"decimal_with_modifier", {CastType::Decimal, 3}},
    };

    type.mergeCasts(std::move(toMerge));

    // Verify merge
    {
        std::unordered_map<QString, CastItem> expected {
            {type.getKeyName(),        CastType::ULongLong},
            {"binary",                 CastType::QDateTime},
            {"decimal",                CastType::Decimal},
            {"decimal_with_modifier", {CastType::Decimal, 3}},
        };
        QCOMPARE(type.getCasts(), expected);
    }

    /* Original casts was empty before the mergeCasts() method call, so all casts
       will be extracted. The 'id' cast in the getCasts() is emplaced on the fly
       on the user casts copy, so the 'id' cast will be extracted too. */
    QVERIFY(toMerge.empty()); // NOLINT(bugprone-use-after-move)
}

void tst_CastAttributes::withCasts_OnTinyBuilder() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto type = Type::on(connection)->withCasts({{"smallint", CastType::UInteger}})
                .find(1);

    QVERIFY(type);
    QVERIFY(type->exists);
    QCOMPARE(type->getKeyCasted(), 1);

    auto attribute = type->getAttribute("smallint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::UInt);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<uint>(), static_cast<uint>(32760));
}

void tst_CastAttributes::withCasts_OnModel() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    Orm::Tiny::ConnectionOverride::connection = connection;

    auto type = Type::withCasts({{"smallint", CastType::UInt}})->find(1);

    QVERIFY(type);
    QVERIFY(type->exists);
    QCOMPARE(type->getKeyCasted(), 1);

    auto attribute = type->getAttribute("smallint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::UInt);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<uint>(), static_cast<uint>(32760));

    // Restore
    Orm::Tiny::ConnectionOverride::connection.clear();
}

/* Cast Exceptions */

void tst_CastAttributes::cast_QByteArray_to_QDateTime_ThrowException() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"binary", CastType::QDateTime}});

    TVERIFY_THROWS_EXCEPTION(InvalidFormatError,
                             type.getAttribute("binary"));
}

void tst_CastAttributes::cast_QDateTime_to_QByteArray_ThrowException() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("The SQLite driver returns the QString type for the datetime column, so "
              "it will not throw (skipping).", );

    auto &type = model(connection);

    type.mergeCasts({{"datetime", CastType::QByteArray}});

    TVERIFY_THROWS_EXCEPTION(InvalidArgumentError,
                             type.getAttribute("datetime"));
}

/* Default casts */

void tst_CastAttributes::defaultCast_bool_true() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("bool_true");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<bool>(), true);
}

void tst_CastAttributes::defaultCast_bool_false() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("bool_false");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<bool>(), false);
}

void tst_CastAttributes::defaultCast_smallint() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("smallint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Short);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<int>(), 32760);
}

void tst_CastAttributes::defaultCast_smallint_u() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("smallint_u");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UShort);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<uint>(), static_cast<uint>(32761));
}

void tst_CastAttributes::defaultCast_int() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("int");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<int>(), 2147483640);
}

void tst_CastAttributes::defaultCast_int_u() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("int_u");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<uint>(), static_cast<uint>(2147483641));
}

void tst_CastAttributes::defaultCast_bigint() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("bigint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<qint64>(), static_cast<qint64>(9223372036854775800));
}

void tst_CastAttributes::defaultCast_bigint_u() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("bigint_u");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<quint64>(), static_cast<quint64>(9223372036854775801));
}

void tst_CastAttributes::defaultCast_smallint_Negative() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNegative(connection);

    auto attribute = type.getAttribute("smallint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Short);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<int>(), -32762);
}

void tst_CastAttributes::defaultCast_int_Negative() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNegative(connection);

    auto attribute = type.getAttribute("int");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<int>(), -2147483642);
}

void tst_CastAttributes::defaultCast_bigint_Negative() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNegative(connection);

    auto attribute = type.getAttribute("bigint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<qint64>(), static_cast<qint64>(-9223372036854775802));
}

void tst_CastAttributes::defaultCast_double() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    auto attribute = type.getAttribute("double");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<double>(), static_cast<double>(1000000.123));
}

void tst_CastAttributes::defaultCast_double_Negative() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNegative(connection);

    auto attribute = type.getAttribute("double");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<double>(), static_cast<double>(-1000000.123));
}

void tst_CastAttributes::defaultCast_double_NaN() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The NaN value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    auto attribute = type.getAttribute("double_nan");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QCOMPARE(attribute.value<double>(), std::numeric_limits<double>::quiet_NaN());
}

void tst_CastAttributes::defaultCast_double_Infinity() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The Infinity value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    auto attribute = type.getAttribute("double_infinity");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QCOMPARE(attribute.value<double>(), std::numeric_limits<double>::infinity());
}

void tst_CastAttributes::defaultCast_decimal() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    auto attribute = type.getAttribute("decimal");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<double>(), static_cast<double>(100000.12));
}

void tst_CastAttributes::defaultCast_decimal_Negative() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNegative(connection);

    auto attribute = type.getAttribute("decimal");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<double>(), static_cast<double>(-100000.12));
}

void tst_CastAttributes::defaultCast_decimal_NaN() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The NaN value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    auto attribute = type.getAttribute("decimal_nan");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QCOMPARE(attribute.value<double>(), std::numeric_limits<double>::quiet_NaN());
}

void tst_CastAttributes::defaultCast_decimal_Infinity() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The Infinity value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    auto attribute = type.getAttribute("decimal_infinity");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QCOMPARE(attribute.value<double>(), std::numeric_limits<double>::infinity());
}

void tst_CastAttributes::defaultCast_string() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("string");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QString);
    QCOMPARE(attribute.value<QString>(), sl("string text"));
}

void tst_CastAttributes::defaultCast_text() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("text");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QString);
    QCOMPARE(attribute.value<QString>(), sl("text text"));
}

void tst_CastAttributes::defaultCast_mediumtext() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("medium_text");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QString);
    QCOMPARE(attribute.value<QString>(), sl("mediumtext text"));
}

void tst_CastAttributes::defaultCast_timestamp() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("timestamp");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QDateTime);

    auto datetimeActual = attribute.value<QDateTime>();

    QCOMPARE(datetimeActual, QDateTime({2022, 9, 9}, {8, 41, 28}, TTimeZone::UTC));
    QCOMPARE(datetimeActual.timeZone(), QTimeZone::utc());
}

void tst_CastAttributes::defaultCast_datetime() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("datetime");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QDateTime);

    auto timestampActual = attribute.value<QDateTime>();

    QCOMPARE(timestampActual, QDateTime({2022, 9, 10}, {8, 41, 28}, TTimeZone::UTC));
    QCOMPARE(timestampActual.timeZone(), QTimeZone::utc());
}

void tst_CastAttributes::defaultCast_date() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("date");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QDate);

    QCOMPARE(attribute.value<QDate>(), QDate(2022, 9, 11));
}

void tst_CastAttributes::defaultCast_time() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("time");
    auto typeId = attribute.typeId();

    // Postgres returns QTime() because its time type can't be out of 24 hours range
    if (DB::driverName(connection) == QPSQL) {
        QCOMPARE(typeId, QMetaType::QTime);

        QCOMPARE(attribute.value<QTime>(), QTime(17, 1, 5));
    }
    else {
        QCOMPARE(typeId, QMetaType::QString);

        QCOMPARE(attribute.value<QString>(), sl("17:01:05"));
    }
}

void tst_CastAttributes::defaultCast_timestamp_QSQLITE_OffReturnQDateTime() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QSQLITE)
        QSKIP("The return_qdatetime connection config. option is only supported "
              "for the QSQLITE driver.", );

    disableReturnQDateTime(connection);

    // Skip model cache (I will not create special cache logic for this)
    auto type = Type::on(connection)->find(1);
    QVERIFY(type);
    QCOMPARE(type->getKeyCasted(), 1);

    auto attribute = type->getAttribute("timestamp");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QString);

    const auto datetimeActual = attribute.value<QDateTime>();

    QCOMPARE(datetimeActual, QDateTime({2022, 9, 9}, {8, 41, 28}));
    QCOMPARE(datetimeActual.timeZone(), QTimeZone::systemTimeZone());

    // Restore
    enableReturnQDateTime(connection);
}

void tst_CastAttributes::defaultCast_datetime_QSQLITE_OffReturnQDateTime() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QSQLITE)
        QSKIP("The return_qdatetime connection config. option is only supported "
              "for the QSQLITE driver.", );

    disableReturnQDateTime(connection);

    // Skip model cache (I will not create special cache logic for this)
    auto type = Type::on(connection)->find(1);
    QVERIFY(type);
    QCOMPARE(type->getKeyCasted(), 1);

    auto attribute = type->getAttribute("datetime");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QString);

    const auto timestampActual = attribute.value<QDateTime>();

    QCOMPARE(timestampActual, QDateTime({2022, 9, 10}, {8, 41, 28}));
    QCOMPARE(timestampActual.timeZone(), QTimeZone::systemTimeZone());

    // Restore
    enableReturnQDateTime(connection);
}

void tst_CastAttributes::defaultCast_date_QSQLITE_OffReturnQDateTime() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QSQLITE)
        QSKIP("The return_qdatetime connection config. option is only supported "
              "for the QSQLITE driver.", );

    disableReturnQDateTime(connection);

    // Skip model cache (I will not create special cache logic for this)
    auto type = Type::on(connection)->find(1);
    QVERIFY(type);
    QCOMPARE(type->getKeyCasted(), 1);

    auto attribute = type->getAttribute("date");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QString);

    QCOMPARE(attribute.value<QDate>(), QDate(2022, 9, 11));

    // Restore
    enableReturnQDateTime(connection);
}

void tst_CastAttributes::defaultCast_blob() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    auto attribute = type.getAttribute("binary");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QByteArray);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QByteArray);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QByteArray);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QByteArray>(),
             QByteArray::fromHex("517420697320677265617421"));
}

/* Default Null casts */

/* Different behavior for null values

   The SQLite driver returns QString type for all null values, look at
   QSQLiteResultPrivate::fetchNext() and case SQLITE_NULL.

   The MySQL driver returns Char for tinyint instead of int for null values (described
   also in test methods).

   The PostgreSQL driver returns LongLong for bigint for null values, the - and
   ULongLong logic is not applied (described also in test methods).
*/

void tst_CastAttributes::defaultCast_Null_bool_true() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("bool_true");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        /* Look at QMYSQLResult::data(), if not null then it returns int, but the same
           logic is not true if (f.nullIndicator). */
        QCOMPARE(typeId, QMetaType::Char);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_smallint() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("smallint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Short);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_smallint_u() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("smallint_u");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UShort);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_int() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("int");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_int_u() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("int_u");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_bigint() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("bigint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        /* Look at QPSQLResult::data(), the logic for - and ULongLong is not applied
           for the null values, so it's everytime LongLong. */
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_bigint_u() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("bigint_u");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QPSQL)
        /* Look at QPSQLResult::data(), the logic for - and ULongLong is not applied
           for the null values, so it's everytime LongLong. */
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_double() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("double");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_double_NaN() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The NaN value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("double_nan");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_double_Infinity() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The Infinity value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("double_infinity");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_decimal() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("decimal");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_decimal_NaN() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The NaN value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("decimal_nan");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_decimal_Infinity() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The Infinity value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("decimal_infinity");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_string() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("string");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QString);
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_text() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("text");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::QString);
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_timestamp() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("timestamp");
    auto typeId = attribute.typeId();

    if (DB::driverName(connection) == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        QCOMPARE(typeId, QMetaType::QDateTime);

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_datetime() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("datetime");
    auto typeId = attribute.typeId();

    if (DB::driverName(connection) == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        QCOMPARE(typeId, QMetaType::QDateTime);

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_date() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("date");
    auto typeId = attribute.typeId();

    if (DB::driverName(connection) == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        QCOMPARE(typeId, QMetaType::QDate);

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_time() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("time");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    // Postgres returns QTime() because its time type can't be out of 24 hours range
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QTime);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::defaultCast_Null_blob() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    auto attribute = type.getAttribute("binary");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QByteArray);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QByteArray);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

/* Custom casts */

void tst_CastAttributes::cast_bool_true_to_bool() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"bool_true", CastType::Bool}});

    auto attribute = type.getAttribute("bool_true");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Bool);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<bool>(), true);
}

void tst_CastAttributes::cast_bool_true_to_int() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"bool_true", CastType::Integer}});

    auto attribute = type.getAttribute("bool_true");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Int);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<int>(), 1);
}

void tst_CastAttributes::cast_bool_false_to_bool() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"bool_false", CastType::Bool}});

    auto attribute = type.getAttribute("bool_false");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Bool);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<bool>(), false);
}

void tst_CastAttributes::cast_bool_false_to_int() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"bool_false", CastType::Integer}});

    auto attribute = type.getAttribute("bool_false");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Int);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<int>(), 0);
}

void tst_CastAttributes::cast_smallint_to_uint() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"smallint", CastType::UInteger}});

    auto attribute = type.getAttribute("smallint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::UInt);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<uint>(), static_cast<uint>(32760));
}

void tst_CastAttributes::cast_smallint_u_to_ulonglong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"smallint_u", CastType::ULongLong}});

    auto attribute = type.getAttribute("smallint_u");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::ULongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<quint64>(), static_cast<quint64>(32761));
}

void tst_CastAttributes::cast_int_to_ulonglong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"int", CastType::ULongLong}});

    auto attribute = type.getAttribute("int");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::ULongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<quint64>(), static_cast<quint64>(2147483640));
}

void tst_CastAttributes::cast_int_u_to_ulonglong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"int_u", CastType::ULongLong}});

    auto attribute = type.getAttribute("int_u");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::ULongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<quint64>(), static_cast<quint64>(2147483641));
}

void tst_CastAttributes::cast_bigint_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"bigint", CastType::QString}});

    auto attribute = type.getAttribute("bigint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("9223372036854775800"));
}

void tst_CastAttributes::cast_bigint_u_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"bigint_u", CastType::QString}});

    auto attribute = type.getAttribute("bigint_u");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("9223372036854775801"));
}

void tst_CastAttributes::cast_smallint_Negative_to_short() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNegative(connection);

    type.mergeCasts({{"smallint", CastType::Short}});

    auto attribute = type.getAttribute("smallint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Short);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Short);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Short);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<int>(), -32762);
}

void tst_CastAttributes::cast_smallint_Negative_to_int() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNegative(connection);

    type.mergeCasts({{"smallint", CastType::Int}});

    auto attribute = type.getAttribute("smallint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Int);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<int>(), -32762);
}

void tst_CastAttributes::cast_smallint_Negative_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNegative(connection);

    type.mergeCasts({{"smallint", CastType::QString}});

    auto attribute = type.getAttribute("smallint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("-32762"));
}

void tst_CastAttributes::cast_int_Negative_to_longlong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNegative(connection);

    type.mergeCasts({{"int", CastType::LongLong}});

    auto attribute = type.getAttribute("int");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<qint64>(), static_cast<qint64>(-2147483642));
}

void tst_CastAttributes::cast_int_Negative_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNegative(connection);

    type.mergeCasts({{"int", CastType::QString}});

    auto attribute = type.getAttribute("int");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("-2147483642"));
}

void tst_CastAttributes::cast_bigint_Negative_to_longlong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNegative(connection);

    type.mergeCasts({{"bigint", CastType::LongLong}});

    auto attribute = type.getAttribute("bigint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<qint64>(), static_cast<qint64>(-9223372036854775802));
}

void tst_CastAttributes::cast_bigint_Negative_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNegative(connection);

    type.mergeCasts({{"bigint", CastType::QString}});

    auto attribute = type.getAttribute("bigint");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("-9223372036854775802"));
}

void tst_CastAttributes::cast_double_to_longlong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    type.mergeCasts({{"double", CastType::LongLong}});

    auto attribute = type.getAttribute("double");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<qint64>(), static_cast<qint64>(1000000));
}

void tst_CastAttributes::cast_double_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    type.mergeCasts({{"double", CastType::QString}});

    auto attribute = type.getAttribute("double");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("1000000.123"));
}

void tst_CastAttributes::cast_double_Negative_to_longlong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNegative(connection);

    type.mergeCasts({{"double", CastType::LongLong}});

    auto attribute = type.getAttribute("double");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<qint64>(), static_cast<qint64>(-1000000));
}

void tst_CastAttributes::cast_double_Negative_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNegative(connection);

    type.mergeCasts({{"double", CastType::QString}});

    auto attribute = type.getAttribute("double");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("-1000000.123"));
}

void tst_CastAttributes::cast_decimal_to_Decimal() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    type.mergeCasts({{"decimal", CastType::Decimal}});

    auto attribute = type.getAttribute("decimal");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<double>(), static_cast<double>(100000.12));
}

void tst_CastAttributes::cast_decimal_to_longlong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    type.mergeCasts({{"decimal", CastType::LongLong}});

    auto attribute = type.getAttribute("decimal");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<qint64>(), static_cast<qint64>(100000));
}

void tst_CastAttributes::cast_decimal_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    type.mergeCasts({{"decimal", CastType::QString}});

    auto attribute = type.getAttribute("decimal");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("100000.12"));
}

void tst_CastAttributes::cast_decimal_Negative_to_Decimal() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNegative(connection);

    type.mergeCasts({{"decimal", CastType::Decimal}});

    auto attribute = type.getAttribute("decimal");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<double>(), static_cast<double>(-100000.12));
}

void tst_CastAttributes::cast_decimal_Negative_to_longlong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNegative(connection);

    type.mergeCasts({{"decimal", CastType::LongLong}});

    auto attribute = type.getAttribute("decimal");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<qint64>(), static_cast<qint64>(-100000));
}

void tst_CastAttributes::cast_decimal_Negative_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNegative(connection);

    type.mergeCasts({{"decimal", CastType::QString}});

    auto attribute = type.getAttribute("decimal");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("-100000.12"));
}

void tst_CastAttributes::cast_decimal_to_Decimal_With_Modifier_2_Down() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    type.mergeCasts({{"decimal_down", {CastType::Decimal, 2}}});

    auto attribute = type.getAttribute("decimal_down");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<double>(), static_cast<double>(100.12));
}

void tst_CastAttributes::cast_decimal_to_Decimal_With_Modifier_2_Up() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = model(connection);

    type.mergeCasts({{"decimal_up", {CastType::Decimal, 2}}});

    auto attribute = type.getAttribute("decimal_up");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<double>(), static_cast<double>(100.13));
}

void tst_CastAttributes::cast_decimal_to_Decimal_With_Modifier_2_Down_Negative() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNegative(connection);

    type.mergeCasts({{"decimal_down", {CastType::Decimal, 2}}});

    auto attribute = type.getAttribute("decimal_down");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<double>(), static_cast<double>(-100.13));
}

void tst_CastAttributes::cast_decimal_to_Decimal_With_Modifier_2_Up_Negative() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNegative(connection);

    type.mergeCasts({{"decimal_up", {CastType::Decimal, 2}}});

    auto attribute = type.getAttribute("decimal_up");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<double>(), static_cast<double>(-100.12));
}

void tst_CastAttributes::cast_timestamp_to_QDateTime() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"timestamp", CastType::QDateTime}});

    auto attribute = type.getAttribute("timestamp");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QDateTime);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QDateTime);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QDateTime);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QDateTime>(),
             QDateTime({2022, 9, 9}, {8, 41, 28}, TTimeZone::UTC));
}

void tst_CastAttributes::cast_timestamp_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"timestamp", CastType::QString}});

    auto attribute = type.getAttribute("timestamp");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    /* This compare is little weird because it's converted to the QDateTime by the MySQL,
       and PostgreSQL drivers, then it's converted to the QString by Cast Attributes
       feature and then I'm converting it back to the QDateTime and comparing.
       At least, it tests that nothing weird happened during all these conversions. */
    QCOMPARE(attribute.value<QDateTime>().toString(Qt::ISODate),
             QString("2022-09-09T08:41:28Z"));
}

void tst_CastAttributes::cast_timestamp_to_Timestamp() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"timestamp", CastType::Timestamp}});

    auto attribute = type.getAttribute("timestamp");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<qint64>(), static_cast<qint64>(1662712888));
    // This is not 100% ok, but I want to do also this QCOMPARE()
    QCOMPARE(QDateTime::fromSecsSinceEpoch(attribute.value<qint64>(), TTimeZone::UTC),
             QDateTime({2022, 9, 9}, {8, 41, 28}, TTimeZone::UTC));
}

void tst_CastAttributes::cast_datetime_to_QDateTime() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"datetime", CastType::QDateTime}});

    auto attribute = type.getAttribute("datetime");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QDateTime);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QDateTime);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QDateTime);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QDateTime>(),
             QDateTime({2022, 9, 10}, {8, 41, 28}, TTimeZone::UTC));
}

void tst_CastAttributes::cast_datetime_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"datetime", CastType::QString}});

    auto attribute = type.getAttribute("datetime");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    /* This compare is little weird because it's converted to the QDateTime by the MySQL,
       and PostgreSQL drivers, then it's converted to the QString by Cast Attributes
       feature and then I'm converting it back to the QDateTime and comparing.
       At least, it tests that nothing weird happened during all these conversions. */
    QCOMPARE(attribute.value<QDateTime>().toString(Qt::ISODate),
             QString("2022-09-10T08:41:28Z"));
}

void tst_CastAttributes::cast_date_to_QDate() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"date", CastType::QDate}});

    auto attribute = type.getAttribute("date");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QDate);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QDate);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QDate);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QDate>(),
             // QDate doesn't have a time zone
             QDate(2022, 9, 11));
}

void tst_CastAttributes::cast_date_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"date", CastType::QString}});

    auto attribute = type.getAttribute("date");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("2022-09-11"));
}

void tst_CastAttributes::cast_time_to_QTime() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"time", CastType::QTime}});

    auto attribute = type.getAttribute("time");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QTime);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QTime);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QTime);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QTime>(),
             // QTime doesn't have a time zone
             QTime(17, 1, 5));
}

void tst_CastAttributes::cast_time_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"time", CastType::QString}});

    auto attribute = type.getAttribute("time");
    auto typeId = attribute.typeId();

    const auto driverName = DB::driverName(connection);

    if (driverName == QMYSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    if (driverName == QPSQL)
        /* This is kind of special case, it has .000 because Postgres returns QTime() and
           if the QVariant::convert() (code is actually in qmetatype.cpp) detects
           conversion from QTime to QString then it uses .toString(Qt::ISODateWithMs). */
        QCOMPARE(attribute.value<QString>(), QString("17:01:05.000"));
    else
        QCOMPARE(attribute.value<QString>(), QString("17:01:05"));
}

void tst_CastAttributes::cast_blob_to_QByteArray() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"binary", CastType::QByteArray}});

    auto attribute = type.getAttribute("binary");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QByteArray);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QByteArray);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QByteArray);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QByteArray>(),
             QByteArray::fromHex("517420697320677265617421"));
}

void tst_CastAttributes::cast_blob_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = model(connection);

    type.mergeCasts({{"binary", CastType::QString}});

    auto attribute = type.getAttribute("binary");
    auto typeId = attribute.typeId();

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QCOMPARE(attribute.value<QString>(), QString("Qt is great!"));
}

void tst_CastAttributes::cast_Null_bool_true_to_bool() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"bool_true", CastType::Bool}});

    auto attribute = type.getAttribute("bool_true");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Bool);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Bool);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

/* Custom Null and Invalid casts */

void tst_CastAttributes::cast_Null_bool_true_to_int() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"bool_true", CastType::Integer}});

    auto attribute = type.getAttribute("bool_true");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Int);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Existing_Invalid_to_int() const
{
    /* Don't convert/cast invalid QVariant because it changes validity from false
       to true and sets a QVariant to null, so no cast can't happen. */
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    Type type;
    type.setAttribute("invalid", {});

    type.mergeCasts({{"invalid", CastType::Integer}});

    auto attribute = type.getAttribute("invalid");
    auto typeId = attribute.typeId();

    QVERIFY(!attribute.isValid());
    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UnknownType);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::UnknownType);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::UnknownType);
    else
        Q_UNREACHABLE();

    QVERIFY(!attribute.isValid());
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_NonExisting_Invalid_to_int() const
{
    /* Don't convert/cast invalid QVariant because it changes validity from false
       to true and sets a QVariant to null, so no cast can't happen. */
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    Type type;

    type.mergeCasts({{dummy_NONEXISTENT, CastType::Integer}});

    auto attribute = type.getAttribute(dummy_NONEXISTENT);
    auto typeId = attribute.typeId();

    QVERIFY(!attribute.isValid());
    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UnknownType);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::UnknownType);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::UnknownType);
    else
        Q_UNREACHABLE();

    QVERIFY(!attribute.isValid());
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_smallint_to_Short() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"smallint", CastType::Short}});

    auto attribute = type.getAttribute("smallint");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Short);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Short);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Short);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_smallint_u_to_UShort() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"smallint_u", CastType::UShort}});

    auto attribute = type.getAttribute("smallint_u");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UShort);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::UShort);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::UShort);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_int_to_Int() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"int", CastType::Int}});

    auto attribute = type.getAttribute("int");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Int);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Int);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_int_u_to_UInt() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"int_u", CastType::UInt}});

    auto attribute = type.getAttribute("int_u");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::UInt);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::UInt);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_bigint_to_LongLong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"bigint", CastType::LongLong}});

    auto attribute = type.getAttribute("bigint");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_bigint_u_to_ULongLong() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"bigint_u", CastType::ULongLong}});

    auto attribute = type.getAttribute("bigint_u");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::ULongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::ULongLong);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_double_to_Double() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"double", CastType::Double}});

    auto attribute = type.getAttribute("double");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_double_NaN() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The NaN value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNull(connection);

    type.mergeCasts({{"double", CastType::Double}});

    auto attribute = type.getAttribute("double_nan");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_double_Infinity() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The Infinity value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNull(connection);

    type.mergeCasts({{"double", CastType::Double}});

    auto attribute = type.getAttribute("double_infinity");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_decimal_to_Decimal() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"decimal", CastType::Decimal}});

    auto attribute = type.getAttribute("decimal");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::Double);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::Double);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_decimal_NaN() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The NaN value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNull(connection);

    type.mergeCasts({{"double", CastType::Double}});

    auto attribute = type.getAttribute("decimal_nan");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_decimal_Infinity() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) != QPSQL)
        QSKIP("The Infinity value for the double type column is supported only "
              "on the PostgreSQL database.", );

    QCOMPARE(defaultNumericalPrecisionPolicy(connection),
             TPrecisionPolicy::LowPrecisionDouble);

    auto &type = modelNull(connection);

    type.mergeCasts({{"double", CastType::Double}});

    auto attribute = type.getAttribute("decimal_infinity");
    auto typeId = attribute.typeId();

    QCOMPARE(typeId, QMetaType::Double);
    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_string_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"string", CastType::QString}});

    auto attribute = type.getAttribute("string");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_text_to_QString() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"text", CastType::QString}});

    auto attribute = type.getAttribute("text");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QString);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QString);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_timestamp_to_Timestamp() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"timestamp", CastType::Timestamp}});

    auto attribute = type.getAttribute("timestamp");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::LongLong);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::LongLong);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_datetime_to_QDateTime() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"datetime", CastType::QDateTime}});

    auto attribute = type.getAttribute("datetime");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QDateTime);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QDateTime);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QDateTime);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_date_to_QDate() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"date", CastType::QDate}});

    auto attribute = type.getAttribute("date");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QDate);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QDate);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QDate);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_time_to_QTime() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"time", CastType::QTime}});

    auto attribute = type.getAttribute("time");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QTime);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QTime);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QTime);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}

void tst_CastAttributes::cast_Null_blob_to_QByteArray() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto &type = modelNull(connection);

    type.mergeCasts({{"binary", CastType::QByteArray}});

    auto attribute = type.getAttribute("binary");
    auto typeId = attribute.typeId();

    QVERIFY(attribute.isNull());

    if (const auto driverName = DB::driverName(connection);
        driverName == QMYSQL
    )
        QCOMPARE(typeId, QMetaType::QByteArray);
    else if (driverName == QPSQL)
        QCOMPARE(typeId, QMetaType::QByteArray);
    else if (driverName == QSQLITE)
        QCOMPARE(typeId, QMetaType::QByteArray);
    else
        Q_UNREACHABLE();

    QVERIFY(attribute.isNull());
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

Type &
tst_CastAttributes::model(const QString &connection, const RowType rowType) const
{
    ModelCacheKey cacheKey {connection, rowType};

    // Return a cached Type model
    if (m_modelsCache.contains(cacheKey))
        return resetCasts(m_modelsCache.at(cacheKey));

    const auto id = static_cast<std::underlying_type_t<RowType>>(rowType);

    // Obtain a Type model from the DB
    auto type = Type::on(connection)->find(id);

    if (!type)
        throw Orm::Exceptions::RuntimeError(
                sl("Can't find the Type model with the ID(%1).").arg(id));

    // Cache the Type model
    auto [itType, ok] = m_modelsCache.insert({std::move(cacheKey),
                                              std::move(type.value())});

    // Validate insertion to the cache 😮
    if (!ok)
        throw Orm::Exceptions::RuntimeError(
                sl("Insertion to the tst_CastAttributes::m_modelCache for the '%1' "
                   "connection and ID(%3) failed in %2().")
                .arg(connection, __tiny_func__).arg(id));

    /* The u_casts has to be reset everytime since the u_casts is static because
       if the u_casts is modified for eg. MySQL connection test and right after is
       executed the same unit test for eg. SQLite connection then we need the u_casts
       static data member clean.
       This was not needed when the u_casts was normal data member. 😔 */
    return resetCasts(itType->second);
}

Type &
tst_CastAttributes::modelNegative(const QString &connection) const
{
    return model(connection, Negative);
}

Type &
tst_CastAttributes::modelNull(const QString &connection) const
{
    return model(connection, Null);
}

Type &tst_CastAttributes::resetCasts(Type &type)
{
    return type.resetCasts();
}

void tst_CastAttributes::disableReturnQDateTime(const QString &connection)
{
    dynamic_cast<SQLiteConnection &>(DB::connection(connection))
            .setReturnQDateTime(false);
}

void tst_CastAttributes::enableReturnQDateTime(const QString &connection)
{
    dynamic_cast<SQLiteConnection &>(DB::connection(connection))
            .setReturnQDateTime(true);
}

TPrecisionPolicy
tst_CastAttributes::defaultNumericalPrecisionPolicy(const QString &connection)
{
    return DB::driver(connection)
#ifdef TINYORM_USING_QTSQLDRIVERS
            ->numericalPrecisionPolicy();
#elif defined(TINYORM_USING_TINYDRIVERS)
            ->defaultNumericalPrecisionPolicy();
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
}

QTEST_MAIN(tst_CastAttributes) // NOLINT(bugprone-exception-escape)

#include "tst_castattributes.moc"
