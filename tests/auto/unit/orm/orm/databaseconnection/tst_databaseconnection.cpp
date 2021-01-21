#include <QCoreApplication>
#include <QtTest>

#include "database.hpp"

using namespace Orm;

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
    ConnectionInterface &m_connection;
};

tst_DatabaseConnection::tst_DatabaseConnection()
    : m_connection(TestUtils::Database::createConnection())
{}

void tst_DatabaseConnection::initTestCase()
{}

void tst_DatabaseConnection::cleanupTestCase()
{}

void tst_DatabaseConnection::pingDatabase()
{
    const auto result = m_connection.pingDatabase();

    QVERIFY2(result, "Ping database failed.");
}

QTEST_MAIN(tst_DatabaseConnection)

#include "tst_databaseconnection.moc"
