# Database: Getting Started

- [Introduction](#introduction)
    - [Configuration](#configuration)
- [Running SQL Queries](#running-sql-queries)
    - [Using Multiple Database Connections](#using-multiple-database-connections)
- [Database Transactions](#database-transactions)

<a name="introduction"></a>
## Introduction

Almost every modern application interacts with a database. TinyORM makes interacting with a database extremely simple using raw SQL, a [fluent query builder](/docs/{{version}}/query-builder), and the [TinyORM](/docs/{{version}}/tinyorm). Currently, TinyORM provides first-party support for one database:

<div class="content-list" markdown="1">
- MySQL or MariaDB 5.0+ ([Version Policy](https://en.wikipedia.org/wiki/MySQL#Release_history))
</div>

TinyORM internally uses `QtSql` module, you can look for [supported databases](https://doc.qt.io/qt-5/sql-driver.html#supported-databases).

> {note} TinyORM's code is ready and designed to simply add support for the following databases PostgreSQL, SQLite and SQL Server.

<a name="configuration"></a>
### Configuration

You can create and configure new database connection by `create` method provided by `DB` facade:

    #include <orm/db.hpp>

    auto manager = DB::create({
        {"driver",    "QMYSQL"},
        {"host",      qEnvironmentVariable("DB_HOST", "127.0.0.1")},
        {"port",      qEnvironmentVariable("DB_PORT", "3306")},
        {"database",  qEnvironmentVariable("DB_DATABASE", "")},
        {"username",  qEnvironmentVariable("DB_USERNAME", "root")},
        {"password",  qEnvironmentVariable("DB_PASSWORD", "")},
        {"charset",   qEnvironmentVariable("DB_CHARSET", "utf8mb4")},
        {"collation", qEnvironmentVariable("DB_COLLATION", "utf8mb4_0900_ai_ci")},
        {"strict",    true},
        {"options",   QVariantHash()},
    });

The first argument is configuration hash which is of type `QVariantHash` and the second argument specifies the name of the *connection*, this connection will also be a *default connection*. You can configure multiple database connections at once and choose the needed one before executing SQL query, section [Using Multiple Database Connections](#using-multiple-database-connections) describes how to create and use multiple database connections.

You may also configure connection options by `options` key as `QVariantHash` or `QString`, you can pass any [connection options](https://doc.qt.io/qt-5/qsqldatabase.html#setConnectOptions) supported by `QSqlDatabase`.

> {note} A database connection is resolved lazily, which means that the connection configuration is only saved after the `DB::create` method call. The connection will be resolved after you run some query or you can create it using the `DB::connection` method.

<a name="running-sql-queries"></a>
## Running SQL Queries

Once you have configured your database connection, you may run queries using the `DB` facade. The `DB` facade provides methods for each type of query: `select`, `update`, `insert`, `delete`, and `statement`.

<a name="running-a-select-query"></a>
#### Running A Select Query

To run a basic SELECT query, you may use the `select` method on the `DB` facade:

    auto [ok, users] = DB::select("select * from users where active = ?", {1});

The first argument passed to the `select` method is the SQL query, while the second argument is any parameter bindings that need to be bound to the query. Typically, these are the values of the `where` clause constraints. Parameter binding provides protection against SQL injection.

The `select` method returns a `std::tuple<bool, QSqlQuery>` containing the results of the query, where each result can be accessed by `QSqlQuery::next` method. Look into the `QSqlQuery` documentation on how to obtain results from the "query". You may access each column's value by `QSqlQuery::value` method. The first `bool` return value is the value returned from `QSqlQuery::exec` method:

    #include <QDebug>

    #include <orm/db.hpp>

    auto [ok, users] = DB::select("select * from users");

    while(users.next())
        qDebug() << users.value("name").toString();

<a name="running-an-insert-statement"></a>
#### Running An Insert Statement

To execute an `insert` statement, you may use the `insert` method on the `DB` facade. Like `select`, this method accepts the SQL query as its first argument and bindings as its second argument and returns `std::tuple<bool, QSqlQuery>`:

    #include <QDebug>

    #include <orm/db.hpp>

    auto [ok, query] = DB::insert("insert into users (id, name) values (?, ?)", {1, "Marc"});

    if (!ok)
        qDebug() << "Insert failed.";

<a name="running-an-update-statement"></a>
#### Running An Update Statement

The `update` method should be used to update existing records in the database. The number of rows affected by the statement and `QSqlQuery` is returned by the method as `std::tuple<int, QSqlQuery>`:

    #include <QDateTime>

    #include <orm/db.hpp>

    auto [affected, query] = DB::update(
        "update users set updated_at = ? where name = ?",
        {QDateTime::currentDateTime(), "Anita"}
    );

    if (!affected)
        qDebug() << "Any record was updated.";

<a name="running-a-delete-statement"></a>
#### Running A Delete Statement

The `remove` method should be used to delete records from the database. Like `update`, the number of affected rows and `QSqlQuery` will be returned by the method as `std::tuple<int, QSqlQuery>`:

    #include <orm/db.hpp>

    auto [affected, query] = DB::remove("delete from users");

> {note} `DB::delete` can not be used as the method name because it is the reserved word.

<a name="running-a-general-statement"></a>
#### Running A General Statement

Some database statements do not return any value. For these types of operations, you may use the `statement` method on the `DB` facade:

    DB::statement("drop table users");

<a name="implicit-commits-in-transactions"></a>
#### Implicit Commits

When using the `DB` facade's `statement` methods within transactions, you must be careful to avoid statements that cause [implicit commits](https://dev.mysql.com/doc/refman/8.0/en/implicit-commit.html). These statements will cause the database engine to indirectly commit the entire transaction, leaving TinyORM unaware of the database's transaction level. An example of such a statement is creating a database table:

    DB::statement("create table users (name varchar(255) null)");

Please refer to the MySQL manual for [a list of all statements](https://dev.mysql.com/doc/refman/8.0/en/implicit-commit.html) that trigger implicit commits.

<a name="using-multiple-database-connections"></a>
### Using Multiple Database Connections

You can configure multiple database connections at once during `DatabaseManager` instantiation using the `DB::create` overload, where the first argument is a hash of multiple connections and is of type `QHash<QString, QVariantHash>` and the second argument is the name of the default connection:

    #include <orm/db.hpp>

    auto manager = DB::create({
        {"mysql", {
            {"driver",    "QMYSQL"},
            {"host",      qEnvironmentVariable("DB_HOST", "127.0.0.1")},
            {"port",      qEnvironmentVariable("DB_PORT", "3306")},
            {"database",  qEnvironmentVariable("DB_DATABASE", "")},
            {"username",  qEnvironmentVariable("DB_USERNAME", "root")},
            {"password",  qEnvironmentVariable("DB_PASSWORD", "")},
            {"charset",   qEnvironmentVariable("DB_CHARSET", "utf8mb4")},
            {"collation", qEnvironmentVariable("DB_COLLATION", "utf8mb4_0900_ai_ci")},
            {"strict",    true},
            {"options",   QVariantHash()},
        }},
        {"mysql_test", {
            {"driver",    "QMYSQL"},
            {"host",      qEnvironmentVariable("DB_HOST", "127.0.0.1")},
            {"port",      qEnvironmentVariable("DB_PORT", "3306")},
            {"database",  qEnvironmentVariable("DB_DATABASE", "")},
            {"username",  qEnvironmentVariable("DB_USERNAME", "root")},
            {"password",  qEnvironmentVariable("DB_PASSWORD", "")},
            {"charset",   qEnvironmentVariable("DB_CHARSET", "utf8mb4")},
            {"collation", qEnvironmentVariable("DB_COLLATION", "utf8mb4_0900_ai_ci")},
            {"strict",    true},
            {"options",   QVariantHash()},
        }},
    }, "mysql");

If your application needs to use multiple connections, you may access each connection via the `connection` method provided by the `DB` facade. The connection name passed to the `connection` method should correspond to one of the connections key listed in your configuration:

    #include <orm/db.hpp>

    auto [ok, query] = DB::connection("mysql_test").select(...);

You may access the raw underlying `QSqlQuery` instance of a connection using the `getQtQuery` method on a connection instance:

    auto query = DB::connection().getQtQuery();

Or you can use the shortcut method `qtQuery` provided by the `DB` facade:

    auto query = DB::qtQuery();

<a name="database-transactions"></a>
## Database Transactions

<a name="manually-using-transactions"></a>
#### Manually Using Transactions

If you would like to begin a transaction manually and have complete control over rollbacks and commits, you may use the `beginTransaction` method provided by the `DB` facade:

    #include <orm/db.hpp>

    DB::beginTransaction();

You can rollback the transaction via the `rollBack` method:

    DB::rollBack();

Lastly, you can commit a transaction via the `commit` method:

    DB::commit();

All transaction methods accept a connection name as the optional argument:

    DB::beginTransaction("mysql_test");

> {tip} The `DB` facade's transaction methods control the transactions for both the [query builder](/docs/{{version}}/query-builder) and [TinyORM](/docs/{{version}}/tinyorm).
