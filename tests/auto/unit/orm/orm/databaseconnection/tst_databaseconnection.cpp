#include <QtTest>

#include "orm/databaseconnection.h"

class tst_DatabaseConnection : public QObject
{
    Q_OBJECT

public:
    tst_DatabaseConnection();
    ~tst_DatabaseConnection();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void pingDatabase();
};

tst_DatabaseConnection::tst_DatabaseConnection()
{

}

tst_DatabaseConnection::~tst_DatabaseConnection()
{

}

void tst_DatabaseConnection::initTestCase()
{

}

void tst_DatabaseConnection::cleanupTestCase()
{

}

void tst_DatabaseConnection::pingDatabase()
{
    auto &conn = Orm::DatabaseConnection::instance();

    const auto result = conn.pingDatabase();

    QVERIFY2(result, "Ping database failed.");
}

QTEST_APPLESS_MAIN(tst_DatabaseConnection)

#include "tst_databaseconnection.moc"
