# TinyORM: Getting Started

- [Introduction](#introduction)
- [TinyORM Model Conventions](#tinyorm-model-conventions)
    - [Table Names](#table-names)
    - [Primary Keys](#primary-keys)
    - [Timestamps](#timestamps)
    - [Database Connections](#database-connections)
- [Retrieving Models](#retrieving-models)
    - [Containers](#containers)
- [Retrieving Single Models](#retrieving-single-models)
    - [Retrieving Or Creating Models](#retrieving-or-creating-models)
- [Inserting & Updating Models](#inserting-and-updating-models)
    - [Inserts](#inserts)
    - [Updates](#updates)
- [Comparing Models](#comparing-models)

<a name="introduction"></a>
## Introduction

TinyORM is an object-relational mapper (ORM) that makes it enjoyable to interact with your database. When using TinyORM, each database table has a corresponding "Model" that is used to interact with that table. In addition to retrieving records from the database table, TinyORM models allow you to insert, update, and delete records from the table as well.

> {tip} Before getting started, be sure to configure a database connection in your application's `config/database.php` configuration file. For more information on configuring your database, check out [the database configuration documentation](/docs/{{version}}/database#configuration).

<a name="tinyorm-model-conventions"></a>
## TinyORM Model Conventions

Let's examine a basic model class and discuss some of TinyORM's key conventions:

    #ifndef FLIGHT_H
    #define FLIGHT_H

    #include "orm/tiny/basemodel.hpp"

    class Flight final : public Orm::Tiny::BaseModel<Flight>
    {
    public:
        friend class BaseModel;

        using BaseModel::BaseModel;
    };

    #endif // FLIGHT_H

<a name="table-names"></a>
### Table Names

After glancing at the example above, you may have noticed that we did not tell TinyORM which database table corresponds to our `Flight` model. By convention, the "snake case", plural name of the class will be used as the table name unless another name is explicitly specified. So, in this case, TinyOrm will assume the `Flight` model stores records in the `flights` table, while an `AirTrafficOperator` model would store records in an `air_traffic_operators` table.

If your model's corresponding database table does not fit this convention, you may manually specify the model's table name by defining the private `u_table` data member on the model:

    #include "orm/tiny/basemodel.hpp"

    class Flight final : public Orm::Tiny::BaseModel<Flight>
    {
    public:
        friend class BaseModel;
        using BaseModel::BaseModel;

    private:
        /*! The table associated with the model. */
        QString u_table {"torrents"};
    };

<a name="primary-keys"></a>
### Primary Keys

TinyORM will also assume that each model's corresponding database table has a primary key column named `id`. If necessary, you may define a private `u_primaryKey` data member on your model to specify a different column that serves as your model's primary key:

    #include "orm/tiny/basemodel.hpp"

    class Flight final : public Orm::Tiny::BaseModel<Flight>
    {
    public:
        friend class BaseModel;
        using BaseModel::BaseModel;

    private:
        /*! The primary key associated with the table. */
        QString u_primaryKey {"id"};
    };

In addition, TinyORM assumes that the primary key is an incrementing integer value. If you wish to use a non-incrementing or a non-numeric primary key you must define a private `u_incrementing` data member on your model that is set to `false`:

    #include "orm/tiny/basemodel.hpp"

    class Flight final : public Orm::Tiny::BaseModel<Flight>
    {
    public:
        friend class BaseModel;
        using BaseModel::BaseModel;

    private:
        /*! Indicates if the model's ID is auto-incrementing. */
        bool u_incrementing = false;
    };

<a name="composite-primary-keys"></a>
#### "Composite" Primary Keys

TinyOrm requires each model to have at least one uniquely identifying "ID" that can serve as its primary key. "Composite" primary keys are not supported by TinyOrm models. However, you are free to add additional multi-column unique indexes to your database tables, in addition to the table's uniquely identifying primary key.

<a name="timestamps"></a>
### Timestamps

By default, TinyOrm expects `created_at` and `updated_at` columns to exist on your model's corresponding database table.  TinyOrm will automatically set these column's values when models are created or updated. If you do not want these columns to be automatically managed by TinyOrm, you should define a private `u_timestamps` data member on your model with a value of `false`:

    #include "orm/tiny/basemodel.hpp"

    class Flight final : public Orm::Tiny::BaseModel<Flight>
    {
    public:
        friend class BaseModel;
        using BaseModel::BaseModel;

    private:
        /*! Indicates if the model should be timestamped. */
        bool u_timestamps = false;
    };

If you need to customize the format of your model's timestamps, set the private `u_dateFormat` data member on your model. This data member determines how date attributes are stored in the database, supported formats are described in the `QDateTime` documentation:

    #include "orm/tiny/basemodel.hpp"

    class Flight final : public Orm::Tiny::BaseModel<Flight>
    {
    public:
        friend class BaseModel;
        using BaseModel::BaseModel;

    private:
        /*! The storage format of the model's date columns. */
        QString u_dateFormat {"yyyy-MM-dd HH:mm:ss"};
    };

If you need to customize the names of the columns used to store the timestamps, you may define `CREATED_AT` and `UPDATED_AT` private static constants on your model:

    #include "orm/tiny/basemodel.hpp"

    class Flight final : public Orm::Tiny::BaseModel<Flight>
    {
    public:
        friend class BaseModel;
        using BaseModel::BaseModel;

    private:
        /*! The name of the "created at" column. */
        inline static const QString CREATED_AT = QStringLiteral("created_at");
        /*! The name of the "updated at" column. */
        inline static const QString UPDATED_AT = QStringLiteral("updated_at");
    };

<a name="database-connections"></a>
### Database Connections

By default, all TinyOrm models will use the default database connection that is configured for your application. If you would like to specify a different connection that should be used when interacting with a particular model, you should define a `u_connection` private data member on the model:

    #include "orm/tiny/basemodel.hpp"

    class Flight final : public Orm::Tiny::BaseModel<Flight>
    {
    public:
        friend class BaseModel;
        using BaseModel::BaseModel;

    private:
        /*! The database connection that should be used by the model. */
        QString u_connection {"sqlite"};
    };

<a name="retrieving-models"></a>
## Retrieving Models

Once you have created a model and its associated database table, you are ready to start retrieving data from your database. You can think of each TinyOrm model as a powerful [query builder](/docs/{{version}}/query-builder) allowing you to fluently query the database table associated with the model. The model's `all` method will retrieve all of the records from the model's associated database table:

    #include "models/flight.hpp"

    for (const auto &flight : Flight::all())
        qDebug() << flight["name"].toString();

<a name="building-queries"></a>
#### Building Queries

The TinyOrm `all` method will return all of the results in the model's table. However, since each TinyOrm model serves as a [query builder](/docs/{{version}}/query-builder), you may add additional constraints to queries and then invoke the `get` method to retrieve the results:

    auto flights = Flight::whereEq("active", 1)
               ->orderBy("name")
               .take(10)
               .get();

> {tip} Since TinyOrm models are query builders, you should review all of the methods provided by TinyORM's [query builder](/docs/{{version}}/query-builder). You may use any of these methods when writing your TinyOrm queries.

> {note} All the static methods defined on the `BaseModel` class, which start building queries like `where`, `latest`, `oldest`, `with`, ... return `std::unique_ptr<TinyBuilder<Model>>`, `TinyBuilder = Orm::Tiny::Builder` and `Model` template argument is queried model class.

<a name="refreshing-models"></a>
#### Refreshing Models

If you already have an instance of an TinyOrm model that was retrieved from the database, you can "refresh" the model using the `fresh` and `refresh` methods. The `fresh` method will re-retrieve the model from the database. The existing model instance will not be affected:

    auto flight = Flight::whereEq("number", "FR 900")->first();

    auto freshFlight = flight->fresh();

The `refresh` method will re-hydrate the existing model using fresh data from the database. In addition, all of its loaded relationships will be refreshed as well:

    auto flight = Flight::whereEq("number", "FR 900")->first();

    flight->setAttribute("number", "FR 456");

    flight->refresh();

    flight->getAttribute("number"); // "FR 900"

<a name="containers"></a>
### Containers

As we have seen, TinyORM methods like `all` and `get` retrieve multiple records from the database. Since these methods return a `QVector<Model>`, so you can iterate it like any other container with the [Range-based for loop](https://en.cppreference.com/w/cpp/language/range-for), [STL-Style Iterators](https://doc.qt.io/qt-5/containers.html#stl-style-iterators), [Java-Style Iterators](https://doc.qt.io/qt-5/containers.html#java-style-iterators) or [Ranges](https://www.walletfox.com/course/quickref_range_v3.php).

    #include "models/flight.hpp"

    for (const auto &flight : Flight::all())
        qDebug() << flight["name"].toString();

> {note} `all` method is defined on the `Orm::Tiny::BaseModel` class and `get` method is defined on the `Orm::Tiny::Builder`, may be also referred as `TinyBuilder`, it extends `Orm::Query::Builder` alias `QueryBuilder`.

<a name="retrieving-single-models"></a>
## Retrieving Single Models

In addition to retrieving all of the records matching a given query, you may also retrieve single records using the `find`, `first`, `firstWhere`, or `firstWhereEq` methods. Instead of returning a collection of models, these methods return a single model instance:

    #include "models/flight.hpp"

    // Retrieve a model by its primary key...
    auto flight = Flight::find(1);

    // Retrieve the first model matching the query constraints...
    auto flight = Flight::whereEq("active", 1)->first();

    // Alternative to retrieving the first model matching the query constraints...
    auto flight = Flight::firstWhere("active", "=", 1);

    // Alternative where method syntax
    auto flight = Flight::firstWhereEq("active", 1);

<a name="not-found-exceptions"></a>
#### Not Found Exceptions

Sometimes you may wish to throw an exception if a model is not found. The `findOrFail` and `firstOrFail` methods will retrieve the first result of the query; however, if no result is found, an `Orm::Tiny::ModelNotFoundError` will be thrown:

    auto flight = Flight::findOrFail(1);

    auto flight = Flight::where("legs", ">", 3)->firstOrFail();

<a name="retrieving-or-creating-models"></a>
### Retrieving Or Creating Models

The `firstOrCreate` method will attempt to locate a database record using the given column / value pairs. If the model can not be found in the database, a record will be inserted with the attributes resulting from merging the first `QVector<WhereItem>` argument with the optional second `QVector<AttributeItem>` argument:

The `firstOrNew` method, like `firstOrCreate`, will attempt to locate a record in the database matching the given attributes. However, if a model is not found, a new model instance will be returned. Note that the model returned by `firstOrNew` has not yet been persisted to the database. You will need to manually call the `save` method to persist it:

    #include "models/flight.hpp"

    // Retrieve flight by name or create it if it doesn't exist...
    auto flight = Flight::firstOrCreate({
        {"name", "London to Paris"}
    });

    // Retrieve flight by name or create it with the name, delayed, and arrival_time attributes...
    auto flight = Flight::firstOrCreate(
        {{"name", "London to Paris"}},
        {{"delayed", 1}, {"arrival_time", "11:30"}}
    );

    // Retrieve flight by name or instantiate a new Flight instance...
    auto flight = Flight::firstOrNew({
        {"name", "London to Paris"}
    });

    // Retrieve flight by name or instantiate with the name, delayed, and arrival_time attributes...
    auto flight = Flight::firstOrNew(
        {{"name", "Tokyo to Sydney"}},
        {{"delayed", 1}, {"arrival_time", "11:30"}}
    );

<a name="inserting-and-updating-models"></a>
## Inserting & Updating Models

<a name="inserts"></a>
### Inserts

Of course, when using TinyOrm, we don't only need to retrieve models from the database. We also need to insert new records. Thankfully, TinyOrm makes it simple. To insert a new record into the database, you should instantiate a new model instance and set attributes on the model. Then, call the `save` method on the model instance:

    #include "models/flight.hpp"

    // Store a new flight in the database
    Flight flight;
    flight.setAttribute("name", "Slovakia to Czech");
    flight.save();

In this example, we assign the `name` attribute of the `Flight` model instance. When we call the `save` method, a record will be inserted into the database. The model's `created_at` and `updated_at` timestamps will automatically be set when the `save` method is called, so there is no need to set them manually.

Alternatively, you may use the `create` method to "save" a new model using a single c++ statement. The inserted model instance will be returned to you by the `create` method:

    #include "models/flight.hpp"

    auto flight = Flight::create({
        {"name", "London to Paris"},
    });

<a name="updates"></a>
### Updates

The `save` method may also be used to update models that already exist in the database. To update a model, you should retrieve it and set any attributes you wish to update. Then, you should call the model's `save` method. Again, the `updated_at` timestamp will automatically be updated, so there is no need to manually set its value:

    #include "models/flight.hpp"

    auto flight = Flight::find(1);

    flight->setAttribute("name", "Paris to London");

    flight->save();

<a name="mass-updates"></a>
#### Mass Updates

Updates can also be performed against models that match a given query. In this example, all flights that are `active` and have a `destination` of `San Diego` will be marked as delayed:

    Flight::whereEq("active", 1)
          ->whereEq("destination", "San Diego")
          .update({{"delayed", 1}});

The `update` method expects the `QVector<UpdateItem>` of column and value pairs representing the columns that should be updated.

<a name="examining-attribute-changes"></a>
#### Examining Attribute Changes

TinyOrm provides the `isDirty`, `isClean`, and `wasChanged` methods to examine the internal state of your model and determine how its attributes have changed from when the model was originally retrieved.

The `isDirty` method determines if any of the model's attributes have been changed since the model was retrieved. You may pass a specific attribute name to the `isDirty` method to determine if a particular attribute is dirty. The `isClean` will determine if an attribute has remained unchanged since the model was retrieved. This method also accepts an optional attribute argument:

    #include "models/user.hpp"

    auto user = User::create({
        {"first_name", "Silver"},
        {"last_name", "Zachara"},
        {"title", "Developer"},
    });

    user.setAttribute("title", "Painter");

    user.isDirty(); // true
    user.isDirty("title"); // true
    user.isDirty("first_name"); // false

    user.isClean(); // false
    user.isClean("title"); // false
    user.isClean("first_name"); // true

    user.save();

    user.isDirty(); // false
    user.isClean(); // true

The `wasChanged` method determines if any attributes were changed when the model was last saved into the database. If needed, you may pass an attribute name to see if a particular attribute was changed:

    auto user = User::create({
        {"first_name", "Silver"},
        {"last_name", "Zachara"},
        {"title", "Developer"},
    });

    user.setAttribute("title", "Painter");

    user.wasChanged(); // false

    user.save();

    user.wasChanged(); // true
    user.wasChanged("title"); // true
    user.wasChanged("first_name"); // false

<a name="comparing-models"></a>
## Comparing Models

Sometimes you may need to determine if two models are the "same". The `is` method may be used to quickly verify two models have the same primary key, table, and database connection:

    if (post.is(anotherPost)) {
        //
    }
