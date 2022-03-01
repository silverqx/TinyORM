#pragma once
#ifndef ORM_TINY_CONCERNS_QUERIESRELATIONSHIPS_HPP
#define ORM_TINY_CONCERNS_QUERIESRELATIONSHIPS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <stack>

#include "orm/exceptions/invalidtemplateargumenterror.hpp"
#include "orm/query/querybuilder.hpp"
#include "orm/tiny/relations/relation.hpp"
#include "orm/tiny/tinytypes.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Concerns
{

    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HasRelationStore;

    template<typename Model>
    class QueriesRelationships;

namespace Private
{
    /*! Save arguments needed to build a query for last relation in hasNested(). */
    class HasNestedStore
    {
        Q_DISABLE_COPY(HasNestedStore)

        // Used by QueriesRelationships::hasNested()
        template<typename T>
        friend class Tiny::Concerns::QueriesRelationships;

        /*! Arguments needed to save for the last relation in a hasNested(). */
        template<typename Related>
        struct NestedStore
        {
            /*! Comparison operator. */
            const QString &comparison;
            /*! Number to compare against. */
            const qint64 count;
            /*! User defined callback. */
            const std::function<void(TinyBuilder<Related> &)> &callback;
        };

        // BUG clang and thread_local, gcc on MinGW doesn't work too silverqx
        /*! Data member for nested stores. */
        template<typename Related>
        T_THREAD_LOCAL
        inline static
        std::stack<std::shared_ptr<NestedStore<Related>>> STORE;

        /*! Stored Related type is used to avoid a cryptic message when a bad type-id
            was passed to the has() nested method. */
        T_THREAD_LOCAL
        inline static
        std::stack<std::type_index> STORE_TYPEID;

    public:
        /*! Deleted default constructor, this is a pure library class. */
        HasNestedStore() = delete;
        /*! Deleted destructor. */
        ~HasNestedStore() = delete;
    };
} // namespace Private

    /*! Queries Relationship Existence/Absence with nesting support. */
    template<typename Model>
    class QueriesRelationships
    {
        Q_DISABLE_COPY(QueriesRelationships)

        // Used by HasRelationStore::QueriesRelationshipsStore::visited()
        friend typename Model::template ModelTypeApply<HasRelationStore>;
        // Used by QueriesRelationships::hasInternalVisited()
        template<typename T>
        friend class QueriesRelationships;

        /*! Expression alias. */
        using Expression = Orm::Query::Expression;
        /*! QueriesRelationships builder type passed to the callback, shortcut alias. */
        template<typename Related>
        using CallbackType = QueriesRelationshipsCallback<Related>;

    protected:
        /*! Alias for the Relations::Relation. */
        template<typename Related>
        using Relation = Orm::Tiny::Relations::Relation<Model, Related>;

    public:
        /*! Default constructor. */
        inline QueriesRelationships() = default;
        /*! Default destructor. */
        inline ~QueriesRelationships() = default;

        /*! Add a relationship count / exists condition to the query. */
        template<typename Related = void>
        TinyBuilder<Model> &
        has(const QString &relation, const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(CallbackType<Related> &)> &callback = nullptr);

        /*! Add a relationship count / exists condition to the query with an "or". */
        template<typename Related = void>
        TinyBuilder<Model> &
        orHas(const QString &relation, const QString &comparison = GE, qint64 count = 1);
        /*! Add a relationship count / exists condition to the query. */
        template<typename Related = void>
        TinyBuilder<Model> &
        doesntHave(const QString &relation, const QString &condition = AND,
                   const std::function<void(
                       CallbackType<Related> &)> &callback = nullptr);
        /*! Add a relationship count / exists condition to the query with an "or". */
        template<typename Related = void>
        TinyBuilder<Model> &
        orDoesntHave(const QString &relation);

        /*! Add a relationship count / exists condition to the query. */
        template<typename Related>
        TinyBuilder<Model> &
        has(std::unique_ptr<Relation<Related>> &&relation,
            const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(QueryBuilder &)> &callback = nullptr);
        /*! Add a relationship count / exists condition to the query, prefer this over
            above overload, void type to avoid ambiguity. */
        template<typename Related, typename = void>
        TinyBuilder<Model> &
        has(std::unique_ptr<Relation<Related>> &&relation,
            const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(TinyBuilder<Related> &)> &callback = nullptr);

        /*! Add a relationship count / exists condition to the query. */
#if defined(__clang__) || defined(__GNUG__)
        template<typename Related, typename Method,
                 std::enable_if_t<std::is_member_function_pointer_v<Method>, bool> = true>
#else
        template<typename Related, typename Method>
        requires std::is_member_function_pointer_v<Method>
#endif
        TinyBuilder<Model> &
        has(Method relation, const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(TinyBuilder<Related> &)> &callback = nullptr);

        /*! Add a relationship count / exists condition to the query with where
            clauses. */
        template<typename Related = void>
        TinyBuilder<Model> &
        whereHas(const QString &relation,
                 const std::function<void(CallbackType<Related> &)> &callback = nullptr,
                 const QString &comparison = GE, qint64 count = 1);

        /*! Add a relationship count / exists condition to the query with where
            clauses and an "or". */
        template<typename Related = void>
        TinyBuilder<Model> &
        orWhereHas(const QString &relation,
                   const std::function<void(
                       CallbackType<Related> &)> &callback = nullptr,
                   const QString &comparison = GE, qint64 count = 1);
        /*! Add a relationship count / exists condition to the query with where
            clauses. */
        template<typename Related = void>
        TinyBuilder<Model> &
        whereDoesntHave(const QString &relation,
                        const std::function<void(
                            CallbackType<Related> &)> &callback = nullptr);
        /*! Add a relationship count / exists condition to the query with where
            clauses and an "or". */
        template<typename Related = void>
        TinyBuilder<Model> &
        orWhereDoesntHave(const QString &relation,
                          const std::function<void(
                              CallbackType<Related> &)> &callback = nullptr);

        /*! Add a relationship count / exists condition to the query with where
            clauses. */
#if defined(__clang__) || defined(__GNUG__)
        template<typename Related, typename Method,
                 std::enable_if_t<std::is_member_function_pointer_v<Method>, bool> = true>
#else
        template<typename Related, typename Method>
        requires std::is_member_function_pointer_v<Method>
#endif
        TinyBuilder<Model> &
        whereHas(Method relation,
                 const std::function<void(TinyBuilder<Related> &)> &callback = nullptr,
                 const QString &comparison = GE, qint64 count = 1);

    protected:
        /*! Sets up recursive call to whereHas until we finish the nested relation. */
        template<typename Related>
        TinyBuilder<Model> &
        hasNeseted(const QString &relation, QString comparison = GE,
                   qint64 count = 1, const QString &condition = AND,
                   const std::function<void(
                       TinyBuilder<Related> &)> &callback = nullptr);

        /*! Get the "has relation" base query instance. */
        template<typename Related, typename Method>
        std::unique_ptr<Relation<Related>>
        getRelationWithoutConstraints(Method method);

        /*! Add the "has" condition where clause to the query. */
        template<typename Related>
        TinyBuilder<Model> &
        addHasWhere(TinyBuilder<Related> &hasQuery, const Relation<Related> &relation,
                    const QString &comparison, qint64 count, const QString &condition);

        /*! Add a sub-query count clause to this query. */
        TinyBuilder<Model> &
        addWhereCountQuery(
                const QSharedPointer<QueryBuilder> &query,
                const QString &comparison = GE, qint64 count = 1,
                const QString &condition = AND);

        /*! Obtain an "exists" normal or "exists" with the count(*) query. */
        template<typename Related>
        std::unique_ptr<TinyBuilder<Related>>
        getHasQueryByExistenceCheck(const QString &comparison, qint64 count,
                                    Relation<Related> &relation) const;
        /*! Check if we can run an "exists" query to optimize performance. */
        bool canUseExistsForExistenceCheck(const QString &comparison,
                                           qint64 count) const;

    private:
        /*! Static cast this to a child's instance TinyBuilder type. */
        TinyBuilder<Model> &query();
        /*! Static cast this to a child's instance TinyBuilder type, const version. */
        const TinyBuilder<Model> &query() const;

        /*! Merge the where constraints from another query to the current query. */
        template<typename Related>
        TinyBuilder<Model> &mergeConstraintsFrom(const TinyBuilder<Related> &from);

        /* hasNested() related methods */
        /*! has() method used in hasNested() for a recursive calls. */
        TinyBuilder<Model> &
        hasInternal(const QString &relation, const QString &comparison,
                    qint64 count, const QString &condition, QStringList &relations);
        /*! Called from model store after a relation was visited and Related type was
            obtained and calls next nested relation. */
        template<typename Related>
        void hasInternalVisited(
                std::unique_ptr<Relation<Related>> &&relation,
                const QString &comparison, qint64 count, const QString &condition,
                QStringList &relations);
        /*! Used by last nested relation that has a tiny query callback. */
        template<typename Related>
        void hasNestedInternalFromStore(std::unique_ptr<Relation<Related>> &&relation);

        /*! Store for the last hasNested() relation properties ( comparison, count, and
            callback ). */
        template<typename Related>
        void createHasNestedStore(
                const QString &comparison, qint64 count,
                const std::function<void(TinyBuilder<Related> &)> &callback) const;
        /*! Destroy hasNested store. */
        template<typename Related>
        void destroyHasNestedStore() const;
        /*! Check if Related template argument passed to the has() method is correct. */
        template<typename Related>
        void checkNestedRelationType() const;
    };

    /*
       The code below is a little chaotic because it contains recursions, the best way
       to understand it is to place breakpoints and debug it. The mess makes hasNested()
       and also that I support more overloads for has().
       Method overloads are not needed, but I leave them here whereas they are already
       functional and debugged.
    */

    template<typename Model>
    template<typename Related>
    TinyBuilder<Model> &
    QueriesRelationships<Model>::has(
            const QString &relation, const QString &comparison,
            const qint64 count, const QString &condition,
            const std::function<void(CallbackType<Related> &)> &callback)
    {
        if (relation.contains(DOT)) {
            if constexpr (std::is_void_v<Related>)
                throw Orm::Exceptions::InvalidArgumentError(
                        "This has() overload doesn't support nested 'has' statements, "
                        "please use has() overload with 'std::function<void("
                        "TinyBuilder<Related> &)> &callback', what means pass <Related> "
                        "template parameter to the called method.");
            else
                return hasNeseted<Related>(relation, comparison, count, condition,
                                           callback);
        }

        query().getModel()
                .template queriesRelationshipsWithVisitor<Related>(
                    relation, *this, comparison, count, condition, callback);

        return query();
    }

    template<typename Model>
    template<typename Related>
    inline TinyBuilder<Model> &
    QueriesRelationships<Model>::orHas(
            const QString &relation, const QString &comparison, const qint64 count)
    {
        return has<Related>(relation, comparison, count, OR);
    }

    template<typename Model>
    template<typename Related>
    inline TinyBuilder<Model> &
    QueriesRelationships<Model>::doesntHave(
            const QString &relation, const QString &condition,
            const std::function<void(CallbackType<Related> &)> &callback)
    {
        return has<Related>(relation, LT, 1, condition, callback);
    }

    template<typename Model>
    template<typename Related>
    inline TinyBuilder<Model> &
    QueriesRelationships<Model>::orDoesntHave(const QString &relation)
    {
        return doesntHave<Related>(relation, OR);
    }

    template<typename Model>
    template<typename Related>
    TinyBuilder<Model> &
    QueriesRelationships<Model>::has(
            std::unique_ptr<Relation<Related>> &&relation, const QString &comparison,
            const qint64 count, const QString &condition,
            const std::function<void(QueryBuilder &)> &callback)
    {
        // Ownership of a unique_ptr()
        auto hasQuery = getHasQueryByExistenceCheck(comparison, count, *relation);

        /* Next we will call any given callback as an "anonymous" scope so they can get
           the proper logical grouping of the where clauses if needed by this TinyORM
           query builder. Then, we will be ready to finalize and return this query
           instance. */
        if (callback)
            std::invoke(callback, hasQuery->getQuery());

        return addHasWhere(*hasQuery, *relation, comparison, count, condition);
    }

    template<typename Model>
    template<typename Related, typename>
    TinyBuilder<Model> &
    QueriesRelationships<Model>::has(
            std::unique_ptr<Relation<Related>> &&relation, const QString &comparison,
            const qint64 count, const QString &condition,
            const std::function<void(TinyBuilder<Related> &)> &callback)
    {
        // Ownership of a unique_ptr()
        auto hasQuery = getHasQueryByExistenceCheck(comparison, count, *relation);

        /* Next we will call any given callback as an "anonymous" scope so they can get
           the proper logical grouping of the where clauses if needed by this TinyORM
           query builder. Then, we will be ready to finalize and return this query
           instance. */
        if (callback)
            std::invoke(callback, *hasQuery);

        return addHasWhere(*hasQuery, *relation, comparison, count, condition);
    }

    template<typename Model>
#if defined(__clang__) || defined(__GNUG__)
    template<typename Related, typename Method,
             std::enable_if_t<std::is_member_function_pointer_v<Method>, bool>>
#else
    template<typename Related, typename Method>
    requires std::is_member_function_pointer_v<Method>
#endif
    inline TinyBuilder<Model> &
    QueriesRelationships<Model>::has(
            const Method relation, const QString &comparison, const qint64 count,
            const QString &condition,
            const std::function<void(TinyBuilder<Related> &)> &callback)
    {
        return has<Related>(getRelationWithoutConstraints<Related>(relation), comparison,
                            count, condition, callback);
    }

    template<typename Model>
    template<typename Related>
    inline TinyBuilder<Model> &
    QueriesRelationships<Model>::whereHas(
            const QString &relation,
            const std::function<void(CallbackType<Related> &)> &callback,
            const QString &comparison, const qint64 count)
    {
        return has<Related>(relation, comparison, count, AND, callback);
    }

    template<typename Model>
    template<typename Related>
    inline TinyBuilder<Model> &
    QueriesRelationships<Model>::orWhereHas(
            const QString &relation,
            const std::function<void(CallbackType<Related> &)> &callback,
            const QString &comparison, const qint64 count)
    {
        return has<Related>(QString(relation), comparison, count, OR, callback);
    }

    template<typename Model>
    template<typename Related>
    inline TinyBuilder<Model> &
    QueriesRelationships<Model>::whereDoesntHave(
            const QString &relation,
            const std::function<void(CallbackType<Related> &)> &callback)
    {
        return doesntHave<Related>(relation, AND, callback);
    }

    template<typename Model>
    template<typename Related>
    inline TinyBuilder<Model> &
    QueriesRelationships<Model>::orWhereDoesntHave(
            const QString &relation,
            const std::function<void(CallbackType<Related> &)> &callback)
    {
        return doesntHave<Related>(relation, OR, callback);
    }

    template<typename Model>
#if defined(__clang__) || defined(__GNUG__)
    template<typename Related, typename Method,
             std::enable_if_t<std::is_member_function_pointer_v<Method>, bool>>
#else
    template<typename Related, typename Method>
    requires std::is_member_function_pointer_v<Method>
#endif
    inline TinyBuilder<Model> &
    QueriesRelationships<Model>::whereHas(
            const Method relation,
            const std::function<void(TinyBuilder<Related> &)> &callback,
            const QString &comparison, const qint64 count)
    {
        return has<Related>(relation, comparison, count, AND, callback);
    }

    template<typename Model>
    template<typename Related>
    TinyBuilder<Model> &
    QueriesRelationships<Model>::hasNeseted(
            const QString &relation, QString comparison, qint64 count,
            const QString &condition,
            const std::function<void(TinyBuilder<Related> &)> &callback)
    {
        /* comparison and count are passed to the last nested relation's addHasWhere().
           condition is passed to the first nested relation's addHasWhere().
           All the middle nested relations are simple whereHas("relation") queries
           without any parameters/callback.
           comparison and count are passed to the last nested relation by help of
           the HasNestedStore to avoid them passing around by method arguments. */

        auto relations = relation.split(DOT);

        const auto doesntHave = comparison == LT && count == 1;

        if (doesntHave) {
            comparison = GE;
            count = 1;
        }

        /* Save properties for the last hasNested() relation, so I don't have to pass
           them around as method arguments. */
        createHasNestedStore<Related>(comparison, count, callback);

        return hasInternal(relations.takeFirst(), doesntHave ? LT : GE, 1, condition,
                           relations);
    }

    template<typename Model>
    template<typename Related>
    TinyBuilder<Model> &
    QueriesRelationships<Model>::addHasWhere(
            TinyBuilder<Related> &hasQuery, const Relation<Related> &relation,
            const QString &comparison, const qint64 count, const QString &condition)
    {
        hasQuery.mergeConstraintsFrom(relation.getQuery());

        if (canUseExistsForExistenceCheck(comparison, count))
            return query().addWhereExistsQuery(
                        // FEATURE scopes, here should be toBase() silverqx
                        hasQuery.getQuerySharedPointer(), condition,
                        comparison == LT && count == 1);

        // FEATURE scopes, here should be toBase() too silverqx
        return query().addWhereCountQuery(hasQuery.getQuerySharedPointer(), comparison,
                                          count, condition);
    }

    template<typename Model>
    TinyBuilder<Model> &
    QueriesRelationships<Model>::addWhereCountQuery(
            const QSharedPointer<QueryBuilder> &query, const QString &comparison,
            const qint64 count, const QString &condition)
    {
        this->query().getQuery().addBinding(query->getBindings(), BindingType::WHERE);

        return this->query().where(Expression(PARENTH_ONE.arg(query->toSql())),
                                   comparison, Expression(count), condition);
    }

    template<typename Model>
    template<typename Related>
    std::unique_ptr<TinyBuilder<Related>>
    QueriesRelationships<Model>::getHasQueryByExistenceCheck(
            const QString &comparison, const qint64 count,
            Relation<Related> &relation) const
    {
        /* If we only need to check for the existence of the relation, then we can
           optimize the subquery to only run a "where exists" clause instead of this
           full "count" clause. This will make these queries run much faster compared
           with a full "count" clause. */
        if (canUseExistsForExistenceCheck(comparison, count))
            return std::invoke(&Relation<Related>::getRelationExistenceQuery,
                               relation,
                               relation.getRelated().newQueryWithoutRelationships(),
                               query(), QVector<Column> {ASTERISK});

        return std::invoke(&Relation<Related>::getRelationExistenceCountQuery,
                           relation,
                           relation.getRelated().newQueryWithoutRelationships(),
                           query());
    }

    template<typename Model>
    inline bool QueriesRelationships<Model>::canUseExistsForExistenceCheck(
            const QString &comparison, const qint64 count) const
    {
        return (comparison == GE || comparison == LT) && count == 1;
    }

    template<typename Model>
    inline TinyBuilder<Model> &QueriesRelationships<Model>::query()
    {
        return static_cast<TinyBuilder<Model> &>(*this);
    }

    template<typename Model>
    inline const TinyBuilder<Model> &QueriesRelationships<Model>::query() const
    {
        return static_cast<const TinyBuilder<Model> &>(*this);
    }

    template<typename Model>
    template<typename Related>
    TinyBuilder<Model> &
    QueriesRelationships<Model>::mergeConstraintsFrom(const TinyBuilder<Related> &from)
    {
        /* Here we have some other query that we want to merge the where constraints
           from. We will copy over any where constraints on the query as well as remove
           any global scopes the query might have removed. Then we will return ourselves
           with the finished merging. */
        // FEATURE scopes silverqx
//        return $this->withoutGlobalScopes(
//            $from->removedScopes()
//        )->mergeWheres(
        // CUR1 after all finished check if wheres and rawbindings can be moved instead of copied silverqx
        return query().mergeWheres(from.getQuery().getWheres(),
                                   from.getQuery().getRawBindings()[BindingType::WHERE]);
    }

    template<typename Model>
    TinyBuilder<Model> &
    QueriesRelationships<Model>::hasInternal(
            const QString &relation, const QString &comparison, const qint64 count,
            const QString &condition, QStringList &relations)
    {
        query().getModel()
                .queriesRelationshipsWithVisitor(
                    relation, *this, comparison, count, condition, nullptr, relations);

        return query();
    }

    template<typename Model>
    template<typename Related>
    void QueriesRelationships<Model>::hasInternalVisited(
            std::unique_ptr<Relation<Related>> &&relation, const QString &comparison,
            const qint64 count, const QString &condition, QStringList &relations)
    {
        if (relations.isEmpty())
            return hasNestedInternalFromStore(std::move(relation));

        // Ownership of a unique_ptr()
        const auto hasQuery = getHasQueryByExistenceCheck(comparison, count, *relation);

        if (relations.isEmpty())
            throw Orm::Exceptions::RuntimeError(
                    QStringLiteral(
                        "wtf, this should never happen :/, 'relations.size() == %1'.")
                    .arg(relations.size()));

        hasQuery->hasInternal(relations.takeFirst(), GE, 1, AND, relations);

        addHasWhere(*hasQuery, *relation, comparison, count, condition);
    }

    template<typename Model>
    template<typename Related>
    void QueriesRelationships<Model>::hasNestedInternalFromStore(
            std::unique_ptr<Relation<Related>> &&relation)
    {
        /* Check if a template argument passed to nested has() agree with guessed Related
           param. for a last nested relation. */
        checkNestedRelationType<Related>();

        /* Parameters to build hasNested() TinyBuilder instace are obtained from
           the nested store. */
        const auto &[comparison, count, callback] =
                *Private::HasNestedStore::STORE<Related>.top();

        // Ownership of a unique_ptr()
        const auto hasQuery = getHasQueryByExistenceCheck(comparison, count, *relation);

        /* Next we will call any given callback as an "anonymous" scope so they can get
           the proper logical grouping of the where clauses if needed by this TinyORM
           query builder. Then, we will be ready to finalize and return this query
           instance. */
        if (callback)
            std::invoke(callback, *hasQuery);

        addHasWhere(*hasQuery, *relation, comparison, count, AND);

        destroyHasNestedStore<Related>();
    }

    template<typename Model>
    template<typename Related, typename Method>
    std::unique_ptr<typename QueriesRelationships<Model>::template Relation<Related>>
    QueriesRelationships<Model>::getRelationWithoutConstraints(const Method method)
    {
        return Relation<Related>::noConstraints([this, method]()
        {
            return std::invoke(method, query().getModel());
        });
    }

    template<typename Model>
    template<typename Related>
    void QueriesRelationships<Model>::createHasNestedStore(
            const QString &comparison, const qint64 count,
            const std::function<void (TinyBuilder<Related> &)> &callback) const
    {
        // BUG clang doesn't know how to init. aggregate ( by forward declaration inside, using construct in place ) silverqx
#ifdef __clang__
        Private::HasNestedStore::STORE<Related>
                .push(std::shared_ptr<Private::HasNestedStore::NestedStore<Related>>(
                          new Private::HasNestedStore::NestedStore<Related> {
                              comparison, count, callback}));
#else
        Private::HasNestedStore::STORE<Related>
                .push(std::make_shared<
                      Private::HasNestedStore::NestedStore<Related>>(
                          comparison, count, callback));
#endif

        Private::HasNestedStore::STORE_TYPEID.push(typeid (Related));
    }

    template<typename Model>
    template<typename Related>
    inline void QueriesRelationships<Model>::destroyHasNestedStore() const
    {
        Private::HasNestedStore::STORE_TYPEID.pop();
        Private::HasNestedStore::STORE<Related>.pop();
    }

    template<typename Model>
    template<typename Related>
    void QueriesRelationships<Model>::checkNestedRelationType() const
    {
        /* The template argument passed to the has() related method has to have the same
           type-id as the last nested relation type obtained by the visitor. This check
           avoids a cryptic crash with the message: "back called on empty deque" during
           HasNestedStore::STORE<Related>.top() call. */
        if (typeid (Related) == Private::HasNestedStore::STORE_TYPEID.top())
            return;

        throw Orm::Exceptions::InvalidTemplateArgumentError(
                QStringLiteral(
                    "Bad template argument passed to the has() related method with "
                    "nested relations. Actual '<Related> = %1', expected '<Related> "
                    "= %2', <Related> has to be of the same type as the 'last' "
                    "relation name passed to the has() related method.")
                .arg(Orm::Utils::Type::classPureBasename(
                         Private::HasNestedStore::STORE_TYPEID.top()),
                     Orm::Utils::Type::classPureBasename<Related>()));
    }

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_QUERIESRELATIONSHIPS_HPP
