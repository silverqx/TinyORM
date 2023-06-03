#pragma once
#ifndef ORM_TINY_MODEL_HPP
#define ORM_TINY_MODEL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <range/v3/view/filter.hpp>

#include "orm/concerns/hasconnectionresolver.hpp"
#include "orm/connectionresolverinterface.hpp"
#include "orm/tiny/concerns/guardsattributes.hpp"
#include "orm/tiny/concerns/hasattributes.hpp"
#include "orm/tiny/concerns/hasrelationships.hpp"
#include "orm/tiny/concerns/hastimestamps.hpp"
#include "orm/tiny/exceptions/massassignmenterror.hpp"
#include "orm/tiny/macros/crtpmodel.hpp"
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

    /*! Alias for the GuardedModel. */
    using GuardedModel = Concerns::GuardedModel;

    // TODO model missing methods Model::loadMissing() silverqx
    // TODO model missing methods EloquentCollection::toQuery() silverqx
    // TODO model missing saveOrFail(), updateOrFail(), deleteOrFail(), I will need to implement ManagesTransaction::transaction(callback) method silverqx
    /*! Base model class. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model : public Orm::Concerns::HasConnectionResolver,
                  public Concerns::HasAttributes<Derived, AllRelations...>,
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
        // Used by BaseRelationStore::visit() and also by visted methods
        friend Concerns::HasRelationStore<Derived, AllRelations...>;
        // To access getUserXx() methods
        friend Concerns::HasRelationships<Derived, AllRelations...>;
        // Used by QueriesRelationships::has()
        friend Concerns::QueriesRelationships<Derived>;
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

        /* Constructors */
        /*! Create a new TinORM model instance, default constructor. */
        Model();
        /*! Default destructor. */
        inline ~Model() = default;

        /*! Model's copy constructor. */
        inline Model(const Model &) = default;
        /*! Model's move constructor. */
        inline Model(Model &&) noexcept = default;

        /*! Model's copy assignment operator. */
        inline Model &operator=(const Model &) = default;
        /*! Model's move assignment operator. */
        inline Model &operator=(Model &&) noexcept = default;

        /*! Create a new TinORM model instance from attributes
            (converting constructor). */
        explicit Model(const QVector<AttributeItem> &attributes);
        /*! Create a new TinORM model instance from attributes
            (converting constructor). */
        explicit Model(QVector<AttributeItem> &&attributes);

        /*! Create a new TinORM model instance from attributes
            (list initialization). */
        Model(std::initializer_list<AttributeItem> attributes);

        /*! Create a new TinORM model instance, skip-filling default attribute
            values. */
        explicit Model(DontFillDefaultAttributes /*unused*/);

        /* Static operations on the Model class */
        /*! Create a new TinyORM model instance. */
        static Derived instance();
        /*! Create a new TinyORM model instance. */
        static Derived instance(const QString &connection);
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(const QVector<AttributeItem> &attributes);
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(QVector<AttributeItem> &&attributes);
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(const QVector<AttributeItem> &attributes,
                                const QString &connection);
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(QVector<AttributeItem> &&attributes,
                                const QString &connection);

        /*! Create a new TinyORM model instance on the heap. */
        static std::unique_ptr<Derived> instanceHeap();
        /*! Create a new TinyORM model instance on the heap. */
        static std::unique_ptr<Derived> instanceHeap(const QString &connection);
        /*! Create a new TinyORM model instance on the heap with given attributes. */
        static std::unique_ptr<Derived>
        instanceHeap(const QVector<AttributeItem> &attributes);
        /*! Create a new TinyORM model instance on the heap with given attributes. */
        static std::unique_ptr<Derived>
        instanceHeap(QVector<AttributeItem> &&attributes);
        /*! Create a new TinyORM model instance on the heap with given attributes. */
        static std::unique_ptr<Derived>
        instanceHeap(const QVector<AttributeItem> &attributes,
                     const QString &connection);
        /*! Create a new TinyORM model instance on the heap with given attributes. */
        static std::unique_ptr<Derived>
        instanceHeap(QVector<AttributeItem> &&attributes, const QString &connection);

        /*! Begin querying the model. */
        static std::unique_ptr<TinyBuilder<Derived>> query();
        /*! Begin querying the model on a given connection. */
        static std::unique_ptr<TinyBuilder<Derived>> on(const QString &connection = "");

        /*! Get all of the models from the database. */
        static ModelsCollection<Derived> all(const QVector<Column> &columns = {ASTERISK});

        /*! Destroy the models for the given IDs. */
        static std::size_t destroy(const QVector<QVariant> &ids);
        /*! Destroy the model by the given ID. */
        inline static std::size_t destroy(const QVariant &id);

        /* Operations on a Model instance */
        /*! Save the model to the database. */
        bool save(SaveOptions options = {});
        /*! Save the model and all of its relationships. */
        bool push();

        /*! Update records in the database. */
        bool update(const QVector<AttributeItem> &attributes, SaveOptions options = {});

        /*! Increment a column's value by a given amount. */
        template<typename T = std::size_t> requires std::is_arithmetic_v<T>
        inline std::tuple<int, QSqlQuery>
        increment(const QString &column, T amount = 1,
                  const QVector<AttributeItem> &extra = {}, bool all = false);
        /*! Decrement a column's value by a given amount. */
        template<typename T = std::size_t> requires std::is_arithmetic_v<T>
        inline std::tuple<int, QSqlQuery>
        decrement(const QString &column, T amount = 1,
                  const QVector<AttributeItem> &extra = {}, bool all = false);

        /*! Delete the model from the database. */
        bool remove();
        /*! Delete the model from the database, alias. */
        bool deleteModel();

        /*! Reload a fresh model instance from the database. */
        std::optional<Derived> fresh(const QVector<WithItem> &relations = {});
        /*! Reload a fresh model instance from the database. */
        std::optional<Derived> fresh(const QString &relation);
        /*! Reload the current model instance with fresh attributes from the database. */
        Derived &refresh();

        /*! Eager load relations on the model. */
        template<typename = void>
        Derived &load(const QVector<WithItem> &relations);
        /*! Eager load relations on the model. */
        template<typename = void>
        Derived &load(QString relation);
        /*! Eager load relations on the model. */
        inline Derived &load(const QVector<QString> &relations);
        /*! Eager load relations on the model. */
        inline Derived &load(QVector<QString> &&relations);

        /*! Determine if two models have the same ID and belong to the same table. */
        template<ModelConcept ModelToCompare>
        bool is(const std::optional<ModelToCompare> &model) const;
        /*! Determine if two models are not the same. */
        template<ModelConcept ModelToCompare>
        bool isNot(const std::optional<ModelToCompare> &model) const;

        /*! Equality comparison operator for the Model. */
        bool operator==(const Model &right) const;

#if defined(__clang__) && __clang_major__ >= 16
        /*! Three-way comparison operator for the Model. */
        std::strong_ordering operator<=>(const Model &right) const;
        requires std::is_integral_v<typename Derived::KeyType>;
#else
        /*! Three-way comparison operator for the Model. */
        std::strong_ordering operator<=>(const Model &right) const;
#endif

        /*! Fill the model with a vector of attributes. */
        Derived &fill(const QVector<AttributeItem> &attributes);
        /*! Fill the model with a vector of attributes. */
        Derived &fill(QVector<AttributeItem> &&attributes);
        /*! Fill the model with a vector of attributes. Force mass assignment. */
        Derived &forceFill(const QVector<AttributeItem> &attributes);
        /*! Fill the model with a vector of attributes. Force mass assignment. */
        Derived &forceFill(QVector<AttributeItem> &&attributes);

        /* Model Instance methods */
        /*! Get a new query builder for the model's table. */
        inline std::unique_ptr<TinyBuilder<Derived>> newQuery();
        /*! Get a new query builder that doesn't have any global scopes. */
        std::unique_ptr<TinyBuilder<Derived>> newQueryWithoutScopes();
        /*! Get a new query builder that doesn't have any global scopes or
            eager loading. */
        std::unique_ptr<TinyBuilder<Derived>> newModelQuery();
        /*! Get a new query builder with no relationships loaded. */
        std::unique_ptr<TinyBuilder<Derived>> newQueryWithoutRelationships();
        /*! Create a new Tiny query builder for the model. */
        std::unique_ptr<TinyBuilder<Derived>>
        newTinyBuilder(std::shared_ptr<QueryBuilder> query);

        /*! Create a new model instance that is existing. */
        Derived newFromBuilder(const QVector<AttributeItem> &attributes = {},
                               const std::optional<QString> &connection = std::nullopt);
        /*! Create a new model instance that is existing. */
        Derived newFromBuilder(QVector<AttributeItem> &&attributes = {},
                               const std::optional<QString> &connection = std::nullopt);
        /*! Create a new instance of the given model. */
        inline Derived newInstance();
        /*! Create a new instance of the given model. */
        Derived newInstance(const QVector<AttributeItem> &attributes,
                            bool exists = false);
        /*! Create a new instance of the given model. */
        Derived newInstance(QVector<AttributeItem> &&attributes,
                            bool exists = false);

        /*! Clone the model into a new, non-existing instance. */
        Derived replicate(const std::unordered_set<QString> &except = {});

        /*! Create a new pivot model instance. */
        template<typename PivotType = Relations::Pivot, typename Parent>
        PivotType
        newPivot(const Parent &parent, const QVector<AttributeItem> &attributes,
                 const QString &table, bool exists, bool withTimestamps = false,
                 const QString &createdAt = Constants::CREATED_AT,
                 const QString &updatedAt = Constants::UPDATED_AT) const;

        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_DECLARATIONS

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
        /*! Determina whether the Derived Model extends the SoftDeletes. */
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
        setKeysForSaveQuery(TinyBuilder<Derived> &query);
        /*! Get the primary key value for a save query. */
        QVariant getKeyForSaveQuery() const;

        /*! Set the keys for a select query. */
        TinyBuilder<Derived> &
        setKeysForSelectQuery(TinyBuilder<Derived> &query);
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
                               const QVector<AttributeItem> &attributes);

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
        QVector<QString> u_with;
        /*! The relationship counts that should be eager loaded on every query. */
//        QVector<WithItem> u_withCount;

    private:
        /* Operations on a Model instance */
        /*! Method to call in the incrementOrDecrement(). */
        enum struct IncrementOrDecrement
        {
            INCREMENT,
            DECREMENT,
        };
        /*! Call the increment() method. */
        constexpr static auto Increment = IncrementOrDecrement::INCREMENT;
        /*! Call the decrement() method. */
        constexpr static auto Decrement = IncrementOrDecrement::DECREMENT;

        /*! Run the increment or decrement method on the model. */
        template<typename T> requires std::is_arithmetic_v<T>
        std::tuple<int, QSqlQuery>
        incrementOrDecrement(
                    const QString &column, T amount, const QVector<AttributeItem> &extra,
                    IncrementOrDecrement method, bool all);

        /* HasAttributes */
        /*! Fill the model with a vector of attributes with the CRTP check. */
        void fillWithCRTPCheck(const QVector<AttributeItem> &attributes);
        /*! Fill the model with a vector of attributes with the CRTP check. */
        void fillWithCRTPCheck(QVector<AttributeItem> &&attributes);

        /*! Throw an InvalidArgumentError if the attributes passed to the constructor
            contain any value that causes access to some data member in the derived
            instance that is not yet initialized. */
        inline static void
        throwIfCRTPctorProblem(const QVector<AttributeItem> &attributes);
        /*! The QDateTime attribute detected, causes CRTP ctor problem. */
        static void throwIfQDateTimeAttribute(const QVector<AttributeItem> &attributes);
        /*! Throw if an attempt to fill a guarded attribute is detected
            (mass assignment). */
        static void throwIfTotallyGuarded(const QString &key);

        /*! Get the u_dateFormat attribute from the Derived model. */
        inline QString &getUserDateFormat();
        /*! Get the u_dateFormat attribute from the Derived model. */
        inline const QString &getUserDateFormat() const;
        /*! Get the u_dates attribute from the Derived model. */
        inline static const QStringList &getUserDates();
        /*! Get the casts hash. */
        inline std::unordered_map<QString, CastItem> &getUserCasts();
        /*! Get the casts hash. */
        inline const std::unordered_map<QString, CastItem> &getUserCasts() const;

        /* GuardsAttributes */
        /*! Get the u_fillable attributes from the Derived model. */
        inline QStringList &getUserFillable();
        /*! Get the u_fillable attributes from the Derived model. */
        inline const QStringList &getUserFillable() const;
        /*! Get the u_guarded attributes from the Derived model. */
        inline QStringList &getUserGuarded();
        /*! Get the u_guarded attributes from the Derived model. */
        inline const QStringList &getUserGuarded() const;

        /* HasRelationships */
        /*! Relation visitor lambda type (an alias for shorter declarations). */
        using RelationVisitorAlias =
                typename Concerns::HasRelationships<Derived, AllRelations...>
                                 ::RelationVisitor;

        /*! Get the u_fillable attributes from the Derived model. */
        inline const QHash<QString, RelationVisitorAlias> &getUserRelations() const;
        /*! Get the u_fillable attributes from the Derived model. */
        inline const QStringList &getUserTouches() const;

        /* HasTimestamps */
        /*! Get the u_timestamps attribute from the Derived model. */
        inline bool &getUserTimestamps();
        /*! Get the u_timestamps attribute from the Derived model. */
        inline bool getUserTimestamps() const;
        /*! Get the CREATED_AT attribute from the Derived model. */
        inline static const QString &getUserCreatedAtColumn() noexcept;
        /*! Get the UPDATED_AT attribute from the Derived model. */
        inline static const QString &getUserUpdatedAtColumn() noexcept;

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
    Model<Derived, AllRelations...>::Model(const QVector<AttributeItem> &attributes)
        : Model()
    {
        fillWithCRTPCheck(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(QVector<AttributeItem> &&attributes)
        : Model()
    {
        fillWithCRTPCheck(std::move(attributes));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(
            std::initializer_list<AttributeItem> attributes
    )
        : Model(QVector<AttributeItem>(attributes.begin(), attributes.end()))
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
    Model<Derived, AllRelations...>::instance(const QVector<AttributeItem> &attributes)
    {
        auto model = instance();

        model.fill(attributes);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(QVector<AttributeItem> &&attributes)
    {
        auto model = instance();

        model.fill(std::move(attributes));

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(const QVector<AttributeItem> &attributes,
                                              const QString &connection)
    {
        auto model = instance(attributes);

        model.setConnection(connection);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(QVector<AttributeItem> &&attributes,
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
            const QVector<AttributeItem> &attributes)
    {
        auto model = instanceHeap();

        model->fill(attributes);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<Derived>
    Model<Derived, AllRelations...>::instanceHeap(QVector<AttributeItem> &&attributes)
    {
        auto model = instanceHeap();

        model->fill(std::move(attributes));

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<Derived>
    Model<Derived, AllRelations...>::instanceHeap(
            const QVector<AttributeItem> &attributes, const QString &connection)
    {
        auto model = instanceHeap(attributes);

        model->setConnection(connection);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<Derived>
    Model<Derived, AllRelations...>::instanceHeap(
            QVector<AttributeItem> &&attributes, const QString &connection)
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
    Model<Derived, AllRelations...>::all(const QVector<Column> &columns)
    {
        return query()->get(columns);
    }

    // TODO cpp check all int types and use std::size_t where appropriate silverqx
    // FEATURE dilemma primarykey, id should be Derived::KeyType, if I don't solve this problem, do runtime type check, QVariant type has to be the same type like KeyType and throw exception silverqx
    // TODO next test all this remove()/destroy() methods, when deletion fails silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::size_t
    Model<Derived, AllRelations...>::destroy(const QVector<QVariant> &ids)
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
        return destroy(QVector<QVariant> {id});
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
            // FUTURE Eloquent uses array_filter on models, investigate when this happens, null value (model) in many relations? silverqx
            /* Following Eloquent API, if any push failed, then quit, remaining push-es
               will not be processed. */
            if (!this->pushWithVisitor(relation, models))
                return false;

        return true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::update(
            const QVector<AttributeItem> &attributes,
            const SaveOptions options)
    {
        if (!exists)
            return false;

        return fill(attributes).save(options);
    }

    // NOTE api different, I have added the 'all' bool param. to avoid updating all rows by mistake silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Model<Derived, AllRelations...>::increment(
            const QString &column, const T amount, const QVector<AttributeItem> &extra,
            const bool all)
    {
        return incrementOrDecrement(column, amount, extra, Increment, all);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Model<Derived, AllRelations...>::decrement(
            const QString &column, const T amount, const QVector<AttributeItem> &extra,
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
    Model<Derived, AllRelations...>::fresh(
            const QVector<WithItem> &relations)
    {
        if (!exists)
            return std::nullopt;

        return model().setKeysForSelectQuery(*newQueryWithoutScopes())
                .with(relations)
                .first();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::fresh(const QString &relation)
    {
        return fresh(QVector<WithItem> {{relation}});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::refresh()
    {
        if (!exists)
            return model();

        this->setRawAttributes(model().setKeysForSelectQuery(*newQueryWithoutScopes())
                               .firstOrFail().getRawAttributes());

        // And reload them again, refresh relations
        load(this->getLoadedRelationsWithoutPivot());

        this->syncOriginal();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename>
    Derived &
    Model<Derived, AllRelations...>::load(const QVector<WithItem> &relations)
    {
        // Ownership of a unique_ptr()
        auto builder = newQueryWithoutRelationships();

        builder->with(relations)
                .eagerLoadRelations(model());

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename>
    Derived &
    Model<Derived, AllRelations...>::load(QString relation)
    {
        return load(QVector<WithItem> {{std::move(relation)}});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::load(const QVector<QString> &relations)
    {
        return load(WithItem::fromStringVector(relations));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::load(QVector<QString> &&relations)
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
               this->model().getTable() == model->getTable() &&
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
    bool Model<Derived, AllRelations...>::operator==(const Model &right) const
    {
        /* Comparing the HasConnectionResolver, GuardsAttributes, ModelProxies, and
           IsModel is not needed as they don't contain any data members or they
           contain ony a static data members. */

        // Compare the HasAttributes concern
        using HasAttributes = Concerns::HasAttributes<Derived, AllRelations...>;
        if (static_cast<const HasAttributes &>(*this) !=
            static_cast<const HasAttributes &>(right)
        )
            return false;

        // Compare the HasRelationships concern
        using HasRelationships = Concerns::HasRelationships<Derived, AllRelations...>;
        if (static_cast<const HasRelationships &>(*this) !=
            static_cast<const HasRelationships &>(right)
        )
            return false;

        // Compare the HasTimestamps concern
        using HasTimestamps = Concerns::HasTimestamps<Derived, AllRelations...>;
        if (static_cast<const HasTimestamps &>(*this) !=
            static_cast<const HasTimestamps &>(right)
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

        // Compare the Derived Model 😮🤯😎
        const auto &derivedRight = static_cast<const Derived &>(right);

        // model().u_relations == derivedRight.u_relations
        /* It compares only the size and keys and doesn't compare hash values because
           the std::function doesn't have a full/complete operator==() (it only compares
           for the nullptr). */
        if (!HasRelationships::compareURelations(model().u_relations,
                                                 derivedRight.u_relations))
            return false;

        return model().u_touches    == derivedRight.u_touches   &&
               model().u_timestamps == derivedRight.u_timestamps;
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
    Model<Derived, AllRelations...>::fill(const QVector<AttributeItem> &attributes)
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
                throwIfTotallyGuarded(key);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::fill(QVector<AttributeItem> &&attributes)
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
                throwIfTotallyGuarded(key);
        }

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::forceFill(
            const QVector<AttributeItem> &attributes)
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
    Model<Derived, AllRelations...>::forceFill(QVector<AttributeItem> &&attributes)
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
    Model<Derived, AllRelations...>::newQuery()
    {
        // Ownership of a unique_ptr()
        auto query = newQueryWithoutScopes();

        if constexpr (extendsSoftDeletes())
            query->enableSoftDeletes();

        return query;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQueryWithoutScopes()
    {
        // Ownership of a unique_ptr()
        auto builder = newModelQuery();

        builder->with(model().u_with);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newModelQuery()
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
    Model<Derived, AllRelations...>::newQueryWithoutRelationships()
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
            std::shared_ptr<QueryBuilder> query)
    {
        return std::make_unique<TinyBuilder<Derived>>(std::move(query), model());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newFromBuilder(
            const QVector<AttributeItem> &attributes,
            const std::optional<QString> &connection)
    {
        auto model = newInstance({}, true);

        model.setRawAttributes(attributes, true);

        model.setConnection(connection ? *connection : getConnectionName());

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newFromBuilder(
            QVector<AttributeItem> &&attributes,
            const std::optional<QString> &connection)
    {
        auto model = newInstance({}, true);

        model.setRawAttributes(std::move(attributes), true);

        model.setConnection(connection ? *connection : getConnectionName());

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newInstance()
    {
        return newInstance({});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newInstance(
            const QVector<AttributeItem> &attributes, const bool exists_)
    {
        /* This method just provides a convenient way for us to generate fresh model
           instances of this current model. It is particularly useful during the
           hydration of new objects via the QueryBuilder instances. */
        auto model = Derived::instance();

        /* setAttribute() can call getDateFormat() inside and it tries to obtain
           the date format from grammar which is obtained from the connection, so
           the connection have to be set before fill(). */
        model.setConnection(getConnectionName());

        model.mergeCasts(std::as_const(*this).getUserCasts());
        model.fill(attributes);

        // I want to have these two as the last thing
        model.exists = exists_;
        model.setTable(this->model().getTable());

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newInstance(
            QVector<AttributeItem> &&attributes, const bool exists_)
    {
        /* This method just provides a convenient way for us to generate fresh model
           instances of this current model. It is particularly useful during the
           hydration of new objects via the QueryBuilder instances. */
        auto model = Derived::instance();

        /* setAttribute() can call getDateFormat() inside and it tries to obtain
           the date format from grammar which is obtained from the connection, so
           the connection have to be set before fill(). */
        model.setConnection(getConnectionName());

        model.mergeCasts(std::as_const(*this).getUserCasts());
        model.fill(std::move(attributes));

        // I want to have these two as the last thing
        model.exists = exists_;
        model.setTable(this->model().getTable());

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived Model<Derived, AllRelations...>::replicate(
            const std::unordered_set<QString> &except)
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
            const Parent &parent, const QVector<AttributeItem> &attributes,
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
        model().u_connection = name;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setConnection(QString &&name)
    {
        model().u_connection = std::move(name);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setTable(const QString &value)
    {
        model().u_table = value;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getTable() const
    {
        const auto &table = model().u_table;

        // Guess as pluralized snake_case table name and set the u_table
        if (table.isEmpty())
            const_cast<QString &>(model().u_table) =
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
        model().u_incrementing = value;

        return model();
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
        model().setKeysForSaveQuery(*newModelQuery()).remove();

        this->exists = false;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    TinyBuilder<Derived> &
    Model<Derived, AllRelations...>::setKeysForSaveQuery(TinyBuilder<Derived> &query)
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
    Model<Derived, AllRelations...>::setKeysForSelectQuery(TinyBuilder<Derived> &query)
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
            const QVector<AttributeItem> &attributes)
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
    std::tuple<int, QSqlQuery>
    Model<Derived, AllRelations...>::incrementOrDecrement(
            const QString &column, const T amount, const QVector<AttributeItem> &extra,
            const IncrementOrDecrement method, const bool all)
    {
        // Ownership of a unique_ptr()
        auto builder = newQueryWithoutRelationships();

        // Increment/Decrement all rows in the database, this is damn dangerous 🙃
        if (!exists) {
            // This makes it much safer
            if (!all)
                return {-1, QSqlQuery()};

            const auto extraConverted = AttributeUtils::convertVectorToUpdateItem(extra);

            if (method == Increment)
                return builder->increment(column, amount, extraConverted);
            if (method == Decrement)
                return builder->decrement(column, amount, extraConverted);

            Q_UNREACHABLE();
        }

        // Prefill an amount and extra attributes on the current model
        {
            auto attributeReference = this->operator[](column);
            attributeReference = attributeReference->template value<T>() + amount;

            forceFill(extra);
        }

        // Execute the increment/decrement query on the database for the current model
        std::tuple<int, QSqlQuery> result;
        {
            auto &model = this->model().setKeysForSaveQuery(*builder);
            const auto extraConverted = AttributeUtils::convertVectorToUpdateItem(extra);

            if (method == Increment)
                result = model.increment(column, amount, extraConverted);
            else if (method == Decrement)
                result = model.decrement(column, amount, extraConverted);
            else
                Q_UNREACHABLE();
        }

        // Synchronize changes manually
        this->syncChanges();

        // Update originals so that they are not dirty
        QStringList updatedAttributes {column};
        std::ranges::transform(extra, std::back_inserter(updatedAttributes),
                               [](const auto &attribute) { return attribute.key; });

        this->syncOriginalAttributes(updatedAttributes);

        return result;
    }

    /* HasAttributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::fillWithCRTPCheck(
            const QVector<AttributeItem> &attributes)
    {
        if (attributes.isEmpty())
            return;

        throwIfCRTPctorProblem(attributes);

        fill(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::fillWithCRTPCheck(
            QVector<AttributeItem> &&attributes)
    {
        if (attributes.isEmpty())
            return;

        throwIfCRTPctorProblem(attributes);

        fill(std::move(attributes));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::throwIfCRTPctorProblem(
            const QVector<AttributeItem> &attributes)
    {
        throwIfQDateTimeAttribute(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::throwIfQDateTimeAttribute(
            const QVector<AttributeItem> &attributes)
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
                Helpers::qVariantTypeId(value) == QMetaType::QDateTime
            )
                throw Orm::Exceptions::InvalidArgumentError(
                        message.arg(TypeUtils::classPureBasename<Derived>(), key));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::throwIfTotallyGuarded(const QString &key)
    {
        throw Exceptions::MassAssignmentError(
                    QStringLiteral("Add '%1' to u_fillable data member to allow "
                                   "mass assignment on the '%2' model.")
                    .arg(key, TypeUtils::classPureBasename<Derived>()));
    }

    /* Getters for u_ data members defined in the Derived models, helps to avoid
       'friend GuardsAttributes/HasTimestamps' declarations in models when a u_ data
       members are private/protected. */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString &
    Model<Derived, AllRelations...>::getUserDateFormat()
    {
        return Derived::u_dateFormat;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getUserDateFormat() const
    {
        return Derived::u_dateFormat;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getUserDates()
    {
        return Derived::u_dates;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unordered_map<QString, CastItem> &
    Model<Derived, AllRelations...>::getUserCasts()
    {
        return Derived::u_casts;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const std::unordered_map<QString, CastItem> &
    Model<Derived, AllRelations...>::getUserCasts() const
    {
        return Derived::u_casts;
    }

    /* GuardsAttributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QStringList &
    Model<Derived, AllRelations...>::getUserFillable()
    {
        return Derived::u_fillable;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getUserFillable() const
    {
        return Derived::u_fillable;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QStringList &
    Model<Derived, AllRelations...>::getUserGuarded()
    {
        return Derived::u_guarded;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getUserGuarded() const
    {
        return Derived::u_guarded;
    }

    /* HasRelationships */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QHash<QString, typename Model<Derived, AllRelations...>::RelationVisitorAlias> &
    Model<Derived, AllRelations...>::getUserRelations() const
    {
        return model().u_relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getUserTouches() const
    {
        return model().u_touches;
    }

    /* HasTimestamps */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool &
    Model<Derived, AllRelations...>::getUserTimestamps()
    {
        return model().u_timestamps;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::getUserTimestamps() const
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
// BUG Qt sql drivers do not work with mysql json columns silverqx
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
// BUG qmake impossible to add d postfix for dlls silverqx
// BUG SemVer version cmake/qmake silverqx
// CUR cmake/qmake SOVERSION silverqx
// CUR 0.1.0 vs 0.1.0.0 Product/FileVersion, investigate, also check versions in pc, prl, ... silverqx
// CUR enable QT_ASCII_CAST_WARNINGS silverqx
// CUR enable QT_NO_CAST_FROM_ASCII silverqx
// BUG qmake MinGW UCRT64 clang static build duplicit symbols, this is MinGW bug silverqx
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
// BUG clang on mingw inline static initialization with another static in the same class defined line before, all other compilers (on linux too) works silverqx
// CUR docs mdx syntax highlight prism Treeview https://prismjs.com/plugins/treeview/ silverqx
// CUR docs IdealImage silverqx
// TODO cache static_cast<>(*this) in the model()/basemodel() CRTP as a class data member std::optional<std::reference_wrapper<Derived>> m_model = std::nullopt, but I will have to create copy ctor to set m_model {std::nullopt}, the same for other similar model() methods like Model::model(), then I can to check if (m_model) and return right away and I will call static_cast or dynamic_cast only once for every instance, it is cast everytime now 😟 silverqx
// CUR docs exceptions, which methods throw which exceptions, so user know which exception classes to define in a catch blocks silverqx
// SEC fix LoadLibrary() before qsql plugin loads? https://www.qt.io/blog/security-advisory-qlockfile-qauthenticator-windows-platform-plugin silverqx
// CUR cmake, make TinyUtils_target in TinyTestCommon.cmake optional, not always needed to link to silverqx
// CUR docs, info about unsupported json column by qtsql drivers and link to schema.mdx Default Expressions silverqx
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
// BUG ccache, msvc and clang-cl support doesn't work because of /Zi param. https://github.com/ccache/ccache/issues/1040 silverqx
// FUTURE security workflows, add ggshield https://github.com/GitGuardian/ggshield silverqx
// BUG clang tidy reports bugprone-exception-escape for Model class, it doesn't mean that code is buggy or can crash, it means that some defualt method generated by the compiler that should be generated noexcet was generetad as throwing silverqx
// CUR mariadb workflows silverqx
// TODO next thing I should implement is Model::toArray() or toVector() and toJson(), Serializing Models & Collections and then JsonResource for simpler general serialization silverqx
// TODO model, HidesAttributes and ModelsCollection::makeVisible/Hidden, ... silverqx
