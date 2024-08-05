#pragma once
#ifndef ORM_TINY_CONCERNS_HASATTRIBUTES_HPP
#define ORM_TINY_CONCERNS_HASATTRIBUTES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/config.hpp" // IWYU pragma: keep

#ifdef TINYORM_DEBUG
#  include <QDebug>
#endif

#include <cmath>

#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/exceptions/invalidformaterror.hpp"
#include "orm/macros/likely.hpp"
#include "orm/macros/threadlocal.hpp"
#include "orm/ormtypes.hpp"
#include "orm/tiny/casts/attribute.hpp"
#include "orm/tiny/exceptions/mutatormappingnotfounderror.hpp"
#include "orm/tiny/macros/crtpmodelwithbase.hpp"
#include "orm/tiny/utils/attribute.hpp"
#include "orm/utils/configuration.hpp"
#include "orm/utils/helpers.hpp"
#include "orm/utils/nullvariant.hpp"
#include "orm/utils/string.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Concerns
{

    /*! Model attributes. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HasAttributes // NOLINT(bugprone-exception-escape, misc-no-recursion)
    {
        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;
        /*! Alias for the configuration utils. */
        using ConfigUtils = Orm::Utils::Configuration;
        /*! Alias for the helper utils. */
        using Helpers = Orm::Utils::Helpers;
        /*! Alias for the null QVariant-s utils. */
        using NullVariant = Orm::Utils::NullVariant;
        /*! Alias for the string utils. */
        using StringUtils = Orm::Utils::String;
        /*! Alias for the type utils. */
        using TypeUtils = Orm::Utils::Type;

    public:
        /*! Alias for the attribute. */
        using Attribute = Orm::Tiny::Casts::Attribute;
        /*! Alias for the attributes vector size type. */
        using AttributesSizeType = typename QList<AttributeItem>::size_type;

        /*! Equality comparison operator for the HasAttributes concern. */
        bool operator==(const HasAttributes &) const = default;

        /*! Set a given attribute on the model. */
        Derived &setAttribute(const QString &key, QVariant value);
        /*! Set a vector of model attributes. No checking is done. */
        Derived &setRawAttributes(const QList<AttributeItem> &attributes,
                                  bool sync = false);
        /*! Set a vector of model attributes. No checking is done. */
        Derived &setRawAttributes(QList<AttributeItem> &&attributes,
                                  bool sync = false);
        /*! Sync the original attributes with the current. */
        Derived &syncOriginal();

        /*! Get all of the current attributes on the model (insertion order). */
        inline const QList<AttributeItem> &getAttributes() const noexcept;
        /*! Get all of the current attributes on the model (for fast lookup). */
        inline const std::unordered_map<QString, AttributesSizeType> &
        getAttributesHash() const;
        /*! Get an attribute from the model. */
        QVariant getAttribute(const QString &key) const;
        /*! Get an attribute from the model casted to the given type. */
        template<typename T>
        T getAttribute(const QString &key) const;
        /*! Get a plain attribute (not a relationship). */
        QVariant getAttributeValue(const QString &key) const;
        /*! Get an attribute from the m_attributes vector. */
        QVariant getAttributeFromArray(const QString &key) const;

        /*! Get the model's original attribute value (transformed). */
        QVariant getOriginal(const QString &key,
                             const QVariant &defaultValue = {}) const;
        /*! Get the model's original attribute values (transformed and insert order). */
        QList<AttributeItem> getOriginals() const;
        /*! Get the model's original attributes hash (for fast lookup). */
        inline const std::unordered_map<QString, AttributesSizeType> &
        getOriginalsHash() const;
        /*! Get the model's raw original attribute value. */
        QVariant getRawOriginal(const QString &key,
                                const QVariant &defaultValue = {}) const;
        /*! Get the model's raw original attribute values (insertion order). */
        inline const QList<AttributeItem> &getRawOriginals() const;

        /*! Unset an attribute on the model, returns the number of attributes removed. */
        Derived &unsetAttribute(const AttributeItem &value);
        /*! Unset an attribute on the model. */
        Derived &unsetAttribute(const QString &key);

        /*! Get a subset of the model's attributes. */
        QList<AttributeItem> only(const QStringList &attributes) const;
        /*! Get a subset of the model's attributes. */
        QList<AttributeItem> only(QStringList &&attributes) const; // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)

        /*! Get the attributes that have been changed since last sync
            (insertion order). */
        QList<AttributeItem> getDirty() const;
        /*! Get the attributes that have been changed since last sync
            (for fast lookup). */
        std::unordered_map<QString, AttributesSizeType> getDirtyHash() const;
        /*! Determine if the model or any of the given attribute(s) have
            been modified. */
        inline bool isDirty(const QStringList &attributes = {}) const;
        /*! Determine if the model or any of the given attribute(s) have
            been modified. */
        inline bool isDirty(const QString &attribute) const;
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        inline bool isClean(const QStringList &attributes = {}) const;
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        inline bool isClean(const QString &attribute) const;

        /*! Get the attributes that were changed (insertion order). */
        inline const QList<AttributeItem> &getChanges() const;
        /*! Get the attributes that were changed (for fast lookup). */
        inline const std::unordered_map<QString, AttributesSizeType> &
        getChangesHash() const;
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        inline bool wasChanged(const QStringList &attributes = {}) const;
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        inline bool wasChanged(const QString &attribute) const;

        /*! Get the format for database stored dates. */
        const QString &getDateFormat() const;
        /*! Set the date format used by the model. */
        Derived &setDateFormat(const QString &format);

        /*! Get the format for database stored times. */
        const QString &getTimeFormat() const;
        /*! Set the time format used by the model. */
        Derived &setTimeFormat(const QString &format);

        /*! Convert a QDateTime or QDate to a storable string. */
        QVariant fromDateTime(const QVariant &value) const;
        /*! Convert a QDateTime or QDate to a storable string. */
        QVariant fromDateTime(const QDateTime &value) const;
        /*! Get the attributes that should be converted to dates. */
        QStringList getDates() const;

        /*! Proxy for an attribute element used in the operator[] &. */
        class AttributeReference // clazy:exclude=rule-of-three
        {
            friend HasAttributes<Derived, AllRelations...>;

        public:
            /*! Default destructor. */
            ~AttributeReference() = default;

            /*! Copy constructor. */
            AttributeReference(const AttributeReference &) = default;
            /*! Move constructor. */
            AttributeReference(AttributeReference &&) noexcept = default;
            /*! Deleted move assignment operator. */
            AttributeReference &operator=(AttributeReference &&) = delete;

            /*! Assign a value of the QVariant to the referenced attribute. */
            inline const AttributeReference & // NOLINT(misc-unconventional-assign-operator, cppcoreguidelines-c-copy-assignment-signature)
            operator=(const QVariant &value) const;
            /*! Assign a value of another attribute reference to the referenced
                attribute. */
            inline const AttributeReference & // NOLINT(misc-unconventional-assign-operator, cppcoreguidelines-c-copy-assignment-signature)
            operator=(const AttributeReference &attributeReference) const;

            /*! Accesses the contained value, only const member functions. */
            inline const QVariant *operator->() const;
            /*! Accesses the contained value. */
            inline QVariant value() const;
            /*! Accesses the contained value. */
            inline QVariant operator*() const;
            /*! Converting operator to the QVariant. */
            inline operator QVariant() const; // NOLINT(google-explicit-constructor)

        private:
            /*! AttributeReference's private constructor. */
            AttributeReference(Model<Derived, AllRelations...> &model,
                               const QString &attribute);  // NOLINT(modernize-pass-by-value)

            /*! The model on which is an attribute set. */
            std::reference_wrapper<Model<Derived, AllRelations...>> m_model;
            /*! Attribute key name. */
            QString m_attribute;
            /*! The temporary cache used during operator->() call, to be able
                to return the QVariant *. */
            mutable QVariant m_attributeCache;
        };

        /*! Return modifiable attribute reference, can be used on the left-hand side
            of an assignment operator. */
        inline AttributeReference operator[](const QString &attribute) &;
        /*! Return an attribute by the given key. */
        inline QVariant operator[](const QString &attribute) const &;
        /*! Return an attribute by the given key. */
        inline QVariant operator[](const QString &attribute) &&;
        /*! Return an attribute by the given key. */
        inline QVariant operator[](const QString &attribute) const &&;

        /* Casting Attributes */
        /*! Get the casts hash. */
        std::unordered_map<QString, CastItem> getCasts() const;
        /*! Determine whether an attribute should be cast to a native type. */
        inline bool hasCast(const QString &key) const;
        /*! Determine whether an attribute should be cast to a native type. */
        inline bool hasCast(const QString &key,
                            const std::unordered_set<CastType> &types) const;

        /*! Merge new casts with existing casts on the model. */
        Derived &mergeCasts(const std::unordered_map<QString, CastItem> &casts);
        /*! Merge new casts with existing casts on the model. */
        Derived &mergeCasts(std::unordered_map<QString, CastItem> &casts);
        /*! Merge new casts with existing casts on the model. */
        Derived &mergeCasts(std::unordered_map<QString, CastItem> &&casts);
        /*! Reset the Type::u_casts. */
        inline Derived &resetCasts();

        /* QDateTime time zone */
        /*! Get the QtTimeZoneConfig for the current connection. */
        inline const QtTimeZoneConfig &getQtTimeZone() const;
        /*! Set the QtTimeZoneConfig for the current connection (override qt_timezone). */
        Derived &setQtTimeZone(const QVariant &timezone);
        /*! Set the QtTimeZoneConfig for the current connection (override qt_timezone). */
        Derived &setQtTimeZone(QtTimeZoneConfig &&timezone);
        /*! Determine whether the QDateTime time zone should be converted. */
        inline bool isConvertingTimeZone() const;

        /* Serialization - Attributes */
        /*! Convert the model's attributes to the map. */
        QVariantMap attributesToMap() const;
        /*! Convert the model's attributes to the vector. */
        QList<AttributeItem> attributesToList() const;

        /* Serialization - Appends */
        /*! Append accessor attribute to the u_appends set. */
        inline Derived &append(const QString &attribute);
        /*! Append accessor attribute to the u_appends set. */
        inline Derived &append(QString &&attribute);

        /*! Append accessor attributes to the u_appends set. */
        inline Derived &append(const std::set<QString> &attributes);
        /*! Append accessor attributes to the u_appends set. */
        inline Derived &append(std::set<QString> &&attributes);

        /*! Determine whether the u_appends set contains the given accessor attribute. */
        inline bool hasAppend(const QString &attribute) const;

        /*! Get the accessors that are being appended to the serialized model. */
        inline const std::set<QString> &getAppends() const noexcept;

        /*! Set the accessors to append to the serialized model. */
        inline Derived &setAppends(const std::set<QString> &attributes);
        /*! Set the accessors to append to the serialized model. */
        inline Derived &setAppends(std::set<QString> &&attributes);

        /*! Clear the u_appends accessor attributes for the model. */
        inline Derived &clearAppends() noexcept;

    protected:
        /*! Alias for the mutator method. */
        using MutatorFunction = std::function<Attribute(const Derived &)>;

        /*! Transform a raw model value using mutators, casts, etc. */
        QVariant transformModelValue(const QString &key, const QVariant &value) const;
        /*! Get the model's original attribute values. */
        QVariant getOriginalWithoutRewindingModel(
                const QString &key, const QVariant &defaultValue = {}) const;

        /*! Get all of the current attributes on the model. */
        inline const QList<AttributeItem> &getRawAttributes() const;

        /*! Determine if any of the given attributes were changed. */
        bool hasChanges(const std::unordered_map<QString, AttributesSizeType> &changes,
                        const QStringList &attributes = {}) const;
        /*! Sync the changed attributes. */
        Derived &syncChanges();

        /*! Sync a single original attribute with its current value. */
        inline Derived &syncOriginalAttribute(const QString &attribute);
        /*! Sync multiple original attributes with their current values. */
        Derived &syncOriginalAttributes(const QStringList &attributes);

        /*! Determine if the new and old values for a given key are equivalent
            (used by the getDirty()). */
        bool originalIsEquivalent(const QString &key) const;

        /*! Rehash attribute positions from the given index. */
        static void rehashAttributePositions(
                const QList<AttributeItem> &attributes,
                std::unordered_map<QString, AttributesSizeType> &attributesHash,
                AttributesSizeType from = 0);
        /*! Rehash attribute positions from the given index. */
        static std::unordered_map<QString, AttributesSizeType>
        rehashAttributePositions(const QList<AttributeItem> &attributes,
                                 AttributesSizeType from = 0);

        /* Datetime-related */
        /*! Determine if the given attribute is a date. */
        bool isDateAttribute(const QString &key) const;

        /*! Return a timestamp as QDateTime object. */
        QDateTime asDateTime(const QVariant &value) const;
        /*! Return a timestamp as QDate object. */
        inline QDate asDate(const QVariant &value) const;
        /*! Return a timestamp as QTime object. */
        inline QTime asTime(const QVariant &value) const;
        /*! Return a timestamp as Unix timestamp. */
        inline qint64 asTimestamp(const QVariant &value) const;

        /*! Return a timestamp as QDateTime or QDate object. */
        QVariant asDateOrDateTime(const QVariant &value) const;
        /*! Return the given value as QDateTime, QDate, or QTime object. */
        QVariant asDateOrDateTimeOrTime(const QVariant &value) const;
        /*! Convert a QDateTime, QDate, or QTime to a storable string. */
        QVariant fromDateOrDateTimeOrTime(const QVariant &value,
                                          const QString &format) const;

        /*! Get the correct QVariant(null) by a type in the QVariant and format. */
        static QVariant nullFor_fromDateTime(const QVariant &value,
                                             const QString &format);
        /*! Get the correct QVariant(null) by a type in the QVariant. */
        static QVariant nullFor_addCastAttributesTo(const QVariant &value);

        /*! Convert the QDateTime's time zone to the Model's time zone. */
        QDateTime convertTimeZone(QDateTime &&datetime) const;
        /*! Set the QDateTime's time zone to the Model's time zone. */
        QDateTime &setTimeZone(QDateTime &datetime) const;

        /* Casting Attributes */
        /*! Cast an attribute, convert a QVariant value. */
        QVariant castAttribute(const QString &key, const QVariant &value) const;
        /*! Get the type of cast for a model attribute. */
        inline CastItem getCastItem(const QString &key) const;
        /*! Get the type of cast for a model attribute. */
        inline CastType getCastType(const QString &key) const;

        /*! Determine whether a value is Date / DateTime castable. */
        inline bool isDateCastable(const QString &key) const;
        /*! Determine whether a value is a custom Date / DateTime castable. */
        inline bool isCustomDateCastable(const QString &key) const;

        /*! Determine whether the given type is QDate or QDateTime cast type. */
        inline static bool isDateCastType(CastType type);
        /*! Determine if the cast type is a custom QDate or QDateTime cast type. */
        inline static bool isCustomDateCastType(const CastItem &castItem);

        /*! Round a QVariant(double) to the given decimals (used by castAttribute()). */
        inline static QVariant
        roundDecimals(const QVariant &value, const QVariant &decimals);

        /* Serialization - Attributes */
        /*! The return type for the getVectorableAttributes() method. */
        struct VectorableAttributes
        {
            /*! The model's vectorable attributes (insertion order). */
            QList<AttributeItem> attributes;
            /*! The model's vectorable attributes hash (for fast lookup). */
            std::unordered_map<QString, AttributesSizeType> attributesHash;
        };

        /*! Get an attributes map of all mappable attributes. */
        inline QVariantMap getMappableAttributes() const;
        /*! Get an attributes vector of all vectorable attributes. */
        inline VectorableAttributes getVectorableAttributes() const;

        /*! Get an attributes map/vector of serializable attributes (visible/hidden). */
        template<SerializedAttributes C>
        static C getSerializableAttributes(
                const QList<AttributeItem> &attributes,
                const std::set<QString> &visible, const std::set<QString> &hidden,
                const std::set<QString> &appends);

        /*! Add the date attributes to the attributes map. */
        void addDateAttributesToMap(QVariantMap &attributes) const;
        /*! Add the date attributes to the attributes vector. */
        void addDateAttributesToVector(
                QList<AttributeItem> &attributes,
                const std::unordered_map<QString,
                                         AttributesSizeType> &attributesHash) const;

        /*! Add the casted attributes to the attributes map. */
        void addCastAttributesToMap(QVariantMap &attributes) const;
        /*! Add the casted attributes to the attributes vector. */
        void addCastAttributesToVector(
                QList<AttributeItem> &attributes,
                const std::unordered_map<QString,
                                         AttributesSizeType> &attributesHash) const;

        /*! Get an attributes set of serializable appends (visible/hidden). */
        std::set<QString> getSerializableAppends() const;

        /*! Get an accessor (get) value by the given attribute key. */
        QVariant mutateAccessorAttribute(const QString &key) const;

        /*! Prepare a date, datetime, or time for vector, map, or JSON serialization. */
        static QString serializeDateOrDateTimeOrTime(const QVariant &value);
        /*! Prepare a date for vector, map, or JSON serialization. */
        inline static QString serializeDate(QDate date);
        /*! Prepare a datetime for vector, map, or JSON serialization. */
        inline static QString serializeDateTime(const QDateTime &datetime);
        /*! Prepare a time for vector, map, or JSON serialization. */
        inline static QString serializeTime(QTime time);

        /* Data members */
        /*! The model's default values for attributes. */
        T_THREAD_LOCAL
        inline static QList<AttributeItem> u_attributes;
        /*! The model's attributes (insertion order). */
        QList<AttributeItem> m_attributes;
        /*! The model attribute's original state (insertion order).
            On the model from many-to-many relation also contains all pivot values,
            that is normal (insertion order). */
        QList<AttributeItem> m_original;
        /*! The changed model attributes (insertion order). */
        QList<AttributeItem> m_changes;

        /* Don't want to use std::reference_wrapper to attributes, because if a copy
           of the model is made, all references would be invalidated. */
        /*! The model's attributes hash (for fast lookup). */
        std::unordered_map<QString, AttributesSizeType> m_attributesHash;
        /*! The model attribute's original state (for fast lookup). */
        std::unordered_map<QString, AttributesSizeType> m_originalHash;
        /*! The changed model attributes (for fast lookup). */
        std::unordered_map<QString, AttributesSizeType> m_changesHash;

        /*! The storage format of the model's date columns. */
        T_THREAD_LOCAL
        inline static QString u_dateFormat;
        /*! The storage format of the model's time columns. */
        T_THREAD_LOCAL
        inline static QString u_timeFormat;
        /*! The attributes that should be mutated to dates. */
        T_THREAD_LOCAL
        inline static QStringList u_dates;

        /* Casting Attributes */
        /* Has to be static because of setAttribute - isDateAttribute - ... - getCasts:
           auto casts = model.getUserCasts()
           Called from Models ctor. */
        /*! The attributes that should be cast. */
        T_THREAD_LOCAL
        inline static std::unordered_map<QString, CastItem> u_casts;

        /*! Determine how the QDateTime time zone will be converted. */
        mutable std::optional<QtTimeZoneConfig> m_qtTimeZone = std::nullopt;
        /*! Determine whether the QDateTime time zone should be converted. */
        mutable std::optional<bool> m_isConvertingTimeZone = std::nullopt;

        /* Serialization */
        /*! Indicates whether attributes are snake_cased during serialization. */
        T_THREAD_LOCAL
        inline static bool u_snakeAttributes = true;

        /* Serialization - Appends */
        /*! Map of mutator names to methods. */
        T_THREAD_LOCAL
        inline static QHash<QString, MutatorFunction> u_mutators;
        /*! The accessors append to serialized models. */
        std::set<QString> u_appends;
        /*! The cache for already mutated Casts::Attribute-s. */
        mutable QHash<QString, QVariant> m_attributeMutatorsCache;

    private:
        /*! Throw if the m_attributesHash doesn't contain a given attribute. */
        static void throwIfNoAttributeInHash(
                    const std::unordered_map<QString, AttributesSizeType> &attributesHash,
                    const QString &attribute, const QString &functionName);

        /* Casting Attributes */
        /*! Throw if the given attribute can not be converted to the given cast type. */
        static void throwIfCanNotCastAttribute(
                    const QString &key, CastType castType, QMetaType metaType,
                    const QVariant &value, const QString &functionName);
#ifdef TINYORM_DEBUG
        /*! Log if the QVariant::convert() for the given attribute failed. */
        static void logIfConvertAttributeFailed(
                    const QString &key, CastType castType, QMetaType metaType,
                    const QString &functionName);
#endif

        /*! Return the string name of the given cast type. */
        static QString castTypeName(CastType type);

        /* Serialization */
        /*! Remove the u_appends keys from vectorable attributes. */
        static void removeAppendsFromVectorableAttributes(
                QList<AttributeItem> &attributes, bool isVisibleEmpty,
                const std::set<QString> &hidden, const std::set<QString> &appends);

        /*! Cast the given attribute (used in serialization). */
        void castAttributeForSerialization(QVariant &value, const QString &key,
                                           const CastItem &castItem) const;

        /* Serialization - HidesAttributes */
        /*! Get an attributes map/vector of visible serializable attributes. */
        template<SerializedAttributes C>
        static C
        getSerializableVisibleAttributes(const QList<AttributeItem> &attributes,
                                         const std::set<QString> &visible,
                                         const std::set<QString> &appends);

        /*! Get an attributes map without hidden attributes. */
        static QVariantMap
        removeSerializableHiddenAttributes(QVariantMap &&attributes,
                                           const std::set<QString> &hidden);
        /*! Get an attributes vector without hidden attributes. */
        template<typename T> requires AttributesContainerConcept<T> // requires is needed to select the correct overload
        static QList<AttributeItem>
        removeSerializableHiddenAttributes(T &&attributes,
                                           const std::set<QString> &hidden);

        /* Serialization - Appends */
        /*! Get an appends set of visible serializable appends. */
        static std::set<QString>
        getSerializableVisibleAppends(const std::set<QString> &appends,
                                      const std::set<QString> &visible);

        /*! Get an appends set without hidden appends. */
        static std::set<QString>
        removeSerializableHiddenAppends(std::set<QString> &&appends,
                                        const std::set<QString> &hidden);

        /*! Get an accessor value from the given attribute. */
        QVariant getAccessorValueFrom(const Attribute &attribute) const;

        /*! Throw an exception if a relation is not defined. */
        static void
        validateUserMutator(const QString &name,
                            const QHash<QString, MutatorFunction> &userMutators);

        /*! Prepare a date or datetime for vector, map, or JSON serialization. */
        inline static void serializeDateOrDateTimeForAccessors(QVariant &value);

        /*! Convert a AttributeItem QList to the std::unordered_map with caching. */
        const ModelAttributes &
        convertVectorToModelAttributes(const QList<AttributeItem> &attributes) const;

        /* Others */
        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_WITH_BASE_DECLARATIONS

        /* Data members */
        /*! ModelAttributes cache used in mutators callback (only for vectorable appends
            to avoid repeated converting). */
        mutable std::optional<ModelAttributes>
                m_modelAttributesCacheForMutators = std::nullopt;
    };

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setAttribute(
            const QString &key, QVariant value)
    {
        /* If an attribute is listed as a "date", we'll convert it from a DateTime
           instance into a form proper for storage on the database tables using
           the connection grammar's date format. We will auto set the values. */
        if (const auto typeId = value.typeId();
            value.isValid() && (isDateAttribute(key) ||
            // NOTE api different, if the QDateTime or QDate is detected then take it as datetime silverqx
            typeId == QMetaType::QDateTime || typeId == QMetaType::QDate ||
            typeId == QMetaType::QTime)
        )
            value = fromDateTime(value);

        // Found
        if (const auto attribute = m_attributesHash.find(key);
            attribute != m_attributesHash.end()
        )
            m_attributes[attribute->second].value.swap(value);

        // Not Found
        else {
            auto position = m_attributes.size();

            m_attributes.append({key, value});
            m_attributesHash.emplace(key, position);
        }

        // It's enough to clear this cache and recompute when needed
        m_modelAttributesCacheForMutators.reset();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setRawAttributes(
            const QList<AttributeItem> &attributes,
            const bool sync)
    {
        m_attributes = AttributeUtils::removeDuplicateKeys(attributes);

        // Build attributes hash
        m_attributesHash.clear();
        m_attributesHash.reserve(static_cast<decltype (m_attributesHash)::size_type>(
                                     m_attributes.size()));

        rehashAttributePositions(m_attributes, m_attributesHash);

        if (sync)
            syncOriginal();

        m_attributeMutatorsCache.clear();
        m_modelAttributesCacheForMutators.reset();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setRawAttributes(
            QList<AttributeItem> &&attributes,
            const bool sync)
    {
        m_attributes.reserve(attributes.size());
        m_attributes = AttributeUtils::removeDuplicateKeys(std::move(attributes));

        // Build attributes hash
        m_attributesHash.clear();
        m_attributesHash.reserve(static_cast<decltype (m_attributesHash)::size_type>(
                                     m_attributes.size()));

        rehashAttributePositions(m_attributes, m_attributesHash);

        if (sync)
            syncOriginal();

        m_attributeMutatorsCache.clear();
        m_modelAttributesCacheForMutators.reset();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &HasAttributes<Derived, AllRelations...>::syncOriginal()
    {
        m_original = getAttributes();

        rehashAttributePositions(m_original, m_originalHash);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QList<AttributeItem> &
    HasAttributes<Derived, AllRelations...>::getAttributes() const noexcept
    {
        // FEATURE castable silverqx
//        mergeAttributesFromClassCasts();

        // TODO attributes, getAttributes() doesn't apply transformModelValue() on attributes, worth considering to make getRawAttributes() to return raw and getAttributes() to return transformed values, after this changes would be this api different than Eloquent silverqx
        return m_attributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::unordered_map<QString,
            typename HasAttributes<Derived, AllRelations...>::AttributesSizeType> &
    HasAttributes<Derived, AllRelations...>::getAttributesHash() const
    {
        // FEATURE castable silverqx
//        mergeAttributesFromClassCasts();

        return m_attributesHash;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::getAttribute(const QString &key) const
    {
        if (key.isEmpty() || key.isNull())
            return {};

        /* If the attribute exists in the attribute hash or has a cast we will
           get the attribute's value. Otherwise, we will return invalid QVariant.
           Also, don't user the hasCast(key) check here because there is always primary
           key cast and it would return null or invalid QVariant. */
        if (m_attributesHash.contains(key) || basemodel().getUserCasts().contains(key))
            return getAttributeValue(key);

        // FUTURE add getRelationValue() overload without Related template argument, after that I will be able to use it here, Related template parameter will be obtained by the visitor, I think this task is impossible to do silverqx
        // NOTE api different silverqx
        return {};
//        return $this->getRelationValue($key);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename T>
    T HasAttributes<Derived, AllRelations...>::getAttribute(const QString &key) const
    {
        return getAttribute(key).template value<T>();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::getAttributeValue(const QString &key) const
    {
        return transformModelValue(key, getAttributeFromArray(key));
    }

    // TODO candidate for optional const reference, to be able return null value and use reference at the same time silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::getAttributeFromArray(
            const QString &key) const
    {
        // Not found
        if (!m_attributesHash.contains(key))
            return {};

        return m_attributes.at(m_attributesHash.at(key)).value;
    }

    // NOTE api different, doesn't support key = {} silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::getOriginal(
            const QString &key, const QVariant &defaultValue) const
    {
        return Derived::instance().setRawAttributes(m_original, true)
                .getOriginalWithoutRewindingModel(key, defaultValue);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QList<AttributeItem>
    HasAttributes<Derived, AllRelations...>::getOriginals() const
    {
        QList<AttributeItem> originals;
        originals.reserve(m_original.size());

        for (const auto &original : m_original) {
            const auto &key = original.key;

            originals.append({key, transformModelValue(key, original.value)});
        }

        return originals;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::unordered_map<QString,
            typename HasAttributes<Derived, AllRelations...>::AttributesSizeType> &
    HasAttributes<Derived, AllRelations...>::getOriginalsHash() const
    {
        return m_originalHash;
    }

    // NOTE api different silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::getRawOriginal(
            const QString &key, const QVariant &defaultValue) const
    {
        // Found
        if (m_originalHash.contains(key))
            return m_original.at(m_originalHash.at(key)).value;

        // Not found, return the default value
        return defaultValue;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QList<AttributeItem> &
    HasAttributes<Derived, AllRelations...>::getRawOriginals() const
    {
        return m_original;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::unsetAttribute(const AttributeItem &value)
    {
        const auto &key = value.key;

        // Not found
        if (!m_attributesHash.contains(key))
            return model();

        const auto position = m_attributesHash.at(key);

        // FUTURE all the operations on this containers should be synchronized, later, I think that this is not true because connection or model can be used only from a thread where it was created silverqx
        m_attributes.removeAt(position);
        m_attributesHash.erase(key);

        // Rehash attributes, but only attributes which were shifted
        rehashAttributePositions(m_attributes, m_attributesHash, position);

        /* Need to clear the mutators cache because any mutator can depend on this unset
           attribute, so the recomputation will be needed. */
        m_attributeMutatorsCache.clear();
        // It's enough to clear this cache and recompute when needed
        m_modelAttributesCacheForMutators.reset();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::unsetAttribute(const QString &key)
    {
        // Not found
        if (!m_attributesHash.contains(key))
            return model();

        const auto position = m_attributesHash.at(key);

        m_attributes.removeAt(position);
        m_attributesHash.erase(key);

        // Rehash attributes, but only attributes which were shifted
        rehashAttributePositions(m_attributes, m_attributesHash, position);

        /* Need to clear the mutators cache because any mutator can depend on this unset
           attribute, so the recomputation will be needed. */
        m_attributeMutatorsCache.clear();
        // It's enough to clear this cache and recompute when needed
        m_modelAttributesCacheForMutators.reset();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QList<AttributeItem>
    HasAttributes<Derived, AllRelations...>::only(const QStringList &attributes) const
    {
        QList<AttributeItem> result;
        result.reserve(attributes.size());

        for (const auto &attribute : attributes)
            result.append({attribute, getAttribute(attribute)});

        return result;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QList<AttributeItem>
    HasAttributes<Derived, AllRelations...>::only(QStringList &&attributes) const // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        QList<AttributeItem> result;
        result.reserve(attributes.size());

        for (auto &&attribute : attributes) {
            auto value = getAttribute(attribute);
            result.append({std::move(attribute), std::move(value)});
        }

        return result;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QList<AttributeItem>
    HasAttributes<Derived, AllRelations...>::getDirty() const
    {
        const auto &attributes = getAttributes();

        QList<AttributeItem> dirty;
        dirty.reserve(attributes.size());

        for (const auto &attribute : attributes)
            if (const auto &key = attribute.key;
                !originalIsEquivalent(key)
            )
                dirty.append({key, attribute.value});

        return dirty;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unordered_map<QString,
            typename HasAttributes<Derived, AllRelations...>::AttributesSizeType>
    HasAttributes<Derived, AllRelations...>::getDirtyHash() const
    {
        const auto size = m_attributes.size();

        using DirtyHashType = std::unordered_map<QString, AttributesSizeType>;

        DirtyHashType dirtyHash(static_cast<DirtyHashType::size_type>(size));

        for (auto i = 0; i < size; ++i)
            if (const auto &key = m_attributes.at(i).key;
                !originalIsEquivalent(key)
            )
                dirtyHash.try_emplace(m_attributes.at(i).key, i);

        return dirtyHash;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::isDirty(const QStringList &attributes) const
    {
        return hasChanges(getDirtyHash(), attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::isDirty(const QString &attribute) const
    {
        return hasChanges(getDirtyHash(), QStringList {attribute});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::isClean(const QStringList &attributes) const
    {
        return !isDirty(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::isClean(const QString &attribute) const
    {
        return !isDirty(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QList<AttributeItem> &
    HasAttributes<Derived, AllRelations...>::getChanges() const
    {
        return m_changes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::unordered_map<QString,
            typename HasAttributes<Derived, AllRelations...>::AttributesSizeType> &
    HasAttributes<Derived, AllRelations...>::getChangesHash() const
    {
        return m_changesHash;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::wasChanged(
            const QStringList &attributes) const
    {
        return hasChanges(getChangesHash(), attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::wasChanged(const QString &attribute) const
    {
        return hasChanges(getChangesHash(), QStringList {attribute});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    HasAttributes<Derived, AllRelations...>::getDateFormat() const
    {
        const auto &basemodel = this->basemodel();

        if (const auto &userDateFormat = basemodel.getUserDateFormat();
            !userDateFormat.isEmpty()
        ) T_UNLIKELY
            return userDateFormat;

        else T_LIKELY
            return basemodel.getConnection().getQueryGrammar().getDateFormat();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setDateFormat(const QString &format)
    {
        basemodel().getUserDateFormat() = format;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    HasAttributes<Derived, AllRelations...>::getTimeFormat() const
    {
        const auto &basemodel = this->basemodel();

        if (const auto &userTimeFormat = basemodel.getUserTimeFormat();
            !userTimeFormat.isEmpty()
        ) T_UNLIKELY
            return userTimeFormat;

        else T_LIKELY
            return basemodel.getConnection().getQueryGrammar().getTimeFormat();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setTimeFormat(const QString &format)
    {
        basemodel().getUserTimeFormat() = format;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::fromDateTime(const QVariant &value) const
    {
        /* This method is called from the setAttribute() and originalIsEquivalent() and
           is expecting user input. */

        /* The value argument must be the QDateTime type, this is how this method
           is designed. */
        Q_ASSERT(value.isValid() &&
                 (value.canConvert<QDateTime>() || value.canConvert<QDate>() ||
                  value.canConvert<QTime>()     || value.canConvert<qint64>()));

        const auto &format = getDateFormat();

        /* Special logic for the null values, fix a null value on the base of the format
           and the value type. */
        if (value.isNull())
            return nullFor_fromDateTime(value, format);

        // Support Unix timestamps
        if (format == QLatin1Char('U')) T_UNLIKELY
            return asTimestamp(value);

        // Convert a QDateTime, QDate, or QTime to a storable string
        else T_LIKELY
            return fromDateOrDateTimeOrTime(value, format);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::fromDateTime(const QDateTime &value) const
    {
        /* This method is called from the HasTimestamps and SoftDeletes and the given
           value argument will be a result from the HasTimestamps::freshTimestampString().
           So it's used to create/update model timestamps and it's internal, so special
           logic for handling a user input and null values is not needed.
           There is no need to call the convertTimeZone() here because it's already
           converted in the HasTimestamps::freshTimestamp(). */

        /* As this is the internal method a passed value must be valid and also the null
           value will never be passed into. */
        Q_ASSERT(value.isValid() && !value.isNull());
        // Time zone must match the qt_timezone connection config. option
        // Obtain a QTimeZone() instance on the base of the qtTimeZone
        Q_ASSERT(value.timeZone() == basemodel().freshTimestamp().timeZone());

        const auto &format = getDateFormat();

        // Support Unix timestamps
        /* This should be templated and for the Unix timestamps the return type should be
           qint64, but it would make the code more complex because I would have to move
           the getDateFormat() outside, so I will simply return QString, it's not a big
           deal, INSERT/UPDATE clauses with '1604xxx' for the bigint columns are totaly
           ok, instead of 1604xxx as integer type. */
        if (format == QLatin1Char('U')) T_UNLIKELY
            return asTimestamp(value);

        else T_LIKELY
            return value.toString(format);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QStringList
    HasAttributes<Derived, AllRelations...>::getDates() const
    {
        /* The getDates() is also called from the SoftDeletes::initializeSoftDeletes()
           that is called from the Model::constructor() which means that
           the Derived::u_timestamps data member is not yet initialized, so it will be
           everytime false. This can be considered as a bug but it doesn't matter,
           it doesn't affect the logic in any way and the result will be correct.
           The result is that I'm still successfully avoiding the need to add
           a separate Model initialization method, which I'm super happy with 🙌. */

        if (!basemodel().usesTimestamps())
            return Model<Derived, AllRelations...>::getUserDates();

        auto dates = Model<Derived, AllRelations...>::getUserDates() +
                     Model<Derived, AllRelations...>::timestampColumnNames();

        dates.removeDuplicates();

        return dates;
    }

    /* Model::AttributeReference - begin */

    /* private */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasAttributes<Derived, AllRelations...>::AttributeReference::AttributeReference(
            Model<Derived, AllRelations...> &model,
            const QString &attribute // NOLINT(modernize-pass-by-value)
    )
        : m_model(model)
        , m_attribute(attribute)
    {}

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations> // NOLINT(misc-unconventional-assign-operator, cppcoreguidelines-c-copy-assignment-signature)
    const typename HasAttributes<Derived, AllRelations...>::AttributeReference & // NOLINT(misc-unconventional-assign-operator, cppcoreguidelines-c-copy-assignment-signature)
    HasAttributes<Derived, AllRelations...>::AttributeReference::operator=(
            const QVariant &value) const
    {
        m_model.get().setAttribute(m_attribute, value);

        return *this;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations> // NOLINT(misc-unconventional-assign-operator, cppcoreguidelines-c-copy-assignment-signature)
    const typename HasAttributes<Derived, AllRelations...>::AttributeReference & // NOLINT(misc-unconventional-assign-operator, cppcoreguidelines-c-copy-assignment-signature)
    HasAttributes<Derived, AllRelations...>::AttributeReference::operator=(
            const AttributeReference &attributeReference) const
    {
        m_model.get().setAttribute(m_attribute, attributeReference.value());

        return *this;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QVariant *
    HasAttributes<Derived, AllRelations...>::AttributeReference::operator->() const
    {
        /* The reason why m_attributeCache exists and why QVariant * is returned:
           The overload of operator -> must either return a raw pointer, or return
           an object (by reference or by value) for which operator -> is in turn
           overloaded. */
        m_attributeCache = value();

        return &m_attributeCache;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::AttributeReference::value() const
    {
        return m_model.get().getAttribute(m_attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::AttributeReference::operator*() const
    {
        return value();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasAttributes<Derived, AllRelations...>::AttributeReference
                                           ::operator QVariant() const
    {
        return value();
    }

    /* Model::AttributeReference - end */

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasAttributes<Derived, AllRelations...>::AttributeReference
    HasAttributes<Derived, AllRelations...>::operator[](const QString &attribute) &
    {
        return AttributeReference(basemodel(), attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::operator[](const QString &attribute) const &
    {
        return getAttribute(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::operator[](const QString &attribute) &&
    {
        return getAttribute(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::operator[](const QString &attribute) const &&
    {
        return getAttribute(attribute);
    }

    /* Casting Attributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unordered_map<QString, CastItem>
    HasAttributes<Derived, AllRelations...>::getCasts() const
    {
        const auto &basemodel = this->basemodel();

        const auto &keyName = basemodel.getKeyName();

        /* Needed to make a copy because it can interfere with the check
           in the getAttribute() method (getUserCasts().contains(key)), so don't modify
           the user's u_casts and add the 'id' cast on the fly on the casts copy. */
        auto casts = basemodel.getUserCasts();

        // try_emplace implies casts.contains()
        if (basemodel.getIncrementing()/* && !casts.contains(keyName)*/)
            // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
            casts.try_emplace(keyName, CastType::ULongLong);

        return casts;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasAttributes<Derived, AllRelations...>::hasCast(const QString &key) const
    {
        return getCasts().contains(key);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasAttributes<Derived, AllRelations...>::hasCast(
            const QString &key, const std::unordered_set<CastType> &types) const
    {
        return hasCast(key) && types.contains(getCastType(key));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::mergeCasts(
            const std::unordered_map<QString, CastItem> &casts)
    {
        auto &userCasts = basemodel().getUserCasts();

        std::remove_cvref_t<decltype (casts)> mergedCasts;
        mergedCasts.reserve(userCasts.size() + casts.size());

        mergedCasts = userCasts;

        for (const auto &[attribute, castItem] : casts)
            mergedCasts.insert_or_assign(attribute, castItem);

        // Swap user casts
        userCasts = std::move(mergedCasts);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::mergeCasts(
            std::unordered_map<QString, CastItem> &casts)
    {
        basemodel().getUserCasts().merge(casts);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::mergeCasts(
            std::unordered_map<QString, CastItem> &&casts)
    {
        basemodel().getUserCasts().merge(std::move(casts));

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &HasAttributes<Derived, AllRelations...>::resetCasts()
    {
        basemodel().getUserCasts().clear();

        return model();
    }

    /* QDateTime time zone */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QtTimeZoneConfig &
    HasAttributes<Derived, AllRelations...>::getQtTimeZone() const
    {
        // Cache the value
        if (!m_qtTimeZone)
            m_qtTimeZone = basemodel().getConnection().getQtTimeZone();

        return *m_qtTimeZone;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setQtTimeZone(const QVariant &timezone)
    {
        m_qtTimeZone = ConfigUtils::prepareQtTimeZone(timezone,
                                                      basemodel().getConnectionName());

        m_isConvertingTimeZone = m_qtTimeZone->type != QtTimeZoneType::DontConvert;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setQtTimeZone(QtTimeZoneConfig &&timezone)
    {
        m_qtTimeZone = std::move(timezone);

        m_isConvertingTimeZone = m_qtTimeZone->type != QtTimeZoneType::DontConvert;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasAttributes<Derived, AllRelations...>::isConvertingTimeZone() const
    {
        // Cache the value
        if (!m_isConvertingTimeZone)
            m_isConvertingTimeZone = basemodel().getConnection().isConvertingTimeZone();

        return *m_isConvertingTimeZone;
    }

    /* Serialization - Attributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariantMap
    HasAttributes<Derived, AllRelations...>::attributesToMap() const
    {
        auto attributes = getMappableAttributes();

        /* If an attribute is a date, we will cast it to a string after converting it
           to a QDateTime instance. This is so we will get some consistent
           formatting while accessing attributes vs. mapping / JSONing a model. */
        addDateAttributesToMap(attributes);

        /* Next we will handle any casts that have been setup for this model and cast
           the values to their appropriate type. */
        addCastAttributesToMap(attributes);

        /* Here we will grab all of the appended, calculated attributes to this model
           as these attributes are not really in the attributes vector, but are run
           when we need to serialize or JSON the model for convenience to the coder. */
        for (const auto &key : getSerializableAppends())
            attributes.insert(key, mutateAccessorAttribute(key));

        return attributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QList<AttributeItem>
    HasAttributes<Derived, AllRelations...>::attributesToList() const
    {
        auto [attributes, attributesHash] = getVectorableAttributes();

        /* If an attribute is a date, we will cast it to a string after converting it
           to a QDateTime instance. This is so we will get some consistent
           formatting while accessing attributes vs. vectoring / JSONing a model. */
        addDateAttributesToVector(attributes, attributesHash);

        /* Next we will handle any casts that have been setup for this model and cast
           the values to their appropriate type. */
        addCastAttributesToVector(attributes, attributesHash);

        /* Here we will grab all of the appended, calculated attributes to this model
           as these attributes are not really in the attributes vector, but are run
           when we need to serialize or JSON the model for convenience to the coder. */
        for (const auto &key : getSerializableAppends())
            attributes.emplaceBack(key, mutateAccessorAttribute(key));

        return attributes;
    }

    /* Serialization - Appends */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::append(const QString &attribute)
    {
        basemodel().getUserAppends().emplace(attribute);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::append(QString &&attribute)
    {
        basemodel().getUserAppends().emplace(std::move(attribute));

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::append(const std::set<QString> &attributes)
    {
        basemodel().getUserAppends().merge(attributes);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::append(std::set<QString> &&attributes)
    {
        basemodel().getUserAppends().merge(std::move(attributes));

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::hasAppend(const QString &attribute) const
    {
        return basemodel().getUserAppends().contains(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::set<QString> &
    HasAttributes<Derived, AllRelations...>::getAppends() const noexcept
    {
        return basemodel().getUserAppends();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setAppends(
            const std::set<QString> &attributes)
    {
        basemodel().getUserAppends() = attributes;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setAppends(std::set<QString> &&attributes)
    {
        basemodel().getUserAppends() = std::move(attributes);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::clearAppends() noexcept
    {
        basemodel().getUserAppends().clear();

        return model();
    }

    /* protected */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant HasAttributes<Derived, AllRelations...>::transformModelValue(
            const QString &key,
            const QVariant &value) const
    {
        /* Qt's SQLite driver doesn't apply any logic on the returned types, it returns
           them without type detection, and it is logical, because SQLite only supports
           numeric and string types, it doesn't distinguish a datetime type or any other
           types.
           Qt's MySql driver behaves differently, QVariant already contains the QDateTime
           values, because Qt's MySQL/PostgreSQL drivers return a QDateTime when
           the value from the database is datetime, the same is true for all other types,
           Qt's driver detects it and creates a QVariant with proper types. */

        /* Nothing to do, no transformation possible, don't check for value.isNull()!
           (to support a null QVariant-s). */
        if (!value.isValid())
            return value;

        /* If the attribute exists within the u_casts hash, we will convert it to
           an appropriate QVariant type. */
        if (hasCast(key))
            return castAttribute(key, value);

        /* If the attribute is listed as a date, we will convert it to the QDateTime
           or QDate instance on retrieval, which makes it quite convenient to work with
           date fields without having to create a cast for each attribute.
           Also don't return the nullFor_xyz() or NullVariant::QDateTime() here because
           we need to return the null QVariant(QString) for the SQLite database, so
           the logic here is, whatever the QtSql driver returns if the QVariant is null
           we will return too. */
        if (!value.isNull() && getDates().contains(key))
            return asDateOrDateTime(value);

        return value;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::getOriginalWithoutRewindingModel(
            const QString &key, const QVariant &defaultValue) const
    {
        // Found
        if (m_originalHash.contains(key))
            return transformModelValue(key, m_original.at(m_originalHash.at(key)).value);

        // Not found, return the default value
        return defaultValue;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QList<AttributeItem> &
    HasAttributes<Derived, AllRelations...>::getRawAttributes() const
    {
        return m_attributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasAttributes<Derived, AllRelations...>::hasChanges(
            const std::unordered_map<QString, AttributesSizeType> &changes,
            const QStringList &attributes) const
    {
        /* If no specific attributes were provided, we will just see if the dirty hash
           already contains any attributes. If it does we will just return that this
           count is greater than zero. Else, we need to check specific attributes. */
        if (attributes.isEmpty())
            return !changes.empty();

        /* Here we will spin through every attribute and see if this is in the hash of
           dirty attributes. If it is, we will return true and if we make it through
           all of the attributes for the entire vector we will return false at end. */
        return std::ranges::any_of(attributes, [&changes](const auto &attribute)
        {
            return changes.contains(attribute);
        });
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &HasAttributes<Derived, AllRelations...>::syncChanges()
    {
        m_changes = getDirty();

        rehashAttributePositions(m_changes, m_changesHash);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &HasAttributes<Derived, AllRelations...>::syncOriginalAttribute(
            const QString &attribute)
    {
        return syncOriginalAttributes({attribute});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &HasAttributes<Derived, AllRelations...>::syncOriginalAttributes(
            const QStringList &attributes)
    {
        const auto &modelAttributes = getAttributes();
        const auto &modelAttributesHash = getAttributesHash();

        for (const auto &attribute : attributes) {
            // Initialize as late as possible
            static const auto functionName = QStringLiteral(
                                                 "HasAttributes::syncOriginalAttributes");
            throwIfNoAttributeInHash(modelAttributesHash, attribute, functionName);

            const auto &modelAttributeValue =
                    modelAttributes.at(modelAttributesHash.at(attribute)).value;

            // The 'attribute' already exists in the m_original/Hash, update it
            if (m_originalHash.contains(attribute)) {
                const auto attributeIndex = m_originalHash.at(attribute);
                Q_ASSERT(attributeIndex >= 0 && attributeIndex < m_original.size());

                m_original[attributeIndex].value = modelAttributeValue;
            }
            /* The 'attribute' doesn't exist in the m_original/Hash, so create it and
               rehash m_originalHash, but only from the added position. */
            else {
                /* It should never happen that a model will have 0 attributes because
                   it must contain at least the ID attribute, but if this happen and
                   the rehashFrom index would be -1, then rehash from 0 in this case.
                   <AttributesSizeType> needed to avoid ambigous std::max() overload. */
                const auto rehashFrom = std::max<AttributesSizeType>(
                                            modelAttributes.size() - 1, 0);

                m_original.append({attribute, modelAttributeValue});

                rehashAttributePositions(m_original, m_originalHash, rehashFrom);
            }
        }

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::originalIsEquivalent(
            const QString &key) const
    {
        if (!m_originalHash.contains(key))
            return false;

        const auto attribute = getAttributeFromArray(key);
        const auto original = getRawOriginal(key);

        // Takes into account also milliseconds for the QDateTime attribute
        if (attribute == original)
            return true;

        // TODO next solve how to work with null values and what to do with invalid/unknown values silverqx
        if (!attribute.isValid() || attribute.isNull())
            return false;

        /* This check ignores milliseconds for QDateTime or QTime attributes with
           the default u_date/timeFormat static data member value, so it depends how this
           format is set. Milliseconds aren't ignored if the format contains them,
           eg. ...:ss.zzz. */
        if (isDateAttribute(key) || isCustomDateCastable(key))
            return fromDateTime(attribute) == fromDateTime(original);

        if (hasCast(key, {CastType::Real, CastType::Float, CastType::Double})) {
            if (!original.isValid() || original.isNull())
                return false;

            return std::abs(castAttribute(key, attribute).template value<double>() -
                            castAttribute(key, original).template value<double>()) <
                    // * 4 is still very high precision whether two numbers are the same
                    std::numeric_limits<double>::epsilon() * 4;
        }

        // FEATURE castable, update this if I will support eg. class casts, following check is only for primitive types, but there can be also another cast type like one above for real types silverqx
        if (hasCast(key))
            return castAttribute(key, attribute) == castAttribute(key, original);

        return false;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::rehashAttributePositions(
            const QList<AttributeItem> &attributes,
            std::unordered_map<QString, AttributesSizeType> &attributesHash,
            const AttributesSizeType from)
    {
        /* This member function is universal and can be used for m_attributes,
           m_changes and m_original and it associated unordered_maps m_attributesHash,
           m_changesHash and m_originalHash. */
        for (auto i = from; i < attributes.size(); ++i)
            // 'i' is the position index
            attributesHash[attributes.at(i).key] = i;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unordered_map<QString, typename HasAttributes<Derived, AllRelations...>::
                                         AttributesSizeType>
    HasAttributes<Derived, AllRelations...>::rehashAttributePositions(
            const QList<AttributeItem> &attributes, const AttributesSizeType from)
    {
        std::unordered_map<QString, AttributesSizeType> attributesHash;
        attributesHash.reserve(static_cast<decltype (attributesHash)::size_type>(
                                   attributes.size()));

        rehashAttributePositions(attributes, attributesHash, from);

        return attributesHash;
    }

    /* Datetime-related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::isDateAttribute(const QString &key) const
    {
        return getDates().contains(key) || isDateCastable(key);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QDateTime
    HasAttributes<Derived, AllRelations...>::asDateTime(const QVariant &value) const
    {
        /* This can never happen, null values must be handled outside of the asDateTime,
           asDate, and asTimestamp methods. */
        Q_ASSERT_X(!value.isNull(),
                   "HasAttributes::asDateTime",
                   "Null values must be handled outside of the asDateTime, asDate, "
                   "and asTimestamp methods.");

        /* If this value is already a QDateTime instance, we shall just return it as is.
           This prevents us having to re-parse a QDateTime instance when we know
           it already is one. */
        if (value.typeId() == QMetaType::QDateTime)
            return convertTimeZone(value.value<QDateTime>());

        // The value has to be convertible to the QString so we can work with it
        if (!value.canConvert<QString>())
            throw Orm::Exceptions::InvalidFormatError(
                        QStringLiteral("Could not parse the datetime, could not convert "
                                       "the 'value' to the QString in %1().")
                        .arg(__tiny_func__));

        const auto valueString = value.value<QString>();

        /* If this value is an integer, we will assume it is a Unix timestamp's value
           and format a QDateTime object from this timestamp. This allows flexibility
           when defining your date fields as they might be Unix timestamps here. */
        if (StringUtils::isNumber(valueString))
            // TODO switch ms accuracy? For the u_dateFormat too? silverqx
            if (auto unixTimestamp = QDateTime::fromSecsSinceEpoch(value.value<qint64>());
                unixTimestamp.isValid()
            )
                return convertTimeZone(std::move(unixTimestamp));

        /* If the value is in simply year, month, day format, we will instantiate the
           QDate instances from that format. Again, this provides for simple date
           fields on the database, while still supporting QDateTime conversion. */
        if (Helpers::isStandardDateFormat(valueString))
            if (auto dateSimple = QDateTime::fromString(valueString,
                                                        QStringLiteral("yyyy-M-d"));
                dateSimple.isValid()
            )
                return setTimeZone(dateSimple);

        const auto &format = getDateFormat();

        /* Finally, we will just assume this date is in the format used by default on
           the database connection and use that format to create the QDateTime object
           that is returned back out to the developers after we convert it here. */
        if (auto datetime = QDateTime::fromString(valueString, format);
            datetime.isValid()
        )
            return setTimeZone(datetime);

        /* QDateTime doesn't offer any advanced parsing method that can guess and
           instantiate the QDateTime from many formats, a function like that would be
           ideal here. */

        // But at least we can detect the ISO DateTime-s
        if (auto dateIso = QDateTime::fromString(valueString, Qt::ISODateWithMs);
            dateIso.isValid()
        )
            return convertTimeZone(std::move(dateIso));

        throw Orm::Exceptions::InvalidFormatError(
                    QStringLiteral("Could not parse the datetime '%1' using "
                                   "the given format '%2' in %3().")
                    .arg(valueString, format, __tiny_func__));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QDate
    HasAttributes<Derived, AllRelations...>::asDate(const QVariant &value) const
    {
        return asDateTime(value).date();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QTime
    HasAttributes<Derived, AllRelations...>::asTime(const QVariant &value) const
    {
        /* This can never happen, null values must be handled outside of the asTime()
           method. */
        Q_ASSERT_X(!value.isNull(),
                   "HasAttributes::asTime",
                   "null values must be handled outside of the asTime() methods.");

        /* If this value is already a QTime instance, we shall just return it as is.
           This prevents us having to re-parse a QTime instance when we know
           it already is one. */
        if (value.typeId() == QMetaType::QTime)
            return value.template value<QTime>();

        // The value has to be convertible to the QString so we can work with it
        if (!value.canConvert<QString>())
            throw Orm::Exceptions::InvalidFormatError(
                        QStringLiteral("Could not parse the time, could not convert "
                                       "the 'value' to the QString in %1().")
                        .arg(__tiny_func__));

        const auto valueString = value.value<QString>();

        /* Finally, we will just assume this time is in the format used by default on
           the database connection and use that format to create the QTime object
           that is returned back out to the developers after we convert it here. */
        if (auto time = QTime::fromString(valueString, getTimeFormat());
            time.isValid()
        )
            return time;

        /* QTime doesn't offer any advanced parsing method that can guess and instantiate
           the QTime from many formats, a function like that would be ideal here. */

        // But at least we can detect the ISO Time-s
        if (auto timeIso = QTime::fromString(valueString, Qt::ISODateWithMs);
            timeIso.isValid()
        )
            return timeIso;

        throw Orm::Exceptions::InvalidFormatError(
                    QStringLiteral("Could not parse the time '%1' using "
                                   "the Qt::ISODateWithMs format in %2().")
                    .arg(valueString, __tiny_func__));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::asDateOrDateTime(
            const QVariant &value) const
    {
        // This method is used only for u_dates so no QTime handling is applied

        const auto typeId = value.typeId();

        if (typeId == QMetaType::QDate ||
            (typeId == QMetaType::QString &&
             Helpers::isStandardDateFormat(value.value<QString>()))
        ) T_UNLIKELY
            return asDate(value);

        else T_LIKELY
            return asDateTime(value);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::asDateOrDateTimeOrTime(
            const QVariant &value) const
    {
        const auto typeId = value.typeId();

        if (typeId == QMetaType::QDate ||
            (typeId == QMetaType::QString &&
             Helpers::isStandardDateFormat(value.value<QString>()))
        ) T_UNLIKELY
            return asDate(value);

        else if (typeId == QMetaType::QTime) T_UNLIKELY
            return asTime(value);

        else T_LIKELY
            return asDateTime(value);

        /* I don't want the if (QMetaType::QDateTime) check and the Q_UNREACHABLE() here
           because the NullVariant::QDateTime() must be returned in all other cases. */
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::fromDateOrDateTimeOrTime(
            const QVariant &value, const QString &format) const
    {
        if (const auto typeId = value.typeId();
            typeId == QMetaType::QDate ||
            (typeId == QMetaType::QString &&
             Helpers::isStandardDateFormat(value.value<QString>()))
        ) T_UNLIKELY
            return asDate(value).toString(Qt::ISODate);

        else if(typeId == QMetaType::QTime) T_UNLIKELY
            return asTime(value).toString(getTimeFormat());

        else T_LIKELY
            return asDateTime(value).toString(format);

        /* I don't want the if (QMetaType::QDateTime) check and the Q_UNREACHABLE() here
           because the NullVariant::QDateTime() must be returned in all other cases. */
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    qint64
    HasAttributes<Derived, AllRelations...>::asTimestamp(const QVariant &value) const
    {
        return asDateTime(value).toSecsSinceEpoch();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::nullFor_fromDateTime(
            const QVariant &value, const QString &format)
    {
        const auto typeId = value.typeId();

        if (format == QLatin1Char('U')) T_UNLIKELY
            return NullVariant::LongLong();

        else if (typeId == QMetaType::QDate) T_UNLIKELY
            return NullVariant::QDate();

        else if (typeId == QMetaType::QTime) T_UNLIKELY
            return NullVariant::QTime();

        else T_LIKELY
            return NullVariant::QDateTime();

        /* I don't want the if (QMetaType::QDateTime) check and the Q_UNREACHABLE() here
           because the NullVariant::QDateTime() must be returned in all other cases. */
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::nullFor_addCastAttributesTo(
            const QVariant &value)
    {
        const auto typeId = value.typeId();

        if (typeId == QMetaType::QDate ||
            (typeId == QMetaType::QString &&
             Helpers::isStandardDateFormat(value.value<QString>()))
        ) T_UNLIKELY
            return NullVariant::QDate();

        else if (typeId == QMetaType::QTime) T_UNLIKELY
            return NullVariant::QTime();

        else T_LIKELY
            return NullVariant::QDateTime();

        /* I don't want the if (QMetaType::QDateTime) check and the Q_UNREACHABLE() here
           because the NullVariant::QDateTime() must be returned in all other cases. */
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QDateTime
    HasAttributes<Derived, AllRelations...>::convertTimeZone(QDateTime &&datetime) const
    {
        if (!isConvertingTimeZone())
            return std::move(datetime);

        return Helpers::convertTimeZone(datetime, getQtTimeZone());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QDateTime &
    HasAttributes<Derived, AllRelations...>::setTimeZone(QDateTime &datetime) const
    {
        if (!isConvertingTimeZone())
            return datetime;

        return Helpers::setTimeZone(datetime, getQtTimeZone());
    }

    /* Casting Attributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::castAttribute(
            const QString &key, const QVariant &value) const
    {
        /* Nothing to do, no cast possible, don't check for value.isNull()! (to support
           a null QVariant-s).
           Don't convert/cast invalid QVariant because it changes validity from false
           to true and sets a QVariant to null, so no cast can't happen. */
        if (!value.isValid())
            return value;

        auto value_ = value;
        const auto &castItem = getCastItem(key);
        const auto castType = castItem.type();

        /*! Convert the QVariant value of a attribute. */
        const auto convertAttribute = [&key, &value_, castType]
                                      (const QMetaType metaType)
        {
            // Throw if the given attribute can not be converted to the given cast type
            throwIfCanNotCastAttribute(key, castType, metaType, value_,
                                       QLatin1String("HasAttributes::castAttribute"));

#ifdef TINYORM_DEBUG
            /* Still check for the false value and log to the debug stream, but not if
               the value_ is null, because converting null QVariant will always return
               false and the QVariant type will be changed anyway. */
            if (!value_.convert(metaType) && !value_.isNull())
                // Log if the QVariant::convert() for the given attribute failed
                logIfConvertAttributeFailed(
                            key, castType, metaType,
                            QLatin1String("HasAttributes::castAttribute"));
#else
            value_.convert(metaType);
#endif

            return value_;
        };

        switch (castType) {
        // Int 64-bit
        case CastType::LongLong:
            return convertAttribute(QMetaType(QMetaType::LongLong));
        case CastType::ULongLong:
            return convertAttribute(QMetaType(QMetaType::ULongLong));
        // QString
        case CastType::QString:
            return convertAttribute(QMetaType(QMetaType::QString));
        // Int 32-bit
        case CastType::Int:
        case CastType::Integer:
            return convertAttribute(QMetaType(QMetaType::Int));
        case CastType::UInt:
        case CastType::UInteger:
            return convertAttribute(QMetaType(QMetaType::UInt));
        // QDateTime
        // DateTime-related values need spacial null handling
        case CastType::QDate:
        case CastType::CustomQDate:
            return value_.isNull() ? NullVariant::QDate()
                                   : asDate(value_);
        case CastType::QDateTime:
        case CastType::CustomQDateTime:
            return value_.isNull() ? NullVariant::QDateTime()
                                   : asDateTime(value_);
        case CastType::QTime:
        case CastType::CustomQTime:
            return value_.isNull() ? NullVariant::QTime()
                                   : asTime(value_);
        case CastType::Timestamp:
            return value_.isNull() ? NullVariant::LongLong()
                                   : asTimestamp(value_);
        // Bool
        case CastType::Bool:
        case CastType::Boolean:
            return convertAttribute(QMetaType(QMetaType::Bool));
        // Int 16-bit
        case CastType::Short:
            return convertAttribute(QMetaType(QMetaType::Short));
        case CastType::UShort:
            return convertAttribute(QMetaType(QMetaType::UShort));
        // Float
        case CastType::Real:
        case CastType::Float:
        case CastType::Double:
            /* The fromFloat() not needed here because this logic is handled in the
               TSqlDriver and currently only the PostgreSQL supports NaN and Infinity
               values. The MySQL and SQLite doesn't care. */
            return convertAttribute(QMetaType(QMetaType::Double));

        case CastType::Decimal:
        {
            auto converted = convertAttribute(QMetaType(QMetaType::Double));
            const auto &modifier = castItem.modifier();

            // The isNull() internally also checks if isValid() (from Qt6)
            if (modifier.isNull() || converted.isNull())
                return converted;

            return roundDecimals(converted, modifier);
        }
        // Others
        case CastType::QByteArray:
            return convertAttribute(QMetaType(QMetaType::QByteArray));

        default:
            // Don't throw here, just return invalid QVariant for Release builds
            Q_UNREACHABLE();
        }

        return {};
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    CastItem
    HasAttributes<Derived, AllRelations...>::getCastItem(const QString &key) const
    {
        return getCasts().at(key);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    CastType
    HasAttributes<Derived, AllRelations...>::getCastType(const QString &key) const
    {
        return getCasts().at(key).type();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::isDateCastable(const QString &key) const
    {
        return hasCast(key, {CastType::QDate, CastType::QDateTime, CastType::QTime});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::isCustomDateCastable(
            const QString &key) const
    {
        return hasCast(key, {CastType::CustomQDate, CastType::CustomQDateTime,
                             CastType::CustomQTime});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    HasAttributes<Derived, AllRelations...>::isDateCastType(const CastType type)
    {
        static const std::unordered_set<CastType> dateCastTypes {
            CastType::QDate,
            CastType::QDateTime,
            CastType::QTime,
        };

        return dateCastTypes.contains(type);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasAttributes<Derived, AllRelations...>::isCustomDateCastType(
            const CastItem &castItem)
    {
        static const std::unordered_set<CastType> customDateCastTypes {
            CastType::CustomQDate,
            CastType::CustomQDateTime,
            CastType::CustomQTime,
        };

        const auto &modifier = castItem.modifier();

        /* No need to check for the QVariant isNull and isValid for modifier,
           the canConvert and isEmpty checks do the same thing. */
        return customDateCastTypes.contains(castItem.type()) &&
               modifier.template canConvert<QString>() &&
               !modifier.template value<QString>().isEmpty();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::roundDecimals(const QVariant &value,
                                                           const QVariant &decimals)
    {
        /* All parameters and the return value are the QVariant to simplify and
           for outsource the code from the castAttribute() method, this method is
           designed especially for the castAttribute() method. */
        const auto multiplier = std::pow(10.0, decimals.template value<int>());

        // No cast need, it always returns double because of value<double>()
        return std::round(value.template value<double>() * multiplier) / multiplier;
    }

    /* Serialization - Attributes */

    /* u_dates vs u_casts vs serializeDate/Time()
       If an attribute is in the u_dates and isn't in the u_casts it will be serialized
       using the serializeDatetime(). If it is in the u_casts then it will be serialized
       using the serializeDateOrDateTimeOrTime(). These two methods allow a user to
       override them in the model class.
       If it is defined as the CustomQDate/Time in the u_casts then
       the serializeDateOrDateTimeOrTime() will not be used/called and the custom format
       modifier will be applied instead using the toString(format).
       Also, asDate/Time() method is called before every serialization that converts
       this string date/time attribute to the QDate/Time using the u_date/timeFormat
       using the fromString() and then is applied the logic described above what is
       calling the serializeDateOrDateTimeOrTime() or toString(format) with the custom
       format modifier. */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariantMap
    HasAttributes<Derived, AllRelations...>::getMappableAttributes() const
    {
        const auto &basemodel = this->basemodel();

        return getSerializableAttributes<QVariantMap>(
                    getAttributes(), basemodel.getUserVisible(),
                    basemodel.getUserHidden(), basemodel.getUserAppends());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasAttributes<Derived, AllRelations...>::VectorableAttributes
    HasAttributes<Derived, AllRelations...>::getVectorableAttributes() const
    {
        const auto &basemodel = this->basemodel();
        // Obtain these here and pass down to avoid double or triple obtaining later
        const auto &visible = basemodel.getUserVisible();
        const auto &hidden  = basemodel.getUserHidden();
        const auto &appends = basemodel.getUserAppends();

        auto attributes = getSerializableAttributes<QList<AttributeItem>>(
                              getAttributes(), visible, hidden, appends);

        // Nothing to do
        if (attributes.empty())
            return {};

        /* We need to remove attributes that have the same names as u_appends, to avoid
           having two attributes with the same name. This logic is only needed
           for the vectorable attributes, the mappable attributes, of course, don't need
           this type of logic. */
        removeAppendsFromVectorableAttributes(attributes, visible.empty(), hidden,
                                              appends);

        /* We need to recompute the attributesHash because of visible/hidden,
           it wouldn't be necessary to recompute it if visible/hidden attributes are
           empty but the problem is a reference. I'm not going to do a static variable
           trick for this reference. */
        return {attributes, rehashAttributePositions(attributes)};
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SerializedAttributes C>
    C HasAttributes<Derived, AllRelations...>::getSerializableAttributes(
            const QList<AttributeItem> &attributes, const std::set<QString> &visible,
            const std::set<QString> &hidden, const std::set<QString> &appends)
    {
        // Nothing to do
        if (attributes.empty())
            return {};

        const auto isVisibleEmpty = visible.empty();

        // Nothing to do, the visible and hidden attributes are not defined
        if (isVisibleEmpty && hidden.empty()) {
            if constexpr (std::is_same_v<C, QVariantMap>)
                return AttributeUtils::convertVectorToMap(attributes);

            else // QList<AttributeItem>
                return attributes;
        }

        // Pass the visible and hidden down to avoid obtaining these references twice

        // Compute the hidden attributes only (to serialize) as the visible set is empty
        // No need to compute the visible attributes (also allows forwarding reference)
        if (isVisibleEmpty) {
            if constexpr (std::is_same_v<C, QVariantMap>)
                return removeSerializableHiddenAttributes(
                            AttributeUtils::convertVectorToMap(attributes), hidden);

            else // QList<AttributeItem>
                return removeSerializableHiddenAttributes(attributes, hidden);
        }

        // Compute both visible and also hidden attributes to serialize
        return removeSerializableHiddenAttributes(
                    getSerializableVisibleAttributes<C>(attributes, visible, appends),
                    hidden);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::addDateAttributesToMap(
            QVariantMap &attributes) const
    {
        // Nothing to do
        if (attributes.empty())
            return;

        for (auto &&key : getDates()) {
            // NOTE api different, Eloquent is doing a double cast silverqx
            /* Nothing to do, this attribute is not set OR it has set the cast
               to the QDate, QDateTime, or QTime, in this case, skip the serialization
               to avoid useless double serialization. */
            if (!attributes.contains(key) || isDateCastable(key) ||
                isCustomDateCastable(key)
            )
                continue;

            auto &value = attributes[key];

            value = value.isNull() ? NullVariant::QDateTime()
                                   : Model<Derived, AllRelations...>::
                                     getUserSerializeDateTime(asDateTime(value));
        }
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::addDateAttributesToVector(
            QList<AttributeItem> &attributes,
            const std::unordered_map<QString, AttributesSizeType> &attributesHash) const
    {
        // Nothing to do
        if (attributes.empty())
            return;

        for (auto &&key : getDates()) {
            // NOTE api different, Eloquent is doing a double cast silverqx
            /* Nothing to do, this attribute is not set OR it has set the cast
               to the QDate, QDateTime, or QTime, in this case, skip the serialization
               to avoid useless double serialization. */
            if (!attributesHash.contains(key) || isDateCastable(key) ||
                isCustomDateCastable(key)
            )
                continue;

            auto &value = attributes[attributesHash.at(key)].value;

            value = value.isNull() ? NullVariant::QDateTime()
                                   : Model<Derived, AllRelations...>::
                                     getUserSerializeDateTime(asDateTime(value));
        }
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::addCastAttributesToMap(
            QVariantMap &attributes) const
    {
        // Nothing to do
        if (attributes.empty())
            return;

        for (auto &&[key, castItem] : getCasts()) {
            // Nothing to do, this attribute is not set
            if (!attributes.contains(key))
                continue;

            /* Here we will cast the attribute. Then, if the cast is a QDate, QDateTime,
               or QTime cast then we will serialize the date for the map. This will
               convert the dates to strings based on the date format specified
               for these TinyORM models. */
            auto &value = attributes[key];

            castAttributeForSerialization(value, key, castItem);
        }
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::addCastAttributesToVector(
            QList<AttributeItem> &attributes,
            const std::unordered_map<QString, AttributesSizeType> &attributesHash) const
    {
        // Nothing to do
        if (attributes.empty())
            return;

        for (auto &&[key, castItem] : getCasts()) {
            // Nothing to do, this attribute is not set
            if (!attributesHash.contains(key))
                continue;

            /* Here we will cast the attribute. Then, if the cast is a QDate, QDateTime,
               or QTime cast then we will serialize the date for the vector. This will
               convert the dates to strings based on the date format specified
               for these TinyORM models. */
            auto &value = attributes[attributesHash.at(key)].value;

            castAttributeForSerialization(value, key, castItem);
        }
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::set<QString>
    HasAttributes<Derived, AllRelations...>::getSerializableAppends() const
    {
        const auto &basemodel = this->basemodel();
        const auto &visible   = basemodel.getUserVisible();
        const auto &hidden    = basemodel.getUserHidden();
        const auto &appends   = basemodel.getUserAppends();

        // Nothing to do, no u_appends defined
        if (appends.empty())
            return {};

        // Nothing to do, the visible and hidden attributes are not defined
        if (visible.empty() && hidden.empty())
            return appends;

        // Pass the visible and hidden down to avoid obtaining these references twice
        return removeSerializableHiddenAppends(
                    getSerializableVisibleAppends(appends, visible),
                    hidden);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::mutateAccessorAttribute(
            const QString &key) const
    {
        const auto &userMutators = Model<Derived, AllRelations...>::getUserMutators();

        // Throw exception if a mutator mapping is not defined
        validateUserMutator(key, userMutators);

        // Return the cached get mutator
        if (m_attributeMutatorsCache.contains(key))
            // std::as_const() to prevent detach as it's mutable
            return std::as_const(m_attributeMutatorsCache).find(key).value();

        /* Get an accessor callback from the u_mutators map by the given attribute key,
           and invoke it to obtain an attribute. */
        Attribute attribute = std::invoke(userMutators.find(key).value(), model());
        // Get an accessor (get) value from the given attribute
        auto value = getAccessorValueFrom(attribute);

        // Handle datetime-s returned from accessor
        serializeDateOrDateTimeForAccessors(value);

        // Cache the get mutator (accessor) value
        if (attribute.withCaching()) T_UNLIKELY
            m_attributeMutatorsCache.emplace(key, value);

        // Remove the get mutator (accessor) from the cache if caching is disabled
        else T_LIKELY
            m_attributeMutatorsCache.remove(key);

        return value;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    HasAttributes<Derived, AllRelations...>::serializeDateOrDateTimeOrTime(
            const QVariant &value)
    {
        const auto typeId = value.typeId();

        if (typeId == QMetaType::QDate ||
            (typeId == QMetaType::QString &&
             Helpers::isStandardDateFormat(value.value<QString>()))
        ) T_UNLIKELY
            return Model<Derived, AllRelations...>::
                   getUserSerializeDate(value.template value<QDate>());

        else if (typeId == QMetaType::QTime) T_UNLIKELY
            return Model<Derived, AllRelations...>::
                   getUserSerializeTime(value.template value<QTime>());

        else T_LIKELY
            return Model<Derived, AllRelations...>::
                   getUserSerializeDateTime(value.template value<QDateTime>());

        /* I don't want the if (QMetaType::QDateTime) check and the Q_UNREACHABLE() here
           because the NullVariant::QDateTime() must be returned in all other cases. */
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    HasAttributes<Derived, AllRelations...>::serializeDate(const QDate date)
    {
        return date.toString(Qt::ISODate);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    HasAttributes<Derived, AllRelations...>::serializeDateTime(const QDateTime &datetime)
    {
        return datetime.toUTC().toString(Qt::ISODateWithMs); // Default is with fractional seconds (ms)
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    HasAttributes<Derived, AllRelations...>::serializeTime(const QTime time)
    {
        return time.toString(Qt::ISODateWithMs); // Default is with fractional seconds (ms)
    }

    /* private */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::throwIfNoAttributeInHash(
            const std::unordered_map<QString, AttributesSizeType> &attributesHash,
            const QString &attribute, const QString &functionName)
    {
        if (attributesHash.contains(attribute))
            return;

        throw Orm::Exceptions::InvalidArgumentError(
                QStringLiteral("The '%1' attribute doesn't exist in the '%2' "
                               "model's m_attributes vector in %3().")
                .arg(attribute, TypeUtils::classPureBasename<Derived>(),
                     functionName));
    }

    /* Casting Attributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::throwIfCanNotCastAttribute(
            const QString &key, const CastType castType, const QMetaType metaType,
            const QVariant &value, const QString &functionName)
    {
        if (value.canConvert(metaType))
            return;

        throw Orm::Exceptions::InvalidArgumentError(
                    QStringLiteral(
                        "Bad cast type was defined in the %1::u_casts hash, the '%2' "
                        "attribute can not be cast to the 'CastType::%3' "
                        "(using the QMetaType::%4) in %5().")
                    .arg(TypeUtils::template classPureBasename<Derived>(), key,
                         castTypeName(castType), metaType.name(), functionName));
    }

#ifdef TINYORM_DEBUG
    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::logIfConvertAttributeFailed(
            const QString &key, const CastType castType, const QMetaType metaType,
            const QString &functionName)
    {
        /* This should not happen because the QVariant::canConvert() is called before
           the QVariant::convert(), but the convert() return value is still checked
           for the false value and in this case this method will be called. */
        qDebug().noquote()
                << QStringLiteral(
                        "The QVariant::convert() to the 'QMetaType::%1' for the '%2' "
                        "attribute returned 'false', defined cast type on the "
                        "'%3::u_casts' model was 'CastType::%4' in %5().")
                    .arg(metaType.name(), key,
                         TypeUtils::template classPureBasename<Derived>(),
                         castTypeName(castType), functionName);
    }
#endif

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    HasAttributes<Derived, AllRelations...>::castTypeName(const CastType type)
    {
        switch (type) {
        // Int 64-bit
        case CastType::LongLong:
            return QStringLiteral("LongLong");
        case CastType::ULongLong:
            return QStringLiteral("ULongLong");

        // QString
        case CastType::QString:
            return QStringLiteral("QString");

        // Int 32-bit
        case CastType::Int:
        case CastType::Integer:
            return QStringLiteral("Integer");
        case CastType::UInt:
        case CastType::UInteger:
            return QStringLiteral("UInteger");

        // QDateTime
        case CastType::QDate:
            return QStringLiteral("QDate");
        case CastType::QDateTime:
            return QStringLiteral("QDateTime");
        case CastType::QTime:
            return QStringLiteral("QTime");
        case CastType::Timestamp:
            return QStringLiteral("Timestamp");

        // Bool
        case CastType::Bool:
        case CastType::Boolean:
            return QStringLiteral("Boolean");

        // Int 16-bit
        case CastType::Short:
            return QStringLiteral("Short");
        case CastType::UShort:
            return QStringLiteral("UShort");

        // Float
        case CastType::Real:
            return QStringLiteral("Real");
        case CastType::Float:
            return QStringLiteral("Float");
        case CastType::Double:
            return QStringLiteral("Double");
        case CastType::Decimal:
            return QStringLiteral("Decimal");

        // Others
        case CastType::QByteArray:
            return QStringLiteral("QByteArray");
        default:
            Q_UNREACHABLE();
        }

        return QStringLiteral("<Unknown>");
    }

    /* Serialization */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::removeAppendsFromVectorableAttributes(
            QList<AttributeItem> &attributes, const bool isVisibleEmpty,
            const std::set<QString> &hidden, const std::set<QString> &appends)
    {
        /* Ok, this method is a little sketchy so I have to comment on this logic,
           the logic is all about to boost performance a little. Also, this logic
           corrupted almost the perfect code, but that is ok. */

        /* Nothing to do, if the u_visible set contains attribute names then all
           of the below is handled or prevented by the !u_appends.contains(key)
           in the getSerializableVisibleAttributes(). */
        if (!isVisibleEmpty)
            return;

        const auto attributeKeys = AttributeUtils::keys(attributes);

        // Get the u_appends attributes only
        /* Compute append keys on attributes vector, the intersection is needed
           to compute only keys that really exists. */
        std::set<QString> appendKeysTmp;
        ranges::set_intersection(attributeKeys, appends,
                                 ranges::inserter(appendKeysTmp, appendKeysTmp.cend()));

        /* Nothing to do, no u_appends keys left, this typically happens when
           the u_visible set is empty and the u_hidden set contains all of the u_appends
           keys. */
        if (appendKeysTmp.empty())
            return;

        // Remove hidden appends
        std::set<QString> appendKeys;
        ranges::set_difference(appendKeysTmp, hidden,
                               ranges::inserter(appendKeys, appendKeys.cend()));

        // Nothing to do
        if (appendKeys.empty())
            return;

        attributes.removeIf([&appendKeys](const AttributeItem &attribute)
        {
            return appendKeys.contains(attribute.key);
        });
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::castAttributeForSerialization(
            QVariant &value, const QString &key, const CastItem &castItem) const
    {
        value = castAttribute(key, value);

        /* If the attribute cast was a QDate, QDateTime, or QTime, we will serialize
           the date as a string. This allows the developers to customize how dates are
           serialized into a map without affecting how they are persisted
           into the storage. */
        if (isDateCastType(castItem.type()))
            value = value.isNull() ? nullFor_addCastAttributesTo(value)
                                   : serializeDateOrDateTimeOrTime(
                                         asDateOrDateTimeOrTime(value));

        if (isCustomDateCastType(castItem)) {
            const auto castModifier = castItem.modifier().template value<QString>();

            // Support Unix timestamps
            if (castModifier == QLatin1Char('U')) {
                value = asTimestamp(value);
                return;
            }

            const auto castedDate = asDateOrDateTimeOrTime(value);

            if (const auto typeId = castedDate.typeId();
                typeId == QMetaType::QDate
            ) T_UNLIKELY
                value = castedDate.template value<QDate>().toString(castModifier);

            else if (typeId == QMetaType::QTime) T_UNLIKELY
                value = castedDate.template value<QTime>().toString(castModifier);

            else T_LIKELY
                value = castedDate.template value<QDateTime>().toString(castModifier);
        }
    }

    /* Serialization - HidesAttributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SerializedAttributes C>
    C HasAttributes<Derived, AllRelations...>::getSerializableVisibleAttributes(
            const QList<AttributeItem> &attributes, const std::set<QString> &visible,
            const std::set<QString> &appends)
    {
        // Get visible attributes only
        /* Compute visible keys on attributes map/vector, the intersection is needed
           to compute only keys that really exists. */
        std::set<QString> visibleKeys;
        ranges::set_intersection(AttributeUtils::keys(attributes), visible,
                                 ranges::inserter(visibleKeys, visibleKeys.cend()));

        C serializableAttributes;
        if constexpr (HasReserveMethod<C>)
            serializableAttributes.reserve(attributes.size());

        for (const auto &[key, value] : attributes)
            // Skip the keys that are in the u_appends, they will be added later
            if (visibleKeys.contains(key) && !appends.contains(key)) {
                if constexpr (std::is_same_v<C, QVariantMap>)
                    serializableAttributes.insert(key, value);

                else // QList<AttributeItem>
                    serializableAttributes.emplaceBack(key, value);
            }

        return serializableAttributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariantMap
    HasAttributes<Derived, AllRelations...>::removeSerializableHiddenAttributes(
            QVariantMap &&attributes, const std::set<QString> &hidden)
    {
        // Nothing to do
        if (hidden.empty())
            return std::move(attributes);

        /* Remove hidden attributes, from the map container returned by
           the getSerializableVisibleAttributes()! */
        /* Compute hidden keys on attributes map, the intersection is needed to compute
           only keys that really exists. */
        std::set<QString> hiddenKeys;
        ranges::set_intersection(AttributeUtils::keys(attributes), hidden,
                                 ranges::inserter(hiddenKeys, hiddenKeys.cend()));

        QVariantMap serializableAttributes;

        for (auto it = attributes.constBegin();
             it != attributes.constEnd(); ++it
        )
            if (const auto &key = it.key();
                !hiddenKeys.contains(key)
            )
                serializableAttributes.insert(key, it.value());

        return serializableAttributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename T> requires AttributesContainerConcept<T>
    QList<AttributeItem>
    HasAttributes<Derived, AllRelations...>::removeSerializableHiddenAttributes(
            T &&attributes, const std::set<QString> &hidden)
    {
        // Nothing to do
        if (hidden.empty())
            return std::forward<T>(attributes);

        /* Remove hidden attributes, from the vector container returned by
           the getSerializableVisibleAttributes()! */
        /* Compute hidden keys on attributes vector, the intersection is needed
           to compute only keys that really exists. */
        std::set<QString> hiddenKeys;
        ranges::set_intersection(AttributeUtils::keys(attributes), hidden,
                                 ranges::inserter(hiddenKeys, hiddenKeys.cend()));

        QList<AttributeItem> serializableAttributes;
        serializableAttributes.reserve(attributes.size());

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 26800)
#endif
        for (auto &&[key, value] : attributes)
            if (!hiddenKeys.contains(key))
                serializableAttributes.emplaceBack(std::forward<decltype (key)>(key),
                                                   std::forward<decltype (value)>(value));
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

        return serializableAttributes;
    }

    /* Serialization - Appends */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::set<QString>
    HasAttributes<Derived, AllRelations...>::getSerializableVisibleAppends(
            const std::set<QString> &appends, const std::set<QString> &visible)
    {
        // Nothing to do
        if (visible.empty())
            return appends;

        // Get visible appends only
        std::set<QString> visibleAppends;
        ranges::set_intersection(appends, visible,
                                 ranges::inserter(visibleAppends, visibleAppends.cend()));

        return visibleAppends;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::set<QString>
    HasAttributes<Derived, AllRelations...>::removeSerializableHiddenAppends(
            std::set<QString> &&appends, const std::set<QString> &hidden)
    {
        // Nothing to do
        if (hidden.empty())
            return std::move(appends);

        /* Remove hidden appends, from the set container returned by
           the getSerializableVisibleAppends()! */
        std::set<QString> hiddenAppends;
        ranges::set_difference(appends, hidden,
                               ranges::inserter(hiddenAppends, hiddenAppends.cend()));

        return hiddenAppends;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::getAccessorValueFrom(
            const Attribute &attribute) const
    {
        const auto &accessorVariant = attribute.get();

        using CallbackWithoutParameters = Attribute::CallbackWithoutParameters;

        if (std::holds_alternative<CallbackWithoutParameters>(accessorVariant))
            return std::invoke(attribute.get<CallbackWithoutParameters>());

        using CallbackWithAttributes = Attribute::CallbackWithAttributes;

        if (std::holds_alternative<CallbackWithAttributes>(accessorVariant))
            return std::invoke(attribute.get<CallbackWithAttributes>(),
                               convertVectorToModelAttributes(m_attributes));

        Q_UNREACHABLE(); // Correct, std::variant<> can't hold anything else
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::validateUserMutator(
            const QString &name, const QHash<QString, MutatorFunction> &userMutators)
    {
        // Nothing to do, mutator defined
        if (userMutators.contains(name))
            return;

        throw Exceptions::MutatorMappingNotFoundError(
                    TypeUtils::classPureBasename<Derived>(), name);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::serializeDateOrDateTimeForAccessors(
            QVariant &value)
    {
        const auto typeId = value.typeId();

        // Nothing to do, not a datetime
        if (typeId != QMetaType::QDateTime && typeId != QMetaType::QDate &&
            typeId != QMetaType::QTime
        )
            return;

        value = serializeDateOrDateTimeOrTime(value);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const ModelAttributes &
    HasAttributes<Derived, AllRelations...>::convertVectorToModelAttributes(
            const QList<AttributeItem> &attributes) const
    {
        if (m_modelAttributesCacheForMutators)
            return *m_modelAttributesCacheForMutators;

        m_modelAttributesCacheForMutators = AttributeUtils::
                                            convertVectorToModelAttributes(attributes);

        return *m_modelAttributesCacheForMutators;
    }

    /* Others */

    /* Static cast this to a child's instance type (CRTP) */

    TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(HasAttributes)

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_HASATTRIBUTES_HPP
