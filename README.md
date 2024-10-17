<h1><img src="https://github.com/silverqx/TinyORM/blob/main/resources/icons/logo-optim.svg" width="34" height="34" alt="TinyORM Logo" align="center">&nbsp;TinyORM</h1>

[![MSVC2022 Qt6.7][action-msvc2022-qt6-badge]][action-msvc2022-qt6]
[![Linux GCC/Clang Qt6.4][action-linux-qt6-badge]][action-linux-qt6]
[![MSYS2 UCRT64 GCC/Clang][action-mingw-badge]][action-mingw]
[![clang-cl MSVC2022][action-clang-cl-badge]][action-clang-cl]
[![Clang-Tidy/Clazy 6.7][action-analyzers-qt6-badge]][action-analyzers-qt6]
[![Vcpkg Windows][action-vcpkg-windows-badge]][action-vcpkg-windows]
[![Vcpkg Linux][action-vcpkg-linux-badge]][action-vcpkg-linux]
[![MSVC2022 Qt6.7 TinyDrivers][action-msvc2022-qt6-drivers-badge]][action-msvc2022-qt6-drivers]
[![Linux GCC/Clang Qt6.7 TinyDrivers][action-linux-qt6-drivers-badge]][action-linux-qt6-drivers]

[![TinyORM - www.tinyorm.org][docs-badge]][docs]
[![License MIT][license-badge]][license]<br/>
[![TinyORM v0.38.1][version-tinyorm-badge]][docs]
[![tom v0.10.0][version-tom-badge]][docs-tom]
[![TinyDrivers v0.2.0][version-tinydrivers-badge]][docs-tinydrivers]
[![TinyMySql v0.2.0][version-tinymysql-badge]][docs-tinydrivers]

TinyORM is a modern ORM library that makes interacting with a database extremely simple.

The code is written in the modern c++20 way and is __heavily__ tested with __3378__ unit and functional tests. Almost all the query builder methods are unit tested. The TinyORM's query builder code and the code which is responsible for obtaining relationships, is tested by functional tests against all supported databases. The code coverage is good enough to guarantee API and behavior compatibility.

## Donations 🪙

Bitcoin address&nbsp;&nbsp;1NiF2cTvYxUj8FTZJnGn1ycN4yisWfo1vJ<br/>
PayPal&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;https://paypal.me/silverzachara

## Documentation

The whole library is documented as markdown documents:

[![www.tinyorm.org][docs-badge]][docs]

- [Prologue](/docs/README.mdx#prologue)
- [Dependencies](/docs/dependencies.mdx#dependencies)
- [Supported Compilers](/docs/supported-compilers.mdx#supported-compilers)
- [Database](/docs/database#database)
  - [Getting Started](/docs/database/getting-started.mdx#database-getting-started)
  - [Query Builder](/docs/database/query-builder.mdx#database-query-builder)
  - [Migrations](/docs/database/migrations.mdx#database-migrations)
  - [Seeding](/docs/database/seeding.mdx#database-seeding)
- [TinyORM](/docs/tinyorm#tinyorm)
  - [Getting Started](/docs/tinyorm/getting-started.mdx#tinyorm-getting-started)
  - [Relationships](/docs/tinyorm/relationships.mdx#tinyorm-relationships)
  - [Collections](/docs/tinyorm/collections.mdx#tinyorm-collections)
  - [Casts](/docs/tinyorm/casts.mdx#tinyorm-casting)
  - [Serialization](/docs/tinyorm/serialization.mdx#tinyorm-serialization)
- [TinyDrivers](/docs/tinydrivers#tinydrivers)
  - [Getting Started](/docs/tinydrivers/getting-started.mdx#tinydrivers-getting-started)
- [Building](/docs/building#building)
  - [TinyORM](/docs/building/tinyorm.mdx#building-tinyorm)
  - [Hello world](/docs/building/hello-world.mdx#building-hello-world)
  - [Migrations](/docs/building/migrations.mdx#building-migrations)
- [Stability](/docs/stability.mdx#stability)
- [Features Summary](/docs/features-summary.mdx#features-summary)
- [Donations](/docs/donations.mdx#donations)

## Features Summary

- [Summary List](#summary-list)
- [Showcase Images](#showcase-images)

### Summary List

The following list summarizes the main features of the `TinyDrivers` and `TinyMySql` libraries. 🆕

 - can be used instead of the `QtSql` module
   - can be <ins>__swapped__</ins> at compile time 🤯
   - has __1:1__ API as the `QtSql` module 😮
   - swapping is controlled by the `qmake` and `CMake` build system options
   - was designed to drop the `QtSql` dependency while maintaining backward compatibility and without the need for any code changes after the swap
 - currently, only the `MySQL` database driver is supported and finished 😞
   - this is only true for our `TinyDrivers` library ❗
   - all database drivers described in the [Database](https://www.tinyorm.org/database/getting-started#introduction) documentation are supported when linking against the [`QtSql`](https://doc.qt.io/qt/qtsql-index.html) module
 - both, normal and prepared statements are supported
 - TLS/SSL connections using [`MYSQL_OPT_SSL_MODE`](https://dev.mysql.com/doc/c-api/9.1/en/mysql-options.html) (verify_ca, verify_identity) 🔥
 - setting many other MySQL connection options (see [`mysqldriver_p.cpp`](https://github.com/silverqx/TinyORM/blob/main/drivers/mysql/src/orm/drivers/mysql/mysqldriver_p.cpp))
 - building and linking against the [`MariaDB Connector/C`](https://mariadb.com/kb/en/mariadb-connector-c/) 🕺
 - transactions
 - re-using the current `SqlQuery` instance to re-execute the same or another SQL query
 - detaching from the result set (related to freeing/releasing memory)
 - query size, number of affected rows, last inserted ID, testing `isNull()`, ...
 - `TinyDrivers` library supports both build systems `qmake` and also `CMake`
 - all __3378 unit tests__ passed 😮
 - strictly using __smart pointers__ (no `new` keyword in the whole `TinyDrivers` code base 😎)
 - clear code 🤔

The following list quickly summarizes all the `TinyORM` features.

- __simple database connections management__ 🧬
  - database manager that helps with the database connections management
  - `Orm::DB` facade class for nicer and shorter syntax
  - MySQL, MariaDB, SQLite, and PostgreSQL support for __all__ features 💎
  - multi-threading support 👀
  - __SSL__ connections support 🔒
- __impressive query builder__ 🔧
  - allows passing sub-queries and raw expressions practically everywhere, to column names, values, and to every SQL clause as select, where, joins, group by, having, order by 🔥
  - a logical grouping that offers to wrap logical groups in parenthesis
  - chunked results for lower memory footprint ✨
  - raw methods for all SQL clauses
  - all join types (left, right, cross, inner) and also join where clause support 🫤
  - aggregate methods min, max, sum, increment, decrement, ...
  - whereExists and exists methods for an existence queries
  - transactions and pessimistic locking 🔒
  - of course, insert, update, and delete SQL clauses support
  - __correct QDateTime time zone__ using the `qt_timezone` connection configuration option 📅 (returned QDateTime instances will have the correct time zone, and also works for an ORM)
    - this feature allows you to set up the database server time zone to the UTC and all returned QDateTime instances will have the correct UTC time zone
- __clever ORM with all relation types support__ 🎉
  - one-to-one, one-to-many, and many-to-many relation types (also inverse relationships) 🧨
  - eager and lazy loading with custom select and constraints 🚀
  - fluent `ModelsCollection` that expose a variety of map / reduce operations that may be chained using an intuitive interface ✨
  - all query builder methods are proxied from the model instances and also from the relation instances back to the query builder 🤯 (everything that can be called on the query builder can also be called on the model and relation instances)
  - clean active record pattern
  - advanced features like timestamps, touching parent timestamps, __soft deleting__, default models, default model attributes, and attributes casting 🤓
  - querying relationships existence/absence using the has, whereHas, and hasNested methods (using dot notation for selecting nested relationships _users.posts.comments_)
  - __serializing__ models and collection of models including all nested relations to __JSON__ and converting to vectors and maps 🪡
    - supports controlling a custom date format during serialization
    - supports hiding and appending attributes
    - supports defining __Accessors__ (transform attribute value when it is accessed; eg. building value from multiple attributes)
- __compiled database migrations and seeders__ 🕺
  - create, update, drop, and rename database tables
  - create, __update__, drop, and rename table columns
  - extensive schema builder that allows creating of all possible column types
  - terser syntax for creating foreign keys and foreign key constraints
  - supports creating and dropping column indexes (primary, unique, fulltext, spatial)
- __the `tom` console application with tab completion for all shells (pwsh, bash, zsh)__ 🥳
  - tab completion is as accurate as possible
    - completes commands, long and short parameters, argument values
    - even connection names can be completed
  - migrate, fresh, refresh, rollback, and reset migrate operations
  - install and uninstall migration repository (table that tracks migrations and batches; table name can be customized)
  - seed and wipe database operations
  - scaffolding of models, migrations, and seeders
  - impressive models scaffolding, every feature that is supported by models can be generated using the `tom make:model` cli command
- __others__ 🛝
  - a huge amount of code is unit tested, currently __3378 unit tests__ 🤯
  - C++20 only, with all the latest features used like concepts/constraints, ranges, __smart pointers__ (no `new` keyword in the whole code base 😎), folding expressions
  - qmake and CMake build systems support
    - CMake __FetchContent__ module support 🤙
    - __LTO__ support (backed up by CI)
  - vcpkg support (official [tinyorm](https://github.com/microsoft/vcpkg/tree/master/ports/tinyorm) vcpkg port)
    - official port doesn't allow to have alternative implementations so it doesn't contain __TinyDrivers__
    - unofficial [tinyorm](https://github.com/silverqx/TinyORM/tree/main/cmake/vcpkg/ports/tinyorm) vcpkg port with __TinyDrivers__ support (currently MySQL only)
  - it's really fast, you can run 1000 complex queries in 500ms (heavily DB dependent, the PostgreSQL is by far the fastest) ⌚
  - extensive documentation 📃
  - ...

:::info
See the `TinyDrivers` [Features summary](tinydrivers/getting-started#features-summary).
:::

### Showcase Images

###### Tom console application
![Tom console application](/docs/database/assets/img/migrations/tom_cli.png)

###### Passed all unit tests 🥳
![Passed all unit tests](/docs/assets/img/features-summary/tinyorm-passed_all_unit_tests.png)

#### TinyOrmPlayground

The [TinyOrmPlayground](https://github.com/silverqx/TinyOrmPlayground) project is my personal project where I have tested all the TinyORM database queries in the early development phases, currently, it executes ~1600 database queries across the whole TinyORM framework. Every query has a nice title header, is logged to the console, and is counted and measured (elapsed time). Every query also runs on all [Supported Databases](https://www.tinyorm.org/database/getting-started#introduction).

The TinyOrmPlayground project can be compiled in a single-threaded or multi-threaded mode. In the multi-threaded mode, every database connection runs in its own thread. The connection summary is logged at the end of every database connection and the application summary is logged before an exit. Whole TinyOrmPlayground application is configurable through the [src/configuration.hpp](https://github.com/silverqx/TinyOrmPlayground/blob/main/src/configuration.hpp) class.

###### TinyOrmPlayground single-threaded
![Invoked TinyOrmPlayground single-threaded](/docs/assets/img/features-summary/tinyormplayground-single-threaded.png)

###### TinyOrmPlayground multi-threaded
![Invoked TinyOrmPlayground multi-threaded](/docs/assets/img/features-summary/tinyormplayground-multi-threaded.png)

[action-msvc2022-qt6]: https://github.com/silverqx/TinyORM/actions/workflows/msvc2022-qt6.yml
[action-msvc2022-qt6-badge]: https://github.com/silverqx/TinyORM/actions/workflows/msvc2022-qt6.yml/badge.svg
[action-linux-qt6]: https://github.com/silverqx/TinyORM/actions/workflows/linux-qt6.yml
[action-linux-qt6-badge]: https://github.com/silverqx/TinyORM/actions/workflows/linux-qt6.yml/badge.svg
[action-mingw]: https://github.com/silverqx/TinyORM/actions/workflows/msys2-ucrt64.yml
[action-mingw-badge]: https://github.com/silverqx/TinyORM/actions/workflows/msys2-ucrt64.yml/badge.svg
[action-clang-cl]: https://github.com/silverqx/TinyORM/actions/workflows/clang-cl-qt6.yml
[action-clang-cl-badge]: https://github.com/silverqx/TinyORM/actions/workflows/clang-cl-qt6.yml/badge.svg
[action-analyzers-qt6]: https://github.com/silverqx/TinyORM/actions/workflows/analyzers.yml
[action-analyzers-qt6-badge]: https://github.com/silverqx/TinyORM/actions/workflows/analyzers.yml/badge.svg
[action-vcpkg-windows]: https://github.com/silverqx/TinyORM/actions/workflows/vcpkg-windows.yml
[action-vcpkg-windows-badge]: https://github.com/silverqx/TinyORM/actions/workflows/vcpkg-windows.yml/badge.svg
[action-vcpkg-linux]: https://github.com/silverqx/TinyORM/actions/workflows/vcpkg-linux.yml
[action-vcpkg-linux-badge]: https://github.com/silverqx/TinyORM/actions/workflows/vcpkg-linux.yml/badge.svg
[action-msvc2022-qt6-drivers]: https://github.com/silverqx/TinyORM/actions/workflows/msvc2022-qt6-drivers.yml
[action-msvc2022-qt6-drivers-badge]: https://github.com/silverqx/TinyORM/actions/workflows/msvc2022-qt6-drivers.yml/badge.svg
[action-linux-qt6-drivers]: https://github.com/silverqx/TinyORM/actions/workflows/linux-qt6-drivers.yml
[action-linux-qt6-drivers-badge]: https://github.com/silverqx/TinyORM/actions/workflows/linux-qt6-drivers.yml/badge.svg
[docs-badge]: https://img.shields.io/badge/Docs-www.tinyorm.org-blue
[docs]: https://www.tinyorm.org
[docs-tom]: https://www.tinyorm.org/database/migrations
[docs-tinydrivers]: https://www.tinyorm.org/tinydrivers/getting-started
[license-badge]: https://img.shields.io/github/license/silverqx/TinyORM
[license]: https://github.com/silverqx/TinyORM/blob/main/LICENSE
[version-tinyorm-badge]: https://img.shields.io/badge/TinyORM-v0.38.1-blue
[version-tom-badge]: https://img.shields.io/badge/tom-v0.10.0-blue
[version-tinydrivers-badge]: https://img.shields.io/badge/TinyDrivers-v0.2.0-blue
[version-tinymysql-badge]: https://img.shields.io/badge/TinyMySql-v0.2.0-blue
