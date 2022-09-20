#include <QCoreApplication>
#include <QtTest>

#include "models/user.hpp"

#include "databases.hpp"

using Orm::Constants::EMPTY;

using Orm::Tiny::ConnectionOverride;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

using Models::Phone;
using Models::Role;
using Models::User;

class tst_Relations_Connection_Independent : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private slots:
    void initTestCase();

    /* HasOne */
    void is_HasOne() const;
    void isNot_HasOne() const;

    /* BelongsTo */
    void is_BelongsTo() const;
    void isNot_BelongsTo() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection {};
};

/* private slots */

void tst_Relations_Connection_Independent::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

/* HasOne */

void tst_Relations_Connection_Independent::is_HasOne() const
{
    auto user = User::find(1);
    auto phone = Phone::find(1);

    // The same primary key, table name and connection name
    QVERIFY(user->phone()->is(phone));
}

void tst_Relations_Connection_Independent::isNot_HasOne() const
{
    auto user1 = User::find(1);

    // Different primary key
    {
        auto phone2 = Phone::find(2);

        QVERIFY(user1->phone()->isNot(phone2));
    }
    // Different table name (also different type)
    {
        auto role1 = Role::find(1);

        QVERIFY(user1->phone()->isNot(role1));
    }
    // Different connection name
    {
        auto phone1 = Phone::find(1);

        phone1->setConnection("dummy_connection");
        /* Disable connection override, so the isNot() can pickup a connection from
           the model itself (don't pickup an overridden connection). */
        ConnectionOverride::connection.clear();

        QVERIFY(user1->phone()->isNot(phone1));

        // Restore
        ConnectionOverride::connection = m_connection;
    }
}

/* BelongsTo */

void tst_Relations_Connection_Independent::is_BelongsTo() const
{
    auto phone = Phone::find(1);
    auto user = User::find(1);

    // The same primary key, table name and connection name
    QVERIFY(phone->user()->is(user));
}

void tst_Relations_Connection_Independent::isNot_BelongsTo() const
{
    auto phone1 = Phone::find(1);

    // Different primary key
    {
        auto user2 = User::find(2);

        QVERIFY(phone1->user()->isNot(user2));
    }
    // Different table name (also different type)
    {
        auto role1 = Role::find(1);

        QVERIFY(phone1->user()->isNot(role1));
    }
    // Different connection name
    {
        auto user1 = User::find(1);

        user1->setConnection("dummy_connection");
        /* Disable connection override, so the isNot() can pickup a connection from
           the model itself (don't pickup an overridden connection). */
        ConnectionOverride::connection.clear();

        QVERIFY(phone1->user()->isNot(user1));

        // Restore
        ConnectionOverride::connection = m_connection;
    }
}

QTEST_MAIN(tst_Relations_Connection_Independent)

#include "tst_relations_connection_independent.moc"
