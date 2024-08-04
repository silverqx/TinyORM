#include <QCoreApplication>
#include <QTest>

#include "databases.hpp"

#include "models/setting.hpp"
#include "models/user.hpp"

using Orm::Constants::DELETED_AT;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOTE;
using Orm::Constants::UPDATED_AT;

using TypeUtils = Orm::Utils::Type;

using Orm::Tiny::ConnectionOverride;

using TestUtils::Databases;

using Models::Setting;
using Models::User;

class tst_SoftDeletes : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void model_User_Extends_SoftDeletes();
    void model_Setting_NotExtends_SoftDeletes();

    void trashed() const;
    void trashed_Not() const;

    void withoutTrashed_Default() const;
    void withoutTrashed_Explicit() const;
    void withTrashed() const;
    void withTrashed_False() const;
    void onlyTrashed() const;

    void restore_remove_OnModel() const;
    void restore_remove_OnTinyBuilder() const;

    void restore_Trashed_OnModel() const;
    void restore_Trashed_OnTinyBuilder() const;

    void restore_NotTrashed_OnModel() const; // Edge case, should behave the same
    void restore_NotTrashed_OnTinyBuilder() const; // Edge case, should behave the same

    void remove_Trashed_OnModel() const; // Edge case, should behave the same
    void remove_Trashed_OnTinyBuilder() const; // Edge case, should behave the same

    void remove_NotTrashed_OnModel() const;
    void remove_NotTrashed_OnTinyBuilder() const;

    void forceDelete_Trashed_OnModel() const;
    void forceDelete_Trashed_OnTinyBuilder() const;

    void forceDelete_NotTrashed_OnModel() const;
    void forceDelete_NotTrashed_OnTinyBuilder() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Connection name used in this test case. */
    QString m_connection;
};

/*! User type used to compare values. */
struct UserType
{
    /*! Model exists? */
    bool exists;
    /*! Primary key ID. */
    quint64 id;
    /*! Updated at timestamp. */
    QDateTime updatedAt;
    /*! Created at timestamp. */
    QDateTime deletedAt;
    /*! Is model trashed? */
    bool trashed;

    /*! Equality comparison operator for the UserType. */
    bool operator==(const UserType &) const = default;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_SoftDeletes::initTestCase()
{
    ConnectionOverride::connection = m_connection =
            Databases::createConnection(Databases::MYSQL);

    if (m_connection.isEmpty())
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );
}

void tst_SoftDeletes::model_User_Extends_SoftDeletes()
{
    QVERIFY(User::extendsSoftDeletes());
}

void tst_SoftDeletes::model_Setting_NotExtends_SoftDeletes()
{
    QVERIFY(!Setting::extendsSoftDeletes());
}

void tst_SoftDeletes::trashed() const
{
    auto user = User::withTrashed()->find(5);

    QVERIFY(user);
    QVERIFY(user->exists);
    QCOMPARE(user->getKeyCasted(), static_cast<quint64>(5));
    QVERIFY(user->trashed());
}

void tst_SoftDeletes::trashed_Not() const
{
    auto user = User::find(1);

    QVERIFY(user);
    QVERIFY(user->exists);
    QCOMPARE(user->getKeyCasted(), static_cast<quint64>(1));
    QVERIFY(!user->trashed());
}

void tst_SoftDeletes::withoutTrashed_Default() const
{
    QList<quint64> expectedIds {1, 2, 3};

    QList<quint64> actualIds;
    actualIds.reserve(expectedIds.size());

    auto users = User::orderBy(ID)->get({ID});

    QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(3));

    for (const auto &user : std::as_const(users))
        actualIds << user.getKeyCasted();

    QCOMPARE(actualIds, expectedIds);
}

void tst_SoftDeletes::withoutTrashed_Explicit() const
{
    QList<quint64> expectedIds {1, 2, 3};

    QList<quint64> actualIds;
    actualIds.reserve(expectedIds.size());

    auto users = User::withoutTrashed()->orderBy(ID).get({ID});

    QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(3));

    for (const auto &user : std::as_const(users))
        actualIds << user.getKeyCasted();

    QCOMPARE(actualIds, expectedIds);
}

void tst_SoftDeletes::withTrashed() const
{
    QList<quint64> expectedIds {1, 2, 3, 4, 5};

    QList<quint64> actualIds;
    actualIds.reserve(expectedIds.size());

    auto users = User::withTrashed()->orderBy(ID).get({ID});

    QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(5));

    for (const auto &user : std::as_const(users))
        actualIds << user.getKeyCasted();

    QCOMPARE(actualIds, expectedIds);
}

void tst_SoftDeletes::withTrashed_False() const
{
    QList<quint64> expectedIds {1, 2, 3};

    QList<quint64> actualIds;
    actualIds.reserve(expectedIds.size());

    auto users = User::withTrashed(false)->orderBy(ID).get({ID});

    QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(3));

    for (const auto &user : std::as_const(users))
        actualIds << user.getKeyCasted();

    QCOMPARE(actualIds, expectedIds);
}

void tst_SoftDeletes::onlyTrashed() const
{
    QList<quint64> expectedIds {4, 5};

    QList<quint64> actualIds;
    actualIds.reserve(expectedIds.size());

    auto users = User::onlyTrashed()->orderBy(ID).get({ID});

    QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

    for (const auto &user : std::as_const(users))
        actualIds << user.getKeyCasted();

    QCOMPARE(actualIds, expectedIds);
}

namespace
{
    using Orm::TTimeZone;

    const auto UpdatedAt4Original = QDateTime({2022, 1, 4}, {17, 46, 31}, TTimeZone::UTC); // clazy:exclude=non-pod-global-static
    const auto DeletedAt4Original = QDateTime({2022, 1, 4}, {20, 46, 31}, TTimeZone::UTC); // clazy:exclude=non-pod-global-static
    const auto UpdatedAt5Original = QDateTime({2022, 1, 5}, {17, 46, 31}, TTimeZone::UTC); // clazy:exclude=non-pod-global-static
    const auto DeletedAt5Original = QDateTime({2022, 1, 5}, {20, 46, 31}, TTimeZone::UTC); // clazy:exclude=non-pod-global-static
} // namespace

void tst_SoftDeletes::restore_remove_OnModel() const
{
    // This is a big test that tests all at once
    auto user = User::withTrashed()->find(5);

    // Loop two times isn't totally bad idea 😎
    for (auto i = 0; i < 2; ++i) {

        // Validate original user record
        QVERIFY(user);
        QVERIFY(user->exists);
        QCOMPARE(user->getKeyCasted(), static_cast<quint64>(5));
        QCOMPARE(user->getAttribute<QDateTime>(UPDATED_AT), UpdatedAt5Original);
        QCOMPARE(user->getAttribute<QDateTime>(DELETED_AT), DeletedAt5Original);
        QVERIFY(user->trashed());

        auto timeBeforeRestore = QDateTime::currentDateTimeUtc();
        // Reset milliseconds to 0
        {
            auto time = timeBeforeRestore.time();
            timeBeforeRestore.setTime(QTime(time.hour(), time.minute(), time.second()));
        }

        // Restore the user
        QVERIFY(user);
        QVERIFY(user->restore());

        QVERIFY(user->exists);
        QCOMPARE(user->getKeyCasted(), static_cast<quint64>(5));
        QVERIFY(user->getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRestore);
        QCOMPARE(user->getAttribute<QDateTime>(DELETED_AT), QDateTime());
        QVERIFY(!user->trashed());

        // Validate also a user record in the database after the restore
        {
            auto userValidate = User::find(5);

            QVERIFY(userValidate);
            QVERIFY(userValidate->exists);
            QCOMPARE(userValidate->getKeyCasted(), static_cast<quint64>(5));
            QVERIFY(userValidate->getAttribute<QDateTime>(UPDATED_AT) >=
                    timeBeforeRestore);
            QCOMPARE(userValidate->getAttribute<QDateTime>(DELETED_AT), QDateTime());
            QVERIFY(!userValidate->trashed());
        }

        auto timeBeforeRemove = QDateTime::currentDateTimeUtc();
        // Reset milliseconds to 0
        {
            auto time = timeBeforeRemove.time();
            timeBeforeRemove.setTime(QTime(time.hour(), time.minute(), time.second()));
        }

        // Soft delete
        QVERIFY(user);
        QVERIFY(user->remove());

        QVERIFY(user->exists);
        QCOMPARE(user->getKeyCasted(), static_cast<quint64>(5));
        QVERIFY(user->getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRemove);
        QVERIFY(user->getAttribute<QDateTime>(DELETED_AT) >= timeBeforeRemove);
        QVERIFY(user->trashed());

        // Validate also a user record in the database after the remove
        {
            auto userValidate = User::withTrashed()->find(5);

            QVERIFY(userValidate);
            QVERIFY(userValidate->exists);
            QCOMPARE(userValidate->getKeyCasted(), static_cast<quint64>(5));
            QVERIFY(userValidate->getAttribute<QDateTime>(UPDATED_AT) >=
                    timeBeforeRemove);
            QVERIFY(userValidate->getAttribute<QDateTime>(DELETED_AT) >=
                    timeBeforeRemove);
            QVERIFY(userValidate->trashed());
        }

        // Restore db
        QVERIFY(user);
        user->setAttribute(UPDATED_AT, UpdatedAt5Original);
        user->setAttribute(DELETED_AT, DeletedAt5Original);
        QVERIFY(user->save());

        // Validate a user record in the database after restoring the db
        {
            auto userValidate = User::withTrashed()->find(5);

            QVERIFY(userValidate);
            QVERIFY(userValidate->exists);
            QCOMPARE(userValidate->getKeyCasted(), static_cast<quint64>(5));
            QCOMPARE(userValidate->getAttribute<QDateTime>(UPDATED_AT),
                     UpdatedAt5Original);
            QCOMPARE(userValidate->getAttribute<QDateTime>(DELETED_AT),
                     DeletedAt5Original);
            QVERIFY(userValidate->trashed());
        }
    }
}

void tst_SoftDeletes::restore_remove_OnTinyBuilder() const
{
    // This is a big test that tests all at once
    // Validate original user records
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<UserType> expectedValues {
            {true, 4, UpdatedAt4Original, DeletedAt4Original, true},
            {true, 5, UpdatedAt5Original, DeletedAt5Original, true},
        };

        QList<UserType> actualValues;
        actualValues.reserve(expectedValues.size());

        for (const auto &user : std::as_const(users))
            actualValues.append({user.exists,
                                 user.getKeyCasted(),
                                 user.getAttribute<QDateTime>(UPDATED_AT),
                                 user.getAttribute<QDateTime>(DELETED_AT),
                                 user.trashed()});

        QCOMPARE(actualValues, expectedValues);
    }

    auto timeBeforeRestore = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeRestore.time();
        timeBeforeRestore.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    // Restore more users at once
    {
        auto [affected, query] = User::withTrashed()->whereBetween(ID, {4, 5}).restore();

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(2));
    }

    // Validate user records in the database after the restore
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<quint64> expectedIds {4, 5};
        QList<quint64> actualIds;
        actualIds.reserve(expectedIds.size());

        for (const auto &user : std::as_const(users)) {
            QVERIFY(user.exists);
            QVERIFY(user.getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRestore);
            QCOMPARE(user.getAttribute<QDateTime>(DELETED_AT), QDateTime());
            QVERIFY(!user.trashed());

            actualIds << user.getKeyCasted();
        }

        QCOMPARE(actualIds, expectedIds);
    }

    auto timeBeforeRemove = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeRemove.time();
        timeBeforeRemove.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    // Soft delete more users at once
    {
        auto [affected, query] = User::withTrashed()->whereBetween(ID, {4, 5}).remove();

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(2));
    }

    // Validate user records in the database after the remove
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<quint64> expectedIds {4, 5};
        QList<quint64> actualIds;
        actualIds.reserve(expectedIds.size());

        for (const auto &user : std::as_const(users)) {
            QVERIFY(user.exists);
            QVERIFY(user.getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRemove);
            QVERIFY(user.getAttribute<QDateTime>(DELETED_AT) >= timeBeforeRemove);
            QVERIFY(user.trashed());

            actualIds << user.getKeyCasted();
        }

        QCOMPARE(actualIds, expectedIds);
    }

    // Restore db
    {
        auto [affected, query] = User::withTrashed()->whereKey(4)
                                 .update({{UPDATED_AT, UpdatedAt4Original},
                                          {DELETED_AT, DeletedAt4Original}});

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(1));
    }
    {
        auto [affected, query] = User::withTrashed()->whereKey(5)
                                 .update({{UPDATED_AT, UpdatedAt5Original},
                                          {DELETED_AT, DeletedAt5Original}});

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(1));
    }

    // Validate user records in the database after restoring the db
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<UserType> expectedValues {
            {true, 4, UpdatedAt4Original, DeletedAt4Original, true},
            {true, 5, UpdatedAt5Original, DeletedAt5Original, true},
        };

        QList<UserType> actualValues;
        actualValues.reserve(expectedValues.size());

        for (const auto &user : std::as_const(users))
            actualValues.append({user.exists,
                                 user.getKeyCasted(),
                                 user.getAttribute<QDateTime>(UPDATED_AT),
                                 user.getAttribute<QDateTime>(DELETED_AT),
                                 user.trashed()});

        QCOMPARE(actualValues, expectedValues);
    }
}

void tst_SoftDeletes::restore_Trashed_OnModel() const
{
    // Validate before Restore
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(user->trashed());
        QCOMPARE(user->getAttribute<QDateTime>(UPDATED_AT), UpdatedAt5Original);
        QCOMPARE(user->getAttribute<QDateTime>(DELETED_AT), DeletedAt5Original);
    }
    // Restore
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(user->trashed());

        auto timeBeforeRestore = QDateTime::currentDateTimeUtc();
        // Reset milliseconds to 0
        {
            auto time = timeBeforeRestore.time();
            timeBeforeRestore.setTime(QTime(time.hour(), time.minute(), time.second()));
        }

        // Restore
        QVERIFY(user->restore());

        QVERIFY(user->exists);
        QVERIFY(!user->trashed());
        QVERIFY(user->getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRestore);
        QCOMPARE(user->getAttribute<QDateTime>(DELETED_AT), QDateTime());
    }

    // Validate a user record in db
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(!user->trashed());

        // Restore db
        QVERIFY(user);
        user->setAttribute(UPDATED_AT, UpdatedAt5Original);
        user->setAttribute(DELETED_AT, DeletedAt5Original);
        QVERIFY(user->save());

        // Validate a user record in the database after restoring the db
        {
            auto userValidate = User::withTrashed()->find(5);

            QVERIFY(userValidate);
            QVERIFY(userValidate->exists);
            QCOMPARE(userValidate->getKeyCasted(), static_cast<quint64>(5));
            QCOMPARE(userValidate->getAttribute<QDateTime>(UPDATED_AT),
                     UpdatedAt5Original);
            QCOMPARE(userValidate->getAttribute<QDateTime>(DELETED_AT),
                     DeletedAt5Original);
            QVERIFY(userValidate->trashed());
        }
    }
}

void tst_SoftDeletes::restore_Trashed_OnTinyBuilder() const
{
    // Validate original user records before Restore
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<UserType> expectedValues {
            {true, 4, UpdatedAt4Original, DeletedAt4Original, true},
            {true, 5, UpdatedAt5Original, DeletedAt5Original, true},
        };

        QList<UserType> actualValues;
        actualValues.reserve(expectedValues.size());

        for (const auto &user : std::as_const(users))
            actualValues.append({user.exists,
                                 user.getKeyCasted(),
                                 user.getAttribute<QDateTime>(UPDATED_AT),
                                 user.getAttribute<QDateTime>(DELETED_AT),
                                 user.trashed()});

        QCOMPARE(actualValues, expectedValues);
    }

    auto timeBeforeRestore = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeRestore.time();
        timeBeforeRestore.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    // Restore more users at once
    {
        auto [affected, query] = User::withTrashed()->whereBetween(ID, {4, 5}).restore();

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(2));
    }

    // Validate user records in the database after the restore
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<quint64> expectedIds {4, 5};
        QList<quint64> actualIds;
        actualIds.reserve(expectedIds.size());

        for (const auto &user : std::as_const(users)) {
            QVERIFY(user.exists);
            QVERIFY(user.getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRestore);
            QCOMPARE(user.getAttribute<QDateTime>(DELETED_AT), QDateTime());
            QVERIFY(!user.trashed());

            actualIds << user.getKeyCasted();
        }

        QCOMPARE(actualIds, expectedIds);
    }

    // Restore db
    {
        auto [affected, query] = User::withTrashed()->whereKey(4)
                                 .update({{UPDATED_AT, UpdatedAt4Original},
                                          {DELETED_AT, DeletedAt4Original}});

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(1));
    }
    {
        auto [affected, query] = User::withTrashed()->whereKey(5)
                                 .update({{UPDATED_AT, UpdatedAt5Original},
                                          {DELETED_AT, DeletedAt5Original}});

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(1));
    }

    // Validate user records in the database after restoring the db
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<UserType> expectedValues {
            {true, 4, UpdatedAt4Original, DeletedAt4Original, true},
            {true, 5, UpdatedAt5Original, DeletedAt5Original, true},
        };

        QList<UserType> actualValues;
        actualValues.reserve(expectedValues.size());

        for (const auto &user : std::as_const(users))
            actualValues.append({user.exists,
                                 user.getKeyCasted(),
                                 user.getAttribute<QDateTime>(UPDATED_AT),
                                 user.getAttribute<QDateTime>(DELETED_AT),
                                 user.trashed()});

        QCOMPARE(actualValues, expectedValues);
    }
}

void tst_SoftDeletes::restore_NotTrashed_OnModel() const
{
    // Prepare (restore)
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(user->trashed());

        QVERIFY(user->restore());
        QVERIFY(user->exists);
        QVERIFY(!user->trashed());
        QCOMPARE(user->getAttribute<QDateTime>(DELETED_AT), QDateTime());
    }

    // Restore NOT TRASHED (need to test also this scenario 🤯)
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(!user->trashed());

        /* This is different than others, an updated_at column value must be the same
           after the restore() operation because model is NOT dirty (nothing changed),
           so nothing will be updated/saved.
           It also behaves differently than the TinyBuilder/BuildsSoftDeletes::restore(),
           this BuildsSoftDeletes's method will call the SQL UPDATE query even if
           the records aren't trashed because of course, the TinyBuilder doesn't track
           dirty attributes.
           I think this was the reason for occasional failure of this test (5% rate). */
        auto timeBeforeRestore = user->getAttribute<QDateTime>(UPDATED_AT);

        // Restore
        QVERIFY(user->restore());

        QVERIFY(user->exists);
        QVERIFY(!user->trashed());
        // They must be EQUAL because NO update was performed
        QVERIFY(user->getAttribute<QDateTime>(UPDATED_AT) == timeBeforeRestore);
        QCOMPARE(user->getAttribute<QDateTime>(DELETED_AT), QDateTime());
    }

    // Validate a user record in db
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(!user->trashed());

        // Restore db
        QVERIFY(user);
        user->setAttribute(UPDATED_AT, UpdatedAt5Original);
        user->setAttribute(DELETED_AT, DeletedAt5Original);
        QVERIFY(user->save());

        // Validate a user record in the database after restoring the db
        {
            auto userValidate = User::withTrashed()->find(5);

            QVERIFY(userValidate);
            QVERIFY(userValidate->exists);
            QCOMPARE(userValidate->getKeyCasted(), static_cast<quint64>(5));
            QCOMPARE(userValidate->getAttribute<QDateTime>(UPDATED_AT),
                     UpdatedAt5Original);
            QCOMPARE(userValidate->getAttribute<QDateTime>(DELETED_AT),
                     DeletedAt5Original);
            QVERIFY(userValidate->trashed());
        }
    }
}

void tst_SoftDeletes::restore_NotTrashed_OnTinyBuilder() const
{
    /* Below, the restore will update the deleted_at column even if the record isn't
       trashed, the TinyBuilder doesn't track dirty attributes as a model does. */

    // Prepare (restore)
    {
        auto timeBeforeRestore = QDateTime::currentDateTimeUtc();
        // Reset milliseconds to 0
        {
            auto time = timeBeforeRestore.time();
            timeBeforeRestore.setTime(QTime(time.hour(), time.minute(), time.second()));
        }

        {
            auto [affected, query] = User::withTrashed()->whereBetween(ID, {4, 5})
                                     .restore();

            QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
            QCOMPARE(affected, static_cast<int>(2));
        }

        // Validate user records after prepare
        {
            auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                         .orderBy(ID).get();

            QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

            QList<quint64> expectedIds {4, 5};
            QList<quint64> actualIds;
            actualIds.reserve(expectedIds.size());

            for (const auto &user : std::as_const(users)) {
                QVERIFY(user.exists);
                QVERIFY(user.getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRestore);
                QCOMPARE(user.getAttribute<QDateTime>(DELETED_AT), QDateTime());
                QVERIFY(!user.trashed());

                actualIds << user.getKeyCasted();
            }

            QCOMPARE(actualIds, expectedIds);
        }
    }

    auto timeBeforeRestore = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeRestore.time();
        timeBeforeRestore.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    // Restore more users at once
    {
        auto [affected, query] = User::withTrashed()->whereBetween(ID, {4, 5}).restore();

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        /* Longstanding bug, OK, the problem was here, sometimes can happen that
           the restore() method call above isn't fast enough and this restore also
           updates the updated_at column, so even if the deleted_at column isn't updated
           as it's NULL (static_cast<int>(0)), so the updated_at column is updated. */
        QVERIFY(affected == static_cast<int>(0) || affected == static_cast<int>(2));
    }

    // Validate user records in the database after the restore
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<quint64> expectedIds {4, 5};
        QList<quint64> actualIds;
        actualIds.reserve(expectedIds.size());

        for (const auto &user : std::as_const(users)) {
            QVERIFY(user.exists);
            QVERIFY(user.getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRestore);
            QCOMPARE(user.getAttribute<QDateTime>(DELETED_AT), QDateTime());
            QVERIFY(!user.trashed());

            actualIds << user.getKeyCasted();
        }

        QCOMPARE(actualIds, expectedIds);
    }

    // Restore db
    {
        auto [affected, query] = User::withTrashed()->whereKey(4)
                                 .update({{UPDATED_AT, UpdatedAt4Original},
                                          {DELETED_AT, DeletedAt4Original}});

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(1));
    }
    {
        auto [affected, query] = User::withTrashed()->whereKey(5)
                                 .update({{UPDATED_AT, UpdatedAt5Original},
                                          {DELETED_AT, DeletedAt5Original}});

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(1));
    }

    // Validate user records in the database after restoring the db
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<UserType> expectedValues {
            {true, 4, UpdatedAt4Original, DeletedAt4Original, true},
            {true, 5, UpdatedAt5Original, DeletedAt5Original, true},
        };

        QList<UserType> actualValues;
        actualValues.reserve(expectedValues.size());

        for (const auto &user : std::as_const(users))
            actualValues.append({user.exists,
                                 user.getKeyCasted(),
                                 user.getAttribute<QDateTime>(UPDATED_AT),
                                 user.getAttribute<QDateTime>(DELETED_AT),
                                 user.trashed()});

        QCOMPARE(actualValues, expectedValues);
    }
}

void tst_SoftDeletes::remove_Trashed_OnModel() const
{
    // Validate before Soft delete
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(user->trashed());
        QCOMPARE(user->getAttribute<QDateTime>(UPDATED_AT), UpdatedAt5Original);
        QCOMPARE(user->getAttribute<QDateTime>(DELETED_AT), DeletedAt5Original);
    }

    auto timeBeforeRemove = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeRemove.time();
        timeBeforeRemove.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    // Soft delete
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(user->trashed());

        // Soft delete
        QVERIFY(user->remove());

        QVERIFY(user->exists);
        QVERIFY(user->trashed());
        QVERIFY(user->getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRemove);
        QVERIFY(user->getAttribute<QDateTime>(DELETED_AT) >= timeBeforeRemove);
    }

    // Validate a user record in db
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(user->trashed());
        QVERIFY(user->getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRemove);
        QVERIFY(user->getAttribute<QDateTime>(DELETED_AT) >= timeBeforeRemove);

        // Restore db
        QVERIFY(user);
        user->setAttribute(UPDATED_AT, UpdatedAt5Original);
        user->setAttribute(DELETED_AT, DeletedAt5Original);
        QVERIFY(user->save());

        // Validate a user record in the database after restoring the db
        {
            auto userValidate = User::withTrashed()->find(5);

            QVERIFY(userValidate);
            QVERIFY(userValidate->exists);
            QCOMPARE(userValidate->getKeyCasted(), static_cast<quint64>(5));
            QCOMPARE(userValidate->getAttribute<QDateTime>(UPDATED_AT),
                     UpdatedAt5Original);
            QCOMPARE(userValidate->getAttribute<QDateTime>(DELETED_AT),
                     DeletedAt5Original);
            QVERIFY(userValidate->trashed());
        }
    }
}

void tst_SoftDeletes::remove_Trashed_OnTinyBuilder() const
{
    // Validate original user records before Restore
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<UserType> expectedValues {
            {true, 4, UpdatedAt4Original, DeletedAt4Original, true},
            {true, 5, UpdatedAt5Original, DeletedAt5Original, true},
        };

        QList<UserType> actualValues;
        actualValues.reserve(expectedValues.size());

        for (const auto &user : std::as_const(users))
            actualValues.append({user.exists,
                                 user.getKeyCasted(),
                                 user.getAttribute<QDateTime>(UPDATED_AT),
                                 user.getAttribute<QDateTime>(DELETED_AT),
                                 user.trashed()});

        QCOMPARE(actualValues, expectedValues);
    }

    auto timeBeforeRemove = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeRemove.time();
        timeBeforeRemove.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    // Soft delete more users at once
    {
        auto [affected, query] = User::withTrashed()->whereBetween(ID, {4, 5}).remove();

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(2));
    }

    // Validate user records in the database after the remove
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<quint64> expectedIds {4, 5};
        QList<quint64> actualIds;
        actualIds.reserve(expectedIds.size());

        for (const auto &user : std::as_const(users)) {
            QVERIFY(user.exists);
            QVERIFY(user.getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRemove);
            QVERIFY(user.getAttribute<QDateTime>(DELETED_AT) >= timeBeforeRemove);
            QVERIFY(user.trashed());

            actualIds << user.getKeyCasted();
        }

        QCOMPARE(actualIds, expectedIds);
    }

    // Restore db
    {
        auto [affected, query] = User::withTrashed()->whereKey(4)
                                 .update({{UPDATED_AT, UpdatedAt4Original},
                                          {DELETED_AT, DeletedAt4Original}});

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(1));
    }
    {
        auto [affected, query] = User::withTrashed()->whereKey(5)
                                 .update({{UPDATED_AT, UpdatedAt5Original},
                                          {DELETED_AT, DeletedAt5Original}});

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(1));
    }

    // Validate user records in the database after restoring the db
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<UserType> expectedValues {
            {true, 4, UpdatedAt4Original, DeletedAt4Original, true},
            {true, 5, UpdatedAt5Original, DeletedAt5Original, true},
        };

        QList<UserType> actualValues;
        actualValues.reserve(expectedValues.size());

        for (const auto &user : std::as_const(users))
            actualValues.append({user.exists,
                                 user.getKeyCasted(),
                                 user.getAttribute<QDateTime>(UPDATED_AT),
                                 user.getAttribute<QDateTime>(DELETED_AT),
                                 user.trashed()});

        QCOMPARE(actualValues, expectedValues);
    }
}

void tst_SoftDeletes::remove_NotTrashed_OnModel() const
{
    // Prepare
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(user->trashed());

        QVERIFY(user->restore());
        QVERIFY(user->exists);
        QVERIFY(!user->trashed());
        QCOMPARE(user->getAttribute<QDateTime>(DELETED_AT), QDateTime());
    }

    auto timeBeforeRemove = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeRemove.time();
        timeBeforeRemove.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    // Soft delete
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(!user->trashed());

        // Soft delete
        QVERIFY(user->remove());

        QVERIFY(user->exists);
        QVERIFY(user->trashed());
        QVERIFY(user->getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRemove);
        QVERIFY(user->getAttribute<QDateTime>(DELETED_AT) >= timeBeforeRemove);
    }

    // Validate a user record in db
    {
        auto user = User::withTrashed()->find(5);

        QVERIFY(user);
        QVERIFY(user->exists);
        QVERIFY(user->trashed());
        QVERIFY(user->getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRemove);
        QVERIFY(user->getAttribute<QDateTime>(DELETED_AT) >= timeBeforeRemove);

        // Restore db
        QVERIFY(user);
        user->setAttribute(UPDATED_AT, UpdatedAt5Original);
        user->setAttribute(DELETED_AT, DeletedAt5Original);
        QVERIFY(user->save());

        // Validate a user record in the database after restoring the db
        {
            auto userValidate = User::withTrashed()->find(5);

            QVERIFY(userValidate);
            QVERIFY(userValidate->exists);
            QCOMPARE(userValidate->getKeyCasted(), static_cast<quint64>(5));
            QCOMPARE(userValidate->getAttribute<QDateTime>(UPDATED_AT),
                     UpdatedAt5Original);
            QCOMPARE(userValidate->getAttribute<QDateTime>(DELETED_AT),
                     DeletedAt5Original);
            QVERIFY(userValidate->trashed());
        }
    }
}

void tst_SoftDeletes::remove_NotTrashed_OnTinyBuilder() const
{
    // Prepare (restore)
    {
        auto timeBeforeRestore = QDateTime::currentDateTimeUtc();
        // Reset milliseconds to 0
        {
            auto time = timeBeforeRestore.time();
            timeBeforeRestore.setTime(QTime(time.hour(), time.minute(), time.second()));
        }

        {
            auto [affected, query] = User::withTrashed()->whereBetween(ID, {4, 5})
                                     .restore();

            QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
            QCOMPARE(affected, static_cast<int>(2));
        }

        // Validate user records after prepare
        {
            auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                         .orderBy(ID).get();

            QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

            QList<quint64> expectedIds {4, 5};
            QList<quint64> actualIds;
            actualIds.reserve(expectedIds.size());

            for (const auto &user : std::as_const(users)) {
                QVERIFY(user.exists);
                QVERIFY(user.getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRestore);
                QCOMPARE(user.getAttribute<QDateTime>(DELETED_AT), QDateTime());
                QVERIFY(!user.trashed());

                actualIds << user.getKeyCasted();
            }

            QCOMPARE(actualIds, expectedIds);
        }
    }

    auto timeBeforeRemove = QDateTime::currentDateTimeUtc();
    // Reset milliseconds to 0
    {
        auto time = timeBeforeRemove.time();
        timeBeforeRemove.setTime(QTime(time.hour(), time.minute(), time.second()));
    }

    // Soft delete more users at once
    {
        auto [affected, query] = User::withTrashed()->whereBetween(ID, {4, 5}).remove();

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(2));
    }

    // Validate user records in the database after the remove
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<quint64> expectedIds {4, 5};
        QList<quint64> actualIds;
        actualIds.reserve(expectedIds.size());

        for (const auto &user : std::as_const(users)) {
            QVERIFY(user.exists);
            QVERIFY(user.getAttribute<QDateTime>(UPDATED_AT) >= timeBeforeRemove);
            QVERIFY(user.getAttribute<QDateTime>(DELETED_AT) >= timeBeforeRemove);
            QVERIFY(user.trashed());

            actualIds << user.getKeyCasted();
        }

        QCOMPARE(actualIds, expectedIds);
    }

    // Restore db
    {
        auto [affected, query] = User::withTrashed()->whereKey(4)
                                 .update({{UPDATED_AT, UpdatedAt4Original},
                                          {DELETED_AT, DeletedAt4Original}});

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(1));
    }
    {
        auto [affected, query] = User::withTrashed()->whereKey(5)
                                 .update({{UPDATED_AT, UpdatedAt5Original},
                                          {DELETED_AT, DeletedAt5Original}});

        QVERIFY(query.isActive() && !query.isSelect() && !query.isValid());
        QCOMPARE(affected, static_cast<int>(1));
    }

    // Validate user records in the database after restoring the db
    {
        auto users = User::withTrashed()->whereBetween(ID, {4, 5})
                     .orderBy(ID).get();

        QCOMPARE(users.size(), static_cast<QList<quint64>::size_type>(2));

        QList<UserType> expectedValues {
            {true, 4, UpdatedAt4Original, DeletedAt4Original, true},
            {true, 5, UpdatedAt5Original, DeletedAt5Original, true},
        };

        QList<UserType> actualValues;
        actualValues.reserve(expectedValues.size());

        for (const auto &user : std::as_const(users))
            actualValues.append({user.exists,
                                 user.getKeyCasted(),
                                 user.getAttribute<QDateTime>(UPDATED_AT),
                                 user.getAttribute<QDateTime>(DELETED_AT),
                                 user.trashed()});

        QCOMPARE(actualValues, expectedValues);
    }
}

void tst_SoftDeletes::forceDelete_Trashed_OnModel() const
{
    // Create test user
    auto user = User::create({{NAME, "liltrash"},
                              {"is_banned", false},
                              {NOTE, "forceDelete test"}});

    QVERIFY(user.exists);
    QVERIFY(!user.trashed());
    QVERIFY(User::withTrashed()->whereKey(user.getKey()).exists());

    // Soft delete
    QVERIFY(user.remove());
    QVERIFY(user.exists);
    QVERIFY(user.trashed());

    // Force delete
    QVERIFY(user.forceDelete());

    QVERIFY(!user.exists);
    QVERIFY(User::withTrashed()->whereKey(user.getKey()).doesntExist());
}

void tst_SoftDeletes::forceDelete_Trashed_OnTinyBuilder() const
{
    // Create test users
    auto user1 = User::create({{NAME, "liltrash"},
                               {"is_banned", false},
                               {NOTE, "forceDelete test"}});
    QVERIFY(user1.exists);
    QVERIFY(!user1.trashed());
    QVERIFY(User::withTrashed()->whereKey(user1.getKey()).exists());

    auto user2 = User::create({{NAME, "loltrash"},
                               {"is_banned", false},
                               {NOTE, "forceDelete test"}});
    QVERIFY(user2.exists);
    QVERIFY(!user2.trashed());
    QVERIFY(User::withTrashed()->whereKey(user2.getKey()).exists());

    // Soft delete
    QVERIFY(user1.remove());
    QVERIFY(user1.exists);
    QVERIFY(user1.trashed());
    QVERIFY(user2.remove());
    QVERIFY(user2.exists);
    QVERIFY(user2.trashed());

    // Force delete more users at once
    auto [affected, query] = User::whereEq(ID, user1.getKey())
                             ->orWhereEq(ID, user2.getKey())
                             .forceDelete();

    QCOMPARE(affected, static_cast<int>(2));
    QVERIFY(query.isActive() && !query.isValid() && !query.isSelect());

    QVERIFY(User::withTrashed()->whereKey(user1.getKey()).doesntExist());
    QVERIFY(User::withTrashed()->whereKey(user2.getKey()).doesntExist());
}

void tst_SoftDeletes::forceDelete_NotTrashed_OnModel() const
{
    // Create test user
    auto user = User::create({{NAME, "lilnot-trash"},
                              {"is_banned", false},
                              {NOTE, "forceDelete test"}});

    QVERIFY(user.exists);
    QVERIFY(!user.trashed());
    QVERIFY(User::withTrashed()->whereKey(user.getKey()).exists());

    // Force delete
    QVERIFY(user.forceDelete());

    QVERIFY(!user.exists);
    QVERIFY(User::withTrashed()->whereKey(user.getKey()).doesntExist());
}

void tst_SoftDeletes::forceDelete_NotTrashed_OnTinyBuilder() const
{
    // Create test users
    auto user1 = User::create({{NAME, "liltrash"},
                               {"is_banned", false},
                               {NOTE, "forceDelete test"}});
    QVERIFY(user1.exists);
    QVERIFY(!user1.trashed());
    QVERIFY(User::withTrashed()->whereKey(user1.getKey()).exists());

    auto user2 = User::create({{NAME, "loltrash"},
                               {"is_banned", false},
                               {NOTE, "forceDelete test"}});
    QVERIFY(user2.exists);
    QVERIFY(!user2.trashed());
    QVERIFY(User::withTrashed()->whereKey(user2.getKey()).exists());

    // Force delete more users at once
    auto [affected, query] = User::whereEq(ID, user1.getKey())
                             ->orWhereEq(ID, user2.getKey())
                             .forceDelete();

    QCOMPARE(affected, static_cast<int>(2));
    QVERIFY(query.isActive() && !query.isValid() && !query.isSelect());

    QVERIFY(User::withTrashed()->whereKey(user1.getKey()).doesntExist());
    QVERIFY(User::withTrashed()->whereKey(user2.getKey()).doesntExist());
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_SoftDeletes)

#include "tst_softdeletes.moc"
