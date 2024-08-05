#pragma once
#ifndef ORM_TINY_TYPES_MODELSCOLLECTION_HPP
#define ORM_TINY_TYPES_MODELSCOLLECTION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QJsonArray>
#include <QJsonDocument>

#include <unordered_map>

#include <range/v3/action/erase.hpp>
#include <range/v3/algorithm/contains.hpp>
#include <range/v3/algorithm/stable_sort.hpp>
#include <range/v3/algorithm/unique.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>

#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/tiny/utils/attribute.hpp"
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

    /*! Function object for performing models comparison. */
    struct ModelsLess
    {
        /*! Accepts arguments of arbitrary types and uses perfect forwarding. */
        using is_transparent = void;

        /*! Compare models call operator. */
        template<typename T, typename U>
        requires ranges::totally_ordered_with<T, U>
        constexpr bool operator()(T &&left, U &&right) const
        noexcept(noexcept(std::forward<T>(left) < std::forward<U>(right)))
        {
            using V = std::remove_reference_t<T>;

            if constexpr (std::is_pointer_v<V> &&
                          std::is_base_of_v<IsModel, std::remove_pointer_t<V>>
            )
                return *left < *right;

            else
                return std::forward<T>(left) < std::forward<U>(right);
        }
    };

    /*! Models collection (QList) with additional handy methods. */
    template<DerivedCollectionModel Model>
    class ModelsCollection : public QList<Model> // NOLINT(misc-no-recursion)
    {
        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;

    public:
        /*! Inherit constructors. */
        using QList<Model>::QList;

        /* Container related */
        /*! Determine whether the current collection contains pointers to models. */
        constexpr static auto IsPointersCollection = std::is_pointer_v<Model>;
        /*! The base class type (used as the storage container). */
        using StorageType     = QList<Model>;

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
        using ModelLoopType   = std::conditional_t<std::is_pointer_v<Model>,
                                                   ModelRawType *const, ModelRawType &>;
        /*! Const Model type used in the for-ranged loops. */
        using ConstModelLoopType = std::conditional_t<std::is_pointer_v<Model>,
                                                      const ModelRawType *const,
                                                      const ModelRawType &>;
        using parameter_type  = typename StorageType::parameter_type;

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
        /* From QList<Model> */
        /*! Converting constructor from the QList<Model>. */
        ModelsCollection(const QList<Model> &models) // NOLINT(google-explicit-constructor)
        requires (!std::is_pointer_v<Model>);
        /*! Converting constructor from the QList<Model>. */
        ModelsCollection(QList<Model> &&models) noexcept // NOLINT(google-explicit-constructor)
        requires (!std::is_pointer_v<Model>);

        /*! Converting copy assignment operator from the QList<Model>. */
        ModelsCollection<Model> &operator=(const QList<Model> &models)
        requires (!std::is_pointer_v<Model>);
        /*! Converting move assignment operator from the QList<Model>. */
        ModelsCollection<Model> &operator=(QList<Model> &&models) noexcept
        requires (!std::is_pointer_v<Model>);

        /* To ModelsCollection<Model> */
        /*! Converting method to the ModelsCollection<ModelRawType>. */
        inline ModelsCollection<ModelRawType> toModels() &
        requires std::is_pointer_v<Model>;
        /*! Converting method to the ModelsCollection<ModelRawType>. */
        inline ModelsCollection<ModelRawType> toModels() &&
        requires std::is_pointer_v<Model>;

        /* To ModelsCollection<Model *> */
        /*! Converting method to the ModelsCollection<ModelRawType *>. */
        inline ModelsCollection<ModelRawType *> toPointers()
        requires (!std::is_pointer_v<Model>);

        /* Comparison operators */
        /*! Equality comparison operator for the ModelsCollection. */
        bool operator==(const ModelsCollection<ModelRawType> &other) const // NOLINT(misc-no-recursion)
        requires (!std::is_pointer_v<Model>);
        /*! Equality comparison operator for the ModelsCollection. */
        bool operator==(const ModelsCollection<ModelRawType *> &other) const // NOLINT(misc-no-recursion)
        requires std::is_pointer_v<Model>;

        /*! Equality comparison operator for the ModelsCollection. */
        bool operator==(const ModelsCollection<ModelRawType *> &other) const // NOLINT(misc-no-recursion)
        requires (!std::is_pointer_v<Model>);
        /*! Equality comparison operator for the ModelsCollection. */
        bool operator==(const ModelsCollection<ModelRawType> &other) const // NOLINT(misc-no-recursion)
        requires std::is_pointer_v<Model>;

        /* Redeclared overridden methods from the base class */
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

        /* BaseCollection */
        /*! Run a filter over each of the models in the collection. */
        ModelsCollection<ModelRawType *>
        filter(const std::function<bool(ModelRawType *, size_type)> &callback);
        /*! Run a filter over each of the models in the collection. */
        ModelsCollection<ModelRawType *>
        filter(const std::function<bool(ModelRawType *)> &callback);
        /*! Run a filter over each of the models in the collection (removes nullptr-s). */
        ModelsCollection<ModelRawType *>
        filter() const requires std::is_pointer_v<Model>;

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
        /*! Get a base vector instance from this collection.*/
        inline QList<Model> toBase() const;
        /*! Get all of the models in the collection. */
        QList<Model> all() const;

        /*! Get a vector of primary keys. */
        QList<QVariant> modelKeys() const;
        /*! Get a vector of primary keys. */
        template<typename T>
        QList<T> modelKeys() const;

        /*! Run a map over each of the models. */
        ModelsCollection<ModelRawType>
        map(const std::function<ModelRawType(ModelRawType &&modelCopy,
                                             size_type)> &callback) const;
        /*! Run a map over each of the models. */
        ModelsCollection<ModelRawType>
        map(const std::function<ModelRawType(ModelRawType &&modelCopy)> &callback) const;

        /*! Run a map over each of the models. */
        template<typename T>
        QList<T> map(const std::function<T(ModelRawType &&modelCopy,
                                           size_type)> &callback) const;
        /*! Run a map over each of the models. */
        template<typename T>
        QList<T> map(const std::function<T(ModelRawType &&modelCopy)> &callback) const;

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
        QList<QVariant> pluck(const QString &column) const;
        /*! Get a vector with the values in the given column. */
        template<typename T>
        QList<T> pluck(const QString &column) const;
        /*! Get a map with values in the given column and keyed by values in the key
            column (attribute). */
        template<typename T>
        std::map<T, QVariant> pluck(const QString &column, const QString &key) const;

        /*! Determine if the collection contains a model with the given ID. */
        inline bool contains(KeyType id) const;
        /*! Determine if the collection contains a model with the given ID. */
        inline bool contains(const QVariant &id) const;
        /*! Determine if the collection contains a model using the given callback. */
        bool contains(const std::function<bool(ModelRawType *)> &callback);
        /*! Determine if the model exists in the collection (using the Model::is()). */
        bool contains(const std::optional<ModelRawType> &model) const;

        /*! Determine if the collection doesn't contain a model with the given ID. */
        bool doesntContain(KeyType id) const;
        /*! Determine if the collection doesn't contain a model with the given ID. */
        bool doesntContain(const QVariant &id) const;
        /*! Determine if the collection doesn't contain a model using the given
            callback. */
        bool
        doesntContain(const std::function<bool(ModelRawType *)> &callback);
        /*! Determine if the model doesn't exist in the collection (using
            the Model::is()). */
        bool doesntContain(const std::optional<ModelRawType> &model) const;

        /*! Find a model in the collection by primary key. */
        ModelRawType *find(KeyType id, ModelRawType *defaultModel = nullptr);
        /*! Find a model in the collection by another model (using its ID). */
        ModelRawType *
        find(const ModelRawType &model, ModelRawType *defaultModel = nullptr);
        /*! Find models in the collection by the given IDs. */
        ModelsCollection<ModelRawType *>
        find(const std::unordered_set<KeyType> &ids);

        /*! Sort the collection by the given comparison callback and projection. */
        template<typename C = ModelsLess, typename P = ranges::identity>
        ModelsCollection<ModelRawType *>
        sort(C comparison = C{}, P projection = P{}, bool descending = false);
        /*! Sort the collection by the given comparison callback and projection
            in descending order. */
        template<typename C = ModelsLess, typename P = ranges::identity>
        ModelsCollection<ModelRawType *>
        sortDesc(C comparison = C{}, P projection = P{});

        /*! Sort the collection by the given column. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        sortBy(const QString &column, bool descending = false);
        /*! Sort the collection by the given column in descending order. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        sortByDesc(const QString &column);

        /*! Sort the collection by the given callback (supports multi-columns sorting). */
        ModelsCollection<ModelRawType *>
        sortBy(const QList<std::function<
                           std::strong_ordering(ModelRawType *,
                                                ModelRawType *)>> &callbacks);

        /*! Sort the collection using the given projection. */
        template<typename P>
        ModelsCollection<ModelRawType *>
        sortBy(P projection, bool descending = false);
        /*! Sort the collection using the given projection in descending order. */
        template<typename P>
        ModelsCollection<ModelRawType *>
        sortByDesc(P projection);

        /*! Stable sort the collection by the given comparison callback and projection. */
        template<typename C = ModelsLess, typename P = ranges::identity>
        ModelsCollection<ModelRawType *>
        stableSort(C comparison = C{}, P projection = P{}, bool descending = false);
        /*! Stable sort the collection by the given comparison callback and projection
            in descending order. */
        template<typename C = ModelsLess, typename P = ranges::identity>
        ModelsCollection<ModelRawType *>
        stableSortDesc(C comparison = C{}, P projection = P{});

        /*! Stable sort the collection by the given column. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        stableSortBy(const QString &column, bool descending = false);
        /*! Stable sort the collection by the given column in descending order. */
        template<typename T>
        ModelsCollection<ModelRawType *>
        stableSortByDesc(const QString &column);

        /*! Sort the collection by the given callback (supports multi-columns sorting). */
        ModelsCollection<ModelRawType *>
        stableSortBy(const QList<std::function<
                                 std::strong_ordering(ModelRawType *,
                                                      ModelRawType *)>> &callbacks);

        /*! Stable sort the collection using the given projection. */
        template<typename P>
        ModelsCollection<ModelRawType *>
        stableSortBy(P projection, bool descending = false);
        /*! Stable sort the collection using the given projection in descending order. */
        template<typename P>
        ModelsCollection<ModelRawType *>
        stableSortByDesc(P projection);

        /*! Return only the unique models from the SORTED collection. */
        ModelsCollection<ModelRawType *> unique(bool sort = true);
        /*! Return only the unique models from the SORTED collection by the given
            column. */
        template<typename T>
        ModelsCollection<ModelRawType *> uniqueBy(const QString &column,
                                                  bool sort = true);

        /*! Return only the unique models from the collection. */
        ModelsCollection<ModelRawType *> uniqueRelaxed();
        /*! Return only the unique models from the collection by the given column. */
        template<typename T>
        ModelsCollection<ModelRawType *> uniqueRelaxedBy(const QString &column);

        /*! Get the TinyBuilder from the collection. */
        std::unique_ptr<TinyBuilder<ModelRawType>> toQuery() const;

        /* Collection - Relations related */
        /*! Reload a fresh model instance from the database for all the entities. */
        template<typename = void>
        ModelsCollection<ModelRawType> fresh(const QList<WithItem> &relations = {});
        /*! Reload a fresh model instance from the database for all the entities. */
        template<typename = void>
        ModelsCollection<ModelRawType> fresh(QString relation);
        /*! Reload a fresh model instance from the database for all the entities. */
        ModelsCollection<ModelRawType> fresh(const QList<QString> &relations);
        /*! Reload a fresh model instance from the database for all the entities. */
        ModelsCollection<ModelRawType> fresh(QList<QString> &&relations);

        /*! Load a set of relationships onto the collection. */
        template<typename = void>
        ModelsCollection &load(const QList<WithItem> &relations) &;
        /*! Load a set of relationships onto the collection. */
        template<typename = void>
        ModelsCollection &load(QString relation) &;
        /*! Load a set of relationships onto the collection. */
        inline ModelsCollection &load(const QList<QString> &relations) &;
        /*! Load a set of relationships onto the collection. */
        inline ModelsCollection &load(QList<QString> &&relations) &;

        /*! Load a set of relationships onto the collection. */
        template<typename = void>
        ModelsCollection &&load(const QList<WithItem> &relations) &&;
        /*! Load a set of relationships onto the collection. */
        template<typename = void>
        ModelsCollection &&load(QString relation) &&;
        /*! Load a set of relationships onto the collection. */
        inline ModelsCollection &load(const QList<QString> &relations) &&;
        /*! Load a set of relationships onto the collection. */
        inline ModelsCollection &load(QList<QString> &&relations) &&;

        /* EnumeratesValues */
        /*! Get the vector of models as a attributes vector with serialized models. */
        template<typename PivotType = void> // PivotType is primarily internal
        QList<QList<AttributeItem>> toList() const;
        /*! Get the vector of models as a variant map with serialized models. */
        template<typename PivotType = void> // PivotType is primarily internal
        QList<QVariantMap> toMap() const;

        /*! Get the vector of models as the variant (variant map inside) with serialized
            models (used by toJson()). */
        template<typename PivotType = void> // PivotType is primarily internal
        QVariantList toListVariantList() const;
        /*! Get the vector of models as the variant (variant map inside) with serialized
            models (used by toJson()). */
        template<typename PivotType = void> // PivotType is primarily internal
        QVariantList toMapVariantList() const;

        /*! Convert a collection to QJsonArray. */
        template<typename PivotType = void>
        QJsonArray toJsonArray() const;
        /*! Convert a collection to QJsonDocument. */
        template<typename PivotType = void>
        QJsonDocument toJsonDocument() const;
        /*! Convert a collection to JSON. */
        template<typename PivotType = void> // PivotType is primarily internal
        inline QByteArray
        toJson(QJsonDocument::JsonFormat format = QJsonDocument::Compact) const;

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
        std::function<bool(const ModelRawType *)>
        operatorForWhere(const QString &column, const QString &comparison, V value) const;

        /*! Convert the Collection<ModelRawType> to the Collection<ModelRawType *>. */
        ModelsCollection<ModelRawType *>
        toPointersCollection() requires (!std::is_pointer_v<Model>);
        /*! Convert to the Collection<ModelRawType *>, return itself (no-op). */
        inline ModelsCollection<ModelRawType *>
        toPointersCollection() const noexcept requires std::is_pointer_v<Model>;

        /*! Return a model copy. */
        inline static ModelRawType getModelCopy(const ModelRawType &model);
        /*! Return a model copy. */
        inline static ModelRawType getModelCopy(const ModelRawType *model);

        /*! Throw if the given operator is not valid for the where() method. */
        static void throwIfInvalidWhereOperator(const QString &comparison);
    };

    /* public */

    /* Constructors */

    /* From QList<Model> */

    template<DerivedCollectionModel Model>
    ModelsCollection<Model>::ModelsCollection(const QList<Model> &models)
    requires (!std::is_pointer_v<Model>)
    {
        for (const auto &model : models)
            this->push_back(model);
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model>::ModelsCollection(QList<Model> &&models) noexcept // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    requires (!std::is_pointer_v<Model>)
    {
        for (auto &&model : models)
            this->push_back(std::move(model));
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::operator=(const QList<Model> &models)
    requires (!std::is_pointer_v<Model>)
    {
        for (const auto &model : models)
            this->push_back(model);

        return *this;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::operator=(QList<Model> &&models) noexcept // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    requires (!std::is_pointer_v<Model>)
    {
        for (auto &&model : models)
            this->push_back(std::move(model));

        return *this;
    }

    /* To ModelsCollection<Model> */

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType>
    ModelsCollection<Model>::toModels() &
    requires std::is_pointer_v<Model>
    {
        ModelsCollection<ModelRawType> result;
        result.reserve(this->size());

        for (ModelRawType *const model : *this)
            result.push_back(*model);

        return result;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType>
    ModelsCollection<Model>::toModels() &&
    requires std::is_pointer_v<Model>
    {
        ModelsCollection<ModelRawType> result;
        result.reserve(this->size());

        for (ModelRawType *const model : *this)
            result.push_back(std::move(*model));

        return result;
    }

    /* To ModelsCollection<Model *> */

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::toPointers()
    requires (!std::is_pointer_v<Model>)
    {
        return toPointersCollection();
    }

    /* Comparison operators */

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::operator==( // NOLINT(misc-no-recursion)
            const ModelsCollection<ModelRawType> &other) const
    requires (!std::is_pointer_v<Model>)
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
                model != otherModel // NOLINT(misc-no-recursion)
            )
                return false;
        }

        return true;
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::operator==( // NOLINT(misc-no-recursion)
            const ModelsCollection<ModelRawType *> &other) const
    requires std::is_pointer_v<Model>
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
    bool ModelsCollection<Model>::operator==( // NOLINT(misc-no-recursion)
            const ModelsCollection<ModelRawType *> &other) const
    requires (!std::is_pointer_v<Model>)
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
    bool ModelsCollection<Model>::operator==( // NOLINT(misc-no-recursion)
            const ModelsCollection<ModelRawType> &other) const
    requires std::is_pointer_v<Model>
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

    /* Redeclared overridden methods from the base class */

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
    ModelsCollection<Model>::filter() const
    requires std::is_pointer_v<Model>
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
        // Nothing to do
        if (this->isEmpty())
            return {};

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
    QList<Model>
    ModelsCollection<Model>::toBase() const
    {
        return all();
    }

    template<DerivedCollectionModel Model>
    QList<Model>
    ModelsCollection<Model>::all() const
    {
        return *this;
    }

    template<DerivedCollectionModel Model>
    QList<QVariant>
    ModelsCollection<Model>::modelKeys() const
    {
        return *this
                | ranges::views::transform([](ConstModelLoopType model)
        {
            return getKey(model);
        })
                | ranges::to<QList<QVariant>>();
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    QList<T>
    ModelsCollection<Model>::modelKeys() const
    {
        return *this
                | ranges::views::transform([](ConstModelLoopType model) -> T
        {
            return getKey(model).template value<T>();
        })
                | ranges::to<QList<T>>();
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType>
    ModelsCollection<Model>::map(
            const std::function<ModelRawType(ModelRawType &&modelCopy,
                                             size_type)> &callback) const
    {
        const auto size = this->size();

        ModelsCollection<ModelRawType> result;
        result.reserve(size);

        for (size_type index = 0; index < size; ++index)
            result.push_back(std::invoke(callback, getModelCopy(this->operator[](index)),
                                         index));

        return result;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType>
    ModelsCollection<Model>::map(
            const std::function<ModelRawType(ModelRawType &&modelCopy)> &callback) const
    {
        const auto size = this->size();

        ModelsCollection<ModelRawType> result;
        result.reserve(size);

        for (auto &&model : *this)
            result.push_back(std::invoke(callback, getModelCopy(model)));

        return result;
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    QList<T>
    ModelsCollection<Model>::map(
            const std::function<T(ModelRawType &&modelCopy, size_type)> &callback) const
    {
        const auto size = this->size();

        QList<T> result;
        result.reserve(size);

        for (size_type index = 0; index < size; ++index)
            result.emplaceBack(
                        std::invoke(callback, getModelCopy(this->operator[](index)),
                                    index));

        return result;
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    QList<T>
    ModelsCollection<Model>::map(
            const std::function<T(ModelRawType &&modelCopy)> &callback) const
    {
        const auto size = this->size();

        QList<T> result;
        result.reserve(size);

        for (auto &&model : *this)
            result.emplaceBack(std::invoke(callback, getModelCopy(model)));

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
        std::unordered_map<K, V> result;
        result.reserve(static_cast<decltype (result)::size_type>(this->size()));

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
    QList<QVariant>
    ModelsCollection<Model>::pluck(const QString &column) const
    {
        QList<QVariant> result;
        result.reserve(this->size());

        for (ConstModelLoopType model : *this) {
            const ModelRawType *const modelPointer = toPointer(model);

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
    QList<T>
    ModelsCollection<Model>::pluck(const QString &column) const
    {
        QList<T> result;
        result.reserve(this->size());

        for (ConstModelLoopType model : *this) {
             const ModelRawType *const modelPointer = toPointer(model);

            // Don't handle the nullptr
            if (const auto &attributesHash = modelPointer->getAttributesHash();
                attributesHash.contains(column)
            )
                // Don't handle the null and not valid
                result << modelPointer->getAttributes().at(attributesHash.at(column))
                          .value.template value<T>();
        }

        return result;
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    std::map<T, QVariant>
    ModelsCollection<Model>::pluck(const QString &column, const QString &key) const
    {
        std::map<T, QVariant> result;

        for (ConstModelLoopType model : *this) {
            const ModelRawType *const modelPointer = toPointer(model);

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
    bool ModelsCollection<Model>::contains(const KeyType id) const
    {
        return ranges::contains(*this, true, [id](ConstModelLoopType model)
        {
            return getKeyCasted(model) == id;
        });
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::contains(const QVariant &id) const
    {
        // Don't handle the null and not valid
        return contains(castKey(id));
    }

    template<DerivedCollectionModel Model>
    bool
    ModelsCollection<Model>::contains(
            const std::function<bool(ModelRawType *)> &callback)
    {
        for (ModelLoopType model : *this) // NOLINT(readability-use-anyofallof)
            // Don't handle the nullptr
            if (std::invoke(callback, toPointer(model)))
                return true;

        return false;
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::contains(const std::optional<ModelRawType> &model) const
    {
        // Early return
        if (!model)
            return false;

        return ranges::contains(*this, true, [&model](ConstModelLoopType modelThis)
        {
            return toPointer(modelThis)->is(model);
        });
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::doesntContain(const KeyType id) const
    {
        return !contains(id);
    }

    template<DerivedCollectionModel Model>
    bool ModelsCollection<Model>::doesntContain(const QVariant &id) const
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
    bool
    ModelsCollection<Model>::doesntContain(const std::optional<ModelRawType> &model) const
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

    /* No need to use views in the following sort algorithms as they operates
       on pointers collection so they are cheap. */

    template<DerivedCollectionModel Model>
    template<typename C, typename P>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::sort(C comparison, P projection, const bool descending)
    {
        // Nothing to do
        if (this->isEmpty())
            return {};

        auto result = toPointersCollection();

                             // To correctly support the descending parameter
        ranges::sort(result, [comparison = std::move(comparison), &descending]
                             (auto &&left, auto &&right)
        {
            /* The XOR is like magic here, if the descending is false it behaves as if
               it wasn't there, but if the descending is true it returns negated values,
               the result is that it behaves as left > right if the descending is true.
               Nice table at https://en.wikipedia.org/wiki/XOR_gate */
            return descending ^ std::invoke(std::move(comparison),
                                            std::forward<decltype (left)>(left),
                                            std::forward<decltype (right)>(right));
        }, std::move(projection));

        return result;
    }

    template<DerivedCollectionModel Model>
    template<typename C, typename P>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::sortDesc(C comparison, P projection)
    {
        return sort(std::move(comparison), std::move(projection), true);
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::sortBy(const QString &column, const bool descending)
    {
        // Nothing to do
        if (this->isEmpty())
            return {};

        auto result = toPointersCollection();

        ranges::sort(result, [&column, descending](const ModelRawType *const left,
                                                   const ModelRawType *const right)
        {
            /* The XOR is like magic here, if the descending is false it behaves as if
               it wasn't there, but if the descending is true it returns negated values,
               the result is that it behaves as left > right if the descending is true.
               Nice table at https://en.wikipedia.org/wiki/XOR_gate */
            return descending ^ (left->template getAttribute<T>(column) <
                                 right->template getAttribute<T>(column));
        });

        return result;
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::sortByDesc(const QString &column)
    {
        return sortBy<T>(column, true);
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::sortBy(
            const QList<std::function<
                        std::strong_ordering(ModelRawType *,
                                             ModelRawType *)>> &callbacks)
    {
        // Nothing to do
        if (this->isEmpty())
            return {};

        auto result = toPointersCollection();

        std::ranges::sort(result, [&callbacks](ModelRawType *const left,
                                               ModelRawType *const right)
        {
            for (const auto &callback : callbacks) {
                const auto compared = std::invoke(callback, left, right);

                // If the values are the same then sort by the next callback
                if (compared == std::strong_ordering::equal)
                    continue;

                return compared == std::strong_ordering::less;
            }

            return false;
        });

        return result;
    }

    template<DerivedCollectionModel Model>
    template<typename P>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::sortBy(P projection, const bool descending)
    {
        return sort({}, std::move(projection), descending);
    }

    template<DerivedCollectionModel Model>
    template<typename P>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::sortByDesc(P projection)
    {
        return sort({}, std::move(projection), true);
    }

    template<DerivedCollectionModel Model>
    template<typename C, typename P>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::stableSort(C comparison, P projection, const bool descending)
    {
        // Nothing to do
        if (this->isEmpty())
            return {};

        auto result = toPointersCollection();

                                    // To correctly support the descending parameter
        ranges::stable_sort(result, [comparison = std::move(comparison), &descending]
                                    (auto &&left, auto &&right)
        {
            /* The XOR is like magic here, if the descending is false it behaves as if
               it wasn't there, but if the descending is true it returns negated values,
               the result is that it behaves as left > right if the descending is true.
               Nice table at https://en.wikipedia.org/wiki/XOR_gate */
            return descending ^ std::invoke(std::move(comparison),
                                            std::forward<decltype (left)>(left),
                                            std::forward<decltype (right)>(right));
        }, std::move(projection));

        return result;
    }

    template<DerivedCollectionModel Model>
    template<typename C, typename P>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::stableSortDesc(C comparison, P projection)
    {
        return stableSort(std::move(comparison), std::move(projection), true);
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::stableSortBy(const QString &column, const bool descending)
    {
        // Nothing to do
        if (this->isEmpty())
            return {};

        auto result = toPointersCollection();

        ranges::stable_sort(result, [&column, descending](const ModelRawType *const left,
                                                          const ModelRawType *const right)
        {
            /* The XOR is like magic here, if the descending is false it behaves as if
               it wasn't there, but if the descending is true it returns negated values,
               the result is that it behaves as left > right if the descending is true.
               Nice table at https://en.wikipedia.org/wiki/XOR_gate */
            return descending ^ (left->template getAttribute<T>(column) <
                                 right->template getAttribute<T>(column));
        });

        return result;
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::stableSortByDesc(const QString &column)
    {
        return stableSortBy<T>(column, true);
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::stableSortBy(
            const QList<std::function<
                          std::strong_ordering(ModelRawType *,
                                               ModelRawType *)>> &callbacks)
    {
        // Nothing to do
        if (this->isEmpty())
            return {};

        auto result = toPointersCollection();

        std::ranges::stable_sort(result, [&callbacks](ModelRawType *const left,
                                                      ModelRawType *const right)
        {
            for (const auto &callback : callbacks) {
                const auto compared = std::invoke(callback, left, right);

                // If the values are the same then sort by the next callback
                if (compared == std::strong_ordering::equal)
                    continue;

                return compared == std::strong_ordering::less;
            }

            return false;
        });

        return result;
    }

    template<DerivedCollectionModel Model>
    template<typename P>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::stableSortBy(P projection, const bool descending)
    {
        return stableSort({}, std::move(projection), descending);
    }

    template<DerivedCollectionModel Model>
    template<typename P>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::stableSortByDesc(P projection)
    {
        return stableSort({}, std::move(projection), true);
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::unique(const bool sort)
    {
        // Nothing to do, needed because of constFirst()
        if (this->isEmpty())
            return {};

        return uniqueBy<typename ModelRawType::KeyType>(
                    toPointer(StorageType::constFirst())->getKeyName(), sort);
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::uniqueBy(const QString &column, const bool sort)
    {
        // Nothing to do
        if (this->isEmpty())
            return {};

        auto result = toPointersCollection();

        if (sort)
            ranges::sort(result, [&column](const ModelRawType *const left,
                                           const ModelRawType *const right)
            {
                return left->template getAttribute<T>(column) <
                       right->template getAttribute<T>(column);
            });

        const auto it = ranges::unique(result, [&column](const ModelRawType *const left,
                                                         const ModelRawType *const right)
        {
            return left->template getAttribute<T>(column) ==
                   right->template getAttribute<T>(column);
        });
        // Remove duplicates from the end
        ranges::erase(result, it, ranges::end(result));

        return result;
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::uniqueRelaxed()
    {
        // Nothing to do, needed because of constFirst()
        if (this->isEmpty())
            return {};

        return uniqueRelaxedBy<typename ModelRawType::KeyType>(
                    toPointer(StorageType::constFirst())->getKeyName());
    }

    template<DerivedCollectionModel Model>
    template<typename T>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::uniqueRelaxedBy(const QString &column)
    {
        // Nothing to do
        if (this->isEmpty())
            return {};

        const auto size = this->size();

        ModelsCollection<ModelRawType *> result;
        result.reserve(size);

        QSet<T> values;
        result.reserve(size);

        for (ModelLoopType model : *this) {
            ModelRawType *const modelPointer = toPointer(model);
            auto value = modelPointer->template getAttribute<T>(column);

            if (values.contains(value))
                continue;

            result << modelPointer;
            values.insert(std::move(value));
        }

        return result;
    }

    template<DerivedCollectionModel Model>
    std::unique_ptr<TinyBuilder<typename ModelsCollection<Model>::ModelRawType>>
    ModelsCollection<Model>::toQuery() const
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
    template<typename>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType>
    ModelsCollection<Model>::fresh(const QList<WithItem> &relations)
    {
        // Nothing to do
        if (this->isEmpty())
            return {};

        // Don't handle the nullptr
        // Ownership of a unique_ptr()
        auto freshModelsRaw = toPointer(first())->newQueryWithoutScopes()
                              ->with(relations)
                              .whereKey(modelKeys<QVariant>())
                              .get();

        const auto freshModels = freshModelsRaw.mapWithModelKeys();

        return filter([&freshModels](const ModelRawType *const model)
        {
            return model->exists && freshModels.contains(model->getKeyCasted());
        })
            .map([&freshModels](ModelRawType &&model) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        {
            return *freshModels.at(model.getKeyCasted());
        });
    }

    template<DerivedCollectionModel Model>
    template<typename>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType>
    ModelsCollection<Model>::fresh(QString relation)
    {
        return fresh(QList<WithItem> {{std::move(relation)}});
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType>
    ModelsCollection<Model>::fresh(const QList<QString> &relations)
    {
        return fresh(WithItem::fromStringVector(relations));
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType>
    ModelsCollection<Model>::fresh(QList<QString> &&relations)
    {
        return fresh(WithItem::fromStringVector(std::move(relations)));
    }

    template<DerivedCollectionModel Model>
    template<typename>
    ModelsCollection<Model> &
    ModelsCollection<Model>::load(const QList<WithItem> &relations) &
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
    template<typename>
    ModelsCollection<Model> &
    ModelsCollection<Model>::load(QString relation) &
    {
        return load(QList<WithItem> {{std::move(relation)}});
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::load(const QList<QString> &relations) &
    {
        return load(WithItem::fromStringVector(relations));
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::load(QList<QString> &&relations) &
    {
        return load(WithItem::fromStringVector(std::move(relations)));
    }

    template<DerivedCollectionModel Model>
    template<typename>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::load(const QList<WithItem> &relations) &&
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
    template<typename>
    ModelsCollection<Model> &&
    ModelsCollection<Model>::load(QString relation) &&
    {
        return std::move(*this).load(QList<WithItem> {{std::move(relation)}});
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::load(const QList<QString> &relations) &&
    {
        return load(WithItem::fromStringVector(relations));
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<Model> &
    ModelsCollection<Model>::load(QList<QString> &&relations) &&
    {
        return load(WithItem::fromStringVector(std::move(relations)));
    }

    /* EnumeratesValues */

    template<DerivedCollectionModel Model>
    template<typename PivotType>
    QList<QList<AttributeItem>>
    ModelsCollection<Model>::toList() const
    {
        return map<QList<AttributeItem>>([](ModelRawType &&model) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        {
            return model.template toList<PivotType>();
        });
    }

    template<DerivedCollectionModel Model>
    template<typename PivotType>
    QList<QVariantMap>
    ModelsCollection<Model>::toMap() const
    {
        return map<QVariantMap>([](ModelRawType &&model) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        {
            return model.template toMap<PivotType>();
        });
    }

    template<DerivedCollectionModel Model>
    template<typename PivotType>
    QVariantList
    ModelsCollection<Model>::toListVariantList() const
    {
        return map<QVariant>([](ModelRawType &&model) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        {
            return QVariant::fromValue(model.template toList<PivotType>());
        });
    }

    template<DerivedCollectionModel Model>
    template<typename PivotType>
    QVariantList
    ModelsCollection<Model>::toMapVariantList() const
    {
        return map<QVariant>([](ModelRawType &&model) -> QVariant // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
        {
            return model.template toMap<PivotType>();
        });
    }

    template<DerivedCollectionModel Model>
    template<typename PivotType>
    QJsonArray
    ModelsCollection<Model>::toJsonArray() const
    {
        return QJsonArray::fromVariantList(
                AttributeUtils::fixQtNullVariantBug(toMapVariantList<PivotType>()));
    }

    template<DerivedCollectionModel Model>
    template<typename PivotType>
    QJsonDocument
    ModelsCollection<Model>::toJsonDocument() const
    {
        return QJsonDocument(toJsonArray<PivotType>());
    }

    template<DerivedCollectionModel Model>
    template<typename PivotType>
    QByteArray
    ModelsCollection<Model>::toJson(const QJsonDocument::JsonFormat format) const
    {
        return toJsonDocument<PivotType>().toJson(format);
    }

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
        return filter([&column](const ModelRawType *const model)
        {
            return model->getAttribute(column).isNull();
        });
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::whereNotNull(const QString &column)
    {
        return filter([&column](const ModelRawType *const model)
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

        return filter([&column, &values](const ModelRawType *const model)
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

        return reject([&column, &values](const ModelRawType *const model)
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
        return filter([&column, &values](const ModelRawType *const model)
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
        return filter([&column, &values](const ModelRawType *const model)
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
    typename ModelsCollection<Model>::KeyType
    ModelsCollection<Model>::castKey(const QVariant &key)
    {
        return key.template value<KeyType>();
    }

    template<DerivedCollectionModel Model>
    typename ModelsCollection<Model>::KeyType
    ModelsCollection<Model>::getKeyCasted(const ModelRawType *const model)
    {
        return castKey(getKey(model));
    }

    template<DerivedCollectionModel Model>
    typename ModelsCollection<Model>::KeyType
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
    std::function<bool(typename ModelsCollection<Model>::ModelRawType const *)>
    ModelsCollection<Model>::operatorForWhere(
            const QString &column, const QString &comparison, V value) const
    {
        throwIfInvalidWhereOperator(comparison);

        return [&column, &comparison, value = std::move(value)]
               (const ModelRawType *const model)
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

            // The comparison value is checked above and it throws there
            Q_UNREACHABLE();
        };
    }

    template<DerivedCollectionModel Model>
    ModelsCollection<typename ModelsCollection<Model>::ModelRawType *>
    ModelsCollection<Model>::toPointersCollection()
    requires (!std::is_pointer_v<Model>)
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
    requires std::is_pointer_v<Model>
    {
        return *this;
    }

    template<DerivedCollectionModel Model>
    typename ModelsCollection<Model>::ModelRawType
    ModelsCollection<Model>::getModelCopy(const ModelRawType &model)
    {
        return model;
    }

    template<DerivedCollectionModel Model>
    typename ModelsCollection<Model>::ModelRawType
    ModelsCollection<Model>::getModelCopy(const ModelRawType *const model)
    {
        Q_CHECK_PTR(model);

        return *model;
    }

    template<DerivedCollectionModel Model>
    void ModelsCollection<Model>::throwIfInvalidWhereOperator(const QString &comparison)
    {
        static const std::unordered_set<QString> allowedOperators {
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

    /*! Create a ModelsCollection from the given attributes. */
    template<Tiny::ModelConcept Model>
    Tiny::Types::ModelsCollection<Model>
    collect(const QList<QList<Tiny::AttributeItem>> &attributesList)
    {
        Tiny::Types::ModelsCollection<Model> result;
        result.reserve(attributesList.size());

        for (const auto &attributes : attributesList)
            result << Model::instance(attributes);

        return result;
    }

    /*! Create a ModelsCollection from the given attributes. */
    template<Tiny::ModelConcept Model>
    Tiny::Types::ModelsCollection<Model>
    collect(const QList<QList<Tiny::AttributeItem>> &attributesList,
            const QString &connection)
    {
        Tiny::Types::ModelsCollection<Model> result;
        result.reserve(attributesList.size());

        for (const auto &attributes : attributesList)
            result << Model::instance(attributes, connection);

        return result;
    }

    /*! Create a ModelsCollection from the given attributes. */
    template<Tiny::ModelConcept Model>
    Tiny::Types::ModelsCollection<Model>
    collect(QList<QList<Tiny::AttributeItem>> &&attributesList) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
    {
        Tiny::Types::ModelsCollection<Model> result;
        result.reserve(attributesList.size());

        for (auto &&attributes : attributesList)
            result << Model::instance(std::move(attributes));

        return result;
    }

    /*! Create a ModelsCollection from the given attributes. */
    template<Tiny::ModelConcept Model>
    Tiny::Types::ModelsCollection<Model>
    collect(QList<QList<Tiny::AttributeItem>> &&attributesList, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
            const QString &connection)
    {
        Tiny::Types::ModelsCollection<Model> result;
        result.reserve(attributesList.size());

        for (auto &&attributes : attributesList)
            result << Model::instance(std::move(attributes), connection);

        return result;
    }

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
