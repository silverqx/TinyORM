<h1><img src="https://github.com/silverqx/TinyORM/blob/main/resources/icons/logo-optim.svg" width="34" height="34" alt="TinyORM Logo" align="center">&nbsp;TinyORM</h1>

[![MSVC2019 Qt5.15][action-msvc2019-qt5-badge]][action-msvc2019-qt5]
[![MSVC2022 Qt6.2][action-msvc2022-qt6-badge]][action-msvc2022-qt6]
[![Linux GCC/Clang Qt5.15][action-linux-badge-qt5]][action-linux-qt5]
[![Linux GCC/Clang Qt6.2][action-linux-badge-qt6]][action-linux-qt6]
[![MSYS2 UCRT64 GCC/Clang][action-mingw-badge]][action-mingw]
[![clang-cl MSVC2022][clang-cl-badge]][clang-cl]
[![Clang Tidy/Clazy Qt5.15][action-analyzers-qt5-badge]][action-analyzers-qt5]
[![Clang Tidy/Clazy Qt6.2][action-analyzers-qt6-badge]][action-analyzers-qt6]

[![www.tinyorm.org][docs-badge]][docs]
[![License MIT][license-badge]][license]
![TinyORM v0.9.3][version-tinyorm-badge]
![tom v0.4.1][version-tom-badge]

TinyORM is a modern ORM library that makes interacting with a database extremely simple.

The code is written in the modern c++20 way and is well tested with the unit and functional tests. Almost all the query builder methods are unit tested. The TinyORM's query builder code and the code which is responsible for obtaining relationships, is tested by functional tests against all supported databases. The code coverage is good enough to guarantee API and behavior compatibility.

## Documentation

Whole library is documented as markdown documents:

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
- [Building](/docs/building#building)
  - [TinyORM](/docs/building/tinyorm.mdx#building-tinyorm)
  - [Hello world](/docs/building/hello-world.mdx#building-hello-world)
  - [Migrations](/docs/building/migrations.mdx#building-migrations)

[action-msvc2019-qt5]: https://github.com/silverqx/TinyORM/actions/workflows/msvc2019-qt5.yml
[action-msvc2019-qt5-badge]: https://github.com/silverqx/TinyORM/actions/workflows/msvc2019-qt5.yml/badge.svg
[action-msvc2022-qt6]: https://github.com/silverqx/TinyORM/actions/workflows/msvc2022-qt6.yml
[action-msvc2022-qt6-badge]: https://github.com/silverqx/TinyORM/actions/workflows/msvc2022-qt6.yml/badge.svg
[action-linux-qt5]: https://github.com/silverqx/TinyORM/actions/workflows/linux-qt5.yml
[action-linux-badge-qt5]: https://github.com/silverqx/TinyORM/actions/workflows/linux-qt5.yml/badge.svg
[action-linux-qt6]: https://github.com/silverqx/TinyORM/actions/workflows/linux-qt6.yml
[action-linux-badge-qt6]: https://github.com/silverqx/TinyORM/actions/workflows/linux-qt6.yml/badge.svg
[action-mingw]: https://github.com/silverqx/TinyORM/actions/workflows/mingw.yml
[action-mingw-badge]: https://github.com/silverqx/TinyORM/actions/workflows/mingw.yml/badge.svg
[clang-cl]: https://github.com/silverqx/TinyORM/actions/workflows/clang-cl.yml
[clang-cl-badge]: https://github.com/silverqx/TinyORM/actions/workflows/clang-cl.yml/badge.svg
[action-analyzers-qt5]: https://github.com/silverqx/TinyORM/actions/workflows/analyzers-qt5.yml
[action-analyzers-qt5-badge]: https://github.com/silverqx/TinyORM/actions/workflows/analyzers-qt5.yml/badge.svg
[action-analyzers-qt6]: https://github.com/silverqx/TinyORM/actions/workflows/analyzers-qt6.yml
[action-analyzers-qt6-badge]: https://github.com/silverqx/TinyORM/actions/workflows/analyzers-qt6.yml/badge.svg
[docs-badge]: https://img.shields.io/badge/Docs-www.tinyorm.org-blue
[docs]: https://www.tinyorm.org
[license-badge]: https://img.shields.io/github/license/silverqx/TinyORM
[license]: https://github.com/silverqx/TinyORM/blob/main/LICENSE
[version-tinyorm-badge]: https://img.shields.io/badge/TinyORM-v0.9.3-blue
[version-tom-badge]: https://img.shields.io/badge/tom-v0.4.1-blue
