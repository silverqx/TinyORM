#include "orm/entitymanager.hpp"

#include <QtSql/QSqlQuery>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

/*!
    \class EntityManager
    \brief The EntityManager class manages repositories and a connection
    to the database.

    \ingroup database
    \inmodule Export

    EntityManager is the base class to work with the database, it creates
    and manages repository classes by helping with the RepositoryFactory
    class.
    Creates the database connection which is represented by
    DatabaseConnection class.
    EntityManager should be used in controllers ( currently TorrentExporter
    is like a controller class ), services, and repository classes to access
    the database. There is no need to use the QSqlDatabase or the
    DatabaseConnection classes directly.
    EntityManager is also injected into a repository and a service
    classes constructors.
    The circular dependency problem is solved by including entitymanager.hpp
    in the baserepository.hpp file.
*/

EntityManager::EntityManager(const QVariantHash &config)
    : m_db(createConnection(config))
    , m_repositoryFactory(*this)
{}

EntityManager::EntityManager(DatabaseConnection &connection)
    : m_db(connection)
    , m_repositoryFactory(*this)
{}

EntityManager::~EntityManager()
{
    DatabaseConnection::freeInstance();
}

EntityManager EntityManager::create(const QVariantHash &config)
{
    return EntityManager(createConnection(config));
}

QSqlQuery EntityManager::query() const
{
    return m_db.query();
}

QSharedPointer<QueryBuilder> EntityManager::queryBuilder() const
{
    return m_db.queryBuilder();
}

bool EntityManager::pingDatabase()
{
    return m_db.pingDatabase();
}

bool EntityManager::transaction()
{
    return m_db.transaction();
}

bool EntityManager::commit()
{
    return m_db.commit();
}

bool EntityManager::rollback()
{
    return m_db.rollback();
}

bool EntityManager::savepoint(const QString &id)
{
    return m_db.savepoint(id);
}

bool EntityManager::rollbackToSavepoint(const QString &id)
{
    return m_db.rollbackToSavepoint(id);
}

DatabaseConnection &
EntityManager::createConnection(const QVariantHash &config)
{
    return DatabaseConnection::create(config.find("database").value().toString(),
                                      config.find("prefix").value().toString(),
                                      config);
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
