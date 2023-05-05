#pragma once
#ifndef ORM_TINY_TYPES_MODELSCOLLECTION_HPP
#define ORM_TINY_TYPES_MODELSCOLLECTION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QDebug>

#include <unordered_map>
#include <unordered_set>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/tiny/tinytypes.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Tiny
{
namespace Types
{

    /*! Tag type for each() related methods (return bool callback overload). */
    struct EachBoolCallbackType {
        /*! Default constructor. */
        explicit EachBoolCallbackType() = default;
    };

    /*! Where item that stores values for the where between for ModelsCollection. */
    template<typename T>
    struct WhereBetweenCollectionItem
    {
        T min {};
        T max {};
    };

    /*! Models collection (QVector) with additional handy methods. */
    template<DerivedModel Model>
    class ModelsCollection : public QVector<Model>
    {
    public:
        /*! Inherit constructors. */
        using QVector<Model>::QVector;

        /* Container related */
        using StorageType     = QVector<Model>;

        using value_type      = typename StorageType::value_type;
        using pointer         = typename StorageType::pointer;
        using const_pointer   = typename StorageType::const_pointer;
        using reference       = value_type &;
        using const_reference = const value_type &;
        using size_type       = typename StorageType::size_type;
        using difference_type = typename StorageType::difference_type;

        /*! Model raw type, without the pointer. */
        using ModelRawType    = std::remove_pointer_t<value_type>;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        using parameter_type  = typename StorageType::parameter_type;
#else
        using parameter_type  = std::conditional_t<std::is_pointer_v<Model>,
                                                   ModelRawType *, const ModelRawType &>;
#endif

        /* Iterators related */
        using iterator               = typename StorageType::iterator;
        using const_iterator         = typename StorageType::const_iterator;
        using reverse_iterator       = typename StorageType::reverse_iterator;
        using const_reverse_iterator = typename StorageType::const_reverse_iterator;
        using Iterator               = typename StorageType::Iterator;
        using ConstIterator          = typename StorageType::ConstIterator;

        /*! The "type" of the primary key (ID). */
        using KeyType = typename std::remove_pointer_t<value_type>::KeyType;

        /* Constructors */
        /*! Converting constructor from the QVector<Model>. */
        inline ModelsCollection(const QVector<Model> &models) // NOLINT(google-explicit-constructor)
        requires (!std::is_pointer_v<Model>);

        /* Redeclared overriden methods from the base class */
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        /*! Returns a reference to the first item in the list. */
        inline Model &first();
        /*! Returns a reference to the first item in the list. */
        inline const Model &first() const noexcept;
        /*! Returns a sub-list that contains the first n elements of this list. */
        inline ModelsCollection<Model> first(size_type count) const;

        /*! Returns the value at index position i in the list. */
        inline Model value(size_type index) const;
        /*! Returns the value at index position i in the list. */
        inline Model value(size_type index, parameter_type defaultValue) const;
#else
        /*! Returns a reference to the first item in the list. */
        inline Model &first();
        /*! Returns a reference to the first item in the list. */
        inline const Model &first() const;

        /*! Returns the value at index position i in the list. */
        inline Model value(size_type index) const;
        /*! Returns the value at index position i in the list. */
        inline Model value(size_type index, parameter_type defaultValue) const;
#endif

        /* BaseCollection */
        /*! Run a filter over each of the items in the collection. */
        inline ModelsCollection<ModelRawType *>
        filter(const std::function<bool(ModelRawType *, size_type)> &callback);
        /*! Run a filter over each of the items in the collection. */
        inline ModelsCollection<ModelRawType *>
        filter(const std::function<bool(ModelRawType *)> &callback);
        /*! Run a filter over each of the items in the collection (removes nullptr-s). */
        inline ModelsCollection<ModelRawType *>
        filter() const requires std::is_pointer_v<Model>;

        /*! Get a first item from the collection passing the given truth test. */
        inline ModelRawType *
        first(const std::function<bool(ModelRawType *)> &callback,
              ModelRawType *defaultModel = nullptr);

        /*! Concatenate values of the given column as a string. */
        inline QString implode(const QString &column, const QString &glue = "");

        /* Collection */
        /*! Get a vector of primary keys. */
        inline QVector<QVariant> modelKeys() const;
        /*! Get a vector of primary keys. */
        template<typename T>
        inline QVector<T> modelKeys() const;

        /*! Run a map over each of the items. */
        inline ModelsCollection<ModelRawType *>
        map(const std::function<ModelRawType *(ModelRawType *, size_type)> &callback);
        /*! Run a map over each of the items. */
        inline ModelsCollection<ModelRawType *>
        map(const std::function<ModelRawType *(ModelRawType *)> &callback);

        /*! Run an associative map over each of the items (keyed by primary key). */
        inline std::unordered_map<KeyType, ModelRawType *> mapWithModelKeys();
        /*! Run an associative map over each of the items (key by the K template). */
        template<typename K, typename V>
        std::unordered_map<K, V>
        mapWithKeys(const std::function<std::pair<K, V>(ModelRawType *)> &callback);

        /*! Return only the models from the collection with specified keys. */
        inline ModelsCollection<ModelRawType *>
        only(const std::unordered_set<KeyType> &ids);
        /*! Return all models in the collection except the models with specified keys. */
        inline ModelsCollection<ModelRawType *>
        except(const std::unordered_set<KeyType> &ids);

        /*! Get a vector with the values in the given column. */
        inline QVector<QVariant> pluck(const QString &column);
        /*! Get a map with values in the given column and keyed by values in the key
            column (attribute). */
        template<typename T>
        std::map<T, QVariant> pluck(const QString &column, const QString &key);

        /*! Determine if the collection contains a model with the given ID. */
        inline bool contains(KeyType id);
        /*! Determine if the collection contains a model with the given ID. */
        inline bool contains(const QVariant &id);
        /*! Determine if the collection contains a model using the given callback. */
        inline bool contains(const std::function<bool(ModelRawType *)> &callback);
        /*! Determine if the model exists in the collection (using the Model::is()). */
        inline bool contains(const std::optional<ModelRawType> &model);

        /*! Determine if the collection doesn't contain a model with the given ID. */
        inline bool doesntContain(KeyType id);
        /*! Determine if the collection doesn't contain a model with the given ID. */
        inline bool doesntContain(const QVariant &id);
        /*! Determine if the collection doesn't contain a model using the given
            callback. */
        inline bool doesntContain(const std::function<bool(ModelRawType *)> &callback);
        /*! Determine if the model doesn't exist in the collection (using
            the Model::is()). */
        inline bool doesntContain(const std::optional<ModelRawType> &model);

        /*! Find a model in the collection by key. */
        inline ModelRawType *find(KeyType id, ModelRawType *defaultModel = nullptr);
        /*! Find a model in the collection by another model (using its ID). */
        inline ModelRawType *
        find(const ModelRawType &model, ModelRawType *defaultModel = nullptr);
        /*! Find models in the collection by the given IDs. */
        inline ModelsCollection<ModelRawType *>
        find(const std::unordered_set<KeyType> &ids);

        /*! Get the TinyBuilder from the collection. */
        std::unique_ptr<TinyBuilder<ModelRawType>> toQuery();

        /* EnumeratesValues */
        /*! Create a collection of all items that do not pass a given truth test. */
        inline ModelsCollection<ModelRawType *>
        reject(const std::function<bool(ModelRawType *, size_type)> &callback);
        /*! Create a collection of all items that do not pass a given truth test. */
        inline ModelsCollection<ModelRawType *>
        reject(const std::function<bool(ModelRawType *)> &callback);

        /*! Filter items by the given key value pair. */
        template<typename V>
        ModelsCollection<ModelRawType *>
        where(const QString &key, const QString &comparison, V value);
        /*! Filter items by the given key value pair. */
        template<typename V>
        ModelsCollection<ModelRawType *>
        whereEq(const QString &key, V value);

        /*! Filter items where the value for the given key is the null QVariant. */
        inline ModelsCollection<ModelRawType *> whereNull(const QString &key);
        /*! Filter items where the value for the given key is not the null QVariant. */
        inline ModelsCollection<ModelRawType *> whereNotNull(const QString &key);

        /*! Filter items by the given key values pair. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        whereIn(const QString &key, const std::unordered_set<T> &values);
        /*! Filter items by the given key values pair. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        whereNotIn(const QString &key, const std::unordered_set<T> &values);

        /*! Filter items such that the value of the given key is between the given
            values. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        whereBetween(const QString &key, const WhereBetweenCollectionItem<T> &values);
        /*! Filter items such that the value of the given key is not between the given
            values. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        whereNotBetween(const QString &key, const WhereBetweenCollectionItem<T> &values);

        /*! Get the first item by the given key value pair. */
        template<typename V>
        ModelRawType *
        firstWhere(const QString &key, const QString &comparison, V value);
        /*! Get the first item by the given key value pair. */
        template<typename V>
        ModelRawType *
        firstWhereEq(const QString &key, V value);

        /*! Get a single key's value from the first matching item in the collection. */
        inline QVariant
        value(const QString &key, const QVariant &defaultValue = {}) const;
        /*! Get a single key's value from the first matching item in the collection. */
        template<typename T>
        T value(const QString &key, const T &defaultValue = {}) const;

        /*! Execute a callback over each item. */
        inline ModelsCollection &
        each(const std::function<void(ModelRawType *)> &callback) &;
        /*! Execute a callback over each item. */
        inline ModelsCollection &
        each(const std::function<void(ModelRawType *, size_type)> &callback) &;
        /*! Execute a callback over each item. */
        inline ModelsCollection &
        each(EachBoolCallbackType tag,
             const std::function<bool(ModelRawType *)> &callback) &;
        /*! Execute a callback over each item. */
        inline ModelsCollection &
        each(EachBoolCallbackType tag,
             const std::function<bool(ModelRawType *, size_type)> &callback) &;

        /*! Execute a callback over each item. */
        inline ModelsCollection &&
        each(const std::function<void(ModelRawType *)> &callback) &&;
        /*! Execute a callback over each item. */
        inline ModelsCollection &&
        each(const std::function<void(ModelRawType *, size_type)> &callback) &&;
        /*! Execute a callback over each item. */
        inline ModelsCollection &&
        each(EachBoolCallbackType tag,
             const std::function<bool(ModelRawType *)> &callback) &&;
        /*! Execute a callback over each item. */
        inline ModelsCollection &&
        each(EachBoolCallbackType tag,
             const std::function<bool(ModelRawType *, size_type)> &callback) &&;

        /*! Pass the collection to the given callback and then return it. */
        inline ModelsCollection &
        tap(const std::function<void(ModelsCollection &)> &callback) &;
        /*! Pass the collection to the given callback and then return it. */
        inline ModelsCollection &&
        tap(const std::function<void(ModelsCollection &)> &callback) &&;

    protected:
        /*! Model type used in the for-ranged loops. */
        using ModelLoopType = std::conditional_t<std::is_pointer_v<Model>,
                                                 ModelRawType *const, ModelRawType &>;
        /*! Const Model type used in the for-ranged loops. */
        using ConstModelLoopType = std::conditional_t<std::is_pointer_v<Model>,
                                                      const ModelRawType *const,
                                                      const ModelRawType &>;

        /*! Convert the Model pointer to the pointer (no-op). */
        constexpr static ModelRawType *toPointer(ModelRawType *model) noexcept;
        /*! Convert the Model pointer to the pointer (no-op). */
        constexpr static const ModelRawType *
        toPointer(const ModelRawType *model) noexcept;
        /*! Convert the Model reference to the pointer. */
        inline static ModelRawType *toPointer(ModelRawType &model) noexcept;
        /*! Convert the const Model reference to the pointer. */
        inline static const ModelRawType *toPointer(const ModelRawType &model) noexcept;

        /*! Cast the given key to the primary key type. */
        inline static KeyType castKey(const QVariant &key);

        /*! Get the value of the model's primary key casted to the Derived::KeyType. */
        inline static KeyType getKeyCasted(const ModelRawType *model);
        /*! Get the value of the model's primary key casted to the Derived::KeyType. */
        inline static KeyType getKeyCasted(const ModelRawType &model);

        /*! Get the value of the model's primary key. */
        inline static QVariant getKey(const ModelRawType *model);
        /*! Get the value of the model's primary key. */
        inline static QVariant getKey(const ModelRawType &model);

        /*! Get an operator checker callback. */
        template<typename V>
        std::function<bool(ModelRawType *)>
        operatorForWhere(const QString &key, const QString &comparison, V value) const;

        /*! Convert the Collection<ModelRawType> to the Collection<ModelRawType *>. */
        inline ModelsCollection<ModelRawType *>
        toPointersCollection() requires (!std::is_pointer_v<Model>);
        /*! Convert to the Collection<ModelRawType *>, return itself (no-op overload). */
        inline ModelsCollection<ModelRawType *>
        toPointersCollection() const noexcept requires std::is_pointer_v<Model>;

        /*! Throw if the given operator is not valid for the where() method. */
        inline static void throwIfInvalidWhereOperator(const QString &comparison);
    };

    /* public */

    /* Constructors */

    template<DerivedModel Model>
    ModelsCollection<Model>::ModelsCollection(const QVector<Model> &models)
    requires (!std::is_pointer_v<Model>)
    {
        for (const auto &model : models)
            this->push_back(model);
    }

    /* Redeclared overriden methods from the base class */

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    template<DerivedModel Model>
    Model &
    ModelsCollection<Model>::first()
    {
        return StorageType::first();
    }

    template<DerivedModel Model>
    const Model &
    ModelsCollection<Model>::first() const noexcept
    {
        return StorageType::first();
    }

    template<DerivedModel Model>
    ModelsCollection<Model>
    ModelsCollection<Model>::first(const size_type count) const
    {
        return StorageType::first(count);
    }

    template<DerivedModel Model>
    Model
    ModelsCollection<Model>::value(const size_type index) const
    {
        return StorageType::value(index);
    }

    template<DerivedModel Model>
    Model
    ModelsCollection<Model>::value(const size_type index,
                                   parameter_type defaultValue) const
    {
        return StorageType::value(index, defaultValue);
    }
#else
    template<DerivedModel Model>
    Model &
    ModelsCollection<Model>::first()
    {
        return StorageType::first();
    }

    template<DerivedModel Model>
    const Model &
    ModelsCollection<Model>::first() const
    {
        return StorageType::first();
    }

    template<DerivedModel Model>
    Model
    ModelsCollection<Model>::value(const size_type index) const
    {
        return StorageType::value(index);
    }

    template<DerivedModel Model>
    Model
    ModelsCollection<Model>::value(const size_type index,
                                   parameter_type defaultValue) const
    {
        return StorageType::value(index, defaultValue);
    }
#endif

    /* BaseCollection */

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::filter(
            const std::function<bool(ModelRawType *, size_type)> &callback)
    {
        const auto size = this->size();

        ModelsCollection<ModelRawType *> result;
        result.reserve(size);

        for (size_type index = 0; index < size; ++index)
            // Don't handle the nullptr
            if (ModelRawType *const modelPointer = toPointer(this->operator[](index));
                std::invoke(callback, modelPointer, index)
            )
                result.push_back(modelPointer);

        return result;
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::filter(
            const std::function<bool(ModelRawType *)> &callback)
    {
        ModelsCollection<ModelRawType *> result;
        result.reserve(this->size());

        for (ModelLoopType model : *this)
            // Don't handle the nullptr
            if (ModelRawType *const modelPointer = toPointer(model);
                std::invoke(callback, modelPointer)
            )
                result.push_back(modelPointer);

        return result;
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::filter() const requires std::is_pointer_v<Model>
    {
        ModelsCollection<ModelRawType *> result;
        result.reserve(this->size());

        for (ModelRawType *const modelPointer : *this)
            if (modelPointer != nullptr)
                result.push_back(modelPointer);

        return result;
    }

    template<DerivedModel Model>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::first(const std::function<bool(ModelRawType *)> &callback,
                                   ModelRawType *const defaultModel)
    {
        if (this->empty())
            return defaultModel;

        for (ModelLoopType model : *this)
            // Don't handle the nullptr
            if (ModelRawType *const modelPointer = toPointer(model);
                std::invoke(callback, modelPointer)
            )
                return modelPointer;

        return defaultModel;
    }

    template<DerivedModel Model>
    QString
    ModelsCollection<Model>::implode(const QString &column, const QString &glue)
    {
        const auto valuesRaw = pluck(column);

        const auto values = valuesRaw
                | ranges::views::transform([](const QVariant &value)
        {
            // Don't handle the null and not valid
            return value.template value<QString>();
        })
                | ranges::to<QStringList>();

        return values.join(glue);
    }

    /* Collection */

    template<DerivedModel Model>
    QVector<QVariant>
    ModelsCollection<Model>::modelKeys() const
    {
        return *this
                | ranges::views::transform([](ConstModelLoopType model)
        {
            return getKey(model);
        })
                | ranges::to<QVector<QVariant>>();
    }

    template<DerivedModel Model>
    template<typename T>
    QVector<T>
    ModelsCollection<Model>::modelKeys() const
    {
        return *this
                | ranges::views::transform([](ConstModelLoopType model) -> T
        {
            return getKey(model).template value<T>();
        })
                | ranges::to<QVector<T>>();
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::map(
            const std::function<ModelRawType *(ModelRawType *, size_type)> &callback)
    {
        const auto size = this->size();

        ModelsCollection<ModelRawType *> result;
        result.reserve(size);

        for (size_type index = 0; index < size; ++index)
            result.push_back(std::invoke(callback,
                                         // Don't handle the nullptr
                                         toPointer(this->operator[](index)), index));

        return result;
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::map(
            const std::function<ModelRawType *(ModelRawType *)> &callback)
    {
        ModelsCollection<ModelRawType *> result;
        result.reserve(this->size());

        for (ModelLoopType model : *this)
            // Don't handle the nullptr
            result.push_back(std::invoke(callback, toPointer(model)));

        return result;
    }

    template<DerivedModel Model>
    std::unordered_map<typename ModelsCollection<Model>::KeyType,
                       typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::mapWithModelKeys()
    {
        return *this
                | ranges::views::transform(
                    [](ModelLoopType model) -> std::pair<KeyType, ModelRawType *>
        {
            return {getKeyCasted(model), toPointer(model)};
        })
                | ranges::to<std::unordered_map<KeyType, ModelRawType *>>();
    }

    template<DerivedModel Model>
    template<typename K, typename V>
    std::unordered_map<K, V>
    ModelsCollection<Model>::mapWithKeys(
            const std::function<std::pair<K, V>(ModelRawType *)> &callback)
    {
        // Nothing to do
        if (this->empty())
            return {};

        std::unordered_map<K, V> result;
        result.reserve(this->size());

        for (ModelLoopType model : *this)
            // Don't handle the nullptr
            result.emplace(std::invoke(callback, toPointer(model)));

        return result;
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::only(const std::unordered_set<KeyType> &ids)
    {
        // NOTE api different, Eloquent returns the whole Collection silverqx
        // Nothing to do, no IDs passed
        if (ids.empty())
            return {};

        ModelsCollection<ModelRawType *> result;
        result.reserve(static_cast<size_type>(ids.size()));

        // Looping over the *this guarantees that the order will be preserved
        for (ModelLoopType model : *this)
            if (ids.contains(getKeyCasted(model)))
                result.push_back(toPointer(model));

        return result;
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::except(const std::unordered_set<KeyType> &ids)
    {
        // Nothing to do, no IDs passed, return a copy
        if (ids.empty())
            return toPointersCollection();

        ModelsCollection<ModelRawType *> result;
        /* Substracting the ids.size() is tricky and there is no guarantee that IDs
           in the ids set will be in the ModelsCollection, so the ids set can contain
           30 IDs, but the ModelsCollection doesn't have to contain either because
           of this the size() is the best solution. */
        result.reserve(this->size());

        // Looping over the *this guarantees that the order will be preserved
        for (ModelLoopType model : *this)
            if (!ids.contains(getKeyCasted(model)))
                result.push_back(toPointer(model));

        return result;
    }

    template<DerivedModel Model>
    QVector<QVariant>
    ModelsCollection<Model>::pluck(const QString &column)
    {
        QVector<QVariant> result;
        result.reserve(this->size());

        for (ModelLoopType model : *this) {
            ModelRawType *const modelPointer = toPointer(model);

            // Don't handle the nullptr
            if (const auto &attributesHash = modelPointer->getAttributesHash();
                attributesHash.contains(column)
            )
                // Don't handle the null and not valid
                result << modelPointer->getAttributes().at(attributesHash.at(column))
                          .value;
        }

        return result;
    }

    template<DerivedModel Model>
    template<typename T>
    std::map<T, QVariant>
    ModelsCollection<Model>::pluck(const QString &column, const QString &key)
    {
        std::map<T, QVariant> result;

        for (ModelLoopType model : *this) {
            ModelRawType *const modelPointer = toPointer(model);

            // Don't handle the nullptr
            if (const auto &attributesHash = modelPointer->getAttributesHash();
                attributesHash.contains(column)
            ) {
                const auto &attributes = modelPointer->getAttributes();

                               // Don't handle the null and not valid
                result.emplace(attributes.at(attributesHash.at(key))
                                         .value.template value<T>(),
                               attributes.at(attributesHash.at(column)).value);
            }
        }

        return result;
    }

    template<DerivedModel Model>
    bool ModelsCollection<Model>::contains(const KeyType id)
    {
        return ranges::contains(*this, true, [id](ModelLoopType model)
        {
            return getKeyCasted(model) == id;
        });
    }

    template<DerivedModel Model>
    bool ModelsCollection<Model>::contains(const QVariant &id)
    {
        // Don't handle the null and not valid
        return contains(castKey(id));
    }

    template<DerivedModel Model>
    bool
    ModelsCollection<Model>::contains(
            const std::function<bool(ModelRawType *)> &callback)
    {
        for (ModelLoopType model : *this)
            // Don't handle the nullptr
            if (std::invoke(callback, toPointer(model)))
                return true;

        return false;
    }

    template<DerivedModel Model>
    bool ModelsCollection<Model>::contains(const std::optional<ModelRawType> &model)
    {
        // Early return
        if (!model)
            return false;

        return ranges::contains(*this, true, [&model](ModelLoopType modelThis)
        {
            return toPointer(modelThis)->is(model);
        });
    }

    template<DerivedModel Model>
    bool ModelsCollection<Model>::doesntContain(const KeyType id)
    {
        return !contains(id);
    }

    template<DerivedModel Model>
    bool ModelsCollection<Model>::doesntContain(const QVariant &id)
    {
        return !contains(castKey(id));
    }

    template<DerivedModel Model>
    bool
    ModelsCollection<Model>::doesntContain(
            const std::function<bool(ModelRawType *)> &callback)
    {
        return !contains(callback);
    }

    template<DerivedModel Model>
    bool ModelsCollection<Model>::doesntContain(const std::optional<ModelRawType> &model)
    {
        return !contains(model);
    }

    template<DerivedModel Model>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::find(const KeyType id, ModelRawType *const defaultModel)
    {
        for (ModelLoopType model : *this)
            if (getKeyCasted(model) == id)
                return toPointer(model);

        return defaultModel;
    }

    template<DerivedModel Model>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::find(const ModelRawType &model,
                                  ModelRawType *const defaultModel)
    {
        for (ModelLoopType modelThis : *this)
            if (getKeyCasted(modelThis) == getKeyCasted(model))
                return toPointer(modelThis);

        return defaultModel;
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::find(const std::unordered_set<KeyType> &ids)
    {
        return only(ids);
    }

    template<DerivedModel Model>
    std::unique_ptr<TinyBuilder<typename ModelsCollection<Model>::ModelRawType>>
    ModelsCollection<Model>::toQuery()
    {
        if (this->isEmpty())
            throw Orm::Exceptions::LogicError(
                    "Unable to create the query (TinyBuilder) from an empty collection.");

        // Don't handle the nullptr, look at the bottom for the reason
        auto builder = toPointer(this->first())->newModelQuery();

        builder->whereKey(modelKeys<QVariant>());

        return builder;
    }

    /* EnumeratesValues */

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::reject(
            const std::function<bool(ModelRawType *, size_type)> &callback)
    {
        const auto size = this->size();

        ModelsCollection<ModelRawType *> result;
        result.reserve(size);

        for (size_type index = 0; index < size; ++index)
            // Don't handle the nullptr
            if (ModelRawType *const modelPointer = toPointer(this->operator[](index));
                !std::invoke(callback, modelPointer, index)
            )
                result.push_back(modelPointer);

        return result;
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::reject(const std::function<bool(ModelRawType *)> &callback)
    {
        ModelsCollection<ModelRawType *> result;
        result.reserve(this->size());

        for (ModelLoopType model : *this)
            // Don't handle the nullptr
            if (ModelRawType *const modelPointer = toPointer(model);
                !std::invoke(callback, modelPointer)
            )
                result.push_back(modelPointer);

        return result;
    }

    template<DerivedModel Model>
    template<typename V>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::where(const QString &key, const QString &comparison,
                                   V value)
    {
        return filter(operatorForWhere(key, comparison, std::move(value)));
    }

    template<DerivedModel Model>
    template<typename V>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereEq(const QString &key, V value)
    {
        return filter(operatorForWhere(key, EQ, std::move(value)));
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereNull(const QString &key)
    {
        return filter([&key](ModelRawType *const model)
        {
            return model->getAttribute(key).isNull();
        });
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereNotNull(const QString &key)
    {
        return filter([&key](ModelRawType *const model)
        {
            return !model->getAttribute(key).isNull();
        });
    }

    template<DerivedModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereIn(const QString &key,
                                     const std::unordered_set<T> &values)
    {
        // Nothing to do, no values passed
        if (values.empty())
            return {};

        return filter([&key, &values](ModelRawType *const model)
        {
            return values.contains(model->getAttribute(key).template value<T>());
        });
    }

    template<DerivedModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereNotIn(const QString &key,
                                        const std::unordered_set<T> &values)
    {
        // Nothing to do, no values passed, return a copy
        if (values.empty())
            return toPointersCollection();

        return reject([&key, &values](ModelRawType *const model)
        {
            return values.contains(model->getAttribute(key).template value<T>());
        });
    }

    template<DerivedModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereBetween(
            const QString &key, const WhereBetweenCollectionItem<T> &values)
    {
        return filter([&key, &values](ModelRawType *const model)
        {
            const auto attribute = model->getAttribute(key);

            // Null or invalid attributes can't be handled in any other way anyway
            if (attribute.isNull() || !attribute.isValid())
                return false;

            const auto retrieved = attribute.template value<T>();

            return retrieved >= values.min && retrieved <= values.max;
        });
    }

    template<DerivedModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereNotBetween(
            const QString &key, const WhereBetweenCollectionItem<T> &values)
    {
        return filter([&key, &values](ModelRawType *const model)
        {
            const auto attribute = model->getAttribute(key);

            // Null or invalid attributes can't be handled in any other way anyway
            if (attribute.isNull() || !attribute.isValid())
                return false;

            const auto retrieved = attribute.template value<T>();

            return retrieved < values.min || retrieved > values.max;
        });
    }

    template<DerivedModel Model>
    template<typename V>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::firstWhere(const QString &key, const QString &comparison,
                                        V value)
    {
        return first(operatorForWhere(key, comparison, std::move(value)));
    }

    template<DerivedModel Model>
    template<typename V>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::firstWhereEq(const QString &key, V value)
    {
        return first(operatorForWhere(key, EQ, std::move(value)));
    }

    template<DerivedModel Model>
    QVariant
    ModelsCollection<Model>::value(const QString &key,
                                   const QVariant &defaultValue) const
    {
        // Nothing to do
        if (this->empty())
            return defaultValue;

        const ModelRawType *const model = toPointer(StorageType::constFirst());

        if (model == nullptr || !model->getAttributesHash().contains(key))
            return defaultValue;

        return model->getAttribute(key);
    }

    template<DerivedModel Model>
    template<typename T>
    T ModelsCollection<Model>::value(const QString &key, const T &defaultValue) const
    {
        // Nothing to do
        if (this->empty())
            return defaultValue;

        const ModelRawType *const model = toPointer(StorageType::constFirst());

        if (model == nullptr || !model->getAttributesHash().contains(key))
            return defaultValue;

        return model->getAttribute(key).template value<T>();
    }

    template<DerivedModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::each(const std::function<void(ModelRawType *)> &callback) &
    {
        for (ModelLoopType model : *this)
            std::invoke(callback, toPointer(model));

        return *this;
    }

    template<DerivedModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::each(
            const std::function<void(ModelRawType *, size_type)> &callback) &
    {
        for (size_type index = 0; index < this->size(); ++index)
            std::invoke(callback, toPointer(this->operator[](index)), index);

        return *this;
    }

    template<DerivedModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::each(const EachBoolCallbackType /*unused */,
                                  const std::function<bool(ModelRawType *)> &callback) &
    {
        for (ModelLoopType model : *this)
            if (!std::invoke(callback, toPointer(model)))
                break;

        return *this;
    }

    template<DerivedModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::each(
            const EachBoolCallbackType /*unused */,
            const std::function<bool(ModelRawType *, size_type)> &callback) &
    {
        for (size_type index = 0; index < this->size(); ++index)
            if (!std::invoke(callback, toPointer(this->operator[](index)), index))
                break;

        return *this;
    }

    template<DerivedModel Model>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::each(const std::function<void(ModelRawType *)> &callback) &&
    {
        for (ModelLoopType model : *this)
            std::invoke(callback, toPointer(model));

        return std::move(*this);
    }

    template<DerivedModel Model>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::each(
            const std::function<void(ModelRawType *, size_type)> &callback) &&
    {
        for (size_type index = 0; index < this->size(); ++index)
            std::invoke(callback, toPointer(this->operator[](index)), index);

        return std::move(*this);
    }

    template<DerivedModel Model>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::each(
            const EachBoolCallbackType /*unused */,
            const std::function<bool(ModelRawType *)> &callback) &&
    {
        for (ModelLoopType model : *this)
            if (!std::invoke(callback, toPointer(model)))
                break;

        return std::move(*this);
    }

    template<DerivedModel Model>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::each(
            const EachBoolCallbackType /*unused */,
            const std::function<bool(ModelRawType *, size_type)> &callback) &&
    {
        for (size_type index = 0; index < this->size(); ++index)
            if (!std::invoke(callback, toPointer(this->operator[](index)), index))
                break;

        return std::move(*this);
    }

    template<DerivedModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::tap(
            const std::function<void(ModelsCollection &)> &callback) &
    {
        std::invoke(callback, *this);

        return *this;
    }

    template<DerivedModel Model>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::tap(
            const std::function<void(ModelsCollection &)> &callback) &&
    {
        std::invoke(callback, *this);

        return std::move(*this);
    }

    /* protected */

    template<DerivedModel Model>
    constexpr typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::toPointer(ModelRawType *const model) noexcept
    {
        return model;
    }

    template<DerivedModel Model>
    constexpr const typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::toPointer(const ModelRawType *const model) noexcept
    {
        return model;
    }

    template<DerivedModel Model>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::toPointer(ModelRawType &model) noexcept
    {
        return &model;
    }

    template<DerivedModel Model>
    const typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::toPointer(const ModelRawType &model) noexcept
    {
        return &model;
    }

    template<DerivedModel Model>
    ModelsCollection<Model>::KeyType
    ModelsCollection<Model>::castKey(const QVariant &key)
    {
        return key.template value<KeyType>();
    }

    template<DerivedModel Model>
    ModelsCollection<Model>::KeyType
    ModelsCollection<Model>::getKeyCasted(const ModelRawType *const model)
    {
        return castKey(getKey(model));
    }

    template<DerivedModel Model>
    ModelsCollection<Model>::KeyType
    ModelsCollection<Model>::getKeyCasted(const ModelRawType &model)
    {
        return castKey(getKey(model));
    }

    template<DerivedModel Model>
    QVariant
    ModelsCollection<Model>::getKey(const ModelRawType *const model)
    {
        Q_CHECK_PTR(model);

        return model->getKey();
    }

    template<DerivedModel Model>
    QVariant
    ModelsCollection<Model>::getKey(const ModelRawType &model)
    {
        return model.getKey();
    }

    template<DerivedModel Model>
    template<typename V>
    std::function<bool(typename ModelsCollection<Model>::ModelRawType *)>
    ModelsCollection<Model>::operatorForWhere(
            const QString &key, const QString &comparison, V value) const
    {
        throwIfInvalidWhereOperator(comparison);

        return [&key, &comparison, value = std::move(value)](ModelRawType *const model)
        {
            const auto attribute = model->getAttribute(key);
            const auto retrieved = attribute.template value<V>();

            static_assert (std::is_convertible_v<decltype (retrieved), V>,
                    "The retrieved attribute (using the key) is not convertible "
                    "to the V type for comparison.");

            // Null or invalid attributes can't be handled in any other way anyway
            if (attribute.isNull() || !attribute.isValid())
                return false;

            if (comparison == EQ)
                return retrieved == value;
            if (comparison == NE)
                return retrieved != value;

            if (comparison == LT)
                return retrieved < value;
            if (comparison == GT)
                return retrieved > value;
            if (comparison == LE)
                return retrieved <= value;
            if (comparison == GE)
                return retrieved >= value;

            Q_UNREACHABLE();
        };
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::toPointersCollection() requires (!std::is_pointer_v<Model>)
    {
        ModelsCollection<ModelRawType *> result;
        result.reserve(this->size());

        for (auto &model : *this)
            result.push_back(&model);

        return result;
    }

    template<DerivedModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::toPointersCollection() const noexcept
    requires std::is_pointer_v<Model>
    {
        return *this;
    }

    template<DerivedModel Model>
    void ModelsCollection<Model>::throwIfInvalidWhereOperator(const QString &comparison)
    {
        const std::unordered_set<QString> allowedOperators {
            EQ, LT, GT, LE, GE, NE, NE_,
        };

        if (allowedOperators.contains(comparison))
            return;

        throw Orm::Exceptions::InvalidArgumentError(
                    QStringLiteral(
                        "The '%1' comparison operator is not allowed in the "
                        "ModelsCollection::where() related methods in %2().")
                    .arg(comparison, __tiny_func__));

    }

} // namespace Types

    /*! Alias for the WhereBetweenCollectionItem. */
    template<typename T>
    using WhereBetweenCollectionItem = Tiny::Types::WhereBetweenCollectionItem<T>;

    /*! Alias for the ModelsCollection. */
    template<typename Model>
    using ModelsCollection = Tiny::Types::ModelsCollection<Model>;

} // namespace Tiny

    /*! Instance of the EachBoolCallbackType. */
    inline constexpr Tiny::Types::EachBoolCallbackType EachBoolCallback {};

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TYPES_MODELSCOLLECTION_HPP

/* The nullptr-s are not handled in the ModelsCollection class because eg.
   the ModelsCollection<Post> can't contain them and the pointer version
   the ModelsCollection<Post *> returned from the Model::getRelation() related
   methods can't contain nullptr as well, these type of methods doesn't return
   vector-s with the nullptr inside.
   If you have created vector that contains nullptr-s use the filter() method
   to remove them. */
