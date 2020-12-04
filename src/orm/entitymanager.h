#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "orm/databaseconnection.h"
#include "orm/repositoryfactory.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class EntityManager final
    {
        Q_DISABLE_COPY(EntityManager)

    public:
        EntityManager();
        ~EntityManager();

        template<typename Repository>
        QSharedPointer<Repository> getRepository() const;

        /*! Create a new QSqlQuery. */
        QSqlQuery query() const;
        QSharedPointer<QueryBuilder> queryBuilder() const;
        /*! Check database connection and show warnings when the state changed. */
        bool pingDatabase();
        bool transaction();
        bool commit();
        bool rollback();
        bool savepoint(const QString &id);
        bool rollbackToSavepoint(const QString &id);

    private:
        DatabaseConnection &m_db;
        RepositoryFactory m_repositoryFactory;
    };

    template<typename Repository>
    QSharedPointer<Repository> EntityManager::getRepository() const
    {
        return m_repositoryFactory.getRepository<Repository>();
    }

} // namespace Orm
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif

#endif // ENTITYMANAGER_H
