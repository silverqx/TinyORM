#pragma once
#ifndef ORM_TINY_MODEL_HPP
#define ORM_TINY_MODEL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QJsonObject>

#include "orm/concerns/hasconnectionresolver.hpp"
#include "orm/connectionresolverinterface.hpp"
#include "orm/tiny/concerns/guardsattributes.hpp"
#include "orm/tiny/concerns/hasattributes.hpp"
#include "orm/tiny/concerns/hasrelationships.hpp"
#include "orm/tiny/concerns/hastimestamps.hpp"
#include "orm/tiny/concerns/hidesattributes.hpp"
#include "orm/tiny/exceptions/massassignmenterror.hpp"
#include "orm/tiny/modelproxies.hpp"
#include "orm/tiny/tinybuilder.hpp" // IWYU pragma: keep
#ifdef TINYORM_TESTS_CODE
#  include "orm/tiny/types/connectionoverride.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
    template<typename Derived>
    class SoftDeletes;

    /*! Alias for the Concerns::GuardedModel, shortcut alias. */
    using Concerns::GuardedModel; // Don't remove // NOLINT(misc-unused-using-decls)

    // TODO model missing methods Model::loadMissing() silverqx
    // TODO model missing saveOrFail(), updateOrFail(), deleteOrFail(), I will need to implement ManagesTransaction::transaction(callback) method silverqx
    /*! Base model class. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model : public Orm::Concerns::HasConnectionResolver,
                  public Concerns::HasAttributes<Derived, AllRelations...>,
                  public Concerns::HidesAttributes<Derived, AllRelations...>,
                  public Concerns::GuardsAttributes<Derived, AllRelations...>,
                  public Concerns::HasRelationships<Derived, AllRelations...>,
                  public Concerns::HasTimestamps<Derived, AllRelations...>,
                  public ModelProxies<Derived, AllRelations...>,
                  public IsModel
    {
        // To access getUserXx() methods
        friend Concerns::GuardsAttributes<Derived, AllRelations...>;
        // To access getUserXx() methods
        friend Concerns::HasAttributes<Derived, AllRelations...>;
        // To access getUserXx() methods
        friend Concerns::HasTimestamps<Derived, AllRelations...>;
        // To access getUserXx() methods
        friend Concerns::HasRelationships<Derived, AllRelations...>;
        // To access getUserXx() methods
        friend Concerns::HidesAttributes<Derived, AllRelations...>;
        // Used by QueriesRelationships::has()
        friend Concerns::QueriesRelationships<Derived>;
        // To access getUserRelations()
        friend Support::Stores::BaseRelationStore<Derived, AllRelations...>;
        // To access getUserRelations()
        friend Support::Stores::BelongsToManyRelatedTableStore<Derived, AllRelations...>;
        // To access setKeysForSaveQuery(), and appendToUserDates()
        friend SoftDeletes<Derived>;
        // FUTURE try to solve problem with forward declarations for friend methods, to allow only relevant methods from TinyBuilder silverqx
        // Used by TinyBuilder::eagerLoadRelations()
        friend TinyBuilder<Derived>;

        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;
        /*! Alias for the helper utils. */
        using Helpers = Orm::Utils::Helpers;
        /*! Alias for the string utils. */
        using StringUtils = Orm::Utils::String;
        /*! Alias for the type utils. */
        using TypeUtils = Orm::Utils::Type;
        /*! Apply all the Model's template parameters to the passed T template
            argument. */
        template<template<typename ...> typename T>
        using ModelTypeApply = T<Derived, AllRelations...>;

    public:
        /*! The "type" of the primary key (ID). */
        using KeyType = quint64;

        /*! The base model type. */
        using BaseModelType = Model<Derived, AllRelations...>;
        /*! The Derived model type. */
        using DerivedType = Derived;

        // Following are not needed in this class, only to avoid using-s in Derived models
        /*! Alias for the AttributeItem. */
        using AttributeItem = Tiny::AttributeItem;
        /*! Alias for the CastItem. */
        using CastItem = Tiny::CastItem;
        /*! Alias for the CastType. */
        using CastType = Tiny::CastType;
        /*! Alias for the Pivot. */
        using Pivot = Relations::Pivot; // Forward declaration is in the interactswithpivottable.hpp
        /*! Alias for the ModelAttributes. */
        using ModelAttributes = Types::ModelAttributes;

        /*! Alias for the BelongsTo. */
        template<class Model, class Related>
        using BelongsTo = Relations::BelongsTo<Model, Related>;
        /*! Alias for the BelongsToMany. */
        template<class Model, class Related, class PivotType = Pivot>
        using BelongsToMany = Relations::BelongsToMany<Model, Related, PivotType>;
        /*! Alias for the HasOne. */
        template<class Model, class Related>
        using HasOne = Relations::HasOne<Model, Related>;
        /*! Alias for the HasMany. */
        template<class Model, class Related>
        using HasMany = Relations::HasMany<Model, Related>;
        /*! Alias for the Relation. */
        template<class Model, class Related>
        using Relation = Relations::Relation<Model, Related>;

        /* Constructors */
        /*! Create a new TinyORM model instance, Default constructor. */
        Model();
        /*! Default destructor. */
        ~Model() = default;

        /*! Model's copy constructor. */
        Model(const Model &) = default; // NOLINT(misc-no-recursion)
        /*! Model's move constructor. */
        Model(Model &&) = default; // NOLINT(bugprone-exception-escape)

        /*! Model's copy assignment operator. */
        Model &operator=(const Model &) = default; // NOLINT(misc-no-recursion)
        /*! Model's move assignment operator. */
        Model &operator=(Model &&) = default; // NOLINT(bugprone-exception-escape)

        /*! Create a new TinyORM model instance from attributes
            (converting constructor). */
        explicit Model(const QList<AttributeItem> &attributes);
        /*! Create a new TinyORM model instance from attributes
            (converting constructor). */
        explicit Model(QList<AttributeItem> &&attributes);

        /*! Create a new TinyORM model instance from attributes
            (list initialization). */
        Model(std::initializer_list<AttributeItem> attributes);

        /*! Create a new TinyORM model instance, skip-filling default attribute
            values. */
        explicit Model(DontFillDefaultAttributes /*unused*/);

        /* Static operations on the Model class */
        /*! Create a new TinyORM model instance. */
        static Derived instance();
        /*! Create a new TinyORM model instance. */
        static Derived instance(const QString &connection);
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(const QList<AttributeItem> &attributes);
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(QList<AttributeItem> &&attributes);
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(const QList<AttributeItem> &attributes,
                                const QString &connection);
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(QList<AttributeItem> &&attributes,
                                const QString &connection);

        /*! Create a new TinyORM model instance on the heap. */
        static std::unique_ptr<Derived> instanceHeap();
        /*! Create a new TinyORM model instance on the heap. */
        static std::unique_ptr<Derived> instanceHeap(const QString &connection);
        /*! Create a new TinyORM model instance on the heap with given attributes. */
        static std::unique_ptr<Derived>
        instanceHeap(const QList<AttributeItem> &attributes);
        /*! Create a new TinyORM model instance on the heap with given attributes. */
        static std::unique_ptr<Derived>
        instanceHeap(QList<AttributeItem> &&attributes);
        /*! Create a new TinyORM model instance on the heap with given attributes. */
        static std::unique_ptr<Derived>
        instanceHeap(const QList<AttributeItem> &attributes,
                     const QString &connection);
        /*! Create a new TinyORM model instance on the heap with given attributes. */
        static std::unique_ptr<Derived>
        instanceHeap(QList<AttributeItem> &&attributes, const QString &connection);

        /*! Begin querying the model. */
        static std::unique_ptr<TinyBuilder<Derived>> query();
        /*! Begin querying the model on a given connection. */
        static std::unique_ptr<TinyBuilder<Derived>> on(const QString &connection = "");

        /*! Get all of the models from the database. */
        static ModelsCollection<Derived> all(const QList<Column> &columns = {ASTERISK});

        /*! Destroy the models for the given IDs. */
        static std::size_t destroy(const QList<QVariant> &ids);
        /*! Destroy the model by the given ID. */
        inline static std::size_t destroy(const QVariant &id);

        /* Operations on a Model instance */
        /*! Save the model to the database. */
        bool save(SaveOptions options = {});
        /*! Save the model and all of its relationships. */
        bool push();

        /*! Update records in the database. */
        bool update(const QList<AttributeItem> &attributes, SaveOptions options = {});

        /*! Increment a column's value by a given amount. */
        template<typename T = std::size_t> requires std::is_arithmetic_v<T>
        std::tuple<int, TSqlQuery>
        increment(const QString &column, T amount = 1,
                  const QList<AttributeItem> &extra = {}, bool all = false);
        /*! Decrement a column's value by a given amount. */
        template<typename T = std::size_t> requires std::is_arithmetic_v<T>
        std::tuple<int, TSqlQuery>
        decrement(const QString &column, T amount = 1,
                  const QList<AttributeItem> &extra = {}, bool all = false);

        /*! Delete the model from the database. */
        bool remove();
        /*! Delete the model from the database, alias. */
        bool deleteModel();

        /*! Reload a fresh model instance from the database. */
        std::optional<Derived> fresh(const QList<WithItem> &relations = {}) const;
        /*! Reload a fresh model instance from the database. */
        std::optional<Derived> fresh(const QString &relation) const;
        /*! Reload the current model instance with fresh attributes from the database. */
        Derived &refresh();

        /*! Eager load relations on the model. */
        template<typename = void>
        Derived &load(const QList<WithItem> &relations);
        /*! Eager load relations on the model. */
        template<typename = void>
        Derived &load(QString relation);
        /*! Eager load relations on the model. */
        inline Derived &load(const QList<QString> &relations);
        /*! Eager load relations on the model. */
        inline Derived &load(QList<QString> &&relations);

        /*! Determine if two models have the same ID and belong to the same table. */
        template<ModelConcept ModelToCompare>
        bool is(const std::optional<ModelToCompare> &model) const;
        /*! Determine if two models are not the same. */
        template<ModelConcept ModelToCompare>
        bool isNot(const std::optional<ModelToCompare> &model) const;

        /*! Equality comparison operator for the Model. */
        bool operator==(const Model &right) const; // NOLINT(misc-no-recursion)

#if defined(__clang__) && __clang_major__ >= 16
        /*! Three-way comparison operator for the Model. */
        std::strong_ordering operator<=>(const Model &right) const
        requires std::is_integral_v<typename Derived::KeyType>;
#else
        /*! Three-way comparison operator for the Model. */
        std::strong_ordering operator<=>(const Model &right) const;
#endif

        /*! Fill the model with a vector of attributes. */
        Derived &fill(const QList<AttributeItem> &attributes);
        /*! Fill the model with a vector of attributes. */
        Derived &fill(QList<AttributeItem> &&attributes);
        /*! Fill the model with a vector of attributes. Force mass assignment. */
        Derived &forceFill(const QList<AttributeItem> &attributes);
        /*! Fill the model with a vector of attributes. Force mass assignment. */
        Derived &forceFill(QList<AttributeItem> &&attributes);

        /* Model Instance methods */
        /*! Get a new query builder for the model's table. */
        inline std::unique_ptr<TinyBuilder<Derived>> newQuery() const;
        /*! Get a new query builder that doesn't have any global scopes. */
        std::unique_ptr<TinyBuilder<Derived>> newQueryWithoutScopes() const;
        /*! Get a new query builder that doesn't have any global scopes or
            eager loading. */
        std::unique_ptr<TinyBuilder<Derived>> newModelQuery() const;
        /*! Get a new query builder with no relationships loaded. */
        std::unique_ptr<TinyBuilder<Derived>> newQueryWithoutRelationships() const;
        /*! Create a new Tiny query builder for the model. */
        std::unique_ptr<TinyBuilder<Derived>>
        newTinyBuilder(std::shared_ptr<QueryBuilder> query) const;

        /*! Create a new model instance that is existing. */
        Derived
        newFromBuilder(const QList<AttributeItem> &attributes = {},
                       const std::optional<QString> &connection = std::nullopt) const;
        /*! Create a new model instance that is existing. */
        Derived
        newFromBuilder(QList<AttributeItem> &&attributes = {},
                       const std::optional<QString> &connection = std::nullopt) const;
        /*! Create a new instance of the given model. */
        inline Derived newInstance() const;
        /*! Create a new instance of the given model. */
        Derived newInstance(const QList<AttributeItem> &attributes,
                            bool exists = false) const;
        /*! Create a new instance of the given model. */
        Derived newInstance(QList<AttributeItem> &&attributes,
                            bool exists = false) const;

        /*! Clone the model into a new, non-existing instance. */
        Derived replicate(const std::unordered_set<QString> &except = {}) const;

        /*! Create a new pivot model instance. */
        template<typename PivotType = Relations::Pivot, typename Parent>
        PivotType
        newPivot(const Parent &parent, const QList<AttributeItem> &attributes,
                 const QString &table, bool exists, bool withTimestamps = false,
                 const QString &createdAt = Constants::CREATED_AT,
                 const QString &updatedAt = Constants::UPDATED_AT) const;

        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_DECLARATIONS

        /* Serialization */
        /*! Convert the model instance to the map of attributes and relations. */
        template<typename PivotType = void> // PivotType is primarily internal
        QVariantMap toMap() const;
        /*! Convert the model instance to the vector of attributes and relations. */
        template<typename PivotType = void> // PivotType is primarily internal
        QList<AttributeItem> toList() const;

        /*! Convert the model instance to QJsonObject. */
        inline QJsonObject toJsonObject() const;
        /*! Convert the model instance to QJsonDocument. */
        inline QJsonDocument toJsonDocument() const;
        /*! Convert the model instance to JSON. */
        inline QByteArray
        toJson(QJsonDocument::JsonFormat format = QJsonDocument::Compact) const;

        /* Getters / Setters */
        /*! Get the current connection name for the model. */
        const QString &getConnectionName() const;
        /*! Get the database connection for the model. */
        inline DatabaseConnection &getConnection() const;
        /*! Set the connection associated with the model. */
        inline Derived &setConnection(const QString &name);
        /*! Set the connection associated with the model. */
        inline Derived &setConnection(QString &&name);
        /*! Set the table associated with the model. */
        inline Derived &setTable(const QString &value);
        /*! Get the table associated with the model. */
        const QString &getTable() const;
        /*! Get the primary key for the model. */
        inline const QString &getKeyName() const;
        /*! Get the table qualified key name. */
        inline QString getQualifiedKeyName() const;
        /*! Get the value of the model's primary key. */
        inline QVariant getKey() const;
        /*! Get the value of the model's primary key casted to the Derived::KeyType. */
        inline auto getKeyCasted() const;
        /*! Get the value indicating whether the IDs are incrementing. */
        inline bool getIncrementing() const;
        /*! Set whether IDs are incrementing. */
        inline Derived &setIncrementing(bool value);
        /*! Get the default foreign key name for the model. */
        QString getForeignKey() const;

        /* Others */
        /*! Qualify the given column name by the model's table. */
        QString qualifyColumn(const QString &column) const;
        /*! Determine whether the Derived Model extends the SoftDeletes. */
        constexpr static bool extendsSoftDeletes();

        /* Data members */
        /*! Indicates if the model exists. */
        bool exists = false;

    protected:
        /* HasTimestamps */
        /*! The name of the "created at" column. */
        inline static const QString &CREATED_AT() noexcept;
        /*! The name of the "updated at" column. */
        inline static const QString &UPDATED_AT() noexcept;

        /* Model Instance methods */
        /*! Get a new query builder instance for the connection. */
        std::shared_ptr<QueryBuilder> newBaseQueryBuilder() const;

        /* Operations on a Model instance */
        /*! Perform the actual delete query on this model instance. */
        void performDeleteOnModel();

        /*! Set the keys for a save update query. */
        TinyBuilder<Derived> &
        setKeysForSaveQuery(TinyBuilder<Derived> &query) const;
        /*! Get the primary key value for a save query. */
        QVariant getKeyForSaveQuery() const;

        /*! Set the keys for a select query. */
        TinyBuilder<Derived> &
        setKeysForSelectQuery(TinyBuilder<Derived> &query) const;
        /*! Get the primary key value for a select query. */
        inline QVariant getKeyForSelectQuery() const;

        /*! Perform a model insert operation. */
        bool performInsert(const TinyBuilder<Derived> &query);
        /*! Perform a model insert operation. */
        bool performUpdate(TinyBuilder<Derived> &query);
        /*! Perform any actions that are necessary after the model is saved. */
        void finishSave(SaveOptions options = {});

        /*! Insert the given attributes and set the ID on the model. */
        quint64 insertAndSetId(const TinyBuilder<Derived> &query,
                               const QList<AttributeItem> &attributes);

        /* Data members */
        /*! The table associated with the model. */
        QString u_table;
        /*! The connection name for the model. */
        QString u_connection;
        /*! Indicates if the model's ID is auto-incrementing. */
        bool u_incrementing = true;
        /*! The primary key associated with the table. */
        QString u_primaryKey = ID;

        // TODO detect (best at compile time) circular eager relation problem, the exception which happens during this problem is stackoverflow in QRegularExpression silverqx
        /*! The relations to eager load on every query. */
        QList<QString> u_with;
        /*! The relationship counts that should be eager loaded on every query. */
//        QList<WithItem> u_withCount;

    private:
        /* Operations on a Model instance */
        /*! Method to call in the incrementOrDecrement(). */
        enum struct IncrementOrDecrement : quint8
        {
            /*! Call the increment() method. */
            INCREMENT,
            /*! Call the decrement() method. */
            DECREMENT,
        };
        /*! Call the increment() method. */
        constexpr static auto Increment = IncrementOrDecrement::INCREMENT;
        /*! Call the decrement() method. */
        constexpr static auto Decrement = IncrementOrDecrement::DECREMENT;
        /* Don't use using enum here as it can't name dependent type.
           See: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1099r5.html */

        /*! Run the increment or decrement method on the model. */
        template<typename T> requires std::is_arithmetic_v<T>
        std::tuple<int, TSqlQuery>
        incrementOrDecrement(
                const QString &column, T amount, const QList<AttributeItem> &extra,
                IncrementOrDecrement method, bool all);
        /*! Invoke the increment or decrement method on the model. */
        template<typename T> requires std::is_arithmetic_v<T>
        std::tuple<int, TSqlQuery>
        invokeIncrementOrDecrement(
                TinyBuilder<Derived> &query, const QString &column, T amount,
                const QList<AttributeItem> &extra, IncrementOrDecrement method,
                bool all);

        /* HasAttributes */
        /*! Fill the model with a vector of attributes with the CRTP check. */
        void fillWithCRTPCheck(const QList<AttributeItem> &attributes);
        /*! Fill the model with a vector of attributes with the CRTP check. */
        void fillWithCRTPCheck(QList<AttributeItem> &&attributes);

        /*! Throw an InvalidArgumentError if the attributes passed to the constructor
            contain any value that causes access to some data member in the derived
            instance that is not yet initialized. */
        inline static void
        throwIfCRTPctorProblem(const QList<AttributeItem> &attributes);
        /*! The QDateTime attribute detected, causes CRTP ctor problem. */
        static void throwIfQDateTimeAttribute(const QList<AttributeItem> &attributes);
        /*! Throw if an attempt to fill a guarded attribute is detected
            (mass assignment). */
        [[noreturn]] static void throwTotallyGuarded(const QString &key);

        /*! Get the u_dateFormat attribute from the Derived model. */
        inline QString &getUserDateFormat() noexcept;
        /*! Get the u_dateFormat attribute from the Derived model. */
        inline const QString &getUserDateFormat() const noexcept;
        /*! Get the u_timeFormat attribute from the Derived model. */
        inline QString &getUserTimeFormat() noexcept;
        /*! Get the u_timeFormat attribute from the Derived model. */
        inline const QString &getUserTimeFormat() const noexcept;
        /*! Get the u_dates attribute from the Derived model. */
        inline static const QStringList &getUserDates() noexcept;
        /*! Get the casts hash. */
        inline std::unordered_map<QString, CastItem> &getUserCasts() noexcept;
        /*! Get the casts hash. */
        inline const std::unordered_map<QString, CastItem> &getUserCasts() const noexcept;
        /*! Get the u_snakeAttributes attribute from the Derived model. */
        inline bool &getUserSnakeAttributes() noexcept;
        /*! Get the u_snakeAttributes attribute from the Derived model. */
        inline bool getUserSnakeAttributes() const noexcept;

        /* GuardsAttributes */
        /*! Get the u_fillable attributes from the Derived model. */
        inline QStringList &getUserFillable() noexcept;
        /*! Get the u_fillable attributes from the Derived model. */
        inline const QStringList &getUserFillable() const noexcept;
        /*! Get the u_guarded attributes from the Derived model. */
        inline QStringList &getUserGuarded() noexcept;
        /*! Get the u_guarded attributes from the Derived model. */
        inline const QStringList &getUserGuarded() const noexcept;

        /* HasRelationships */
        /*! Relation visitor lambda type (an alias for shorter declarations). */
        using RelationVisitorAlias =
                typename Concerns::HasRelationships<Derived, AllRelations...>
                                 ::RelationVisitor;

        /*! Get the u_relations map from the Derived model. */
        inline const QHash<QString, RelationVisitorAlias> &
        getUserRelations() const noexcept;
        /*! Get the u_touches relation names to touch from the Derived model. */
        inline const QStringList &getUserTouches() const noexcept;
        /*! Get the u_touches relation names to touch from the Derived model. */
        inline QStringList &getUserTouches() noexcept;

        /* HasTimestamps */
        /*! Get the u_timestamps attribute from the Derived model. */
        inline bool &getUserTimestamps() noexcept;
        /*! Get the u_timestamps attribute from the Derived model. */
        inline bool getUserTimestamps() const noexcept;
        /*! Get the CREATED_AT attribute from the Derived model. */
        inline static const QString &getUserCreatedAtColumn() noexcept;
        /*! Get the UPDATED_AT attribute from the Derived model. */
        inline static const QString &getUserUpdatedAtColumn() noexcept;

        /* HidesAttributes */
        /*! Get the u_visible attributes from the Derived model. */
        inline std::set<QString> &getUserVisible() noexcept;
        /*! Get the u_visible attributes from the Derived model. */
        inline const std::set<QString> &getUserVisible() const noexcept;
        /*! Get the u_hidden attributes from the Derived model. */
        inline std::set<QString> &getUserHidden() noexcept;
        /*! Get the u_hidden attributes from the Derived model. */
        inline const std::set<QString> &getUserHidden() const noexcept;

        /* Serialization - Appends */
        /*! Get the u_appends attributes from the Derived model. */
        inline std::set<QString> &getUserAppends() noexcept;
        /*! Get the u_appends attributes from the Derived model. */
        inline const std::set<QString> &getUserAppends() const noexcept;
        /*! Get the u_mutators map from the Derived model. */
        inline static
        const QHash<QString, typename Concerns::HasAttributes<Derived, AllRelations...>
                                              ::MutatorFunction> &
        getUserMutators() noexcept;

        /* Serialization - Attributes */
        /*! Prepare a date for vector, map, or JSON serialization (calls Derived). */
        inline static QString getUserSerializeDate(QDate date);
        /*! Prepare a datetime for vector, map, or JSON serialization (calls Derived). */
        inline static QString getUserSerializeDateTime(const QDateTime &datetime);
        /*! Prepare a time for vector, map, or JSON serialization (calls Derived). */
        inline static QString getUserSerializeTime(QTime time);

        /* SoftDeletes */
        /*! Initialize the SoftDeletes (add the deleted_at column to the u_dates). */
        void initializeSoftDeletes() const;
        /*! Append the given column to the u_dates attribute. */
        static void appendToUserDates(const QString &column);
    };

    /* public */

    /* Constructors */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model()
    {
        // Compile time check if a primary key type is supported by a QVariant
        qMetaTypeId<typename Derived::KeyType>();

        // Initialize the SoftDeletes (add the deleted_at column to the u_dates)
        initializeSoftDeletes();

        // Default Attribute Values
        fillWithCRTPCheck(Derived::u_attributes);

        this->syncOriginal();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(const QList<AttributeItem> &attributes)
        : Model()
    {
        fillWithCRTPCheck(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(QList<AttributeItem> &&attributes)
        : Model()
    {
        fillWithCRTPCheck(std::move(attributes));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(
            std::initializer_list<AttributeItem> attributes
    )
        : Model(QList<AttributeItem>(attributes.begin(), attributes.end()))
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(DontFillDefaultAttributes /*unused*/)
    {
        // Compile time check if a primary key type is supported by a QVariant
        qMetaTypeId<typename Derived::KeyType>();

        // Initialize the SoftDeletes (add the deleted_at column to the u_dates)
        initializeSoftDeletes();
    }

    /* Static operations on the Model class */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance()
    {
        Derived model(dontFillDefaultAttributes);

        // Default Attribute Values
        model.fill(Derived::u_attributes);

        model.syncOriginal();

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(const QString &connection)
    {
        auto model = instance();

        model.setConnection(connection);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(const QList<AttributeItem> &attributes)
    {
        auto model = instance();

        model.fill(attributes);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(QList<AttributeItem> &&attributes)
    {
        auto model = instance();

        model.fill(std::move(attributes));

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(const QList<AttributeItem> &attributes,
                                              const QString &connection)
    {
        auto model = instance(attributes);

        model.setConnection(connection);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(QList<AttributeItem> &&attributes,
                                              const QString &connection)
    {
        auto model = instance(std::move(attributes));

        model.setConnection(connection);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<Derived>
    Model<Derived, AllRelations...>::instanceHeap()
    {
        auto model = std::make_unique<Derived>(dontFillDefaultAttributes);

        // Default Attribute Values
        model->fill(Derived::u_attributes);

        model->syncOriginal();

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<Derived>
    Model<Derived, AllRelations...>::instanceHeap(const QString &connection)
    {
        auto model = instanceHeap();

        model->setConnection(connection);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<Derived>
    Model<Derived, AllRelations...>::instanceHeap(
            const QList<AttributeItem> &attributes)
    {
        auto model = instanceHeap();

        model->fill(attributes);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<Derived>
    Model<Derived, AllRelations...>::instanceHeap(QList<AttributeItem> &&attributes)
    {
        auto model = instanceHeap();

        model->fill(std::move(attributes));

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<Derived>
    Model<Derived, AllRelations...>::instanceHeap(
            const QList<AttributeItem> &attributes, const QString &connection)
    {
        auto model = instanceHeap(attributes);

        model->setConnection(connection);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<Derived>
    Model<Derived, AllRelations...>::instanceHeap(
            QList<AttributeItem> &&attributes, const QString &connection)
    {
        auto model = instanceHeap(std::move(attributes));

        model->setConnection(connection);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::query()
    {
        return Derived::instance().newQuery();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::on(const QString &connection)
    {
        /* First we will just create a fresh instance of this model, and then we can
           set the connection on the model so that it is used for the queries we
           execute, as well as being set on every relation we retrieve without
           a custom connection name. */
        auto instance = Derived::instance();

        instance.setConnection(connection);

        return instance.newQuery();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    ModelsCollection<Derived>
    Model<Derived, AllRelations...>::all(const QList<Column> &columns)
    {
        return query()->get(columns);
    }

    // TODO cpp check all int types and use std::size_t where appropriate silverqx
    // FEATURE dilemma primarykey, id should be Derived::KeyType, if I don't solve this problem, do runtime type check, QVariant type has to be the same type like KeyType and throw exception silverqx
    // TODO next test all this remove()/destroy() methods, when deletion fails silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::size_t
    Model<Derived, AllRelations...>::destroy(const QList<QVariant> &ids)
    {
        if (ids.isEmpty())
            return 0;

        /* We will actually pull the models from the database table and call delete on
           each of them individually so that their events get fired properly with a
           correct set of attributes in case the developers wants to check these. */
        auto instance = Derived::instance();

        std::size_t count = 0;

        // Ownership of a unique_ptr()
        for (auto &&model : instance.whereIn(instance.getKeyName(), ids)->get())
            if (model.remove())
                ++count;

        return count;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::size_t
    Model<Derived, AllRelations...>::destroy(const QVariant &id)
    {
        return destroy(QList<QVariant> {id});
    }

    /* Operations on a Model instance */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::save(const SaveOptions options)
    {
//        mergeAttributesFromClassCasts();

        // Ownership of a unique_ptr()
        auto query = newModelQuery();

        auto saved = false;

        /* If the "saving" event returns false we'll bail out of the save and return
           false, indicating that the save failed. This provides a chance for any
           listeners to cancel save operations if validations fail or whatever. */
//        if (fireModelEvent('saving') === false) {
//            return false;
//        }

        /* If the model already exists in the database we can just update our record
           that is already in this database using the current IDs in this "where"
           clause to only update this model. Otherwise, we'll just insert them. */
        if (exists)
            saved = this->isDirty() ? performUpdate(*query) : true;

        /* If the model is brand new, we'll insert it into our database and set the
           ID attribute on the model to the value of the newly inserted row's ID
           which is typically an auto-increment value managed by the database. */
        else {
            saved = performInsert(*query);

            if (const auto &connection = query->getConnection();
                getConnectionName().isEmpty()
            )
                setConnection(connection.getName());
        }

        /* If the model is successfully saved, we need to do a few more things once
           that is done. We will call the "saved" method here to run any actions
           we need to happen after a model gets successfully saved right here. */
        if (saved)
            finishSave(options);

        return saved;
    }

    // FUTURE support for SaveOptions parameter, Eloquent doesn't have this parameter, maybe there's a reason for that, but I didn't find anything on github issues silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::push()
    {
        if (!save())
            return false;

        /* To sync all of the relationships to the database, we will simply spin through
           the relationships and save each model via this "push" method, which allows
           us to recurse into all of these nested relations for the model instance. */
        for (auto &[relation, models] : this->m_relations)
            /* Following Eloquent API, if any push failed, then quit, remaining push-es
               will not be processed. */
            if (!this->pushWithVisitor(relation, models))
                return false;

        return true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::update(
            const QList<AttributeItem> &attributes,
            const SaveOptions options)
    {
        if (!exists)
            return false;

        return fill(attributes).save(options);
    }

    // NOTE api different, I have added the 'all' bool param. to avoid updating all rows by mistake silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, TSqlQuery>
    Model<Derived, AllRelations...>::increment(
            const QString &column, const T amount, const QList<AttributeItem> &extra,
            const bool all)
    {
        return incrementOrDecrement(column, amount, extra, Increment, all);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, TSqlQuery>
    Model<Derived, AllRelations...>::decrement(
            const QString &column, const T amount, const QList<AttributeItem> &extra,
            const bool all)
    {
        return incrementOrDecrement(column, amount, extra, Decrement, all);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::remove()
    {
        // FEATURE castable silverqx
//        mergeAttributesFromClassCasts();

        if (getKeyName().isEmpty())
            throw Orm::Exceptions::RuntimeError("No primary key defined on model.");

        /* If the model doesn't exist, there is nothing to delete so we'll just return
           immediately and not do anything else. Otherwise, we will continue with a
           deletion process on the model, firing the proper events, and so forth. */
        if (!exists)
            // NOTE api different silverqx
            return false;

        // FUTURE add support for model events silverqx
//        if ($this->fireModelEvent('deleting') === false) {
//            return false;
//        }

        /* Here, we'll touch the owning models, verifying these timestamps get updated
           for the models. This will allow any caching to get broken on the parents
           by the timestamp. Then we will go ahead and delete the model instance. */
        this->touchOwners();

        // FUTURE performDeleteOnModel() and return value, check logic here, eg what happens when no model is delete and combinations silverqx
        // FUTURE inconsistent return values save(), update(), remove(), ..., also Pivot::remove() silverqx
        if constexpr (Model::extendsSoftDeletes())
            model().SoftDeletes<Derived>::performDeleteOnModel();
        else
            Model::performDeleteOnModel();

        /* Once the model has been deleted, we will fire off the deleted event so that
           the developers may hook into post-delete operations. We will then return
           a boolean true as the delete is presumably successful on the database. */
//        $this->fireModelEvent('deleted', false);

        return true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::deleteModel()
    {
        return model().remove();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::fresh(const QList<WithItem> &relations) const
    {
        if (!exists)
            return std::nullopt;

        return model().setKeysForSelectQuery(*newQueryWithoutScopes())
                .with(relations)
                .first();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::fresh(const QString &relation) const
    {
        return fresh(QList<WithItem> {{relation}});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::refresh()
    {
        auto &model = this->model();

        if (!exists)
            return model;

        this->setRawAttributes(model.setKeysForSelectQuery(*newQueryWithoutScopes())
                               .firstOrFail().getRawAttributes());

        // And reload them again, refresh relations
        load(this->getLoadedRelationsWithoutPivot());

        this->syncOriginal();

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename>
    Derived &
    Model<Derived, AllRelations...>::load(const QList<WithItem> &relations)
    {
        auto &model = this->model();

        // Ownership of a unique_ptr()
        auto builder = newQueryWithoutRelationships();

        builder->with(relations)
                .eagerLoadRelations(model);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename>
    Derived &
    Model<Derived, AllRelations...>::load(QString relation)
    {
        return load(QList<WithItem> {{std::move(relation)}});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::load(const QList<QString> &relations)
    {
        return load(WithItem::fromStringVector(relations));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::load(QList<QString> &&relations)
    {
        return load(WithItem::fromStringVector(std::move(relations)));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<ModelConcept ModelToCompare>
    bool Model<Derived, AllRelations...>::is(
            const std::optional<ModelToCompare> &model) const
    {
        /* The model's type doesn't have to be checked because this check checks, if
           two models have the same ID and belong to the same table.
           So if the type will be different and will be the same ID and table, then
           it returns true, and this is good! The same ID and table name guarantee
           that it's the same record. */
        return model &&
               // First compare the same table (same model type)
               this->model().getTable() == model->getTable() && // model() needed as it's overridden in the BasePivot
               getKey() == model->getKey() &&
               getConnectionName() == model->getConnectionName();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<ModelConcept ModelToCompare>
    bool Model<Derived, AllRelations...>::isNot(
            const std::optional<ModelToCompare> &model) const
    {
        return !is(model);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::operator==(const Model &right) const // NOLINT(misc-no-recursion)
    {
        /* Comparing the HasConnectionResolver, GuardsAttributes, ModelProxies, and
           IsModel is not needed as they don't contain any data members or they
           contain only a static data members. */

        // Compare the HasAttributes_ concern
        using HasAttributes_ = Concerns::HasAttributes<Derived, AllRelations...>;
        if (static_cast<const HasAttributes_ &>(*this) !=
            static_cast<const HasAttributes_ &>(right)
        )
            return false;

        // Compare the HasRelationships_ concern
        using HasRelationships_ = Concerns::HasRelationships<Derived, AllRelations...>;
        if (static_cast<const HasRelationships_ &>(*this) !=
            static_cast<const HasRelationships_ &>(right)
        )
            return false;

        // Compare the HasTimestamps_ concern
        using HasTimestamps_ = Concerns::HasTimestamps<Derived, AllRelations...>;
        if (static_cast<const HasTimestamps_ &>(*this) !=
            static_cast<const HasTimestamps_ &>(right)
        )
            return false;

        // Compare the Base Model
        if (true != (exists         == right.exists         &&
                     u_table        == right.u_table        &&
                     u_connection   == right.u_connection   &&
                     u_incrementing == right.u_incrementing &&
                     u_primaryKey   == right.u_primaryKey   &&
                     u_with         == right.u_with)
//                   u_withCount    == right.u_withCount
        )
            return false;

        const auto &model = this->model();
        // Compare data members in the Derived Model 😮🤯😎
        const auto &derivedRight = static_cast<const Derived &>(right);

        // model().u_relations == derivedRight.u_relations
        /* It compares only the size and keys and doesn't compare hash values because
           the std::function doesn't have a full/complete operator==() (it only compares
           for the nullptr). */
        if (!HasRelationships_::compareURelations(model.u_relations,
                                                  derivedRight.u_relations))
            return false;

        /* Thanks to the CRTP the user doesn't have to define operator==() in every
           model, the u_xyz data members are compared here. I don't like it though,
           one caveat of this is that if a user defines the operator==() then these
           data members will be compared twice. */
        return model.u_table        == derivedRight.u_table        &&
               model.u_incrementing == derivedRight.u_incrementing &&
               model.u_primaryKey   == derivedRight.u_primaryKey   &&
               model.u_with         == derivedRight.u_with         &&
//               model.u_withCount    == derivedRight.u_withCount    &&
               model.u_connection   == derivedRight.u_connection   &&
               // HasAttributes
               model.u_appends      == derivedRight.u_appends      &&
               // HasRelationships
               model.u_touches      == derivedRight.u_touches      &&
               // HasTimestamps
               model.u_timestamps   == derivedRight.u_timestamps;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::strong_ordering
    Model<Derived, AllRelations...>::operator<=>(const Model &right) const
#if defined(__clang__) && __clang_major__ >= 16
    requires std::is_integral_v<typename Derived::KeyType>
#endif
    {
        const auto leftKey = getKeyCasted();
        const auto rightKey = right.getKeyCasted();

        // Move models with the null, invalid, or undefined primary keys to the bottom
        if (leftKey == 0)
            return std::strong_ordering::greater;
        if (rightKey == 0)
            return std::strong_ordering::less;

        return leftKey <=> rightKey;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::fill(const QList<AttributeItem> &attributes)
    {
        const auto totallyGuarded = this->totallyGuarded();

        for (auto &attribute : this->fillableFromArray(attributes))
            /* The developers may choose to place some attributes in the "fillable" vector
               which means only those attributes may be set through mass assignment to
               the model, and all others will just get ignored for security reasons. */
            if (const auto &key = attribute.key;
                this->isFillable(key)
            )
                this->setAttribute(key, std::move(attribute.value));

            else if (totallyGuarded)
                throwTotallyGuarded(key);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::fill(QList<AttributeItem> &&attributes)
    {
        const auto totallyGuarded = this->totallyGuarded();

        for (auto &attribute : this->fillableFromArray(std::move(attributes))) {
            /* The developers may choose to place some attributes in the "fillable" vector
               which means only those attributes may be set through mass assignment to
               the model, and all others will just get ignored for security reasons. */
            if (const auto &key = attribute.key;
                this->isFillable(key)
            )
                this->setAttribute(key, std::move(attribute.value));

            else if (totallyGuarded)
                throwTotallyGuarded(key);
        }

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::forceFill(
            const QList<AttributeItem> &attributes)
    {
        // Prevent unnecessary unguard
        if (attributes.isEmpty())
            return model();

        GuardedModel::unguarded([this, &attributes]
        {
            fill(attributes);
        });

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::forceFill(QList<AttributeItem> &&attributes)
    {
        // Prevent unnecessary unguard
        if (attributes.isEmpty())
            return model();

        GuardedModel::unguarded([this, &attributes]
        {
            fill(std::move(attributes));
        });

        return model();
    }

    /* Model Instance methods */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQuery() const
    {
        // Ownership of a unique_ptr()
        auto query = newQueryWithoutScopes();

        if constexpr (extendsSoftDeletes())
            query->enableSoftDeletes();

        return query;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQueryWithoutScopes() const
    {
        // Ownership of a unique_ptr()
        auto builder = newModelQuery();

        builder->with(model().u_with);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newModelQuery() const
    {
        // Ownership of the std::shared_ptr<QueryBuilder>
        auto query = newBaseQueryBuilder();

        /* Model is passed to the TinyBuilder ctor, because of that setModel()
           isn't used here. Can't be const because of passed non-const model
           to the TinyBuilder. */
        return newTinyBuilder(std::move(query));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQueryWithoutRelationships() const
    {
        // Ownership of the std::shared_ptr<QueryBuilder>
        auto query = newModelQuery();

        if constexpr (extendsSoftDeletes())
            query->enableSoftDeletes();

        return query;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newTinyBuilder(
            std::shared_ptr<QueryBuilder> query) const
    {
        return std::make_unique<TinyBuilder<Derived>>(std::move(query), model());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newFromBuilder(
            const QList<AttributeItem> &attributes,
            const std::optional<QString> &connection) const
    {
        auto model = newInstance({}, true);

        model.setRawAttributes(attributes, true);

        model.setConnection(connection ? *connection : getConnectionName());

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newFromBuilder(
            QList<AttributeItem> &&attributes,
            const std::optional<QString> &connection) const
    {
        auto model = newInstance({}, true);

        model.setRawAttributes(std::move(attributes), true);

        model.setConnection(connection ? *connection : getConnectionName());

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newInstance() const
    {
        return newInstance({});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newInstance(
            const QList<AttributeItem> &attributes, const bool exists_) const
    {
        /* This method just provides a convenient way for us to generate fresh model
           instances of this current model. It is particularly useful during the
           hydration of new objects via the QueryBuilder instances. */
        auto model = Derived::instance();

        /* setAttribute() can call getDateFormat() inside and it tries to obtain
           the date format from grammar which is obtained from the connection, so
           the connection have to be set before fill(). */
        model.setConnection(getConnectionName());

        model.mergeCasts(getUserCasts());
        model.fill(attributes);

        // I want to have these two as the last thing
        model.exists = exists_;
        model.setTable(this->model().getTable()); // model() needed as it's overridden in the BasePivot

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newInstance(
            QList<AttributeItem> &&attributes, const bool exists_) const
    {
        /* This method just provides a convenient way for us to generate fresh model
           instances of this current model. It is particularly useful during the
           hydration of new objects via the QueryBuilder instances. */
        auto model = Derived::instance();

        /* setAttribute() can call getDateFormat() inside and it tries to obtain
           the date format from grammar which is obtained from the connection, so
           the connection have to be set before fill(). */
        model.setConnection(getConnectionName());

        model.mergeCasts(getUserCasts());
        model.fill(std::move(attributes));

        // I want to have these two as the last thing
        model.exists = exists_;
        model.setTable(this->model().getTable()); // model() needed as it's overridden in the BasePivot

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived Model<Derived, AllRelations...>::replicate(
            const std::unordered_set<QString> &except) const
    {
        /* Get all attributes excluding the primary key, created_at, and updated_at
           attributes and those in the except set. */
        auto attributes = AttributeUtils::exceptAttributesForReplicate(*this, except);

        /* Create a new instance (with correctly set a table, connection name, and casts),
           set obtained attributes and relations. */
        return Helpers::tap<Derived>(newInstance(),
                                     [this, &attributes](Derived &instance)
        {
            instance.setRawAttributes(std::move(attributes));

            instance.setRelations(this->getRelations());
        });
    }

    // NOTE api different, passing down a pivot timestamps data silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename PivotType, typename Parent>
    PivotType
    Model<Derived, AllRelations...>::newPivot(
            const Parent &parent, const QList<AttributeItem> &attributes,
            const QString &table, const bool exists_, const bool withTimestamps,
            const QString &createdAt, const QString &updatedAt) const
    {
        if constexpr (std::is_same_v<PivotType, Relations::Pivot>)
            return PivotType::template fromAttributes<Parent>(
                        parent, attributes, table, exists_, withTimestamps,
                        createdAt, updatedAt);
        else
            return PivotType::template fromRawAttributes<Parent>(
                        parent, attributes, table, exists_, withTimestamps,
                        createdAt, updatedAt);
    }

    /* Static cast this to a child's instance type (CRTP) */

    TINY_CRTP_MODEL_DEFINITIONS(Model)

    /* Serialization */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename PivotType>
    QVariantMap
    Model<Derived, AllRelations...>::toMap() const
    {
        auto attributes = this->attributesToMap();

        attributes.insert(this->template serializeRelations<QVariantMap, PivotType>());

        return attributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename PivotType>
    QList<AttributeItem>
    Model<Derived, AllRelations...>::toList() const
    {
        auto attributes = this->attributesToList();

        attributes << this->template serializeRelations<QList<AttributeItem>,
                                                        PivotType>();
        return attributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QJsonObject
    Model<Derived, AllRelations...>::toJsonObject() const
    {
        return QJsonObject::fromVariantMap(
                AttributeUtils::fixQtNullVariantBug(toMap()));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QJsonDocument
    Model<Derived, AllRelations...>::toJsonDocument() const
    {
        return QJsonDocument(toJsonObject());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QByteArray
    Model<Derived, AllRelations...>::toJson(const QJsonDocument::JsonFormat format) const
    {
        return toJsonDocument().toJson(format);
    }

    /* Getters / Setters */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getConnectionName() const
    {
#ifdef TINYORM_TESTS_CODE
        // Used from tests to override connection
        if (const auto &connection = ConnectionOverride::connection;
            !connection.isEmpty()
        )
            return connection;
#endif

        return model().u_connection;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    DatabaseConnection &
    Model<Derived, AllRelations...>::getConnection() const
    {
        return getConnectionResolver()->connection(getConnectionName());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setConnection(const QString &name)
    {
        auto &model = this->model();

        model.u_connection = name;

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setConnection(QString &&name)
    {
        auto &model = this->model();

        model.u_connection = std::move(name);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setTable(const QString &value)
    {
        auto &model = this->model();

        model.u_table = value;

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getTable() const
    {
        auto &model = this->model();

        const auto &table = model.u_table;

        // Guess as pluralized snake_case table name and set the u_table
        if (table.isEmpty())
            const_cast<QString &>(model.u_table) =
                TMPL_PLURAL.arg(
                    StringUtils::snake(TypeUtils::classPureBasename<Derived>()));

        return table;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getKeyName() const
    {
        return model().u_primaryKey;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    Model<Derived, AllRelations...>::getQualifiedKeyName() const
    {
        return qualifyColumn(getKeyName());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getKey() const
    {
        return this->getAttribute(getKeyName());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    auto
    Model<Derived, AllRelations...>::getKeyCasted() const
    {
        return getKey().template value<typename Derived::KeyType>();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    Model<Derived, AllRelations...>::getIncrementing() const
    {
        return model().u_incrementing;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setIncrementing(const bool value)
    {
        auto &model = this->model();

        model.u_incrementing = value;

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString Model<Derived, AllRelations...>::getForeignKey() const
    {
        return QStringLiteral("%1_%2").arg(
                    StringUtils::snake(TypeUtils::classPureBasename<Derived>()),
                    getKeyName());
    }

    /* Others */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    Model<Derived, AllRelations...>::qualifyColumn(const QString &column) const
    {
        if (column.contains(DOT))
            return column;
                          // model() needed as it's overridden in the BasePivot
        return DOT_IN.arg(model().getTable(), column);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    constexpr bool Model<Derived, AllRelations...>::extendsSoftDeletes()
    {
        return std::is_base_of_v<SoftDeletes<Derived>, Derived>;
    }

    /* protected */

    /* HasTimestamps */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::CREATED_AT() noexcept
    {
        return Orm::Constants::CREATED_AT;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::UPDATED_AT() noexcept
    {
        return Orm::Constants::UPDATED_AT;
    }

    /* Model Instance methods */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::shared_ptr<QueryBuilder>
    Model<Derived, AllRelations...>::newBaseQueryBuilder() const
    {
        return getConnection().query();
    }

    /* Operations on a Model instance */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::performDeleteOnModel()
    {
        /* Ownership of a unique_ptr(), dereferenced and passed down, will be
           destroyed right after this command. */
        model().setKeysForSaveQuery(*newModelQuery()).remove(); // model() needed as it's overridden in the BasePivot

        this->exists = false;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    TinyBuilder<Derived> &
    Model<Derived, AllRelations...>::setKeysForSaveQuery(
            TinyBuilder<Derived> &query) const
    {
        return query.where(getKeyName(), EQ, getKeyForSaveQuery());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant Model<Derived, AllRelations...>::getKeyForSaveQuery() const
    {
        // Found
        if (const auto &keyName = getKeyName();
            this->m_originalHash.contains(keyName)
        )
            return this->m_original.at(this->m_originalHash.at(keyName)).value;

        // Not found, return the primary key value
        return getKey();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    TinyBuilder<Derived> &
    Model<Derived, AllRelations...>::setKeysForSelectQuery(
            TinyBuilder<Derived> &query) const
    {
        return query.where(getKeyName(), EQ, getKeyForSelectQuery());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant Model<Derived, AllRelations...>::getKeyForSelectQuery() const
    {
        // Currently is implemented exactly in the same way
        return getKeyForSaveQuery();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::performInsert(
            const TinyBuilder<Derived> &query)
    {
//        if (!fireModelEvent("creating"))
//            return false;

        /* First we'll need to create a fresh query instance and touch the creation and
           update timestamps on this model, which are maintained by us for developer
           convenience. After, we will just continue saving these model instances. */
        if (this->usesTimestamps())
            this->updateTimestamps();

        /* If the model has an incrementing key, we can use the "insertGetId" method on
           the query builder, which will give us back the final inserted ID for this
           table from the database. Not all tables have to be incrementing though. */
        const auto &attributes = this->getAttributes();

        if (getIncrementing())
            insertAndSetId(query, attributes);

        /* If the table isn't incrementing we'll simply insert these attributes as they
           are. These attribute vectors must contain an "id" column previously placed
           there by the developer as the manually determined key for these models. */
        else
            if (attributes.isEmpty())
                return true;
            else
                query.insert(attributes);

        /* We will go ahead and set the exists property to true, so that it is set when
           the created event is fired, just in case the developer tries to update it
           during the event. This will allow them to do so and run an update here. */
        this->exists = true;

//        fireModelEvent("created", false);

        return true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::performUpdate(TinyBuilder<Derived> &query)
    {
        /* If the updating event returns false, we will cancel the update operation so
           developers can hook Validation systems into their models and cancel this
           operation if the model does not pass validation. Otherwise, we update. */
//        if (!fireModelEvent("updating"))
//            return false;

        /* First we need to create a fresh query instance and touch the creation and
           update timestamp on the model which are maintained by us for developer
           convenience. Then we will just continue saving the model instances. */
        if (this->usesTimestamps())
            this->updateTimestamps();

        /* Once we have run the update operation, we will fire the "updated" event for
           this model instance. This will allow developers to hook into these after
           models are updated, giving them a chance to do any special processing. */
        const auto dirty = this->getDirty();

        if (!dirty.isEmpty()) {
            // model() needed as it's overridden in the BasePivot
            model().setKeysForSaveQuery(query).update(
                        AttributeUtils::convertVectorToUpdateItem(dirty));

            this->syncChanges();

//            fireModelEvent("updated", false);
        }

        return true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::finishSave(const SaveOptions options)
    {
//        fireModelEvent('saved', false);

        if (this->isDirty() && options.touch)
            this->touchOwners();

        this->syncOriginal();
    }

    // FEATURE dilemma primarykey, add support for Derived::KeyType silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    quint64 Model<Derived, AllRelations...>::insertAndSetId(
            const TinyBuilder<Derived> &query,
            const QList<AttributeItem> &attributes)
    {
        const auto &keyName = getKeyName();

        const auto id = query.insertGetId(attributes, keyName);

        // NOTE api different, Eloquent doesn't check like below and returns void instead silverqx
        // Insert was successful
        if (id != 0)
            this->setAttribute(keyName, id);

        /* QSqlQuery returns an invalid QVariant if can't obtain last inserted id,
           which is converted to 0. */
        return id;
    }

    /* private */

    /* Operations on a Model instance */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, TSqlQuery>
    Model<Derived, AllRelations...>::incrementOrDecrement(
            const QString &column, const T amount, const QList<AttributeItem> &extra,
            const IncrementOrDecrement method, const bool all)
    {
        // Ownership of a unique_ptr()
        auto builder = newQueryWithoutRelationships();

        /* Increment/Decrement all rows in the database, this is damn dangerous 🙃.
           It increments/decrements all columns because the model doesn't exist which
           means we don't have the ID. Also, in this case, there is no need to prefill
           attributes because there is nothing to increment/decrement, and filling extras
           also makes no sense. */
        if (!exists) {
            /* This makes it much safer because a user must explicitly select this
               behavior using the all parameter. */
            if (!all)
                return {-1, getConnection().getSqlQuery()};

            return invokeIncrementOrDecrement(*builder, column, amount, extra, method,
                                              all);
        }

        // Prefill an amount and extra attributes on the current model
        {
            auto attributeReference = this->operator[](column);
            attributeReference = attributeReference->template value<T>() + amount;

            forceFill(extra);
        }

        // Execute the increment/decrement query on the database for the current model
        auto result = invokeIncrementOrDecrement(*builder, column, amount, extra, method,
                                                 all);

        // Synchronize changes manually
        this->syncChanges();

        // Update originals so that they are not dirty
        QStringList updatedAttributes {column};
        std::ranges::transform(extra, std::back_inserter(updatedAttributes),
                               [](const auto &attribute) { return attribute.key; });

        this->syncOriginalAttributes(updatedAttributes);

        return result;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, TSqlQuery>
    Model<Derived, AllRelations...>::invokeIncrementOrDecrement(
            TinyBuilder<Derived> &query, const QString &column, const T amount,
            const QList<AttributeItem> &extra, const IncrementOrDecrement method,
            const bool all)
    {
        if (!all)
            setKeysForSaveQuery(query);

        const auto extraConverted = AttributeUtils::convertVectorToUpdateItem(extra);

        if (method == Increment)
            return query.increment(column, amount, extraConverted);

        if (method == Decrement)
            return query.decrement(column, amount, extraConverted);

        Q_UNREACHABLE();
    }

    /* HasAttributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::fillWithCRTPCheck(
            const QList<AttributeItem> &attributes)
    {
        if (attributes.isEmpty())
            return;

        throwIfCRTPctorProblem(attributes);

        fill(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::fillWithCRTPCheck(
            QList<AttributeItem> &&attributes)
    {
        if (attributes.isEmpty())
            return;

        throwIfCRTPctorProblem(attributes);

        fill(std::move(attributes));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::throwIfCRTPctorProblem(
            const QList<AttributeItem> &attributes)
    {
        throwIfQDateTimeAttribute(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::throwIfQDateTimeAttribute(
            const QList<AttributeItem> &attributes)
    {
        static const auto message = QStringLiteral(
            "Attributes passed to the '%1' model's constructor or Default Attribute "
            "Values defined in the '%1::u_attributes' data member can't contain the "
            "QDateTime attribute, to create a '%1' model instance with attributes that "
            "contain the QDateTime attribute use the %1::instance() method instead "
            "(recommended) or convert the '%2' QDateTime attribute to the QString "
            "(not recommended).");

        for (const auto &[key, value] : attributes)
            if (value.isValid() && !value.isNull() &&
                value.typeId() == QMetaType::QDateTime
            )
                throw Orm::Exceptions::InvalidArgumentError(
                        message.arg(TypeUtils::classPureBasename<Derived>(), key));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::throwTotallyGuarded(const QString &key)
    {
        throw Exceptions::MassAssignmentError(
                    QStringLiteral("Add '%1' to u_fillable data member to allow "
                                   "mass assignment on the '%2' model.")
                    .arg(key, TypeUtils::classPureBasename<Derived>()));
    }

    /* Getters for u_ data members defined in the Derived models, helps to avoid
       'friend GuardsAttributes/HasTimestamps' declarations in models when a u_ data
       members are private/protected.
       Also, the non-static methods are ok for u_ static data members, the caller's
       method constness controls the constness. There is one case when the getUserXyz()
       method can be made static and that is when the u_ static data member doesn't
       need be modified like the getUserRelations() or getUserMutators(). */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString &
    Model<Derived, AllRelations...>::getUserDateFormat() noexcept
    {
        return Derived::u_dateFormat;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getUserDateFormat() const noexcept
    {
        return Derived::u_dateFormat;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString &
    Model<Derived, AllRelations...>::getUserTimeFormat() noexcept
    {
        return Derived::u_timeFormat;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getUserTimeFormat() const noexcept
    {
        return Derived::u_timeFormat;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getUserDates() noexcept
    {
        return Derived::u_dates;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unordered_map<QString, CastItem> &
    Model<Derived, AllRelations...>::getUserCasts() noexcept
    {
        return Derived::u_casts;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::unordered_map<QString, CastItem> &
    Model<Derived, AllRelations...>::getUserCasts() const noexcept
    {
        return Derived::u_casts;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool &
    Model<Derived, AllRelations...>::getUserSnakeAttributes() noexcept
    {
        return Derived::u_snakeAttributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::getUserSnakeAttributes() const noexcept
    {
        return Derived::u_snakeAttributes;
    }

    /* GuardsAttributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QStringList &
    Model<Derived, AllRelations...>::getUserFillable() noexcept
    {
        return Derived::u_fillable;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getUserFillable() const noexcept
    {
        return Derived::u_fillable;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QStringList &
    Model<Derived, AllRelations...>::getUserGuarded() noexcept
    {
        return Derived::u_guarded;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getUserGuarded() const noexcept
    {
        return Derived::u_guarded;
    }

    /* HasRelationships */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QHash<QString, typename Model<Derived, AllRelations...>::RelationVisitorAlias> &
    Model<Derived, AllRelations...>::getUserRelations() const noexcept
    {
        return model().u_relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getUserTouches() const noexcept
    {
        return model().u_touches;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QStringList &
    Model<Derived, AllRelations...>::getUserTouches() noexcept
    {
        return model().u_touches;
    }

    /* HasTimestamps */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool &
    Model<Derived, AllRelations...>::getUserTimestamps() noexcept
    {
        return model().u_timestamps;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::getUserTimestamps() const noexcept
    {
        return model().u_timestamps;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getUserCreatedAtColumn() noexcept
    {
        return Derived::CREATED_AT();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getUserUpdatedAtColumn() noexcept
    {
        return Derived::UPDATED_AT();
    }

    /* HidesAttributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::set<QString> &
    Model<Derived, AllRelations...>::getUserVisible() noexcept
    {
        return Derived::u_visible;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::set<QString> &
    Model<Derived, AllRelations...>::getUserVisible() const noexcept
    {
        return Derived::u_visible;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::set<QString> &
    Model<Derived, AllRelations...>::getUserHidden() noexcept
    {
        return Derived::u_hidden;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::set<QString> &
    Model<Derived, AllRelations...>::getUserHidden() const noexcept
    {
        return Derived::u_hidden;
    }

    /* Serialization - Appends */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::set<QString> &
    Model<Derived, AllRelations...>::getUserAppends() noexcept
    {
        return model().u_appends;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::set<QString> &
    Model<Derived, AllRelations...>::getUserAppends() const noexcept
    {
        return model().u_appends;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QHash<QString, typename Concerns::HasAttributes<Derived, AllRelations...>
                                          ::MutatorFunction> &
    Model<Derived, AllRelations...>::getUserMutators() noexcept
    {
        return Derived::u_mutators;
    }

    /* Serialization - Attributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    Model<Derived, AllRelations...>::getUserSerializeDate(const QDate date)
    {
        return Derived::serializeDate(date);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    Model<Derived, AllRelations...>::getUserSerializeDateTime(const QDateTime &datetime)
    {
        return Derived::serializeDateTime(datetime);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    Model<Derived, AllRelations...>::getUserSerializeTime(const QTime time)
    {
        return Derived::serializeTime(time);
    }

    /* SoftDeletes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::initializeSoftDeletes() const
    {
        if constexpr (extendsSoftDeletes())
            model().SoftDeletes<Derived>::initializeSoftDeletes();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::appendToUserDates(const QString &column)
    {
        const_cast<QStringList &>(Derived::u_dates) << column;
    }

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_MODEL_HPP

// TODO release, recompile without PCH silverqx
// TODO release, open cmake generated Visual Studio 2019 solution and fix all errors/warnings silverqx
// TODO release, clang's UndefinedBehaviorSanitizer at https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html silverqx
// TODO repeat, merge changes from latest Eloquent silverqx
// TODO decide/unify when to use class/typename keywords for templates silverqx
// FEATURE EloquentCollection, solve how to implement, also look at Eloquent's Custom Collections silverqx
// FUTURE try to compile every header file by itself and catch up missing dependencies and forward declaration, every header file should be compilable by itself silverqx
// FUTURE include every stl dependency in header files silverqx
// FEATURE logging, add support for custom logging, logging to the defined stream?, I don't exactly know how I will solve this issue, design it 🤔 silverqx
// FEATURE code coverage silverqx
// TODO dilemma, function params. like direction asc/desc for orderBy, operators for where are QStrings, but they should be flags for performance reasons, how to solve this and preserve nice clean api? that is the question 🤔 silverqx
// FEATURE build systems, add docs on how to make a production build of the TinyORM library silverqx
// FEATURE build systems, add docs on how to set up dev. environment and how to run auto tests silverqx
// FEATURE build systems, libuv example how it could look like https://github.com/libuv/libuv silverqx
// FUTURE cmake can generate export header file by GenerateExportHeader module, find way to use it, because I have own export header file, how to unify this? I don't know now silverqx
// CUR generate pkg-config file on unix silverqx
// CUR cmake use gold linker option, https://github.com/frobware/c-hacks/blob/master/cmake/use-gold-linker.cmake silverqx
// BUG mingw64, TinyOrmPlayground seg. fault at the end, but everything runs/passes correctly, but only when invoked from mintty terminal, when I run it from QtCreator that uses cmd I don't see any seg. fault silverqx
// BUG mingw64, seg fault in some tests eg. tst_model, and couldn't execute tests again, mingw64 shell works silverqx
// FUTURE mingw64, find out better solution for .text section exhausted in debug build, -Wa,-mbig-obj didn't help, -flto helps, but again it can not find reference to WinMain, so I had to compile with -O1, then it is ok silverqx
// FUTURE tests, QtCreator Qt AutoTests how to pass -maxwarnings silverqx
// FUTURE constexpr, version header file, good example https://googleapis.dev/cpp/google-cloud-common/0.6.1/version_8h_source.html silverqx
// BUG qmake impossible to add d postfix for dlls; I have found qtLibraryTarget() and qtPlatformTargetSuffix() in qt_functions.prf but I remember there was some other problem like when I added the d suffix something didn't work correctly, I don't exactly remember what it was silverqx
// BUG SemVer version cmake/qmake silverqx
// CUR cmake/qmake SOVERSION silverqx
// CUR 0.1.0 vs 0.1.0.0 Product/FileVersion, investigate, also check versions in pc, prl, ... silverqx
// CUR enable QT_ASCII_CAST_WARNINGS silverqx
// CUR enable QT_NO_CAST_FROM_ASCII silverqx
// BUG qmake MinGW UCRT64 clang static build duplicate symbols, this is MinGW bug silverqx
// BUG qmake MinGW UCRT64 clang shared build with inline_constants cause crashes of 50% of tests, this will be MinGW clang or clang bug, on unix it works without problems silverqx
// BUG cmake MinGW UCRT64 clang static build builds, but cause problem with inline_constants ; shared build with inline_constants cause crashes of 50% of tests, like bug above, this will be MinGW clang or clang bug, on unix it works without problems silverqx
// FUTURE linux, add linker version script https://github.com/sailfishos/qtbase/commit/72ba0079c3967bdfa26acdce78ce6cb98b30c27b?view=parallel https://www.gnu.org/software/gnulib/manual/html_node/Exported-Symbols-of-Shared-Libraries.html https://stackoverflow.com/questions/41061220/where-do-object-file-version-references-come-from silverqx
// TODO Visual Studio memory analyzer https://docs.microsoft.com/en-us/visualstudio/profiling/memory-usage-without-debugging2?view=vs-2019 silverqx
// CUR fix all modernize-pass-by-value silverqx
// CUR use using inside classes where appropriate silverqx
// CUR using namespace Orm::Constants; in ormtypes.hpp silverqx
// CUR make all * to *const silverqx
// CUR cmake/qmake run tests in parallel for tests that allow it silverqx
// CUR study how to use acquire/release memory order for m_queryLogId atomic silverqx
// FUTURE divide Query Builder and TinyOrm to own packages (dlls)? think about it 🤔 silverqx
// BUG clang on mingw inline static initialization with another static in the same class defined line before, all other compilers (on Linux too) works silverqx
// CUR docs mdx syntax highlight prism Treeview https://prismjs.com/plugins/treeview/ silverqx
// CUR docs IdealImage silverqx
// TODO cache static_cast<>(*this) in the model()/basemodel() CRTP as a class data member std::optional<std::reference_wrapper<Derived>> m_model = std::nullopt, but I will have to create copy ctor to set m_model {std::nullopt}, the same for other similar model() methods like Model::model(), then I can to check if (m_model) and return right away and I will call static_cast or dynamic_cast only once for every instance, it is cast every time now 😟 silverqx
// CUR docs exceptions, which methods throw which exceptions, so user know which exception classes to define in a catch blocks silverqx
// SEC fix LoadLibrary() before qsql plugin loads? https://www.qt.io/blog/security-advisory-qlockfile-qauthenticator-windows-platform-plugin silverqx
// CUR cmake, make TinyUtils_target in TinyTestCommon.cmake optional, not always needed to link to silverqx
// CUR model, add whereBelongsTo, whereRelation, orWhereRelation silverqx
// CUR propagation, https://ben.balter.com/2017/11/10/twelve-tips-for-growing-communities-around-your-open-source-project/ silverqx
// TODO vcpkg, solve how to build tom (when solving vcpkg builds again), currently I have hardly added tabulate to the vcpkg.json port and also manifest file; it will have to be conditional base of the TINYORM_DISABLE_TOM macro silverqx
// CUR schema, add tests for enum and set; and json and jsonb, storedAs / virtualAs silverqx
// CUR tom, don't modify migrate:status command, rather extend it and add possibility to only call it through Application::runWithArguments() (this secure to not be able to call it from the command-line), do not show it in the list or help command output silverqx
// CUR tom, think about remove TINYTOM_NO/_DEBUG and TINYTOM_TESTS_CODE and use TINYORM_ defines instead, or at least check also TINYORM_NO/_DEBUG if TINY_NO/_DEBUG is not set silverqx
// CUR use EMPTY constant everywhere silverqx
// CUR tom, squashing migrations silverqx
// CUR tom, create icon silverqx
// BUG qmake, in Makefile.Debug during debug\moc_predefs.h in all tests silverqx
// BUG qmake, clang-cl (win32-clang-msvc) ctrl+shift+alt+b fails with: fatal error: PCH file 'debug\TinyOrm_pch.pch' not found: module file not found silverqx
// FUTURE security workflows, add ggshield https://github.com/GitGuardian/ggshield silverqx
// CUR mariadb workflows silverqx
// FEATURE HidesAttributes, and ModelsCollection::makeVisible/Hidden, ... silverqx
// TODO vcpkg, workflows enable binary caching https://learn.microsoft.com/en-us/vcpkg/users/binarycaching#gha silverqx
// BUG linux if both mysql-c-connector and mariadb-c-connector are installed and is enabled mysql_ping and TinyORM library is linked against the libmariadb instead of the libmysqlclient then MySQL ping fails because bad mysql.h header is included; to fix this there should be some auto-detection against which c connector we are linking OR should be added eg. mysql_ping vs maria_ping or somehow flag we are linking against mariadb c connector silverqx
// FUTURE use Explicit object parameter (deducing this) P0847 everywhere instead of our CRTP pattern when c++23 will be out and ready https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/ silverqx
