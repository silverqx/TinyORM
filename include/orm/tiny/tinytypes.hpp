#pragma once
#ifndef ORM_TINY_TINYTYPES_HPP
#define ORM_TINY_TINYTYPES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <optional>

#include "orm/macros/export.hpp"
#include "orm/ormtypes.hpp" // IWYU pragma: export
#include "orm/tiny/tinyconcepts.hpp" // IWYU pragma: keep

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Tiny
{
namespace Relations
{
    template<class Model, class Related>
    class Relation;
}
namespace Types
{
    template<DerivedCollectionModel Model>
    class ModelsCollection;
}

    template<typename Model>
    class Builder;

    /*! Alias for the TinyBuilder. */
    template<typename Model>
    using TinyBuilder = Builder<Model>;

    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model;

    /*! Alias for the ModelsCollection. */
    template<typename Model>
    using ModelsCollection = Tiny::Types::ModelsCollection<Model>;

    /*! The type in which the relationships are stored. */
    template<AllRelationsConcept ...AllRelations>
    using RelationsType = std::variant<std::monostate,
                                       ModelsCollection<AllRelations>...,
                                       std::optional<AllRelations>...>;

    // TODO pretty print in the debugger silverqx
    /*! Attribute item used in ORM models. */
    struct SHAREDLIB_EXPORT AttributeItem
    {
        QString  key   {};
        QVariant value {};

        /*! Converting operator to the UpdateItem. */
        explicit operator UpdateItem() const;
    };

    /*! Equality comparison operator for the AttributeItem. */
    SHAREDLIB_EXPORT bool
    operator==(const AttributeItem &left, const AttributeItem &right);

    /*! Eager load relation item. */
    struct SHAREDLIB_EXPORT WithItem
    {
        QString name;
        std::function<void(QueryBuilder &)> constraints = nullptr;

        /*! Create the QVector<WithItem> from the QVector<QString>. */
        static QVector<WithItem> fromStringVector(const QVector<QString> &relations);
        /*! Create the QVector<WithItem> from the QVector<QString>. */
        static QVector<WithItem> fromStringVector(QVector<QString> &&relations);
    };

    /*! Equality comparison operator for the WithItem. */
    [[maybe_unused]]
    SHAREDLIB_EXPORT bool operator==(const WithItem &left, const WithItem &right);

    /*! Tag for Model::getRelation() family methods to return Related type
        directly ( not container type ). */
    struct One {};

    /*! Tag for Model::getRelationshipFromMethod() to return ModelsCollection<Related>
        type ( 'Many' relation types ), only internal type for now, used as the template
        tag in the HasRelationships::pushVisited. */
    struct Many {};

    /*! Options parameter type used in Model save() method. */
    struct SaveOptions
    {
        /*! Indicates if timestamps of parent models should be touched. */
        bool touch = true;
    };

    /*! Tag for the model. */
    class IsModel
    {};

    /*! Tag used to select a constructor without filling Default Attribute values. */
    struct DontFillDefaultAttributes
    {
        /*! Default constructor. */
        inline DontFillDefaultAttributes() = default;
    };

    /*! Instance of the tag used to select a constructor without filling Default
        Attribute values. */
    inline constexpr DontFillDefaultAttributes dontFillDefaultAttributes {};

    /*! Cast types. */
    enum struct CastType
    {
        // Bool
        Bool, Boolean,
        // Int 16-bit
        Short,
        UShort,
        // Int 32-bit
        Int,  Integer,
        UInt, UInteger,
        // Int 64-bit
        LongLong,
        ULongLong,
        // Float
        Real, Float,  Double,
        Decimal,
        // String
        QString,
        // QDateTime
        QDate,
        QDateTime,
        Timestamp,
        CustomQDate,
        CustomQDateTime,
        // Others
        QByteArray,
    };

    /*! Cast type item. */
    class CastItem
    {
    public:
        /*! Converting constructor from the CastType. */
        inline CastItem(CastType castType, QVariant modifier = {}) noexcept; // NOLINT(google-explicit-constructor)

        /*! Equality comparison operator for the CastItem. */
        inline bool operator==(const CastItem &) const = default;

        /* Getters */
        /*! Get the cast type. */
        inline CastType type() const noexcept;
        /*! Get a cast modifier/s. */
        inline const QVariant &modifier() const noexcept;

    private:
        /*! Cast Type. */
        CastType m_type;
        /*! Cast type modfier. */
        QVariant m_modifier;
    };

    /* Cast type item */

    /* public */

    CastItem::CastItem(const CastType castType, QVariant modifier) noexcept
        : m_type(castType)
        , m_modifier(std::move(modifier))
    {}

    /* Getters */

    CastType CastItem::type() const noexcept
    {
        return m_type;
    }

    const QVariant &CastItem::modifier() const noexcept
    {
        return m_modifier;
    }

namespace Concerns
{
    /*! QueriesRelationships builder type passed to the callback. */
    template<typename Related>
    using QueriesRelationshipsCallback =
            std::conditional_t<std::is_void_v<Related>, QueryBuilder,
                               TinyBuilder<Related>>;
} // namespace Concerns
} // namespace Tiny

    /*! Alias for the Orm::Tiny::One, shortcut alias. */
    using One = Orm::Tiny::One;

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

// Register custom QVariant types for Qt 5
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#ifdef TINYORM_COMMON_NAMESPACE
Q_DECLARE_METATYPE(TINYORM_COMMON_NAMESPACE::Orm::Tiny::AttributeItem) // NOLINT(performance-no-int-to-ptr, misc-no-recursion)
#else
Q_DECLARE_METATYPE(Orm::Tiny::AttributeItem) // NOLINT(performance-no-int-to-ptr, misc-no-recursion)
#endif
#endif

#endif // ORM_TINY_TINYTYPES_HPP
