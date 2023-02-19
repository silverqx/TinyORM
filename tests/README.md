# Qt Auto Tests

### Requirements

The `tst_PostgreSQL_Connection::searchpath_Undefined_PostgreSQL()` test method hardly depends on the following PostgreSQL configuration (database server configuration, not connection configuration in the TinyUtils::Databases class):

```
search_path = "$user", public
```

The `tst_SchemaBuilder::createTable_Comment()` test method hardly depends on the following PostgreSQL connection configuration (in the TinyUtils::Databases class):

```
search_path = public (set by the env. variable DB_PGSQL_SEARCHPATH = public)
```

### Notes

The `tst_Migrate` is not testing the Qt 5 `QSQLITE` driver because it doesn't support `ALTER TABLE DROP COLUMN`, support for dropping columns was added in the SQLite v3.35.0 as is described in the [release notes](https://www.sqlite.org/releaselog/3_35_0.html).
