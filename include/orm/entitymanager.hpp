#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "orm/databaseconnection.hpp"
#include "orm/repositoryfactory.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    /*! The EntityManager is the central access point to ORM functionality. */
    class SHAREDLIB_EXPORT EntityManager final
    {
        Q_DISABLE_COPY(EntityManager)

    public:
        EntityManager(const QVariantHash &config);
        EntityManager(DatabaseConnection &connection);
        ~EntityManager();

        /*! Factory method to create EntityManager instances. */
        static EntityManager create(const QVariantHash &config);

        /*! Gets the repository for an entity class. */
        template<typename Repository>
        QSharedPointer<Repository> getRepository() const;

        /*! Create a new QSqlQuery. */
        QSqlQuery query() const;
        /*! Get a new query builder instance. */
        QSharedPointer<QueryBuilder> queryBuilder() const;
        /*! Check database connection and show warnings when the state changed. */
        bool pingDatabase();
        /*! Start a new database transaction. */
        bool transaction();
        /*! Commit the active database transaction. */
        bool commit();
        /*! Rollback the active database transaction. */
        bool rollback();
        /*! Start a new named transaction savepoint. */
        bool savepoint(const QString &id);
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(const QString &id);

        /*! Get underlying database connection. */
        inline DatabaseConnection &connection() const
        { return m_db; }

    protected:
        /*! Factory method to create DatabaseConnection instances. */
        static DatabaseConnection &
        createConnection(const QVariantHash &config);

    private:
        /*! The database connection used by the EntityManager. */
        DatabaseConnection &m_db;
        /*! The repository factory used to create dynamic repositories. */
        RepositoryFactory m_repositoryFactory;
    };

    template<typename Repository>
    QSharedPointer<Repository> EntityManager::getRepository() const
    {
        return m_repositoryFactory.getRepository<Repository>();
    }

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ENTITYMANAGER_H
