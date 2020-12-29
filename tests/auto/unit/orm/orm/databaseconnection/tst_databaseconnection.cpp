#include <QCoreApplication>
#include <QtTest>

#include "orm/databaseconnection.hpp"

#include "database.hpp"

class tst_DatabaseConnection : public QObject
{
    Q_OBJECT

public:
    tst_DatabaseConnection();
    ~tst_DatabaseConnection() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void pingDatabase();

private:
    Orm::DatabaseConnection &m_db;
};

tst_DatabaseConnection::tst_DatabaseConnection()
    : m_db(Utils::Database::createConnection())
{}

void tst_DatabaseConnection::initTestCase()
{

}

void tst_DatabaseConnection::cleanupTestCase()
{

}

void tst_DatabaseConnection::pingDatabase()
{
    const auto result = m_db.pingDatabase();

    QVERIFY2(result, "Ping database failed.");
}

QTEST_MAIN(tst_DatabaseConnection)

#include "tst_databaseconnection.moc"
