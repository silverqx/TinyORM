#pragma once
#ifndef ORM_TINY_CONCERNS_HASATTRIBUTES_HPP
#define ORM_TINY_CONCERNS_HASATTRIBUTES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QDateTime>

#include "orm/exceptions/invalidformaterror.hpp"
#include "orm/macros/threadlocal.hpp"
#include "orm/ormtypes.hpp"
#include "orm/tiny/macros/crtpmodelwithbase.hpp"
#include "orm/tiny/utils/attribute.hpp"
#include "orm/tiny/utils/string.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Concerns
{

    /*! Model attributes. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HasAttributes
    {
        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;
        /*! Alias for the string utils. */
        using StringUtils = Orm::Tiny::Utils::String;

    public:
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
        inline const QVector<AttributeItem> &getAttributes() const;
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
        /*! Convert a DateTime to a storable string. */
        QVariant fromDateTime(const QVariant &value) const;
        /*! Convert a DateTime to a storable string. */
        QString fromDateTime(const QDateTime &value) const;
        /*! Get the attributes that should be converted to dates. */
        const QStringList &getDates() const;

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
                               // NOLINTNEXTLINE(modernize-pass-by-value)
                               const QString &attribute);

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
        /*! Sync multiple original attribute with their current values. */
        Derived &syncOriginalAttributes(const QStringList &attributes);

        /*! Determine if the new and old values for a given key are equivalent
            (used by the getDirty()). */
        bool originalIsEquivalent(const QString &key) const;

        /*! Determine if the given attribute is a date. */
        bool isDateAttribute(const QString &key) const;
        /*! Return a timestamp as DateTime object. */
        QDateTime asDateTime(const QVariant &value) const;

        /*! Rehash attribute positions from the given index. */
        void rehashAttributePositions(
                const QVector<AttributeItem> &attributes,
                std::unordered_map<QString, int> &attributesHash,
                int from = 0);

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

        // TODO add support for 'U' like in PHP to support unix timestamp, I will have to manually check if u_dateFormat contains 'U' and use QDateTime::fromSecsSinceEpoch() silverqx
        /*! The storage format of the model's date columns. */
        T_THREAD_LOCAL
        inline static QString u_dateFormat;
        /*! The attributes that should be mutated to dates. */
        T_THREAD_LOCAL
        inline static QStringList u_dates;

    private:
        QStringList getDatesInternal() const;

        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_WITH_BASE_DECLARATIONS
    };

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    HasAttributes<Derived, AllRelations...>::setAttribute(
            const QString &key, QVariant value)
    {
        /* If an attribute is listed as a "date", we'll convert it from a DateTime
           instance into a form proper for storage on the database tables using
           the connection grammar's date format. We will auto set the values. */
        if (value.isValid() && !value.isNull() && (isDateAttribute(key) ||
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            value.typeId() == QMetaType::QDateTime
#else
            value.userType() == QMetaType::QDateTime
#endif
        ))
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
    HasAttributes<Derived, AllRelations...>::getAttributes() const
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

        /* If the attribute exists in the attribute hash or has a "get" mutator we will
           get the attribute's value. Otherwise, we will proceed as if the developers
           are asking for a relationship's value. This covers both types of values. */
        if (m_attributesHash.contains(key)
//            || array_key_exists($key, $this->casts)
//            || hasGetMutator(key)
//            || isClassCastable(key)
        )
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
        return Derived().setRawAttributes(m_original, true)
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
        if (value.isNull())
            return value;

        return asDateTime(value).toString(getDateFormat());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    HasAttributes<Derived, AllRelations...>::fromDateTime(const QDateTime &value) const
    {
        if (value.isValid())
            return value.toString(getDateFormat());

        return {};
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    HasAttributes<Derived, AllRelations...>::getDates() const
    {
        static const QStringList cached = getDatesInternal();

        return cached;
    }

    /* Model::AttributeReference - begin */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasAttributes<Derived, AllRelations...>::AttributeReference::AttributeReference(
            Model<Derived, AllRelations...> &model,
            // NOLINTNEXTLINE(modernize-pass-by-value)
            const QString &attribute
    )
        : m_model(model)
        , m_attribute(attribute)
    {}

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

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasAttributes<Derived, AllRelations...>::AttributeReference
    HasAttributes<Derived, AllRelations...>::operator[](
                const QString &attribute) &
    {
        return AttributeReference(basemodel(), attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::operator[](
                const QString &attribute) const &
    {
        return getAttribute(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::operator[](
                const QString &attribute) &&
    {
        return getAttribute(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    HasAttributes<Derived, AllRelations...>::operator[](
                const QString &attribute) const &&
    {
        return getAttribute(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant HasAttributes<Derived, AllRelations...>::transformModelValue(
            const QString &key,
            const QVariant &value) const
    {
        /* Qt's SQLite driver doesn't apply any logic on the returned types, it returns
           them without type detection, and it is logical, because SQLite only supports
           numeric and string types, it doesn't distinguish datetime type or any other
           types.
           Qt's MySql driver behaves differently, QVariant already contains the QDateTime
           values, because Qt's MySQL driver returns QDateTime when the value from
           the database is datetime, the same is true for all other types, Qt's driver
           detects it and creates QVariant with proper types. */

        if (!value.isValid() || value.isNull())
            return value;

        /* If the attribute is listed as a date, we will convert it to a QDateTime
           instance on retrieval, which makes it quite convenient to work with
           date fields without having to create a mutator for each property. */
        if (getDates().contains(key))
            return asDateTime(value);

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
            const auto attributeIndex = m_originalHash.at(attribute);
            Q_ASSERT(attributeIndex >= 0 && attributeIndex < m_original.size());

            m_original[attributeIndex].value =
                    modelAttributes.at(modelAttributesHash.at(attribute)).value;
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

//        if (hasCast(key, ['object', 'collection']))
//            return castAttribute(key, attribute) == castAttribute(key, original);
//        if (hasCast(key, ['real', 'float', 'double'])) {
//            if (($attribute === null && $original !== null) || ($attribute !== null && $original === null))
//                return false;

//            return abs($this->castAttribute($key, $attribute) - $this->castAttribute($key, $original)) < PHP_FLOAT_EPSILON * 4;
//        }
//        if ($this->hasCast($key, static::$primitiveCastTypes)) {
//            return $this->castAttribute($key, $attribute) ===
//                   $this->castAttribute($key, $original);
//        }

//        return is_numeric($attribute) && is_numeric($original) &&
//               strcmp((string) $attribute, (string) $original) === 0;

        return false;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool HasAttributes<Derived, AllRelations...>::isDateAttribute(const QString &key) const
    {
        // FEATURE castable silverqx
        /* I don't have support for castable attributes, this solution is temporary. */
        return getDates().contains(key);
    }

    // TODO would be good to make it the c++ way, make overload for every type, asDateTime() is protected, so I have full control over it, but I leave it for now, because there will be more methods which will use this method in the future, and it will be more clear later on silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    QDateTime
    HasAttributes<Derived, AllRelations...>::asDateTime(const QVariant &value) const
    {
        /* If this value is already a QDateTime instance, we shall just return it as is.
           This prevents us having to re-parse a QDateTime instance when we know
           it already is one. */
        if (
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            value.typeId() == QMetaType::QDateTime
#else
            value.userType() == QMetaType::QDateTime
#endif
        )
            return value.value<QDateTime>();

        /* If this value is an integer, we will assume it is a UNIX timestamp's value
           and format a Carbon object from this timestamp. This allows flexibility
           when defining your date fields as they might be UNIX timestamps here. */
        if (value.canConvert<QString>() &&
            StringUtils::isNumber(value.value<QString>())
        )
            // TODO switch ms accuracy? For the u_dateFormat too? silverqx
            return QDateTime::fromSecsSinceEpoch(value.value<qint64>());

        const auto &format = getDateFormat();

        /* Finally, we will just assume this date is in the format used by default on
           the database connection and use that format to create the QDateTime object
           that is returned back out to the developers after we convert it here. */
        if (auto date = QDateTime::fromString(value.value<QString>(), format);
            date.isValid()
        )
            return date;

        throw Orm::Exceptions::InvalidFormatError(
                    QStringLiteral("Could not parse the datetime '%1' using "
                                   "the given format '%2'.")
                    .arg(value.value<QString>(), format));
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

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QStringList
    HasAttributes<Derived, AllRelations...>::getDatesInternal() const
    {
        if (!basemodel().usesTimestamps())
            return Model<Derived, AllRelations...>::getUserDates();

        auto dates = Model<Derived, AllRelations...>::getUserDates() +
                Model<Derived, AllRelations...>::timestampColumnNames();

        dates.removeDuplicates();

        return dates;
    }

    /* Static cast this to a child's instance type (CRTP) */
    TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(HasAttributes)

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_HASATTRIBUTES_HPP
