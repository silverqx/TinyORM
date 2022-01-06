#pragma once
#ifndef ORM_TINY_MODEL_HPP
#define ORM_TINY_MODEL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#ifdef __GNUG__
#include <map>
#endif
#include <unordered_map>

#include "orm/concerns/hasconnectionresolver.hpp"
#include "orm/connectionresolverinterface.hpp"
#include "orm/exceptions/invalidformaterror.hpp"
#include "orm/tiny/concerns/guardsattributes.hpp"
#include "orm/tiny/concerns/hasattributes.hpp"
#include "orm/tiny/concerns/hasrelationstore.hpp"
#include "orm/tiny/concerns/hastimestamps.hpp"
#include "orm/tiny/exceptions/massassignmenterror.hpp"
#include "orm/tiny/exceptions/relationnotfounderror.hpp"
#include "orm/tiny/exceptions/relationnotloadederror.hpp"
#include "orm/tiny/macros/crtpmodel.hpp"
#include "orm/tiny/modelproxies.hpp"
#include "orm/tiny/relations/belongsto.hpp"
#include "orm/tiny/relations/belongstomany.hpp"
#include "orm/tiny/relations/hasmany.hpp"
#include "orm/tiny/relations/hasone.hpp"
#include "orm/tiny/tinybuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    class DatabaseConnection;

namespace Tiny
{
namespace Relations
{
    class IsPivotModel;
}

#ifdef TINYORM_TESTS_CODE
    /*! Used by tests to override connection in the Model. */
    struct ConnectionOverride
    {
        /*! The connection to use in the Model, this data member is picked up
            in the Model::getConnectionName(). */
        T_THREAD_LOCAL
        inline static QString connection;
    };
#endif

    // TODO repeat, from time to time try to compile without microsoft extensions, QMAKE_CXXFLAGS *= -Za silverqx
    // TODO repeat, recompile without PCH silverqx
    // TODO repeat, open cmake generated Visual Studio 2019 solution and fix all errors/warnings silverqx
    // TODO repeat, clang's UndefinedBehaviorSanitizer at https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html silverqx
    // TODO repeat, merge changes from latest Eloquent silverqx
    // TODO decide/unify when to use class/typename keywords for templates silverqx
    // TODO model missing methods Soft Deleting, Model::trashed()/restore()/withTrashed()/forceDelete()/onlyTrashed(), check this methods also on EloquentBuilder and SoftDeletes trait silverqx
    // TODO model missing methods Model::replicate() silverqx
    // TODO model missing methods Comparing Models silverqx
    // TODO model missing methods Model::firstOr() silverqx
    // TODO model missing methods Model::updateOrInsert() silverqx
    // TODO model missing methods Model::loadMissing() silverqx
    // TODO model missing methods Model::whereExists() silverqx
    // TODO model missing methods Model::whereBetween() silverqx
    // TODO model missing methods Model::exists()/notExists() silverqx
    // TODO model missing methods Model::saveOrFail() silverqx
    // TODO model missing methods EloquentCollection::toQuery() silverqx
    // TODO model missing methods Model::whereRow() silverqx
    // FEATURE EloquentCollection, solve how to implement, also look at Eloquent's Custom Collections silverqx
    // FUTURE try to compile every header file by itself and catch up missing dependencies and forward declaration, every header file should be compilable by itself silverqx
    // FUTURE include every stl dependency in header files silverqx
    // FEATURE logging, add support for custom logging, logging to the defined stream?, I don't exactly know how I will solve this issue, design it 🤔 silverqx
    // TODO QueryBuilder::updateOrInsert() silverqx
    // FEATURE code coverage silverqx
    // FEATURE CI/CD silverqx
    // TODO perf, run TinyOrmPlayground 30 times with disabled terminal output and calculate sum value of execution times to compare perf silverqx
    // TODO dilemma, function params. like direction asc/desc for orderBy, operators for where are QStrings, but they should be flags for performance reasons, how to solve this and preserve nice clean api? that is the question 🤔 silverqx
    // CUR1 connection SSL support silverqx
    // BUG Qt sql drivers do not work with mysql json columns silverqx
    // CUR1 add Relations::Concerns::ComparesRelatedModels silverqx
    // FEATURE build systems, add autotools build silverqx
    // FEATURE build systems, add docs on how to make a production build of the TinyORM library silverqx
    // FEATURE build systems, add docs on how to set up dev. environment and how to run auto tests silverqx
    // FEATURE build systems, libuv example how it could look like https://github.com/libuv/libuv silverqx
    // FUTURE cmake can generate export header file by GenerateExportHeader module, find way to use it, because I have own export header file, how to unify this? I don't know now silverqx
    // CUR try clang-include-fixer at https://clang.llvm.org/extra/clang-include-fixer.html silverqx
    // CUR try iwyu at https://include-what-you-use.org/ silverqx
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
    // CUR autoconfigure qmake with qmake/TinyOrm.pri and TINY_ROOT_DIR and TINY_TINYORM_BUILDS_DIR silverqx
    // BUG qmake MinGW UCRT64 clang static build duplicit symbols, this is MinGW bug silverqx
    // BUG qmake MinGW UCRT64 clang shared build with inline_constants cause crashes of 50% of tests, this will be MinGW clang or clang bug, on unix it works without problems silverqx
    // BUG cmake MinGW UCRT64 clang static build builds, but cause problem with inline_constants ; shared build with inline_constants cause crashes of 50% of tests, like bug above, this will be MinGW clang or clang bug, on unix it works without problems silverqx
    // FUTURE linux, add linker version script https://github.com/sailfishos/qtbase/commit/72ba0079c3967bdfa26acdce78ce6cb98b30c27b?view=parallel https://www.gnu.org/software/gnulib/manual/html_node/Exported-Symbols-of-Shared-Libraries.html https://stackoverflow.com/questions/41061220/where-do-object-file-version-references-come-from silverqx
    // TODO Visual Studio memory analyzer https://docs.microsoft.com/en-us/visualstudio/profiling/memory-usage-without-debugging2?view=vs-2019 silverqx
    // CUR fix all modernize-pass-by-value silverqx
    // CUR use using inside classes where appropriate silverqx
    // CUR I'm still not comfortable with using Orm::Constants, inside/outside namespace in cpp/hpp files silverqx
    // CUR using namespace Orm::Constants; in ormtypes.hpp silverqx
    // CUR make all * to *const silverqx
    // CUR merge changes from latest Eloquent silverqx
    // CUR cmake/qmake run tests in parallel for tests that allow it silverqx
    // CUR study how to use acquire/release memory order for m_queryLogId atomic silverqx
    // FUTURE divide Query Builder and TinyOrm to own packages (dlls)? think about it 🤔 silverqx
    // BUG clang on mingw inline static initialization with another static in the same class defined line before, all other compilers (on linux too) works silverqx
    // CUR unify default/non-default ctors comment silverqx
    // CUR cmake when MYSQL_PING is on and QMYSQL driver is not build ctest fails, fail configure? I don't knwo how I will solve this for now, also fix qmake silverqx
    // BUG docs many-to-many examples contain queries with created_at in pivot table, but pivot doesn't contain timestamp columns silverqx
    // CUR docs mdx syntax highlight prism Treeview https://prismjs.com/plugins/treeview/ silverqx
    // CUR docs IdealImage silverqx
    // CUR docs, add all the implemented features summary somewhere silverqx
    // CUR export dll paths in Invoke- ps1 scripts, also check dotenv and source if needed and add dotenv example to tools/ silverqx
    /*! Base model class. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model :
            public Orm::Concerns::HasConnectionResolver,
            public Tiny::Concerns::HasAttributes<Derived, AllRelations...>,
            public Tiny::Concerns::GuardsAttributes<Derived, AllRelations...>,
            public Tiny::Concerns::HasTimestamps<Derived, AllRelations...>,
            public Tiny::Concerns::HasRelationStore<Derived, AllRelations...>,
            public ModelProxies<Derived, AllRelations...>
    {
        // To access getUserXx() methods
        friend Concerns::GuardsAttributes<Derived, AllRelations...>;
        // To access getUserXx() methods
        friend Concerns::HasAttributes<Derived, AllRelations...>;
        // To access getUserXx() methods
        friend Concerns::HasTimestamps<Derived, AllRelations...>;
        // Used by BaseRelationStore::visit() and also by visted methods
        friend Concerns::HasRelationStore<Derived, AllRelations...>;
        // Used by QueriesRelationships::has()
        friend Concerns::QueriesRelationships<Derived>;
        // FUTURE try to solve problem with forward declarations for friend methods, to allow only relevant methods from TinyBuilder silverqx
        // Used by TinyBuilder::eagerLoadRelations()
        friend TinyBuilder<Derived>;

        /*! Apply all the Model's template parameters to the passed T template
            argument. */
        template<template<typename ...> typename T>
        using ModelTypeApply = T<Derived, AllRelations...>;

    public:
        /*! The "type" of the primary key ID. */
        using KeyType = quint64;

        /* Constructors */
        /*! Create a new TinORM model instance. */
        Model();

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

        /* Static operations on a model instance */
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(const QVector<AttributeItem> &attributes);
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(QVector<AttributeItem> &&attributes);

        /*! Begin querying the model. */
        static std::unique_ptr<TinyBuilder<Derived>> query();
        /*! Begin querying the model on a given connection. */
        static std::unique_ptr<TinyBuilder<Derived>> on(const QString &connection = "");

        /*! Get all of the models from the database. */
        static QVector<Derived> all(const QVector<Column> &columns = {ASTERISK});

        /* Operations on a model instance */
        /*! Save the model to the database. */
        bool save(SaveOptions options = {});
        /*! Save the model and all of its relationships. */
        bool push();

        /*! Update records in the database. */
        bool update(const QVector<AttributeItem> &attributes, SaveOptions options = {});

        /*! Delete the model from the database. */
        bool remove();
        /*! Delete the model from the database (alias). */
        bool deleteModel();

        /*! Reload a fresh model instance from the database. */
        std::optional<Derived> fresh(const QVector<WithItem> &relations = {});
        /*! Reload a fresh model instance from the database. */
        std::optional<Derived> fresh(const QString &relation);
        /*! Reload the current model instance with fresh attributes from the database. */
        Derived &refresh();

        /*! Eager load relations on the model. */
        Derived &load(const QVector<WithItem> &relations);
        /*! Eager load relations on the model. */
        Derived &load(const QString &relation);

        /*! Determine if two models have the same ID and belong to the same table. */
        template<typename ModelToCompare>
        bool is(const std::optional<ModelToCompare> &model) const;
        /*! Determine if two models are not the same. */
        template<typename ModelToCompare>
        bool isNot(const std::optional<ModelToCompare> &model) const;

        /*! Fill the model with a vector of attributes. */
        Derived &fill(const QVector<AttributeItem> &attributes);
        /*! Fill the model with a vector of attributes. */
        Derived &fill(QVector<AttributeItem> &&attributes);
        /*! Fill the model with a vector of attributes. Force mass assignment. */
        Derived &forceFill(const QVector<AttributeItem> &attributes);

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
        newTinyBuilder(const QSharedPointer<QueryBuilder> &query);

        /*! Create a new model instance that is existing. */
        Derived newFromBuilder(const QVector<AttributeItem> &attributes = {},
                               const std::optional<QString> &connection = std::nullopt);
        /*! Create a new instance of the given model. */
        inline Derived newInstance();
        /*! Create a new instance of the given model. */
        Derived newInstance(const QVector<AttributeItem> &attributes,
                            bool exists = false);
        /*! Create a new instance of the given model. */
        Derived newInstance(QVector<AttributeItem> &&attributes,
                            bool exists = false);

        /*! Create a new pivot model instance. */
        template<typename PivotType = Relations::Pivot, typename Parent>
        PivotType
        newPivot(const Parent &parent, const QVector<AttributeItem> &attributes,
                 const QString &table, bool exists) const;

        /* Static cast this to a child's instance type (CRTP). */
        TINY_CRTP_MODEL

        /* Getters / Setters */
        /*! Get the current connection name for the model. */
        const QString &getConnectionName() const;
        /*! Get the database connection for the model. */
        inline ConnectionInterface &getConnection() const;
        /*! Set the connection associated with the model. */
        inline Derived &setConnection(const QString &name);
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
        /*! Get the value indicating whether the IDs are incrementing. */
        inline bool getIncrementing() const;
        /*! Set whether IDs are incrementing. */
        inline Derived &setIncrementing(bool value);

        /* Others */
        /*! Qualify the given column name by the model's table. */
        QString qualifyColumn(const QString &column) const;

        /* HasRelationships */
        /*! Get a relationship for Many types relation. */
        template<typename Related,
                 template<typename> typename Container = QVector>
        const Container<Related *>
        getRelationValue(const QString &relation);
        /*! Get a relationship for a One type relation. */
        template<typename Related, typename Tag> requires std::same_as<Tag, One>
        Related *
        getRelationValue(const QString &relation);

        /*! Get a specified relationship. */
        template<typename Related,
                 template<typename> typename Container = QVector>
        const Container<Related *>
        getRelation(const QString &relation);
        /*! Get a specified relationship as Related type, for use with HasOne and
            BelongsTo relation types. */
        template<typename Related, typename Tag> requires std::same_as<Tag, One>
        Related *getRelation(const QString &relation);

        /*! Determine if the given relation is loaded. */
        bool relationLoaded(const QString &relation) const;

        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, const QVector<Related> &models);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, QVector<Related> &&models);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation,
                             const std::optional<Related> &model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, std::optional<Related> &&model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        requires std::is_base_of_v<Relations::IsPivotModel, Related>
        Derived &setRelation(const QString &relation,
                             const std::optional<Related> &model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        requires std::is_base_of_v<Relations::IsPivotModel, Related>
        Derived &setRelation(const QString &relation, std::optional<Related> &&model);

        /*! Get the default foreign key name for the model. */
        QString getForeignKey() const;

        /*! Define a one-to-one relationship. */
        template<typename Related>
        std::unique_ptr<Relations::HasOne<Derived, Related>>
        hasOne(QString foreignKey = "", QString localKey = "");
        /*! Define an inverse one-to-one or many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::BelongsTo<Derived, Related>>
        belongsTo(QString foreignKey = "", QString ownerKey = "", QString relation = "");
        /*! Define a one-to-many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::HasMany<Derived, Related>>
        hasMany(QString foreignKey = "", QString localKey = "");
        /*! Define a many-to-many relationship. */
        template<typename Related, typename PivotType = Relations::Pivot>
        std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
        belongsToMany(QString table = "", QString foreignPivotKey = "",
                      QString relatedPivotKey = "", QString parentKey = "",
                      QString relatedKey = "", QString relation = "");

        /*! Touch the owning relations of the model. */
        void touchOwners();

        /*! Get the relationships that are touched on save. */
        inline const QStringList &getTouchedRelations() const;
        /*! Determine if the model touches a given relation. */
        inline bool touches(const QString &relation) const;

        /*! Get all the loaded relations for the instance. */
        inline
#ifdef __GNUG__
        const std::map<QString, RelationsType<AllRelations...>> &
#else
        const std::unordered_map<QString, RelationsType<AllRelations...>> &
#endif
        getRelations() const;
        /*! Get all the loaded relations for the instance. */
        inline
#ifdef __GNUG__
        std::map<QString, RelationsType<AllRelations...>> &
#else
        std::unordered_map<QString, RelationsType<AllRelations...>> &
#endif
        getRelations();

        /*! Unset all the loaded relations for the instance. */
        Derived &unsetRelations();
        /*! Unset a loaded relationship. */
        Derived &unsetRelation(const QString &relation);

        /* HasTimestamps */
        /*! Determine if the given model is ignoring touches. */
        template<typename ClassToCheck = Derived>
        static bool isIgnoringTouch();

        /* Data members */
        /*! Indicates if the model exists. */
        bool exists = false;

    protected:
        /*! Relation visitor lambda type. */
        using RelationVisitor = std::function<void(
                typename Concerns::HasRelationStore<Derived, AllRelations...>
                                 ::BaseRelationStore &)>;

        /* Model Instance methods */
        /*! Get a new query builder instance for the connection. */
        QSharedPointer<QueryBuilder> newBaseQueryBuilder() const;

        /* HasRelationships */
        /*! Get a relationship value from a method. */
        template<class Related,
                 template<typename> typename Container = QVector>
        Container<Related *>
        getRelationshipFromMethod(const QString &relation);
        /*! Get a relationship value from a method. */
        template<class Related, typename Tag> requires std::same_as<Tag, One>
        Related *
        getRelationshipFromMethod(const QString &relation);

        /*! Create a new model instance for a related model. */
        template<typename Related>
        std::unique_ptr<Related> newRelatedInstance() const;

        /*! Instantiate a new HasOne relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::HasOne<Derived, Related>>
        newHasOne(std::unique_ptr<Related> &&related, Derived &parent,
                  const QString &foreignKey, const QString &localKey) const;
        /*! Instantiate a new BelongsTo relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::BelongsTo<Derived, Related>>
        newBelongsTo(std::unique_ptr<Related> &&related,
                     Derived &child, const QString &foreignKey,
                     const QString &ownerKey, const QString &relation) const;
        /*! Instantiate a new HasMany relationship. */
        template<typename Related>
        inline std::unique_ptr<Relations::HasMany<Derived, Related>>
        newHasMany(std::unique_ptr<Related> &&related, Derived &parent,
                   const QString &foreignKey, const QString &localKey) const;
        /*! Instantiate a new BelongsToMany relationship. */
        template<typename Related, typename PivotType>
        inline std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
        newBelongsToMany(std::unique_ptr<Related> &&related, Derived &parent,
                         const QString &table, const QString &foreignPivotKey,
                         const QString &relatedPivotKey, const QString &parentKey,
                         const QString &relatedKey, const QString &relation) const;

        /*! Guess the "belongs to" relationship name. */
        template<typename Related>
        QString guessBelongsToRelation() const;
        /*! Guess the "belongs to many" relationship name. */
        template<typename Related>
        QString guessBelongsToManyRelation() const;
        /*! Get the joining table name for a many-to-many relation. */
        template<typename Related>
        QString pivotTableName() const;

        /*! Set the entire relations hash on the model. */
        Derived &setRelations(
#ifdef __GNUG__
                const std::map<QString, RelationsType<AllRelations...>> &relations);
#else
                const std::unordered_map<QString,
                                         RelationsType<AllRelations...>> &relations);
#endif
        /*! Set the entire relations hash on the model. */
        Derived &setRelations(
#ifdef __GNUG__
                std::map<QString, RelationsType<AllRelations...>> &&relations);
#else
                std::unordered_map<QString, RelationsType<AllRelations...>> &&relations);
#endif

        /* Operations on a model instance */
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

        /*! Map of relation names to methods. */
        QHash<QString, RelationVisitor> u_relations;
        // TODO detect (best at compile time) circular eager relation problem, the exception which happens during this problem is stackoverflow in QRegularExpression silverqx
        /*! The relations to eager load on every query. */
        QVector<QString> u_with;
        /*! The relationship counts that should be eager loaded on every query. */
//        QVector<WithItem> u_withCount;

        /* HasRelationships */
        // BUG std::unordered_map prevents to compile on GCC, if I comment out std::optional<AllRelations>... in the RelationsType<AllRelations...>, or I change it to the QHash, then it compile, I'm absolutelly lost why this is happening 😞😭, I can't change to the QHash because of 25734deb, I have created simple test project gcc_trivial_bug_test in merydeye-gentoo, but std::map works so it is a big win, because now I can compile whole project on gcc ✨🚀 silverqx
        /*! The loaded relationships for the model. */
#ifdef __GNUG__
        std::map<QString, RelationsType<AllRelations...>> m_relations;
#else
        std::unordered_map<QString, RelationsType<AllRelations...>> m_relations;
#endif
        /*! The relationships that should be touched on save. */
        QStringList u_touches;
        // CUR1 use sets instead of QStringList where appropriate silverqx
        /*! Currently loaded Pivot relation names. */
        std::unordered_set<QString> m_pivots;

        /* HasTimestamps */
        /*! The name of the "created at" column. */
        inline static const QString &CREATED_AT = Constants::CREATED_AT; // NOLINT(cppcoreguidelines-interfaces-global-init)
        /*! The name of the "updated at" column. */
        inline static const QString &UPDATED_AT = Constants::UPDATED_AT; // NOLINT(cppcoreguidelines-interfaces-global-init)

    private:
        /*! Alias for the enum struct RelationNotFoundError::From. */
        using RelationFrom = Exceptions::RelationNotFoundError::From;

        /* HasAttributes */
        /*! Throw InvalidArgumentError if attributes passed to the constructor contain
            some value, which will cause access of some data member in a derived
            instance. */
        inline void
        throwIfCRTPctorProblem(const QVector<AttributeItem> &attributes) const;
        /*! The QDateTime attribute detected, causes CRTP ctor problem. */
        void throwIfQDateTimeAttribute(const QVector<AttributeItem> &attributes) const;

        /* HasRelationships */
        /*! Throw exception if a relation is not defined. */
        void validateUserRelation(const QString &name,
                                  RelationFrom from = RelationFrom::UNDEFINED) const;

        /*! Obtain related models from "relationships" data member hash
            without any checks. */
        template<class Related,
                 template<typename> typename Container = QVector>
        Container<Related *>
        getRelationFromHash(const QString &relation);
        /*! Obtain related models from "relationships" data member hash
            without any checks. */
        template<class Related, typename Tag> requires std::same_as<Tag, One>
        Related *
        getRelationFromHash(const QString &relation);

        /*! Create lazy store and obtain a relationship from defined method. */
        template<typename Related, typename Result>
        Result getRelationshipFromMethodWithVisitor(const QString &relation);

        /*! Throw exception if correct getRelation/Value() method was not used, to avoid
            std::bad_variant_access. */
        template<typename Result, typename Related, typename T>
        void checkRelationType(const T &relationVariant, const QString &relation,
                               const QString &source) const;

        /*! Guess the relationship name for belongsTo/belongsToMany. */
        template<typename Related>
        QString guessBelongsToRelationInternal() const;

        /* Eager load relation store related */
        /*! Obtain a relationship instance for eager loading. */
        void eagerLoadRelationWithVisitor(
                const WithItem &relation, const TinyBuilder<Derived> &builder,
                QVector<Derived> &models);

        /*! Get Related model table name if the relation is BelongsToMany, otherwise
            return empty std::optional. */
        std::optional<QString>
        getRelatedTableForBelongsToManyWithVisitor(const QString &relation);

        /* Push relation store related */
        /*! Create push store and call push for every model. */
        bool pushWithVisitor(const QString &relation,
                             RelationsType<AllRelations...> &models);

        /*! On the base of alternative held by m_relations decide, which
            pushVisitied() to execute. */
        template<typename Related>
        void pushVisited();
        /*! Push for Many relation types. */
        template<typename Related, typename Tag> requires std::same_as<Tag, Many>
        void pushVisited();
        /*! Push for One relation type. */
        template<typename Related, typename Tag> requires std::same_as<Tag, One>
        void pushVisited();

        /* Touch owners store related */
        /*! Create 'touch owners relation store' and touch all related models. */
        void touchOwnersWithVisitor(const QString &relation);
        /*! On the base of alternative held by m_relations decide, which
            touchOwnersVisited() to execute. */
        template<typename Related, typename Relation>
        void touchOwnersVisited(Relation &&relation);

        /* QueriesRelationships store related */
        /*! Create 'QueriesRelationships relation store' to obtain relation instance. */
        template<typename Related = void>
        void queriesRelationshipsWithVisitor(
                const QString &relation, Concerns::QueriesRelationships<Derived> &origin,
                const QString &comparison, qint64 count, const QString &condition,
                const std::function<void(
                        Concerns::QueriesRelationshipsCallback<Related> &)> &callback,
                std::optional<std::reference_wrapper<
                        QStringList>> relations = std::nullopt);

        /* Operations on a model instance */
        /*! Obtain all loaded relation names except pivot relations. */
        QVector<WithItem> getLoadedRelationsWithoutPivot();

        /*! Replace relations in the m_relation. */
        void replaceRelations(
#ifdef __GNUG__
                std::map<QString, RelationsType<AllRelations...>> &relations,
#else
                std::unordered_map<QString, RelationsType<AllRelations...>> &relations,
#endif
                const QVector<WithItem> &onlyRelations);

        /* HasRelationships */
        /*! The reference to the u_relations hash. */
        inline const QHash<QString, RelationVisitor> &
        getUserRelationsRawMap() const;

        /* HasAttributes */
        /*! Get the u_dateFormat attribute from the Derived model. */
        inline QString &getUserDateFormat();
        /*! Get the u_dateFormat attribute from the Derived model. */
        inline const QString &getUserDateFormat() const;
        /*! Get the u_dates attribute from the Derived model. */
        inline static const QStringList &getUserDates();

        /* GuardsAttributes */
        /*! Get the u_fillable attributes from the Derived model. */
        inline QStringList &getUserFillable();
        /*! Get the u_fillable attributes from the Derived model. */
        inline const QStringList &getUserFillable() const;
        /*! Get the u_guarded attributes from the Derived model. */
        inline QStringList &getUserGuarded();
        /*! Get the u_guarded attributes from the Derived model. */
        inline const QStringList &getUserGuarded() const;

        /* HasTimestamps */
        /*! Get the u_timestamps attribute from the Derived model. */
        inline bool &getUserTimestamps();
        /*! Get the u_timestamps attribute from the Derived model. */
        inline bool getUserTimestamps() const;
        /*! Get the CREATED_AT attribute from the Derived model. */
        inline static const QString &getUserCreatedAtColumn();
        /*! Get the UPDATED_AT attribute from the Derived model. */
        inline static const QString &getUserUpdatedAtColumn();
    };

    /* Constructors */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model()
    {
        // Compile time check if a primary key type is supported by a QVariant
        qMetaTypeId<typename Derived::KeyType>();

        // Default Attribute Values
        fill(Derived::u_attributes);

        this->syncOriginal();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(const QVector<AttributeItem> &attributes)
        : Model()
    {
        throwIfCRTPctorProblem(attributes);

        fill(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(QVector<AttributeItem> &&attributes)
        : Model()
    {
        throwIfCRTPctorProblem(attributes);

        fill(std::move(attributes));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(
            std::initializer_list<AttributeItem> attributes
    )
        : Model(QVector<AttributeItem>(attributes.begin(), attributes.end()))
    {}

    /* Static operations on a model instance */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(const QVector<AttributeItem> &attributes)
    {
        Derived model;

        model.fill(attributes);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(QVector<AttributeItem> &&attributes)
    {
        Derived model;

        model.fill(std::move(attributes));

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::query()
    {
        return Derived().newQuery();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::on(const QString &connection)
    {
        /* First we will just create a fresh instance of this model, and then we can
           set the connection on the model so that it is used for the queries we
           execute, as well as being set on every relation we retrieve without
           a custom connection name. */
        Derived instance;

        instance.setConnection(connection);

        return instance.newQuery();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVector<Derived>
    Model<Derived, AllRelations...>::all(const QVector<Column> &columns)
    {
        return query()->get(columns);
    }

    /* Operations on a model instance */

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

        // If the model is brand new, we'll insert it into our database and set the
        // ID attribute on the model to the value of the newly inserted row's ID
        // which is typically an auto-increment value managed by the database.
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
        for (auto &[relation, models] : m_relations)
            // FUTURE Eloquent uses array_filter on models, investigate when this happens, null value (model) in many relations? silverqx
            /* Following Eloquent API, if any push failed, then quit, remaining push-es
               will not be processed. */
            if (!pushWithVisitor(relation, models))
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
        touchOwners();

        // FUTURE performDeleteOnModel() and return value, check logic here, eg what happens when no model is delete and combinations silverqx
        // FUTURE inconsistent return values save(), update(), remove(), ... silverqx
        performDeleteOnModel();

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
        load(getLoadedRelationsWithoutPivot());

        this->syncOriginal();

        return model();
    }

    // FUTURE LoadItem for Model::load() even it will have the same implmentation, or common parent and inherit silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::load(
            const QVector<WithItem> &relations)
    {
        // Ownership of a unique_ptr()
        auto builder = newQueryWithoutRelationships();

        builder->with(relations);

        // FUTURE make possible to pass single model to eagerLoadRelations() and whole relation flow, I indicative counted how many methods would have to rewrite and it is around 12 methods silverqx
        /* I have to make a copy here of this, because of eagerLoadRelations(),
           the solution would be to add a whole new chain for eager load relations,
           which will be able to work only on one Model &, but it is around
           refactoring of 10-15 methods, or add a variant which can process
           QVector<std::reference_wrapper<Derived>>.
           For now, I have made a copy here and save it into the QVector and after
           that move relations from this copy to the real instance. */
        QVector<Derived> models {model()};

        builder->eagerLoadRelations(models);

        /* Replace only relations which was passed to this method, leave other
           relations untouched.
           They do not need to be removed before 'eagerLoadRelations(models)'
           call, because only the relations passed to the 'with' at the beginning
           will be loaded anyway. */
        replaceRelations(models.first().getRelations(), relations);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::load(const QString &relation)
    {
        return load(QVector<WithItem> {{relation}});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename ModelToCompare>
    bool Model<Derived, AllRelations...>::is(
            const std::optional<ModelToCompare> &model) const
    {
        return model
                && getKey() == model->getKey()
                && this->model().getTable() == model->getTable()
                && getConnectionName() == model->getConnectionName();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename ModelToCompare>
    bool Model<Derived, AllRelations...>::isNot(
            const std::optional<ModelToCompare> &model) const
    {
        return !is(model);
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
            if (auto &key = attribute.key;
                this->isFillable(key)
            )
                this->setAttribute(key, std::move(attribute.value));

            else if (totallyGuarded)
                throw Exceptions::MassAssignmentError(
                        QStringLiteral("Add '%1' to u_fillable data member to allow "
                                       "mass assignment on '%2'.")
                        .arg(key, Orm::Utils::Type::classPureBasename<Derived>()));

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
            if (auto &key = attribute.key;
                this->isFillable(key)
            )
                this->setAttribute(key, std::move(attribute.value));

            else if (totallyGuarded)
                throw Exceptions::MassAssignmentError(
                        QStringLiteral("Add '%1' to u_fillable data member to allow "
                                       "mass assignment on '%2'.")
                        .arg(key, Orm::Utils::Type::classPureBasename<Derived>()));
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

        this->unguarded([this, &attributes]
        {
            fill(attributes);
        });

        return model();
    }

    /* Model Instance methods */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQuery()
    {
        return newQueryWithoutScopes();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQueryWithoutScopes()
    {
        // Transform the QString vector to the WithItem vector
        const auto &relations = model().u_with;

        QVector<WithItem> relationsConverted;
        relationsConverted.reserve(relations.size());

        for (const auto &relation : relations)
            relationsConverted.append({relation});

        // Ownership of a unique_ptr()
        auto tinyBuilder = newModelQuery();

        tinyBuilder->with(relationsConverted);

        return tinyBuilder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newModelQuery()
    {
        // Ownership of the QSharedPointer<QueryBuilder>
        const auto query = newBaseQueryBuilder();

        /* Model is passed to the TinyBuilder ctor, because of that setModel()
           isn't used here. Can't be const because of passed non-const model
           to the TinyBuilder. */
        return newTinyBuilder(query);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQueryWithoutRelationships()
    {
        return newModelQuery();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newTinyBuilder(
            const QSharedPointer<QueryBuilder> &query)
    {
        return std::make_unique<TinyBuilder<Derived>>(query, model());
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
        Derived model;

        /* setAttribute() can call getDateFormat() inside and it tries to obtain
           the date format from grammar which is obtained from the connection, so
           the connection have to be set before fill(). */
        model.setConnection(getConnectionName());

        model.fill(attributes);

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
        Derived model;

        /* setAttribute() can call getDateFormat() inside and it tries to obtain
           the date format from grammar which is obtained from the connection, so
           the connection have to be set before fill(). */
        model.setConnection(getConnectionName());

        model.fill(std::move(attributes));

        model.exists = exists_;
        model.setTable(this->model().getTable());

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename PivotType, typename Parent>
    PivotType
    Model<Derived, AllRelations...>::newPivot(
            const Parent &parent, const QVector<AttributeItem> &attributes,
            const QString &table, const bool exists_) const
    {
        if constexpr (std::is_same_v<PivotType, Relations::Pivot>)
            return PivotType::template fromAttributes<Parent>(
                        parent, attributes, table, exists_);
        else
            return PivotType::template fromRawAttributes<Parent>(
                        parent, attributes, table, exists_);
    }

    /* Static cast this to a child's instance type (CRTP). */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::model()
    {
        // Can not be cached with static because a copy can be made
        return static_cast<Derived &>(*this);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const Derived &Model<Derived, AllRelations...>::model() const
    {
        return static_cast<const Derived &>(*this);
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
    ConnectionInterface &
    Model<Derived, AllRelations...>::getConnection() const
    {
        return m_resolver->connection(getConnectionName());
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

        // Get pluralized snake-case table name
        if (table.isEmpty())
            const_cast<QString &>(model().u_table) =
                QStringLiteral("%1s").arg(
                    TinyUtils::String::toSnake(
                        Orm::Utils::Type::classPureBasename<Derived>()));

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

    /* Others */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    Model<Derived, AllRelations...>::qualifyColumn(const QString &column) const
    {
        if (column.contains(DOT))
            return column;

        return QStringLiteral("%1.%2").arg(model().getTable(), column);
    }

    /* HasRelationships */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, template<typename> typename Container>
    const Container<Related *> // NOLINT(readability-const-return-type)
    Model<Derived, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships hash, it just means the
            relationship has already been loaded, so we'll just return it out of
            here because there is no need to query within the relations twice. */
        if (relationLoaded(relation))
            return getRelationFromHash<Related, Container>(relation);

        /*! If the relation is defined on the model, then lazy load and return results
            from the query and hydrate the relationship's value on the "relationships"
            data member m_relations. */
        if (model().u_relations.contains(relation))
            return getRelationshipFromMethod<Related, Container>(relation);

        return {};
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    Model<Derived, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships hash, it just means the
            relationship has already been loaded, so we'll just return it out of
            here because there is no need to query within the relations twice. */
        if (relationLoaded(relation))
            return getRelationFromHash<Related, Tag>(relation);

        /*! If the relation is defined on the model, then lazy load and return results
            from the query and hydrate the relationship's value on the "relationships"
            data member m_relations. */
        if (model().u_relations.contains(relation))
            return getRelationshipFromMethod<Related, Tag>(relation);

        return nullptr;
    }

    // TODO make getRelation() Container argument compatible with STL containers API silverqx
    // TODO solve different behavior like Eloquent getRelation() silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, template<typename> typename Container>
    const Container<Related *> // NOLINT(readability-const-return-type)
    Model<Derived, AllRelations...>::getRelation(const QString &relation)
    {
        if (!relationLoaded(relation))
            throw Exceptions::RelationNotLoadedError(
                    Orm::Utils::Type::classPureBasename<Derived>(), relation);

        return getRelationFromHash<Related, Container>(relation);
    }

    // TODO smart pointer for this relation stuffs? silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    Model<Derived, AllRelations...>::getRelation(const QString &relation)
    {
        if (!relationLoaded(relation))
            throw Exceptions::RelationNotLoadedError(
                    Orm::Utils::Type::classPureBasename<Derived>(), relation);

        return getRelationFromHash<Related, Tag>(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    Model<Derived, AllRelations...>::relationLoaded(const QString &relation) const
    {
        return m_relations.contains(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 const QVector<Related> &models)
    {
        m_relations[relation] = models;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 QVector<Related> &&models)
    {
        m_relations[relation] = std::move(models);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 const std::optional<Related> &model)
    {
        m_relations[relation] = model;

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 std::optional<Related> &&model)
    {
        m_relations[relation] = std::move(model);

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    requires std::is_base_of_v<Relations::IsPivotModel, Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 const std::optional<Related> &model)
    {
        m_pivots.insert(relation);

        m_relations[relation] = model;

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    requires std::is_base_of_v<Relations::IsPivotModel, Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 std::optional<Related> &&model)
    {
        m_pivots.insert(relation);

        m_relations[relation] = std::move(model);

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString Model<Derived, AllRelations...>::getForeignKey() const
    {
        return QStringLiteral("%1_%2").arg(
                    TinyUtils::String::toSnake(
                        Orm::Utils::Type::classPureBasename<Derived>()),
                    getKeyName());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasOne<Derived, Related>>
    Model<Derived, AllRelations...>::hasOne(QString foreignKey, QString localKey)
    {
        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = model().getForeignKey();

        if (localKey.isEmpty())
            localKey = getKeyName();

        return newHasOne<Related>(std::move(instance), model(),
                                  QStringLiteral("%1.%2").arg(instance->getTable(),
                                                              foreignKey),
                                  localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::BelongsTo<Derived, Related>>
    Model<Derived, AllRelations...>::belongsTo(QString foreignKey, QString ownerKey,
                                               QString relation)
    {
        /* If no relation name was given, we will use the Related class type to extract
           the name and use that as the relationship name as most of the time this
           will be what we desire to use for the belongsTo relationships. */
        if (relation.isEmpty())
            relation = guessBelongsToRelation<Related>();

        auto instance = newRelatedInstance<Related>();

        const auto &relatedKeyName = instance->getKeyName();

        /* If no foreign key was supplied, we can guess the proper foreign key name
           by using the snake case name of the relationship, which when combined
           with an "_id" should conventionally match the columns. */
        if (foreignKey.isEmpty())
            foreignKey = QStringLiteral("%1_%2").arg(relation, relatedKeyName);

        /* Once we have the foreign key names, we return the relationship instance,
           which will actually be responsible for retrieving and hydrating every
           relations. */
        if (ownerKey.isEmpty())
            ownerKey = relatedKeyName;

        return newBelongsTo<Related>(std::move(instance), model(),
                                     foreignKey, ownerKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasMany<Derived, Related>>
    Model<Derived, AllRelations...>::hasMany(QString foreignKey, QString localKey)
    {
        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = model().getForeignKey();

        if (localKey.isEmpty())
            localKey = getKeyName();

        return newHasMany<Related>(std::move(instance), model(),
                                   QStringLiteral("%1.%2").arg(instance->getTable(),
                                                               foreignKey),
                                   localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename PivotType>
    std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
    Model<Derived, AllRelations...>::belongsToMany(
            QString table, QString foreignPivotKey, QString relatedPivotKey,
            QString parentKey, QString relatedKey, QString relation)
    {
        /* If no relation name was given, we will use the Related class type to extract
           the name, suffix it with 's' and use that as the relationship name, as most
           of the time this will be what we desire to use for the belongsToMany
           relationships. */
        if (relation.isEmpty())
            relation = guessBelongsToManyRelation<Related>();

        /* First, we'll need to determine the foreign key and "other key"
           for the relationship. Once we have determined the keys, we'll make
           the relationship instance we need for this. */
        auto instance = newRelatedInstance<Related>();

        if (foreignPivotKey.isEmpty())
            foreignPivotKey = model().getForeignKey();

        if (relatedPivotKey.isEmpty())
            relatedPivotKey = instance->getForeignKey();

        /* If no table name was provided, we can guess it by concatenating the two
           models using underscores in alphabetical order. The two model names
           are transformed to snake case from their default CamelCase also. */
        if (table.isEmpty())
            table = pivotTableName<Related>();

        if (parentKey.isEmpty())
            parentKey = getKeyName();

        if (relatedKey.isEmpty())
            relatedKey = instance->getKeyName();

        return newBelongsToMany<Related, PivotType>(
                    std::move(instance), model(), table, foreignPivotKey,
                    relatedPivotKey, parentKey, relatedKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::touchOwners()
    {
        for (const auto &relation : getTouchedRelations())
            touchOwnersWithVisitor(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getTouchedRelations() const
    {
        return model().u_touches;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    Model<Derived, AllRelations...>::touches(const QString &relation) const
    {
        return getTouchedRelations().contains(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
#ifdef __GNUG__
    const std::map<QString, RelationsType<AllRelations...>> &
#else
    const std::unordered_map<QString, RelationsType<AllRelations...>> &
#endif
    Model<Derived, AllRelations...>::getRelations() const
    {
        return m_relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
#ifdef __GNUG__
    std::map<QString, RelationsType<AllRelations...>> &
#else
    std::unordered_map<QString, RelationsType<AllRelations...>> &
#endif
    Model<Derived, AllRelations...>::getRelations()
    {
        return m_relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::unsetRelations()
    {
        m_relations.clear();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::unsetRelation(const QString &relation)
    {
        m_relations.erase(relation);

        return model();
    }

    /* HasTimestamps */

    // CUR move to hasTimestamps silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename ClassToCheck>
    bool Model<Derived, AllRelations...>::isIgnoringTouch()
    {
        // FUTURE implement withoutTouching() and related data member $ignoreOnTouch silverqx

        return !ClassToCheck().usesTimestamps()
                || ClassToCheck::getUpdatedAtColumn().isEmpty();
    }

    /* protected */

    /* Model Instance methods */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QSharedPointer<QueryBuilder>
    Model<Derived, AllRelations...>::newBaseQueryBuilder() const
    {
        return getConnection().query();
    }

    /* HasRelationships */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related *>
    Model<Derived, AllRelations...>::getRelationshipFromMethod(const QString &relation)
    {
        // Obtain related models
        auto relatedModels =
                getRelationshipFromMethodWithVisitor<Related, QVector<Related>>(relation);

        setRelation(relation, std::move(relatedModels));

        return getRelationFromHash<Related, Container>(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    Model<Derived, AllRelations...>::getRelationshipFromMethod(const QString &relation)
    {
        // Obtain related model
        auto relatedModel =
                getRelationshipFromMethodWithVisitor<Related,
                                                     std::optional<Related>>(relation);

        setRelation(relation, std::move(relatedModel));

        return getRelationFromHash<Related, Tag>(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Related>
    Model<Derived, AllRelations...>::newRelatedInstance() const
    {
        auto instance = std::make_unique<Related>();

        if (instance->getConnectionName().isEmpty())
            instance->setConnection(getConnectionName());

        return instance;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasOne<Derived, Related>>
    Model<Derived, AllRelations...>::newHasOne(
            std::unique_ptr<Related> &&related, Derived &parent,
            const QString &foreignKey, const QString &localKey) const
    {
        return Relations::HasOne<Derived, Related>::instance(
                    std::move(related), parent, foreignKey, localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::BelongsTo<Derived, Related>>
    Model<Derived, AllRelations...>::newBelongsTo(
            std::unique_ptr<Related> &&related, Derived &child,
            const QString &foreignKey, const QString &ownerKey,
            const QString &relation) const
    {
        return Relations::BelongsTo<Derived, Related>::instance(
                    std::move(related), child, foreignKey, ownerKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasMany<Derived, Related>>
    Model<Derived, AllRelations...>::newHasMany(
            std::unique_ptr<Related> &&related, Derived &parent,
            const QString &foreignKey, const QString &localKey) const
    {
        return Relations::HasMany<Derived, Related>::instance(
                    std::move(related), parent, foreignKey, localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename PivotType>
    std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
    Model<Derived, AllRelations...>::newBelongsToMany(
            std::unique_ptr<Related> &&related, Derived &parent, const QString &table,
            const QString &foreignPivotKey, const QString &relatedPivotKey,
            const QString &parentKey, const QString &relatedKey,
            const QString &relation) const
    {
        return Relations::BelongsToMany<Derived, Related, PivotType>::instance(
                    std::move(related), parent, table, foreignPivotKey,
                    relatedPivotKey, parentKey, relatedKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::guessBelongsToRelation() const
    {
        static const QString relation = guessBelongsToRelationInternal<Related>();

        /* validateUserRelation() method call can not be cached, has to be called
           every time, to correctly inform the user about invalid relation name. */

        // Validate if the guessed relation name exists in the u_relations
        validateUserRelation(relation, RelationFrom::BELONGS_TO);

        return relation;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::guessBelongsToManyRelation() const
    {
        static const QString relation =
                QStringLiteral("%1s").arg(guessBelongsToRelationInternal<Related>());

        /* validateUserRelation() method call can not be cached, has to be called
           every time, to correctly inform the user about invalid relation name. */

        // Validate if the guessed relation name exists in the u_relations
        validateUserRelation(relation, RelationFrom::BELONGS_TO_MANY);

        return relation;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::pivotTableName() const
    {
        /* The joining table name, by convention, is simply the snake cased, models
           sorted alphabetically and concatenated with an underscore, so we can
           just sort the models and join them together to get the table name. */
        QStringList segments {
            // The table name of the current model instance
            Orm::Utils::Type::classPureBasename<Derived>(),
            // The table name of the related model instance
            Orm::Utils::Type::classPureBasename<Related>(),
        };

        /* Now that we have the model names in the vector, we can just sort them and
           use the join function to join them together with an underscore,
           which is typically used by convention within the database system. */
        segments.sort(Qt::CaseInsensitive);

        return segments.join(UNDERSCORE).toLower();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setRelations(
#ifdef __GNUG__
            const std::map<QString, RelationsType<AllRelations...>> &relations)
#else
            const std::unordered_map<QString, RelationsType<AllRelations...>> &relations)
#endif
    {
        m_relations = relations;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setRelations(
#ifdef __GNUG__
            std::map<QString, RelationsType<AllRelations...>> &&relations)
#else
            std::unordered_map<QString, RelationsType<AllRelations...>> &&relations)
#endif
    {
        m_relations = std::move(relations);

        return model();
    }

    /* Operations on a model instance */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::performDeleteOnModel()
    {
        /* Ownership of a unique_ptr(), if right passed down, then the
           will be destroyed right after this command. */
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
        if (const auto keyName = getKeyName();
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
        // Currently is the implementation exactly the same, so I can call it
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
                        TinyUtils::Attribute::convertVectorToUpdateItem(dirty));

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
            touchOwners();

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
        // When insert was successful
        if (id != 0)
            this->setAttribute(keyName, id);

        /* QSqlQuery returns an invalid QVariant if can't obtain last inserted id,
           which is converted to 0. */
        return id;
    }

    /* private */

    /* HasAttributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::throwIfCRTPctorProblem(
            const QVector<AttributeItem> &attributes) const
    {
        throwIfQDateTimeAttribute(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::throwIfQDateTimeAttribute(
            const QVector<AttributeItem> &attributes) const
    {
        static const QString message = QStringLiteral(
            "Attributes passed to the '%1' model's constructor can't contain the "
            "QDateTime attribute, to create a '%1' model instance with attributes "
            "that contain the QDateTime attribute use %1::instance() "
            "method instead.");

        for (const auto &attribute : attributes)
            if (const auto &value = attribute.value;
                value.isValid() && !value.isNull() &&
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                value.typeId() == QMetaType::QDateTime
#else
                value.userType() == QMetaType::QDateTime
#endif
            )
                throw Orm::Exceptions::InvalidArgumentError(
                        message.arg(Orm::Utils::Type::classPureBasename<Derived>()));
    }

    /* HasRelationships */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void
    Model<Derived, AllRelations...>::validateUserRelation(
            const QString &name, const RelationFrom from) const
    {
        if (!model().u_relations.contains(name))
            throw Exceptions::RelationNotFoundError(
                    Orm::Utils::Type::classPureBasename<Derived>(), name, from);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related *>
    Model<Derived, AllRelations...>::getRelationFromHash(const QString &relation)
    {
        auto &relationVariant = m_relations.find(relation)->second;

        // Check relation type to avoid std::bad_variant_access
        checkRelationType<QVector<Related>, Related>(
                    relationVariant, relation, "getRelation");

        /* Obtain related models from data member hash as QVector, it is internal
           format and transform it into a Container of pointers to related models,
           so a user can directly modify these models and push or save them
           afterward. */
        namespace views = ranges::views;
        return std::get<QVector<Related>>(relationVariant)
                | views::transform([](Related &model) -> Related * { return &model; })
                | ranges::to<Container<Related *>>();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    Model<Derived, AllRelations...>::getRelationFromHash(const QString &relation)
    {
        auto &relationVariant = m_relations.find(relation)->second;

        // Check relation type to avoid std::bad_variant_access
        checkRelationType<std::optional<Related>, Related>(
                    relationVariant, relation, "getRelation");

        /* Obtain related model from data member hash and return it as a pointer or
           nullptr if no model is associated, so a user can directly modify this
           model and push or save it afterward. */

        auto &relatedModel = std::get<std::optional<Related>>(relationVariant);

        return relatedModel ? &*relatedModel : nullptr;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Result>
    Result
    Model<Derived, AllRelations...>::getRelationshipFromMethodWithVisitor(
            const QString &relation)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->template createLazyStore<Related>().visit(relation);

        // Obtain result, related model/s
        const auto lazyResult = this->template lazyStore<Related>().m_result;

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();

        // Check relation type to avoid std::bad_variant_access
        checkRelationType<Result, Related>(lazyResult, relation, "getRelationValue");

        return std::get<Result>(lazyResult);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Result, typename Related, typename T>
    void Model<Derived, AllRelations...>::checkRelationType(
            const T &relationVariant, const QString &relation,
            const QString &source) const
    {
        if constexpr (std::is_same_v<Result, std::optional<Related>>) {
            if (!std::holds_alternative<Result>(relationVariant))
                throw Orm::Exceptions::RuntimeError(
                        QStringLiteral(
                            "The relation '%1' is many type relation, use "
                            "%2<%3>() method overload without an 'Orm::One' tag.")
                        .arg(relation, source,
                             Orm::Utils::Type::classPureBasename<Related>()));
        } else if constexpr (std::is_same_v<Result, QVector<Related>>) {
            if (!std::holds_alternative<Result>(relationVariant))
                throw Orm::Exceptions::RuntimeError(
                        QStringLiteral(
                            "The relation '%1' is one type relation, use "
                            "%2<%3, Orm::One>() method overload "
                            "with an 'Orm::One' tag.")
                        .arg(relation, source,
                             Orm::Utils::Type::classPureBasename<Related>()));
        } else
            throw Orm::Exceptions::InvalidArgumentError(
                    "Unexpected 'Result' template argument.");
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::guessBelongsToRelationInternal() const
    {
        // TODO reliability, also add Orm::Tiny::Utils::String::studly silverqx
        auto relation = Orm::Utils::Type::classPureBasename<Related>();

        relation[0] = relation[0].toLower();

        return relation;
    }

    /* Eager load relation store related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::eagerLoadRelationWithVisitor(
            const WithItem &relation, const TinyBuilder<Derived> &builder,
            QVector<Derived> &models)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation.name);

        /* Save the needed variables to the store to avoid passing variables
           to the visitor. */
        this->createEagerStore(builder, models, relation).visit(relation.name);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<QString>
    Model<Derived, AllRelations...>::getRelatedTableForBelongsToManyWithVisitor(
            const QString &relation)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Create the store and visit relation
        this->createBelongsToManyRelatedTableStore().visit(relation);

        // NRVO kicks in
        auto relatedTable = this->belongsToManyRelatedTableStore().m_result;

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();

        return relatedTable;
    }

    /* Push relation store related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::pushWithVisitor(
            const QString &relation, RelationsType<AllRelations...> &models)
    {
        // TODO prod remove, I don't exactly know if this can really happen silverqx
        /* Check for empty variant, the std::monostate is at zero index and
           consider it as success to continue 'pushing'. */
        const auto variantIndex = models.index();
        Q_ASSERT(variantIndex > 0);
        if (variantIndex == 0)
            return true;

        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->createPushStore(models).visit(relation);

        const auto pushResult = this->pushStore().m_result;

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();

        return pushResult;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    void Model<Derived, AllRelations...>::pushVisited()
    {
        const RelationsType<AllRelations...> &models = this->pushStore().m_models;

        // Invoke pushVisited() on the base of hold alternative in the models
        if (std::holds_alternative<QVector<Related>>(models))
            pushVisited<Related, Many>();
        else if (std::holds_alternative<std::optional<Related>>(models))
            pushVisited<Related, One>();
        else
            throw Orm::Exceptions::RuntimeError(
                    "this->pushStore().models holds unexpected alternative.");
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, Many>
    void Model<Derived, AllRelations...>::pushVisited()
    {
        auto &pushStore = this->pushStore();

        for (auto &model : std::get<QVector<Related>>(pushStore.m_models))
            if (!model.push()) {
                pushStore.m_result = false;
                return;
            }

        pushStore.m_result = true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, One>
    void Model<Derived, AllRelations...>::pushVisited()
    {
        auto &pushStore = this->pushStore();

        auto &model = std::get<std::optional<Related>>(pushStore.m_models);
        Q_ASSERT(model);

        // Skip a null model, consider it as success
        if (!model) {
            pushStore.m_result = true;
            return;
        }

        pushStore.m_result = model->push();
    }

    /* Touch owners store related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void
    Model<Derived, AllRelations...>::touchOwnersWithVisitor(const QString &relation)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->createTouchOwnersStore(relation).visit(relation);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Relation>
    void Model<Derived, AllRelations...>::touchOwnersVisited(Relation &&relation)
    {
        const auto &relationName = this->touchOwnersStore().m_relation;

        relation->touch();

        // Many type relation
        if constexpr (std::is_base_of_v<Relations::ManyRelation,
                                        typename Relation::element_type>)
        {
            for (auto *const relatedModel : getRelationValue<Related>(relationName))
                // WARNING check and add note after, if many type relation QVector can contain nullptr silverqx
                if (relatedModel)
                    relatedModel->touchOwners();
        }

        // One type relation
        else if constexpr (std::is_base_of_v<Relations::OneRelation,
                                             typename Relation::element_type>)
        {
            if (auto *const relatedModel = getRelationValue<Related, One>(relationName);
                relatedModel
            )
                relatedModel->touchOwners();
        } else
            throw Orm::Exceptions::RuntimeError(
                    "Bad relation type passed to the Model::touchOwnersVisited().");
    }

    /* QueriesRelationships store related */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    void Model<Derived, AllRelations...>::queriesRelationshipsWithVisitor(
            const QString &relation, Concerns::QueriesRelationships<Derived> &origin,
            const QString &comparison, const qint64 count, const QString &condition,
            const std::function<void(
                Concerns::QueriesRelationshipsCallback<Related> &)> &callback,
            const std::optional<std::reference_wrapper<QStringList>> relations)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->template createQueriesRelationshipsStore<Related>(
                    origin, comparison, count, condition, callback, relations)
                .visit(relation);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();
    }

    /* Operations on a model instance */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVector<WithItem>
    Model<Derived, AllRelations...>::getLoadedRelationsWithoutPivot()
    {
        QVector<WithItem> relations;

        /* Current model (this) contains a pivot relation alternative
           in the m_relations std::variant. */
        auto hasPivotRelation = std::disjunction_v<std::is_base_of<
                                Relations::IsPivotModel, AllRelations>...>;

        /* Get all currently loaded relation names except pivot relations. We need
           to check for the pivot models, but only if the std::variant which holds
           relations also holds a pivot model alternative, otherwise it is useless. */
        for (const auto &relation : m_relations) {
            const auto &relationName = relation.first;

            // Skip pivot relations
            if (hasPivotRelation && m_pivots.contains(relationName))
                    continue;

            relations.append({relationName});
        }

        return relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::replaceRelations(
#ifdef __GNUG__
            std::map<QString, RelationsType<AllRelations...>> &relations,
#else
            std::unordered_map<QString, RelationsType<AllRelations...>> &relations,
#endif
            const QVector<WithItem> &onlyRelations)
    {
        /* Replace only relations which was passed to this method, leave other
           relations untouched. */
        for (auto itRelation = relations.begin(); itRelation != relations.end();
             ++itRelation)
        {
            const auto &key = itRelation->first;

            const auto relationsContainKey =
                    ranges::contains(onlyRelations, true, [&key](const auto &relation)
            {
                if (!relation.name.contains(COLON))
                    return relation.name == key;

                // Support for select constraints
                return relation.name.split(COLON).at(0).trimmed() == key;
            });

            if (!relationsContainKey)
                continue;

            m_relations[key] = std::move(itRelation->second);
        }
    }

    /* Getters for u_ data members defined in the Derived models, helps to avoid
       'friend GuardsAttributes/HasTimestamps' declarations in models when a u_ data
       members are private/protected. */

    /* HasRelationships */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QHash<QString, typename Model<Derived, AllRelations...>::RelationVisitor> &
    Model<Derived, AllRelations...>::getUserRelationsRawMap() const
    {
        return model().u_relations;
    }

    /* HasAttributes */

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
    Model<Derived, AllRelations...>::getUserCreatedAtColumn()
    {
        return Derived::CREATED_AT;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getUserUpdatedAtColumn()
    {
        return Derived::UPDATED_AT;
    }

} // namespace Tiny
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_MODEL_HPP
