# TinyORM vcpkg ports

The TinyORM vcpkg ports are officially committed to the vcpkg package manager. You can install them using the following commands.

#### Qt latest

vcpkg install tinyorm[core] qtbase[core,sql] --dry-run
vcpkg install tinyorm[core] qtbase[core,sql-sqlite] --dry-run

vcpkg install tinyorm
vcpkg install tinyorm[core,tom-example]
vcpkg install tinyorm[core,sql-psql]

#### Qt 5

vcpkg install tinyorm-qt5[core] qt5-base --dry-run
vcpkg install tinyorm-qt5[core] qt5-base[core,sql-sqlite] --dry-run

vcpkg install tinyorm-qt5
vcpkg install tinyorm-qt5[core,tom-example]
vcpkg install tinyorm-qt5[core,sql-psql]
