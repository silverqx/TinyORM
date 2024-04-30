# TinyORM vcpkg ports

The TinyORM vcpkg ports are officially committed to the vcpkg package manager. You can install them using the following commands.

The `Qt v5` port isn't commit to the official vcpkg repository.

The `Qt latest` port differs from the port in the official vcpkg repository, it additionally contains the `build-mysql-driver` feature which isn't allowed and can't be committed to the official vcpkg repository as it violates the [Maintainer guide: Do not use features to implement alternatives](https://learn.microsoft.com/en-us/vcpkg/contributing/maintainer-guide#do-not-use-features-to-implement-alternatives).

These ports are supposed to be used as [overlay ports](https://learn.microsoft.com/en-us/vcpkg/concepts/overlay-ports) eg. using the `VCPKG_OVERLAY_PORTS` environment variable.

```
VCPKG_OVERLAY_PORTS=C:\Users\<username>\AppData\Roaming\vcpkg\ports
```

#### Qt latest (TinyDrivers)

Currently, only the MySQL database driver is supported with `TinyDrivers`. It drops the `QtSql` dependency.

vcpkg install tinyorm[core,build-mysql-driver] qtbase[core] libmysql --dry-run

vcpkg install tinyorm[core,build-mysql-driver,tom-example]

#### Qt latest (QtSql)

vcpkg install tinyorm[core] qtbase[core] --dry-run
vcpkg install tinyorm[core,mysql] qtbase[core,sql-mysql] --dry-run
vcpkg install tinyorm[core,sqlite] qtbase[core,sql-sqlite] --dry-run

vcpkg install tinyorm
vcpkg install tinyorm[core,tom-example]
vcpkg install tinyorm[core,sql-psql]

#### Qt v5

vcpkg install tinyorm-qt5[core] qt5-base --dry-run
vcpkg install tinyorm-qt5[core] qt5-base[core,sql-sqlite] --dry-run

vcpkg install tinyorm-qt5
vcpkg install tinyorm-qt5[core,tom-example]
vcpkg install tinyorm-qt5[core,sql-psql]
