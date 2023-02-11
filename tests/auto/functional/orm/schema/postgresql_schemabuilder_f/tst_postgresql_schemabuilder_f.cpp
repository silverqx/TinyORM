#include <QCoreApplication>
#include <QtTest>

#include "orm/exceptions/searchpathemptyerror.hpp"
#include "orm/postgresconnection.hpp"
#include "orm/schema.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

using Orm::Constants::EMPTY;
using Orm::Constants::PUBLIC;
using Orm::Constants::search_path;

using Orm::Exceptions::SearchPathEmptyError;
using Orm::PostgresConnection;
using Orm::Schema;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_PostgreSQL_SchemaBuilder_f : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void hasTable_NoSearchPath_InConfiguration() const;
    void hasTable_EmptySearchPath_InConfiguration_UnqualifiedTablename_ThrowException() const;
    void hasTable_EmptySearchPath_InConfiguration_QualifiedTablename() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! The Database Manager instance in the TinyUtils. */
    std::shared_ptr<Orm::DatabaseManager> m_dm;
    /*! Connection name used in this test case. */
    QString m_connection {};
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_PostgreSQL_SchemaBuilder_f::initTestCase()
{
    m_connection = Databases::createConnection(Databases::POSTGRESQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    m_dm = Databases::manager();
}

void tst_PostgreSQL_SchemaBuilder_f::hasTable_NoSearchPath_InConfiguration() const
{
    const auto connectionName =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_PostgreSQL_SchemaBuilder_f-"
                "hasTable_NoSearchPath_InConfiguration");

    // Make configuration copy so I can modify it
    auto configuration = Databases::configuration(Databases::POSTGRESQL)->get();

    // Prepare/modify configuration
    QVERIFY(configuration.remove(search_path));

    // Add a new database connection
    m_dm->addConnections({
        {connectionName, configuration},
    // Don't setup any default connection
    }, EMPTY);

    // Verify
    const auto hasTable = Schema::on(connectionName).hasTable(QStringLiteral("users"));

    /* This check is really weird, our implementation queries the PostgreSQL database
       using the 'show search_path' query to obtain a real search_path if
       the 'search_path' configuration option is not defined. Because of that I have to
       check if the database search_path starts with the 'public' schema or is empty
       and based on that do the QVERIFY().
       Because I can't know what is the default for the search_path on foreign DB, this
       secures that this test passes whatever search_path will be set to.
       This also makes auto-tests dependent on the 'public' schema. */
    const auto searchPath = dynamic_cast<PostgresConnection &>(
                                m_dm->connection(connectionName))
                            .searchPath();

    if (PostgresConnection::isSearchPathEmpty(searchPath) ||
        searchPath.constFirst() != PUBLIC
    )
        QVERIFY(!hasTable);
    else
        QVERIFY(hasTable);

    // Restore
    QVERIFY(m_dm->removeConnection(connectionName));
}

void tst_PostgreSQL_SchemaBuilder_f::
     hasTable_EmptySearchPath_InConfiguration_UnqualifiedTablename_ThrowException() const
{
    const auto connectionName =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_PostgreSQL_SchemaBuilder_f-"
                "hasTable_EmptySearchPath_InConfiguration_UnqualifiedTablename_"
                "ThrowException");

    // Make configuration copy so I can modify it
    auto configuration = Databases::configuration(Databases::POSTGRESQL)->get();

    // Prepare/modify configuration
    configuration[search_path] = QStringLiteral("''");

    // Add a new database connection
    m_dm->addConnections({
        {connectionName, configuration},
    // Don't setup any default connection
    }, EMPTY);

    // Verify
    QVERIFY_EXCEPTION_THROWN(
                Schema::on(connectionName).hasTable(QStringLiteral("users")),
                SearchPathEmptyError);

    // Restore
    QVERIFY(m_dm->removeConnection(connectionName));
}

void tst_PostgreSQL_SchemaBuilder_f::
     hasTable_EmptySearchPath_InConfiguration_QualifiedTablename() const
{
    const auto connectionName =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_PostgreSQL_SchemaBuilder_f-"
                "hasTable_EmptySearchPath_InConfiguration_QualifiedTablename");

    // Make configuration copy so I can modify it
    auto configuration = Databases::configuration(Databases::POSTGRESQL)->get();

    // Prepare/modify configuration
    configuration[search_path] = QStringLiteral("''");

    // Add a new database connection
    m_dm->addConnections({
        {connectionName, configuration},
    // Don't setup any default connection
    }, EMPTY);

    // Verify
    QVERIFY(Schema::on(connectionName).hasTable(QStringLiteral("public.users")));

    // Restore
    QVERIFY(m_dm->removeConnection(connectionName));
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_PostgreSQL_SchemaBuilder_f)

#include "tst_postgresql_schemabuilder_f.moc"
