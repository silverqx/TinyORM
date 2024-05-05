# TinyDrivers

The `TinyDrivers` library is an underlying SQL database layer for `TinyORM`. It can be used instead of the `QtSql` module, can be <u>__swapped__</u> at compile time, and has __1:1__ API as the `QtSql` module. 😮 Swapping is controlled by the `qmake` and `CMake` build system options.

It was designed to drop the `QtSql` dependency while maintaining backward compatibility and without the need for any code changes after the swap.

- [Getting Started](getting-started.mdx#tinydrivers-getting-started)
