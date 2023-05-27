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
#include <range/v3/view/reverse.hpp>
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
    template<DerivedCollectionModel Model>
    class ModelsCollection : public QVector<Model>
    {
    public:
        /*! Inherit constructors. */
        using QVector<Model>::QVector;

        /* Container related */
        /*! Determine whether the current collection contains pointers to models. */
        constexpr static auto IsPointersCollection = std::is_pointer_v<Model>;
        /*! The base class type (used as the storage container). */
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
        /*! Model type used in the for-ranged loops. */
        using ModelLoopType   = std::conditional_t<IsPointersCollection,
                                                   ModelRawType *const, ModelRawType &>;
        /*! Const Model type used in the for-ranged loops. */
        using ConstModelLoopType = std::conditional_t<IsPointersCollection,
                                                      const ModelRawType *const,
                                                      const ModelRawType &>;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        using parameter_type  = typename StorageType::parameter_type;
#else
        using parameter_type  = std::conditional_t<IsPointersCollection,
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
        ModelsCollection(const QVector<Model> &models) // NOLINT(google-explicit-constructor)
        requires (!IsPointersCollection);

        /* Comparison operators */
        /*! Equality comparison operator for the ModelsCollection. */
        bool operator==(const ModelsCollection<ModelRawType> &other) const
        requires (!IsPointersCollection);
        /*! Equality comparison operator for the ModelsCollection. */
        bool operator==(const ModelsCollection<ModelRawType *> &other) const
        requires IsPointersCollection;

        /*! Equality comparison operator for the ModelsCollection. */
        bool operator==(const ModelsCollection<ModelRawType *> &other) const
        requires (!IsPointersCollection);
        /*! Equality comparison operator for the ModelsCollection. */
        bool operator==(const ModelsCollection<ModelRawType> &other) const
        requires IsPointersCollection;

        /* Redeclared overriden methods from the base class */
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        /*! Returns a reference to the first model in the collection. */
        inline Model &first();
        /*! Returns a reference to the first model in the collection. */
        inline const Model &first() const noexcept;
        /*! Returns a sub-collection that contains the first n models of this
            collection. */
        inline ModelsCollection<Model> first(size_type count) const;

        /*! Returns a reference to the last model in the collection. */
        inline Model &last();
        /*! Returns a reference to the last model in the collection. */
        inline const Model &last() const noexcept;
        /*! Returns a sub-collection that contains the last n models of this
            collection. */
        inline ModelsCollection<Model> last(size_type count) const;

        /*! Returns the value at index position i in the collection. */
        inline Model value(size_type index) const;
        /*! Returns the value at index position i in the collection. */
        inline Model value(size_type index, parameter_type defaultValue) const;
#else
        /*! Returns a reference to the first model in the collection. */
        inline Model &first();
        /*! Returns a reference to the first model in the collection. */
        inline const Model &first() const;

        /*! Returns a reference to the last model in the collection. */
        inline Model &last();
        /*! Returns a reference to the last model in the collection. */
        inline const Model &last() const;

        /*! Returns the value at index position i in the collection. */
        inline Model value(size_type index) const;
        /*! Returns the value at index position i in the collection. */
        inline Model value(size_type index, parameter_type defaultValue) const;
#endif

        /* BaseCollection */
        /*! Run a filter over each of the models in the collection. */
        ModelsCollection<ModelRawType *>
        filter(const std::function<bool(ModelRawType *, size_type)> &callback);
        /*! Run a filter over each of the models in the collection. */
        ModelsCollection<ModelRawType *>
        filter(const std::function<bool(ModelRawType *)> &callback);
        /*! Run a filter over each of the models in the collection (removes nullptr-s). */
        ModelsCollection<ModelRawType *>
        filter() const requires IsPointersCollection;

        /*! Get a first model from the collection passing the given truth test. */
        ModelRawType *
        first(const std::function<bool(ModelRawType *)> &callback,
              ModelRawType *defaultModel = nullptr);
        /*! Get a last model from the collection passing the given truth test. */
        ModelRawType *
        last(const std::function<bool(ModelRawType *)> &callback,
             ModelRawType *defaultModel = nullptr);

        /*! Concatenate values of the given column as a string. */
        QString implode(const QString &column, const QString &glue = "");

        /*! Determine if the collection is not empty. */
        inline bool isNotEmpty() const noexcept;

        /* Collection */
        /*! Get a vector of primary keys. */
        QVector<QVariant> modelKeys() const;
        /*! Get a vector of primary keys. */
        template<typename T>
        QVector<T> modelKeys() const;

        /*! Run a map over each of the models. */
        ModelsCollection<ModelRawType *>
        map(const std::function<ModelRawType *(ModelRawType *, size_type)> &callback);
        /*! Run a map over each of the models. */
        ModelsCollection<ModelRawType *>
        map(const std::function<ModelRawType *(ModelRawType *)> &callback);

        /*! Run a map over each of the models. */
        template<typename T>
        QVector<T> map(const std::function<T(ModelRawType *, size_type)> &callback);
        /*! Run a map over each of the models. */
        template<typename T>
        QVector<T> map(const std::function<T(ModelRawType *)> &callback);

        /*! Run an associative map over each of the models (keyed by primary key). */
        std::unordered_map<KeyType, ModelRawType *> mapWithModelKeys();
        /*! Run an associative map over each of the models (key by the K template). */
        template<typename K, typename V>
        std::unordered_map<K, V>
        mapWithKeys(const std::function<std::pair<K, V>(ModelRawType *)> &callback);

        /*! Return only the models from the collection with specified primary keys. */
        ModelsCollection<ModelRawType *> only(const std::unordered_set<KeyType> &ids);
        /*! Return all models in the collection except the models with specified
            primary keys. */
        ModelsCollection<ModelRawType *> except(const std::unordered_set<KeyType> &ids);

        /*! Get a vector with the values in the given column. */
        QVector<QVariant> pluck(const QString &column);
        /*! Get a map with values in the given column and keyed by values in the key
            column (attribute). */
        template<typename T>
        std::map<T, QVariant> pluck(const QString &column, const QString &key);

        /*! Determine if the collection contains a model with the given ID. */
        inline bool contains(KeyType id);
        /*! Determine if the collection contains a model with the given ID. */
        inline bool contains(const QVariant &id);
        /*! Determine if the collection contains a model using the given callback. */
        bool contains(const std::function<bool(ModelRawType *)> &callback);
        /*! Determine if the model exists in the collection (using the Model::is()). */
        bool contains(const std::optional<ModelRawType> &model);

        /*! Determine if the collection doesn't contain a model with the given ID. */
        bool doesntContain(KeyType id);
        /*! Determine if the collection doesn't contain a model with the given ID. */
        bool doesntContain(const QVariant &id);
        /*! Determine if the collection doesn't contain a model using the given
            callback. */
        bool doesntContain(const std::function<bool(ModelRawType *)> &callback);
        /*! Determine if the model doesn't exist in the collection (using
            the Model::is()). */
        bool doesntContain(const std::optional<ModelRawType> &model);

        /*! Find a model in the collection by primary key. */
        ModelRawType *find(KeyType id, ModelRawType *defaultModel = nullptr);
        /*! Find a model in the collection by another model (using its ID). */
        ModelRawType *
        find(const ModelRawType &model, ModelRawType *defaultModel = nullptr);
        /*! Find models in the collection by the given IDs. */
        ModelsCollection<ModelRawType *>
        find(const std::unordered_set<KeyType> &ids);

        /*! Get the TinyBuilder from the collection. */
        std::unique_ptr<TinyBuilder<ModelRawType>> toQuery();

        /* Collection - Relations related */
        /*! Load a set of relationships onto the collection. */
        ModelsCollection &load(const QVector<WithItem> &relations) &;
        /*! Load a set of relationships onto the collection. */
        inline ModelsCollection &load(const QString &relation) &;

        /*! Load a set of relationships onto the collection. */
        ModelsCollection &&load(const QVector<WithItem> &relations) &&;
        /*! Load a set of relationships onto the collection. */
        inline ModelsCollection &&load(const QString &relation) &&;

        /* EnumeratesValues */
        /*! Create a collection of all models that do not pass a given truth test. */
        ModelsCollection<ModelRawType *>
        reject(const std::function<bool(ModelRawType *, size_type)> &callback);
        /*! Create a collection of all models that do not pass a given truth test. */
        ModelsCollection<ModelRawType *>
        reject(const std::function<bool(ModelRawType *)> &callback);

        /*! Filter models by the given column value pair. */
        template<typename V>
        ModelsCollection<ModelRawType *>
        where(const QString &column, const QString &comparison, V value);
        /*! Filter models by the given column value pair. */
        template<typename V>
        ModelsCollection<ModelRawType *> whereEq(const QString &column, V value);

        /*! Filter models where the value for the given column is the null QVariant. */
        ModelsCollection<ModelRawType *> whereNull(const QString &column);
        /*! Filter models where the value for the given column is not the null
            QVariant. */
        ModelsCollection<ModelRawType *> whereNotNull(const QString &column);

        /*! Filter models by the given column values pair. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        whereIn(const QString &column, const std::unordered_set<T> &values);
        /*! Filter models by the given column values pair. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        whereNotIn(const QString &column, const std::unordered_set<T> &values);

        /*! Filter models such that the value of the given column is between the given
            values. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        whereBetween(const QString &column, const WhereBetweenCollectionItem<T> &values);
        /*! Filter models such that the value of the given column is not between
            the given values. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        whereNotBetween(const QString &column,
                        const WhereBetweenCollectionItem<T> &values);

        /*! Get the first model by the given column value pair. */
        template<typename V>
        ModelRawType *
        firstWhere(const QString &column, const QString &comparison, V value);
        /*! Get the first model by the given column value pair. */
        template<typename V>
        ModelRawType *firstWhereEq(const QString &column, V value);

        /*! Get a single column value from the first matching model in the collection. */
        QVariant value(const QString &column, const QVariant &defaultValue = {}) const;
        /*! Get a single column value from the first matching model in the collection. */
        template<typename T>
        T value(const QString &column, const T &defaultValue = {}) const;

        /*! Execute a callback over each model. */
        ModelsCollection &
        each(const std::function<void(ModelRawType *)> &callback) &;
        /*! Execute a callback over each model. */
        ModelsCollection &
        each(const std::function<void(ModelRawType *, size_type)> &callback) &;
        /*! Execute a callback over each model. */
        ModelsCollection &
        each(EachBoolCallbackType tag,
             const std::function<bool(ModelRawType *)> &callback) &;
        /*! Execute a callback over each model. */
        ModelsCollection &
        each(EachBoolCallbackType tag,
             const std::function<bool(ModelRawType *, size_type)> &callback) &;

        /*! Execute a callback over each model. */
        ModelsCollection &&
        each(const std::function<void(ModelRawType *)> &callback) &&;
        /*! Execute a callback over each model. */
        ModelsCollection &&
        each(const std::function<void(ModelRawType *, size_type)> &callback) &&;
        /*! Execute a callback over each model. */
        ModelsCollection &&
        each(EachBoolCallbackType tag,
             const std::function<bool(ModelRawType *)> &callback) &&;
        /*! Execute a callback over each model. */
        ModelsCollection &&
        each(EachBoolCallbackType tag,
             const std::function<bool(ModelRawType *, size_type)> &callback) &&;

        /*! Pass the collection to the given callback and then return it. */
        ModelsCollection &
        tap(const std::function<void(ModelsCollection &)> &callback) &;
        /*! Pass the collection to the given callback and then return it. */
        ModelsCollection &&
        tap(const std::function<void(ModelsCollection &)> &callback) &&;

    protected:
        /*! Convert the Model pointer to the pointer (no-op). */
        constexpr static ModelRawType *toPointer(ModelRawType *model);
        /*! Convert the Model pointer to the pointer (no-op). */
        constexpr static const ModelRawType *
        toPointer(const ModelRawType *model);
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
        static QVariant getKey(const ModelRawType *model);
        /*! Get the value of the model's primary key. */
        inline static QVariant getKey(const ModelRawType &model);

        /*! Get an operator checker callback. */
        template<typename V>
        std::function<bool(ModelRawType *)>
        operatorForWhere(const QString &column, const QString &comparison, V value) const;

        /*! Convert the Collection<ModelRawType> to the Collection<ModelRawType *>. */
        ModelsCollection<ModelRawType *>
        toPointersCollection() requires (!IsPointersCollection);
        /*! Convert to the Collection<ModelRawType *>, return itself (no-op overload). */
        inline ModelsCollection<ModelRawType *>
        toPointersCollection() const noexcept requires IsPointersCollection;

        /*! Throw if the given operator is not valid for the where() method. */
        static void throwIfInvalidWhereOperator(const QString &comparison);
    };

    /* public */

    /* Constructors */

    template<DerivedCollectionModel Model>
    ModelsCollection<Model>::ModelsCollection(const QVector<Model> &models)
    requires (!IsPointersCollection)
    {
        for (const auto &model : models)
            this->push_back(model);
    }

    /* Comparison operators */

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::operator==(
            const ModelsCollection<ModelRawType> &other) const
    requires (!IsPointersCollection)
    {
        const auto size = this->size();

        // Nothing to compare, if the size of collections differ
        if (size != other.size())
            return false;

        for (size_type index = 0; index < size; ++index) {
            const ModelRawType &model = this->at(index);
            const ModelRawType &otherModel = other.at(index);

            /* First compare pointer addresses; if they are the same, then it's at 100%
               the same model; if pointer addresses differ, then compare models using
               the operator==(). */
            if (std::addressof(model) != std::addressof(otherModel) &&
                model != otherModel
            )
                return false;
        }

        return true;
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::operator==(
            const ModelsCollection<ModelRawType *> &other) const
    requires IsPointersCollection
    {
        const auto size = this->size();

        // Nothing to compare, if the size of collections differ
        if (size != other.size())
            return false;

        for (size_type index = 0; index < size; ++index) {
            const ModelRawType *const model = this->at(index);
            const ModelRawType *const otherModel = other.at(index);

            /* First compare pointer addresses; if they are the same, then it's at 100%
               the same model; if pointer addresses differ, then compare models using
               the operator==(). */
            if (model != otherModel && *model != *otherModel)
                return false;
        }

        return true;
    }

    /* The following two overloads allow comparing of:
       ModelsCollection<Model>   == ModelsCollection<Model *>
       ModelsCollection<Model *> == ModelsCollection<Model> */

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::operator==(
            const ModelsCollection<ModelRawType *> &other) const
    requires (!IsPointersCollection)
    {
        const auto size = this->size();

        // Nothing to compare, if the size of collections differ
        if (size != other.size())
            return false;

        for (size_type index = 0; index < size; ++index) {
            const ModelRawType &model = this->at(index);
            const ModelRawType *const otherModel = other.at(index);

            /* First compare pointer addresses; if they are the same, then it's at 100%
               the same model; if pointer addresses differ, then compare models using
               the operator==(). */
            if (std::addressof(model) != otherModel && model != *otherModel)
                return false;
        }

        return true;
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::operator==(
            const ModelsCollection<ModelRawType> &other) const
    requires IsPointersCollection
    {
        const auto size = this->size();

        // Nothing to compare, if the size of collections differ
        if (size != other.size())
            return false;

        for (size_type index = 0; index < size; ++index) {
            const ModelRawType *const model = this->at(index);
            const ModelRawType &otherModel = other.at(index);

            /* First compare pointer addresses; if they are the same, then it's at 100%
               the same model; if pointer addresses differ, then compare models using
               the operator==(). */
            if (model != std::addressof(otherModel) && *model != otherModel)
                return false;
        }

        return true;
    }

    /* Redeclared overriden methods from the base class */

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    template<DerivedCollectionModel Model>
    Model &
    ModelsCollection<Model>::first()
    {
        return StorageType::first();
    }

    template<DerivedCollectionModel Model>
    const Model &
    ModelsCollection<Model>::first() const noexcept
    {
        return StorageType::first();
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model>
    ModelsCollection<Model>::first(const size_type count) const
    {
        return StorageType::first(count);
    }

    template<DerivedCollectionModel Model>
    Model &
    ModelsCollection<Model>::last()
    {
        return StorageType::last();
    }

    template<DerivedCollectionModel Model>
    const Model &
    ModelsCollection<Model>::last() const noexcept
    {
        return StorageType::last();
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model>
    ModelsCollection<Model>::last(const size_type count) const
    {
        return StorageType::last(count);
    }

    template<DerivedCollectionModel Model>
    Model
    ModelsCollection<Model>::value(const size_type index) const
    {
        return StorageType::value(index);
    }

    template<DerivedCollectionModel Model>
    Model
    ModelsCollection<Model>::value(const size_type index,
                                   parameter_type defaultValue) const
    {
        return StorageType::value(index, defaultValue);
    }
#else
    template<DerivedCollectionModel Model>
    Model &
    ModelsCollection<Model>::first()
    {
        return StorageType::first();
    }

    template<DerivedCollectionModel Model>
    const Model &
    ModelsCollection<Model>::first() const
    {
        return StorageType::first();
    }

    template<DerivedCollectionModel Model>
    Model &
    ModelsCollection<Model>::last()
    {
        return StorageType::last();
    }

    template<DerivedCollectionModel Model>
    const Model &
    ModelsCollection<Model>::last() const
    {
        return StorageType::last();
    }

    template<DerivedCollectionModel Model>
    Model
    ModelsCollection<Model>::value(const size_type index) const
    {
        return StorageType::value(index);
    }

    template<DerivedCollectionModel Model>
    Model
    ModelsCollection<Model>::value(const size_type index,
                                   parameter_type defaultValue) const
    {
        return StorageType::value(index, defaultValue);
    }
#endif

    /* BaseCollection */

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::filter() const requires IsPointersCollection
    {
        ModelsCollection<ModelRawType *> result;
        result.reserve(this->size());

        for (ModelRawType *const modelPointer : *this)
            if (modelPointer != nullptr)
                result.push_back(modelPointer);

        return result;
    }

    template<DerivedCollectionModel Model>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::first(const std::function<bool(ModelRawType *)> &callback,
                                   ModelRawType *const defaultModel)
    {
        if (this->isEmpty())
            return defaultModel;

        for (ModelLoopType model : *this)
            // Don't handle the nullptr
            if (ModelRawType *const modelPointer = toPointer(model);
                std::invoke(callback, modelPointer)
            )
                return modelPointer;

        return defaultModel;
    }

    template<DerivedCollectionModel Model>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::last(const std::function<bool(ModelRawType *)> &callback,
                                  ModelRawType *const defaultModel)
    {
        if (this->isEmpty())
            return defaultModel;

        for (ModelLoopType model : *this | ranges::views::reverse)
            // Don't handle the nullptr
            if (ModelRawType *const modelPointer = toPointer(model);
                std::invoke(callback, modelPointer)
            )
                return modelPointer;

        return defaultModel;
    }

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::isNotEmpty() const noexcept
    {
        return !StorageType::isEmpty();
    }

    /* Collection */

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
    template<typename T>
    QVector<T>
    ModelsCollection<Model>::map(
            const std::function<T(ModelRawType *, size_type)> &callback)
    {
        const auto size = this->size();

        QVector<T> result;
        result.reserve(size);

        for (size_type index = 0; index < size; ++index)
            result.emplace_back(std::invoke(callback,
                                            // Don't handle the nullptr
                                            toPointer(this->operator[](index)), index));

        return result;
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    QVector<T>
    ModelsCollection<Model>::map(const std::function<T(ModelRawType *)> &callback)
    {
        QVector<T> result;
        result.reserve(this->size());

        for (ModelLoopType model : *this)
            // Don't handle the nullptr
            result.emplace_back(std::invoke(callback, toPointer(model)));

        return result;
    }

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
    template<typename K, typename V>
    std::unordered_map<K, V>
    ModelsCollection<Model>::mapWithKeys(
            const std::function<std::pair<K, V>(ModelRawType *)> &callback)
    {
        // Nothing to do
        if (this->isEmpty())
            return {};

        std::unordered_map<K, V> result;
        result.reserve(this->size());

        for (ModelLoopType model : *this)
            // Don't handle the nullptr
            result.emplace(std::invoke(callback, toPointer(model)));

        return result;
    }

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
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
                result.insert_or_assign(attributes.at(attributesHash.at(key))
                                                  .value.template value<T>(),
                                        attributes.at(attributesHash.at(column)).value);
            }
        }

        return result;
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::contains(const KeyType id)
    {
        return ranges::contains(*this, true, [id](ModelLoopType model)
        {
            return getKeyCasted(model) == id;
        });
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::contains(const QVariant &id)
    {
        // Don't handle the null and not valid
        return contains(castKey(id));
    }

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::doesntContain(const KeyType id)
    {
        return !contains(id);
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::doesntContain(const QVariant &id)
    {
        return !contains(castKey(id));
    }

    template<DerivedCollectionModel Model>
    bool
    ModelsCollection<Model>::doesntContain(
            const std::function<bool(ModelRawType *)> &callback)
    {
        return !contains(callback);
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::doesntContain(const std::optional<ModelRawType> &model)
    {
        return !contains(model);
    }

    template<DerivedCollectionModel Model>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::find(const KeyType id, ModelRawType *const defaultModel)
    {
        for (ModelLoopType model : *this)
            if (getKeyCasted(model) == id)
                return toPointer(model);

        return defaultModel;
    }

    template<DerivedCollectionModel Model>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::find(const ModelRawType &model,
                                  ModelRawType *const defaultModel)
    {
        for (ModelLoopType modelThis : *this)
            if (getKeyCasted(modelThis) == getKeyCasted(model))
                return toPointer(modelThis);

        return defaultModel;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::find(const std::unordered_set<KeyType> &ids)
    {
        return only(ids);
    }

    template<DerivedCollectionModel Model>
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

    /* Collection - Relations related */

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::load(const QVector<WithItem> &relations) &
    {
        // Nothing to do
        if (this->isEmpty())
            return *this;

        // Don't handle the nullptr
        // Ownership of a unique_ptr()
        auto builder = toPointer(first())->newQueryWithoutRelationships();

        builder->with(relations).eagerLoadRelations(*this);

        return *this;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::load(const QString &relation) &
    {
        return load(QVector<WithItem> {{relation}});
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::load(const QVector<WithItem> &relations) &&
    {
        // Nothing to do
        if (this->isEmpty())
            return std::move(*this);

        // Don't handle the nullptr
        // Ownership of a unique_ptr()
        auto builder = toPointer(first())->newQueryWithoutRelationships();

        builder->with(relations).eagerLoadRelations(*this);

        return std::move(*this);
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::load(const QString &relation) &&
    {
        return std::move(*this).load(QVector<WithItem> {{relation}});
    }

    /* EnumeratesValues */

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
    template<typename V>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::where(const QString &column, const QString &comparison,
                                   V value)
    {
        return filter(operatorForWhere(column, comparison, std::move(value)));
    }

    template<DerivedCollectionModel Model>
    template<typename V>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereEq(const QString &column, V value)
    {
        return filter(operatorForWhere(column, EQ, std::move(value)));
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereNull(const QString &column)
    {
        return filter([&column](ModelRawType *const model)
        {
            return model->getAttribute(column).isNull();
        });
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereNotNull(const QString &column)
    {
        return filter([&column](ModelRawType *const model)
        {
            return !model->getAttribute(column).isNull();
        });
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereIn(const QString &column,
                                     const std::unordered_set<T> &values)
    {
        // Nothing to do, no values passed
        if (values.empty())
            return {};

        return filter([&column, &values](ModelRawType *const model)
        {
            return values.contains(model->template getAttribute<T>(column));
        });
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereNotIn(const QString &column,
                                        const std::unordered_set<T> &values)
    {
        // Nothing to do, no values passed, return a copy
        if (values.empty())
            return toPointersCollection();

        return reject([&column, &values](ModelRawType *const model)
        {
            return values.contains(model->template getAttribute<T>(column));
        });
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereBetween(
            const QString &column, const WhereBetweenCollectionItem<T> &values)
    {
        return filter([&column, &values](ModelRawType *const model)
        {
            const auto attribute = model->getAttribute(column);

            // Null or invalid attributes can't be handled in any other way anyway
            if (attribute.isNull() || !attribute.isValid())
                return false;

            const auto retrieved = attribute.template value<T>();

            return retrieved >= values.min && retrieved <= values.max;
        });
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereNotBetween(
            const QString &column, const WhereBetweenCollectionItem<T> &values)
    {
        return filter([&column, &values](ModelRawType *const model)
        {
            const auto attribute = model->getAttribute(column);

            // Null or invalid attributes can't be handled in any other way anyway
            if (attribute.isNull() || !attribute.isValid())
                return false;

            const auto retrieved = attribute.template value<T>();

            return retrieved < values.min || retrieved > values.max;
        });
    }

    template<DerivedCollectionModel Model>
    template<typename V>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::firstWhere(const QString &column, const QString &comparison,
                                        V value)
    {
        return first(operatorForWhere(column, comparison, std::move(value)));
    }

    template<DerivedCollectionModel Model>
    template<typename V>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::firstWhereEq(const QString &column, V value)
    {
        return first(operatorForWhere(column, EQ, std::move(value)));
    }

    template<DerivedCollectionModel Model>
    QVariant
    ModelsCollection<Model>::value(const QString &column,
                                   const QVariant &defaultValue) const
    {
        // Nothing to do
        if (this->isEmpty())
            return defaultValue;

        const ModelRawType *const model = toPointer(StorageType::constFirst());

        // Only one place where the nullptr is checked, it's needed
        if (model == nullptr || !model->getAttributesHash().contains(column))
            return defaultValue;

        return model->getAttribute(column);
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    T ModelsCollection<Model>::value(const QString &column, const T &defaultValue) const
    {
        // Nothing to do
        if (this->isEmpty())
            return defaultValue;

        const ModelRawType *const model = toPointer(StorageType::constFirst());

        // Only one place where the nullptr is checked, it's needed
        if (model == nullptr || !model->getAttributesHash().contains(column))
            return defaultValue;

        return model->template getAttribute<T>(column);
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::each(const std::function<void(ModelRawType *)> &callback) &
    {
        for (ModelLoopType model : *this)
            std::invoke(callback, toPointer(model));

        return *this;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::each(
            const std::function<void(ModelRawType *, size_type)> &callback) &
    {
        for (size_type index = 0; index < this->size(); ++index)
            std::invoke(callback, toPointer(this->operator[](index)), index);

        return *this;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::each(const EachBoolCallbackType /*unused */,
                                  const std::function<bool(ModelRawType *)> &callback) &
    {
        for (ModelLoopType model : *this)
            if (!std::invoke(callback, toPointer(model)))
                break;

        return *this;
    }

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::each(const std::function<void(ModelRawType *)> &callback) &&
    {
        for (ModelLoopType model : *this)
            std::invoke(callback, toPointer(model));

        return std::move(*this);
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::each(
            const std::function<void(ModelRawType *, size_type)> &callback) &&
    {
        for (size_type index = 0; index < this->size(); ++index)
            std::invoke(callback, toPointer(this->operator[](index)), index);

        return std::move(*this);
    }

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
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

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::tap(
            const std::function<void(ModelsCollection &)> &callback) &
    {
        std::invoke(callback, *this);

        return *this;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::tap(
            const std::function<void(ModelsCollection &)> &callback) &&
    {
        std::invoke(callback, *this);

        return std::move(*this);
    }

    /* protected */

    template<DerivedCollectionModel Model>
    constexpr typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::toPointer(ModelRawType *const model)
    {
        // I don't have enough courage to remove this
        Q_CHECK_PTR(model);

        return model;
    }

    template<DerivedCollectionModel Model>
    constexpr const typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::toPointer(const ModelRawType *const model)
    {
        // I don't have enough courage to remove this
        Q_CHECK_PTR(model);

        return model;
    }

    template<DerivedCollectionModel Model>
    typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::toPointer(ModelRawType &model) noexcept
    {
        return &model;
    }

    template<DerivedCollectionModel Model>
    const typename ModelsCollection<Model>::ModelRawType *
    ModelsCollection<Model>::toPointer(const ModelRawType &model) noexcept
    {
        return &model;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model>::KeyType
    ModelsCollection<Model>::castKey(const QVariant &key)
    {
        return key.template value<KeyType>();
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model>::KeyType
    ModelsCollection<Model>::getKeyCasted(const ModelRawType *const model)
    {
        return castKey(getKey(model));
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model>::KeyType
    ModelsCollection<Model>::getKeyCasted(const ModelRawType &model)
    {
        return castKey(getKey(model));
    }

    template<DerivedCollectionModel Model>
    QVariant
    ModelsCollection<Model>::getKey(const ModelRawType *const model)
    {
        Q_CHECK_PTR(model);

        return model->getKey();
    }

    template<DerivedCollectionModel Model>
    QVariant
    ModelsCollection<Model>::getKey(const ModelRawType &model)
    {
        return model.getKey();
    }

    template<DerivedCollectionModel Model>
    template<typename V>
    std::function<bool(typename ModelsCollection<Model>::ModelRawType *)>
    ModelsCollection<Model>::operatorForWhere(
            const QString &column, const QString &comparison, V value) const
    {
        throwIfInvalidWhereOperator(comparison);

        return [&column, &comparison, value = std::move(value)]
               (ModelRawType *const model)
        {
            const auto attribute = model->getAttribute(column);
            const auto retrieved = attribute.template value<V>();

            static_assert (std::is_convertible_v<decltype (retrieved), V>,
                    "The retrieved attribute value (using the key) is not convertible "
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

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::toPointersCollection() requires (!IsPointersCollection)
    {
        ModelsCollection<ModelRawType *> result;
        result.reserve(this->size());

        for (auto &model : *this)
            result.push_back(&model);

        return result;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::toPointersCollection() const noexcept
    requires IsPointersCollection
    {
        return *this;
    }

    template<DerivedCollectionModel Model>
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
   methods can't contain nullptr as well, these type of methods don't return
   vector-s with the nullptr-s inside.
   If you have created vector that contains nullptr-s use the filter() method
   to remove them. */
