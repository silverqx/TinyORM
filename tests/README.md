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
