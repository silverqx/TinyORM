#pragma once
#ifndef ORM_TINY_CONCERNS_HASATTRIBUTES_HPP
#define ORM_TINY_CONCERNS_HASATTRIBUTES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/config.hpp"

#ifdef TINYORM_DEBUG
#  include <QDebug>
#endif

#include <cmath>

#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/exceptions/invalidformaterror.hpp"
#include "orm/macros/likely.hpp"
#include "orm/macros/threadlocal.hpp"
#include "orm/ormtypes.hpp"
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
    class HasAttributes
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
        /*! Equality comparison operator for the HasAttributes concern. */
        inline bool operator==(const HasAttributes &) const = default;

        /*! Set a given attribute on the model. */
        Derived &setAttribute(const QString &key, QVariant value);
        /*! Set a vector of model attributes. No checking is done. */
        Derived &setRawAttributes(const QVector<AttributeItem> &attributes,
                                  bool sync = false);
        /*! Set a vector of model attributes. No checking is done. */
        Derived &setRawAttributes(QVector<AttributeItem> &&attributes,
                                  bool sync = false);
        /*! Sync the original attributes with the current. */
        Derived &syncOriginal();

        /*! Get all of the current attributes on the model (insert order). */
        inline const QVector<AttributeItem> &getAttributes() const noexcept;
        /*! Get all of the current attributes on the model (for fast lookup). */
        inline const std::unordered_map<QString, int> &getAttributesHash() const;
        /*! Get an attribute from the model. */
        QVariant getAttribute(const QString &key) const;
        /*! Get a plain attribute (not a relationship). */
        QVariant getAttributeValue(const QString &key) const;
        /*! Get an attribute from the m_attributes vector. */
        QVariant getAttributeFromArray(const QString &key) const;

        /*! Get the model's original attribute value (transformed). */
        QVariant getOriginal(const QString &key,
                             const QVariant &defaultValue = {}) const;
        /*! Get the model's original attribute values (transformed and insert order). */
        QVector<AttributeItem> getOriginals() const;
        /*! Get the model's original attributes hash (for fast lookup). */
        inline const std::unordered_map<QString, int> &getOriginalsHash() const;
        /*! Get the model's raw original attribute value. */
        QVariant getRawOriginal(const QString &key,
                                const QVariant &defaultValue = {}) const;
        /*! Get the model's raw original attribute values (insert order). */
        inline const QVector<AttributeItem> &getRawOriginals() const;

        /*! Unset an attribute on the model, returns the number of attributes removed. */
        Derived &unsetAttribute(const AttributeItem &value);
        /*! Unset an attribute on the model. */
        Derived &unsetAttribute(const QString &key);

        /*! Get the attributes that have been changed since last sync
            (insert order). */
        QVector<AttributeItem> getDirty() const;
        /*! Get the attributes that have been changed since last sync
            (for fast lookup). */
        std::unordered_map<QString, int> getDirtyHash() const;
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

        /*! Get the attributes that were changed (insert order). */
        inline const QVector<AttributeItem> &getChanges() const;
        /*! Get the attributes that were changed (for fast lookup). */
        inline const std::unordered_map<QString, int> &getChangesHash() const;
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
        /*! Convert a QDateTime or QDate to a storable string. */
        QVariant fromDateTime(const QVariant &value) const;
        /*! Convert a QDateTime or QDate to a storable string. */
        QVariant fromDateTime(const QDateTime &value) const;
        /*! Get the attributes that should be converted to dates. */
        QStringList getDates() const;

        /*! Proxy for an attribute element used in the operator[] &. */
        class AttributeReference
        {
            friend HasAttributes<Derived, AllRelations...>;

        public:
            /*! Default destructor. */
            inline ~AttributeReference() = default;

            /*! Copy constructor. */
            inline AttributeReference(const AttributeReference &) = default;
            /*! Move constructor. */
            inline AttributeReference(AttributeReference &&) noexcept = default;
            /*! Deleted move assignment operator. */
            AttributeReference &operator=(AttributeReference &&) = delete;

            /*! Assign a value of the QVariant to the referenced attribute. */
            inline const AttributeReference & // NOLINT(misc-unconventional-assign-operator)
            operator=(const QVariant &value) const;
            /*! Assign a value of another attribute reference to the referenced
                attribute. */
            inline const AttributeReference & // NOLINT(misc-unconventional-assign-operator)
            operator=(const AttributeReference &attributeReference) const;

            /*! Accesses the contained value, only const member functions. */
            inline const QVariant *operator->() const;
            /*! Accesses the contained value. */
            inline QVariant value() const;
            /*! Accesses the contained value. */
            inline QVariant operator*() const;
            /*! Converting operator to the QVariant type. */
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

    protected:
        /*! Transform a raw model value using mutators, casts, etc. */
        QVariant transformModelValue(const QString &key, const QVariant &value) const;
        /*! Get the model's original attribute values. */
        QVariant getOriginalWithoutRewindingModel(
                const QString &key, const QVariant &defaultValue = {}) const;

        /*! Get all of the current attributes on the model. */
        inline const QVector<AttributeItem> &getRawAttributes() const;

        /*! Determine if any of the given attributes were changed. */
        bool hasChanges(const std::unordered_map<QString, int> &changes,
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

        /*! Determine if the given attribute is a date. */
        bool isDateAttribute(const QString &key) const;

        /*! Return a timestamp as QDateTime object. */
        QDateTime asDateTime(const QVariant &value) const;
        /*! Return a timestamp as QDate object. */
        inline QDate asDate(const QVariant &value) const;
        /*! Return a timestamp as unix timestamp. */
        inline qint64 asTimestamp(const QVariant &value) const;

        /*! Return a timestamp as QDateTime or QDate object. */
        QVariant asDateOrDateTime(const QVariant &value) const;
        /*! Convert a QDateTime or QDate to a storable string. */
        QVariant fromDateOrDateTime(const QVariant &value,
                                    const QString &format) const;

        /*! Get the correct QVariant(null) by a type in the QVariant and format. */
        static QVariant nullFor_fromDateTime(const QVariant &value,
                                             const QString &format);

        /*! Convert the QDateTime's time zone to the Model's time zone. */
        QDateTime convertTimeZone(QDateTime &&datetime) const;
        /*! Set the QDateTime's time zone to the Model's time zone. */
        QDateTime &setTimeZone(QDateTime &datetime) const;

        /*! Rehash attribute positions from the given index. */
        void rehashAttributePositions(
                const QVector<AttributeItem> &attributes,
                std::unordered_map<QString, int> &attributesHash,
                int from = 0);

        /* Casting Attributes */
        /*! Cast an attribute, convert a QVariant value. */
        QVariant castAttribute(const QString &key, const QVariant &value) const;
        /*! Get the type of cast for a model attribute. */
        inline CastItem getCastItem(const QString &key) const;
        /*! Get the type of cast for a model attribute. */
        inline CastType getCastType(const QString &key) const;

        /*! Determine whether a value is Date / DateTime castable. */
        inline bool isDateCastable(const QString &key) const;
        /*! Determine if the cast type is a custom date time cast. */
//        static bool isCustomDateTimeCast(const CastItem &cast);

        /*! Round a QVariant(double) to the given decimals (used by castAttribute()). */
        inline static QVariant
        roundDecimals(const QVariant &value, const QVariant &decimals);

        /* Data members */
        /*! The model's default values for attributes. */
        T_THREAD_LOCAL
        inline static QVector<AttributeItem> u_attributes;
        /*! The model's attributes (insert order). */
        QVector<AttributeItem> m_attributes;
        /*! The model attribute's original state (insert order).
            On the model from many-to-many relation also contains all pivot values,
            that is normal (insert order). */
        QVector<AttributeItem> m_original;
        /*! The changed model attributes (insert order). */
        QVector<AttributeItem> m_changes;

        /* Don't want to use std::reference_wrapper to attributes, because if a copy
           of the model is made, all references would be invalidated. */
        /*! The model's attributes hash (for fast lookup). */
        std::unordered_map<QString, int> m_attributesHash;
        /*! The model attribute's original state (for fast lookup). */
        std::unordered_map<QString, int> m_originalHash;
        /*! The changed model attributes (for fast lookup). */
        std::unordered_map<QString, int> m_changesHash;

        /*! The storage format of the model's date columns. */
        T_THREAD_LOCAL
        inline static QString u_dateFormat;
        /*! The attributes that should be mutated to dates. */
        T_THREAD_LOCAL
        inline static QStringList u_dates;

        /* Casting Attributes */
        /*! The attributes that should be cast. */
        std::unordered_map<QString, CastItem> u_casts;

        /*! Determine how the QDateTime time zone will be converted. */
        mutable std::optional<QtTimeZoneConfig> m_qtTimeZone = std::nullopt;
        /*! Determine whether the QDateTime time zone should be converted. */
        mutable std::optional<bool> m_isConvertingTimeZone = std::nullopt;

    private:
        /*! Throw if the m_attributesHash doesn't contain a given attribute. */
        static void throwIfNoAttributeInHash(
                    const std::unordered_map<QString, int> &attributesHash,
                    const QString &attribute, const QString &functionName);

        /* Casting Attributes */
        /* QMetaType isn't trivially copyable in Qt5, so const-lvalue reference needed. */
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        /*! QMetaType used in a function declaration. */
        using QMetaTypeDecl = QMetaType;
        /*! QMetaType used in a function definition. */
        using QMetaTypeDef  = const QMetaType;
#else
        /*! QMetaType used in a function declaration. */
        using QMetaTypeDecl = std::add_lvalue_reference_t<const QMetaType>;
        /*! QMetaType used in a function definition. */
        using QMetaTypeDef  = QMetaTypeDecl;
#endif
        /*! Throw if the given attribute can not be converted to the given cast type. */
        static void throwIfCanNotCastAttribute(
                    const QString &key, CastType castType, QMetaTypeDecl metaType,
                    const QVariant &value, const QString &functionName);
        /*! Log if the QVariant::convert() for the given attribute failed. */
        static void logIfConvertAttributeFailed(
                    const QString &key, CastType castType, QMetaTypeDecl metaType,
                    const QString &functionName);

        /*! Return the string name of the given cast type. */
        static QString castTypeName(CastType type);

        /* Others */
        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_WITH_BASE_DECLARATIONS
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
        if (const auto typeId = Helpers::qVariantTypeId(value);
            value.isValid() && (isDateAttribute(key) ||
            // NOTE api different, if the QDateTime or QDate is detected then take it as datetime silverqx
            typeId == QMetaType::QDateTime || typeId == QMetaType::QDate)
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

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setRawAttributes(
            const QVector<AttributeItem> &attributes,
            const bool sync)
    {
        m_attributes.reserve(attributes.size());
        m_attributes = AttributeUtils::removeDuplicitKeys(attributes);

        // Build attributes hash
        m_attributesHash.clear();
        m_attributesHash.reserve(static_cast<std::size_t>(m_attributes.size()));

        rehashAttributePositions(m_attributes, m_attributesHash);

        if (sync)
            syncOriginal();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setRawAttributes(
            QVector<AttributeItem> &&attributes,
            const bool sync)
    {
        m_attributes.reserve(attributes.size());
        m_attributes = AttributeUtils::removeDuplicitKeys(std::move(attributes));

        // Build attributes hash
        m_attributesHash.clear();
        m_attributesHash.reserve(static_cast<std::size_t>(m_attributes.size()));

        rehashAttributePositions(m_attributes, m_attributesHash);

        if (sync)
            syncOriginal();

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
    const QVector<AttributeItem> &
    HasAttributes<Derived, AllRelations...>::getAttributes() const noexcept
    {
        // FEATURE castable silverqx
//        mergeAttributesFromClassCasts();

        // TODO attributes, getAttributes() doesn't apply transformModelValue() on attributes, worth considering to make getRawAttributes() to return raw and getAttributes() to return transformed values, after this changes would be this api different than Eloquent silverqx
        return m_attributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::unordered_map<QString, int> &
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
           get the attribute's value. Otherwise, we will return invalid QVariant. */
        if (m_attributesHash.contains(key) || model().getUserCasts().contains(key))
            return getAttributeValue(key);

        // FUTURE add getRelationValue() overload without Related template argument, after that I will be able to use it here, Related template parameter will be obtained by the visitor, I think this task is impossible to do silverqx
        // NOTE api different silverqx
        return {};
//        return $this->getRelationValue($key);
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
    QVector<AttributeItem>
    HasAttributes<Derived, AllRelations...>::getOriginals() const
    {
        QVector<AttributeItem> originals;
        originals.reserve(m_original.size());

        for (const auto &original : m_original) {
            const auto &key = original.key;

            originals.append({key, transformModelValue(key, original.value)});
        }

        return originals;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::unordered_map<QString, int> &
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
    const QVector<AttributeItem> &
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

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVector<AttributeItem>
    HasAttributes<Derived, AllRelations...>::getDirty() const
    {
        const auto &attributes = getAttributes();

        QVector<AttributeItem> dirty;
        dirty.reserve(attributes.size());

        for (const auto &attribute : attributes)
            if (const auto &key = attribute.key;
                !originalIsEquivalent(key)
            )
                dirty.append({key, attribute.value});

        return dirty;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unordered_map<QString, int>
    HasAttributes<Derived, AllRelations...>::getDirtyHash() const
    {
        const auto size = m_attributes.size();
        std::unordered_map<QString, int> dirtyHash(static_cast<std::size_t>(size));

        for (auto i = 0; i < size; ++i)
            if (const auto &key = m_attributes.at(i).key;
                !originalIsEquivalent(key)
            )
                dirtyHash.emplace(m_attributes.at(i).key, i);

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
    const QVector<AttributeItem> &
    HasAttributes<Derived, AllRelations...>::getChanges() const
    {
        return m_changes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::unordered_map<QString, int> &
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
        if (const auto &userDateFormat = basemodel().getUserDateFormat();
            !userDateFormat.isEmpty()
        )
            return userDateFormat;

        return basemodel().getConnection().getQueryGrammar().getDateFormat();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setDateFormat(const QString &format)
    {
        basemodel().getUserDateFormat() = format;

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
                  value.canConvert<qint64>()));

        const auto &format = getDateFormat();

        /* Special logic for the null values, fix a null value on the base of the format
           and the value type. */
        if (value.isNull())
            return nullFor_fromDateTime(value, format);

        // Support unix timestamps
        if (format == QChar('U')) T_UNLIKELY
            return asTimestamp(value);

        // Convert a QDateTime or QDate to a storable string
        else T_LIKELY
            return fromDateOrDateTime(value, format);
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

        // Support unix timestamps
        /* This should be templated and for the unix timestamps the return type should be
           qint64, but it would make the code more complex because I would have to move
           the getDateFormat() outside, so I will simply return QString, it's not a big
           deal, INSERT/UPDATE clauses with '1604xxx' for the bigint columns are totaly
           ok, instead of 1604xxx as integer type. */
        if (format == QChar('U')) T_UNLIKELY
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

    // NOLINTNEXTLINE(misc-unconventional-assign-operator)
    template<typename Derived, AllRelationsConcept ...AllRelations>
    const typename HasAttributes<Derived, AllRelations...>::AttributeReference &
    HasAttributes<Derived, AllRelations...>::AttributeReference::operator=(
            const QVariant &value) const
    {
        m_model.get().setAttribute(m_attribute, value);

        return *this;
    }

    // NOLINTNEXTLINE(misc-unconventional-assign-operator)
    template<typename Derived, AllRelationsConcept ...AllRelations>
    const typename HasAttributes<Derived, AllRelations...>::AttributeReference &
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
        const auto &model = this->model();

        const auto &keyName = model.getKeyName();

        /* Needed to make a copy because it can interfere with the check
           in the getAttribute() method (getUserCasts().contains(key)), so don't modify
           the user's u_casts and add the 'id' cast on the fly on the casts copy. */
        auto casts = model.getUserCasts();

        if (model.getIncrementing() && !casts.contains(keyName))
            // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
            casts.emplace(keyName, CastType::ULongLong);

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
        auto &model = this->model();
        auto &userCasts = model.getUserCasts();

        std::remove_cvref_t<decltype (casts)> mergedCasts;
        mergedCasts.reserve(userCasts.size() + casts.size());

        mergedCasts = userCasts;

        for (const auto &[attribute, castItem] : casts)
            mergedCasts.insert_or_assign(attribute, castItem);

        // Swap user casts
        userCasts = std::move(mergedCasts);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::mergeCasts(
            std::unordered_map<QString, CastItem> &casts)
    {
        auto &model = this->model();

        model.getUserCasts().merge(casts);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::mergeCasts(
            std::unordered_map<QString, CastItem> &&casts)
    {
        auto &model = this->model();

        model.getUserCasts().merge(std::move(casts));

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &HasAttributes<Derived, AllRelations...>::resetCasts()
    {
        auto &model = this->model();

        model.getUserCasts().clear();

        return model;
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
           date fields without having to create a cast for each attribute. */
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
    const QVector<AttributeItem> &
    HasAttributes<Derived, AllRelations...>::getRawAttributes() const
    {
        return m_attributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasAttributes<Derived, AllRelations...>::hasChanges(
            const std::unordered_map<QString, int> &changes,
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
            throwIfNoAttributeInHash(modelAttributesHash, attribute, __tiny_func__);

            const auto &modelAttributeValue =
                    modelAttributes.at(modelAttributesHash.at(attribute)).value;

            // The 'attribute' already exists in the m_original/Hash, update it
            if (m_originalHash.contains(attribute)) {
                const auto attributeIndex = m_originalHash.at(attribute);
                Q_ASSERT(attributeIndex >= 0 && attributeIndex < m_original.size());

                m_original[attributeIndex].value = modelAttributeValue;
            }
            /* The 'attribute' doesn't exist in the m_original/Hash, so create it and
               rehash m_originalHash, but only from the addition position. */
            else {
                const auto rehashFrom = static_cast<int>(modelAttributes.size()) - 1;

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

        // This check ignores milliseconds in the QDateTime attribute
        if (isDateAttribute(key))
            return fromDateTime(attribute) == fromDateTime(original);

        if (hasCast(key, {CastType::Real, CastType::Float, CastType::Double})) {
            if (!original.isValid() || original.isNull())
                return false;

            return std::abs(castAttribute(key, attribute).template value<double>() -
                            castAttribute(key, original).template value<double>()) <
                    // * 4 is still very high precision whether two numbers are the same
                    std::numeric_limits<double>::epsilon() * 4;
        }

        // FEATURE castable, update this if I will support eg. class casts, this check is only for primitive types silverqx
        if (hasCast(key))
            return castAttribute(key, attribute) == castAttribute(key, original);

        return false;
    }

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
                   qUtf8Printable(__tiny_func__),
                   "null values must be handled outside of the asDateTime, asDate, "
                   "and asTimestamp methods.");

        /* If this value is already a QDateTime instance, we shall just return it as is.
           This prevents us having to re-parse a QDateTime instance when we know
           it already is one. */
        if (Helpers::qVariantTypeId(value) == QMetaType::QDateTime)
            return convertTimeZone(value.value<QDateTime>());

        // The value has to be convertible to the QString so we can work with it
        if (!value.canConvert<QString>())
            throw Orm::Exceptions::InvalidFormatError(
                        QStringLiteral("Could not parse the datetime, could not convert "
                                       "the 'value' to the QString in %1().")
                        .arg(__tiny_func__));

        const auto valueString = value.value<QString>();

        /* If this value is an integer, we will assume it is a UNIX timestamp's value
           and format a QDateTime object from this timestamp. This allows flexibility
           when defining your date fields as they might be UNIX timestamps here. */
        if (StringUtils::isNumber(valueString))
            // TODO switch ms accuracy? For the u_dateFormat too? silverqx
            if (auto date = QDateTime::fromSecsSinceEpoch(value.value<qint64>());
                date.isValid()
            )
                return convertTimeZone(std::move(date));

        /* If the value is in simply year, month, day format, we will instantiate the
           QDate instances from that format. Again, this provides for simple date
           fields on the database, while still supporting QDateTime conversion. */
        if (Helpers::isStandardDateFormat(valueString))
            if (auto date = QDateTime::fromString(valueString,
                                                  QStringLiteral("yyyy-M-d"));
                date.isValid()
            )
                return setTimeZone(date);

        const auto &format = getDateFormat();

        // CUR tz, add more formats? eg. with the t format expression and also for Qt::TextDate to be able transform more string dates to the QDateTime instance, but have to be carefull, at 100% this would be good for the setAttribute(), but need to think about originalIsEquiv() and other scenarios where this can collide or somehow change correct behavior silverqx
        /* Finally, we will just assume this date is in the format used by default on
           the database connection and use that format to create the QDateTime object
           that is returned back out to the developers after we convert it here. */
        if (auto date = QDateTime::fromString(valueString, format);
            date.isValid()
        )
            return setTimeZone(date);

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
    QVariant
    HasAttributes<Derived, AllRelations...>::asDateOrDateTime(
            const QVariant &value) const
    {
        const auto typeId = Helpers::qVariantTypeId(value);

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
    HasAttributes<Derived, AllRelations...>::fromDateOrDateTime(
            const QVariant &value, const QString &format) const
    {
        if (const auto typeId = Helpers::qVariantTypeId(value);
            typeId == QMetaType::QDate ||
            (typeId == QMetaType::QString &&
             Helpers::isStandardDateFormat(value.value<QString>()))
        ) T_UNLIKELY
            return asDate(value).toString(Qt::ISODate);

        else T_LIKELY
            return asDateTime(value).toString(format);
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
        if (format == QChar('U')) T_UNLIKELY
            return NullVariant::LongLong();

        else if (Helpers::qVariantTypeId(value) == QMetaType::QDate) T_UNLIKELY
            return NullVariant::QDate();

        else T_LIKELY
            return NullVariant::QDateTime();
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

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::rehashAttributePositions(
            const QVector<AttributeItem> &attributes,
            std::unordered_map<QString, int> &attributesHash,
            const int from)
    {
        /* This member function is universal and can be used for m_attributes,
           m_changes and m_original and it associated unordered_maps m_attributesHash,
           m_changesHash and m_originalHash. */
        for (auto i = from; i < attributes.size(); ++i)
            // 'i' is the position
            attributesHash[attributes.at(i).key] = i;
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
        const auto functionName = __tiny_func__;

        /*! Convert the QVariant value of a attribute. */
        const auto convertAttribute = [this, &key, &value_, castType, &functionName]
                                      (QMetaTypeDef metaType)
        {
            // Throw if the given attribute can not be converted to the given cast type
            throwIfCanNotCastAttribute(key, castType, metaType, value_, functionName);

            /* Still check for the false value and log to the debug stream, but not if
               the value_ is null, because converting null QVariant will always return
               false and the QVariant type will be changed anyway. */
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            if (!value_.convert(metaType) && !value_.isNull())
#else
            if (!value_.convert(metaType.id()) && !value_.isNull())
#endif
                // Log if the QVariant::convert() for the given attribute failed
#ifdef TINYORM_DEBUG
                logIfConvertAttributeFailed(key, castType, metaType, functionName);
#else
                ;
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
            return value_.isNull() ? NullVariant::QDate()
                                   : asDate(value_);
        case CastType::QDateTime:
//        case CastType::CustomQDateTime:
            return value_.isNull() ? NullVariant::QDateTime()
                                   : asDateTime(value_);
        case CastType::Timestamp:
            return value_.isNull() ? NullVariant::LongLong()
                                   : asTimestamp(value_);
        // Bool
        case CastType::Bool:
        case CastType::Boolean:
            return convertAttribute(QMetaType(QMetaType::Bool));
        // Int 16-bit
        case CastType::Short:
            /* Qt5 QVariant doesn't define the short int type QVariant::Short, but
               it can be bypassed using the QMetaType. */
            return convertAttribute(QMetaType(QMetaType::Short));
        case CastType::UShort:
            /* Qt5 QVariant doesn't define the short int type QVariant::Short, but
               it can be bypassed using the QMetaType. */
            return convertAttribute(QMetaType(QMetaType::UShort));
        // Float
        case CastType::Real:
        case CastType::Float:
        case CastType::Double:
            /* The fromFloat() not needed here because this logic is handled in the
               QSqlDriver and currently only the PostgreSQL supports NaN and Infinity
               values. The MySQL and SQLite doesn't care. */
            return convertAttribute(QMetaType(QMetaType::Double));

        case CastType::Decimal:
        {
            auto converted = convertAttribute(QMetaType(QMetaType::Double));
            const auto &modifier = castItem.modifier();

            /* This is pure for the performance reasons, in the Qt6 the isNull()
               internally also checks if isValid(). In the Qt5 the logic is different. */
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            if (modifier.isNull() || converted.isNull()
#else
            if (!modifier.isValid()  || modifier.isNull() ||
                !converted.isValid() || converted.isNull()
#endif
            ) T_LIKELY
                return converted;
            else T_UNLIKELY
                return roundDecimals(converted, modifier);
        }
        // Others
        case CastType::QByteArray:
            return convertAttribute(QMetaType(QMetaType::QByteArray));

        default:
            Q_UNREACHABLE();
        }
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
        return hasCast(key, {CastType::QDate, CastType::QDateTime});
    }

//    template<typename Derived, AllRelationsConcept ...AllRelations>
//    bool
//    HasAttributes<Derived, AllRelations...>::isCustomDateTimeCast(
//            const CastItem &cast)
//    {
//        const auto &modifier = cast.modifier();

//        return (cast.type() == CastType::QDateTime || cast.type() == CastType::QDate) &&
//                modifier.template canConvert<QString>() &&
//                !modifier.template value<QString>().isEmpty();
//    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::roundDecimals(const QVariant &value,
                                                           const QVariant &decimals)
    {
        /* All parameters and the return value are the QVariant to simplify and
           for outsource the code from the castAttribute() method, this method is
           designed especially for the castAttribute() method. */
        const double multiplier = std::pow(static_cast<double>(10.0),
                                           decimals.template value<int>());

        return static_cast<double>(
                    std::round(value.template value<double>() * multiplier) /
                    multiplier);
    }

    /* private */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::throwIfNoAttributeInHash(
            const std::unordered_map<QString, int> &attributesHash,
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
            const QString &key, const CastType castType, QMetaTypeDef metaType,
            const QVariant &value, const QString &functionName)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (value.canConvert(metaType))
#else
        if (value.canConvert(metaType.id()))
#endif
            return;

        throw Orm::Exceptions::InvalidArgumentError(
                    QStringLiteral(
                        "Bad cast type was defined in the %1::u_casts hash, the '%2' "
                        "attribute can not be casted to the 'CastType::%3' "
                        "(using the QMetaType::%4) in %5().")
                    .arg(TypeUtils::template classPureBasename<Derived>(), key,
                         castTypeName(castType), metaType.name(), functionName));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasAttributes<Derived, AllRelations...>::logIfConvertAttributeFailed(
            const QString &key, const CastType castType, QMetaTypeDef metaType,
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
    }

    /* Others */

    /* Static cast this to a child's instance type (CRTP) */
    TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(HasAttributes)

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_HASATTRIBUTES_HPP
