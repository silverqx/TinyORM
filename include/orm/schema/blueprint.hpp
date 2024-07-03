#pragma once
#ifndef ORM_SCHEMA_BLUEPRINT_HPP
#define ORM_SCHEMA_BLUEPRINT_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <optional>

#include "orm/ormconcepts.hpp"
#include "orm/schema/foreignidcolumndefinitionreference.hpp"
#include "orm/schema/indexdefinitionreference.hpp"
#include "orm/schema/schemaconstants.hpp"
#ifndef TINYORM_DISABLE_ORM
#  include "orm/tiny/tinytypes.hpp" // IWYU pragma: keep
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class DatabaseConnection;

namespace SchemaNs
{
namespace Grammars
{
    class SchemaGrammar;
}

    /*! Concept for the command definition. */
    template<typename T>
    concept CommandDefinitionConcept = std::derived_from<T, CommandDefinition>;

    /*! Table blueprint for the schema grammar. */
    class TINYORM_EXPORT Blueprint
    {
        Q_DISABLE_COPY_MOVE(Blueprint)

        /*! Alias for the Query::Expression. */
        using Expression    = Query::Expression;
        /*! Alias for the SchemaGrammar. */
        using SchemaGrammar = Grammars::SchemaGrammar;

    public:
        /*! Constructor. */
        explicit Blueprint(QString table,
                           const std::function<void(Blueprint &)> &callback = nullptr,
                           QString &&prefix = "");
        /*! Default destructor. */
        ~Blueprint() = default;

        /*! Execute the blueprint against the database. */
        void build(DatabaseConnection &connection, const SchemaGrammar &grammar);
        /*! Get the raw SQL statements for the blueprint. */
        QList<QString>
        toSql(const DatabaseConnection &connection, const SchemaGrammar &grammar);

        /*! Indicates that the table needs to be created. */
        const BasicCommand &create();
        /*! Add the columns from the callback after the given column. */
        void after(const QString &column,
                   const std::function<void(Blueprint &)> &callback);
        /*! Indicates that the table should be dropped. */
        const BasicCommand &drop();
        /*! Indicates that the table should be dropped if it exists. */
        const BasicCommand &dropIfExists();

        /*! Indicates that the given columns should be dropped. */
        const DropColumnsCommand &dropColumns(const QList<QString> &columns);
        /*! Indicates that the given columns should be dropped. */
        template<QStringConcept ...Args>
        const DropColumnsCommand &dropColumns(Args &&...columns);
        /*! Indicates that the given columns should be dropped. */
        const DropColumnsCommand &dropColumn(const QString &column);

        /*! Indicates that the timestamp columns should be dropped. */
        void dropTimestamps();
        /*! Indicates that the timestamp columns should be dropped. */
        inline void dropTimestampsTz();
        /*! Indicates that the datetime columns should be dropped. */
        inline void dropDatetimes();

        /*! Indicates that the soft delete column should be dropped. */
        inline void dropSoftDeletes(const QString &column = Orm::Constants::DELETED_AT);
        /*! Indicates that the soft delete column should be dropped. */
        inline void dropSoftDeletesTz(const QString &column = Orm::Constants::DELETED_AT);
        /*! Indicates that the soft delete column should be dropped. */
        inline void
        dropSoftDeletesDatetime(const QString &column = Orm::Constants::DELETED_AT);

        /*! Indicates that the remember token column should be dropped. */
        inline void dropRememberToken();

        /*! Rename the table to a given name. */
        const RenameCommand &rename(const QString &to);

        /*! Indicates that the given columns should be renamed. */
        const RenameCommand &renameColumn(const QString &from, const QString &to);

        /*! Specify the primary key(s) for the table. */
        IndexDefinitionReference
        primary(const QList<QString> &columns, const QString &indexName = "",
                const QString &algorithm = "");
        /*! Specify the primary key(s) for the table. */
        template<typename = void>
        IndexDefinitionReference
        primary(const QString &column, const QString &indexName = "",
                const QString &algorithm = "");
        /*! Specify a unique index for the table. */
        IndexDefinitionReference
        unique(const QList<QString> &columns, const QString &indexName = "",
               const QString &algorithm = "");
        /*! Specify a unique index for the table. */
        template<typename = void>
        IndexDefinitionReference
        unique(const QString &column, const QString &indexName = "",
               const QString &algorithm = "");
        /*! Specify an index for the table. */
        IndexDefinitionReference
        index(const QList<QString> &columns, const QString &indexName = "",
              const QString &algorithm = "");
        /*! Specify an index for the table. */
        template<typename = void>
        IndexDefinitionReference
        index(const QString &column, const QString &indexName = "",
              const QString &algorithm = "");
        /*! Specify an fulltext for the table. */
        IndexDefinitionReference
        fullText(const QList<QString> &columns, const QString &indexName = "",
                 const QString &algorithm = "", const QString &language = "");
        /*! Specify an fulltext for the table. */
        template<typename = void>
        IndexDefinitionReference
        fullText(const QString &column, const QString &indexName = "",
                 const QString &algorithm = "", const QString &language = "");
        /*! Specify a spatial index for the table. */
        IndexDefinitionReference
        spatialIndex(const QList<QString> &columns, const QString &indexName = "");
        /*! Specify a spatial index for the table. */
        template<typename = void>
        IndexDefinitionReference
        spatialIndex(const QString &column, const QString &indexName = "");
        /*! Specify a raw index for the table. */
        IndexDefinitionReference
        rawIndex(const Expression &expression, const QString &indexName);
        /*! Specify a foreign key for the table. */
        ForeignKeyDefinitionReference
        foreign(const QList<QString> &columns, const QString &indexName = "");
        /*! Specify a foreign key for the table. */
        template<typename = void>
        ForeignKeyDefinitionReference
        foreign(const QString &column, const QString &indexName = "");

        /*! Drop primary key by the given column names. */
        const IndexCommand &dropPrimary(const QList<QString> &columns = {});
        /*! Drop unique key by the given column names. */
        const IndexCommand &dropUnique(const QList<QString> &columns);
        /*! Drop index by the given column names. */
        const IndexCommand &dropIndex(const QList<QString> &columns);
        /*! Drop fulltext index by the given column names. */
        const IndexCommand &dropFullText(const QList<QString> &columns);
        /*! Drop spatial index by the given column names. */
        const IndexCommand &dropSpatialIndex(const QList<QString> &columns);
        /*! Drop foreign key by the given column names. */
        const IndexCommand &dropForeign(const QList<QString> &columns);
        /*! Drop a column and foreign key by the given column name. */
        const DropColumnsCommand &dropConstrainedForeignId(const QString &column);

        /*! Drop primary key by the given index name. */
        template<typename = void>
        const IndexCommand &dropPrimary(const QString &indexName = "");
        /*! Drop unique key by the given index name. */
        template<typename = void>
        const IndexCommand &dropUnique(const QString &indexName);
        /*! Drop index by the given index name. */
        template<typename = void>
        const IndexCommand &dropIndex(const QString &indexName);
        /*! Drop fulltext index by the given index name. */
        template<typename = void>
        const IndexCommand &dropFullText(const QString &indexName);
        /*! Drop spatial index by the given index name. */
        template<typename = void>
        const IndexCommand &dropSpatialIndex(const QString &indexName);
        /*! Drop foreign key by the given index name. */
        template<typename = void>
        const IndexCommand &dropForeign(const QString &indexName);

        /*! Indicates that the given indexes should be renamed. */
        const RenameCommand &renameIndex(const QString &from, const QString &to);

        /*! Create a new auto-incrementing big integer (8-byte) column on the table. */
        inline ColumnDefinitionReference<> id(const QString &column = Orm::Constants::ID);

        /*! Create a new unsigned big integer (8-byte) column on the table. */
        ForeignIdColumnDefinitionReference foreignId(const QString &column);
#ifndef TINYORM_DISABLE_ORM
        /*! Create a foreign ID column for the given model. */
        template<Tiny::ModelConcept Model>
        ForeignIdColumnDefinitionReference foreignIdFor(const Model &model,
                                                        const QString &column = "");
#endif
        /*! Create a new UUID column on the table with a foreign key constraint. */
        ForeignIdColumnDefinitionReference foreignUuid(const QString &column);

        /*! Create a new auto-incrementing integer (4-byte) column on the table. */
        inline ColumnDefinitionReference<> increments(const QString &column);
        /*! Create a new auto-incrementing integer (4-byte) column on the table. */
        inline ColumnDefinitionReference<> integerIncrements(const QString &column);
        /*! Create a new auto-incrementing tiny integer (1-byte) column on the table. */
        inline ColumnDefinitionReference<> tinyIncrements(const QString &column);
        /*! Create a new auto-incrementing small integer (2-byte) column on the table. */
        inline ColumnDefinitionReference<> smallIncrements(const QString &column);
        /*! Create a new auto-incrementing medium integer (3-byte) column on the table. */
        inline ColumnDefinitionReference<> mediumIncrements(const QString &column);
        /*! Create a new auto-incrementing big integer (8-byte) column on the table. */
        inline ColumnDefinitionReference<> bigIncrements(const QString &column);

        /*! Create a new integer (4-byte) column on the table. */
        ColumnDefinitionReference<>
        integer(const QString &column, bool autoIncrement = false,
                bool isUnsigned = false);
        /*! Create a new tiny integer (1-byte) column on the table. */
        ColumnDefinitionReference<>
        tinyInteger(const QString &column, bool autoIncrement = false,
                    bool isUnsigned = false);
        /*! Create a new small integer (2-byte) column on the table. */
        ColumnDefinitionReference<>
        smallInteger(const QString &column, bool autoIncrement = false,
                     bool isUnsigned = false);
        /*! Create a new medium integer (3-byte) column on the table. */
        ColumnDefinitionReference<>
        mediumInteger(const QString &column, bool autoIncrement = false,
                      bool isUnsigned = false);
        /*! Create a new big integer (8-byte) column on the table. */
        ColumnDefinitionReference<>
        bigInteger(const QString &column, bool autoIncrement = false,
                   bool isUnsigned = false);

        /*! Create a new unsigned integer (4-byte) column on the table. */
        inline ColumnDefinitionReference<>
        unsignedInteger(const QString &column, bool autoIncrement = false);
        /*! Create a new unsigned tiny integer (1-byte) column on the table. */
        inline ColumnDefinitionReference<>
        unsignedTinyInteger(const QString &column, bool autoIncrement = false);
        /*! Create a new unsigned small integer (2-byte) column on the table. */
        inline ColumnDefinitionReference<>
        unsignedSmallInteger(const QString &column, bool autoIncrement = false);
        /*! Create a new unsigned medium integer (3-byte) column on the table. */
        inline ColumnDefinitionReference<>
        unsignedMediumInteger(const QString &column, bool autoIncrement = false);
        /*! Create a new unsigned big integer (8-byte) column on the table. */
        inline ColumnDefinitionReference<>
        unsignedBigInteger(const QString &column, bool autoIncrement = false);

        /*! Create a new char column on the table. */
        ColumnDefinitionReference<>
        Char(const QString &column, int length = DefaultStringLength);

        /*! Create a new string column on the table. */
        ColumnDefinitionReference<> string(const QString &column,
                                           int length = DefaultStringLength);
        /*! Create a new tiny text column on the table. */
        ColumnDefinitionReference<> tinyText(const QString &column);
        /*! Create a new text column on the table. */
        ColumnDefinitionReference<> text(const QString &column);
        /*! Create a new medium text column on the table. */
        ColumnDefinitionReference<> mediumText(const QString &column);
        /*! Create a new long text column on the table. */
        ColumnDefinitionReference<> longText(const QString &column);

        /*! Create a new float column on the table. */
        ColumnDefinitionReference<>
        Float(const QString &column, std::optional<int> total = 8,
              std::optional<int> places = 2, bool isUnsigned = false);
        /*! Create a new double column on the table. */
        ColumnDefinitionReference<>
        Double(const QString &column, std::optional<int> total = std::nullopt,
               std::optional<int> places = std::nullopt, bool isUnsigned = false);
        /*! Create a new decimal column on the table. */
        ColumnDefinitionReference<>
        decimal(const QString &column, std::optional<int> total = 8,
                std::optional<int> places = 2, bool isUnsigned = false);

        /*! Create a new unsigned float column on the table. */
        inline ColumnDefinitionReference<>
        unsignedFloat(const QString &column, std::optional<int> total = 8,
                      std::optional<int> places = 2);
        /*! Create a new unsigned double column on the table. */
        inline ColumnDefinitionReference<>
        unsignedDouble(const QString &column, std::optional<int> total = std::nullopt,
                       std::optional<int> places = std::nullopt);
        /*! Create a new unsigned decimal column on the table. */
        inline ColumnDefinitionReference<>
        unsignedDecimal(const QString &column, std::optional<int> total = 8,
                        std::optional<int> places = 2);

        /*! Create a new boolean column on the table. */
        ColumnDefinitionReference<> boolean(const QString &column);

        /*! Create a new enum column on the table. */
        ColumnDefinitionReference<> Enum(const QString &column,
                                         const QList<QString> &allowed);
        /*! Create a new set column on the table. */
        ColumnDefinitionReference<> set(const QString &column,
                                        const QList<QString> &allowed);

        /*! Create a new json column on the table. */
        ColumnDefinitionReference<> json(const QString &column);
        /*! Create a new jsonb column on the table. */
        ColumnDefinitionReference<> jsonb(const QString &column);

        /*! Create a new date column on the table. */
        ColumnDefinitionReference<> date(const QString &column);
        /*! Create a new date-time column on the table. */
        ColumnDefinitionReference<> datetime(const QString &column,
                                             std::optional<int> precision = 0);
        /*! Create a new date-time column (with time zone) on the table. */
        ColumnDefinitionReference<> datetimeTz(const QString &column,
                                               std::optional<int> precision = 0);

        /*! Create a new time column on the table. */
        ColumnDefinitionReference<> time(const QString &column,
                                         std::optional<int> precision = 0);
        /*! Create a new time column (with time zone) on the table. */
        ColumnDefinitionReference<> timeTz(const QString &column,
                                           std::optional<int> precision = 0);

        /*! Create a new timestamp column on the table. */
        ColumnDefinitionReference<> timestamp(const QString &column,
                                              std::optional<int> precision = 0);
        /*! Create a new timestamp (with time zone) column on the table. */
        ColumnDefinitionReference<> timestampTz(const QString &column,
                                                std::optional<int> precision = 0);

        /*! Add nullable creation and update timestamps to the table. */
        void timestamps(std::optional<int> precision = 0);
        /*! Add creation and update timestampTz columns to the table. */
        void timestampsTz(std::optional<int> precision = 0);

        /*! Add creation and update datetime columns to the table. */
        inline void datetimes(std::optional<int> precision = 0);

        /*! Add a "deleted at" timestamp for the table. */
        ColumnDefinitionReference<>
        softDeletes(const QString &column = Orm::Constants::DELETED_AT,
                    std::optional<int> precision = 0);
        /*! Add a "deleted at" timestampTz for the table. */
        ColumnDefinitionReference<>
        softDeletesTz(const QString &column = Orm::Constants::DELETED_AT,
                      std::optional<int> precision = 0);

        /*! Add a "deleted at" datetime column to the table. */
        inline ColumnDefinitionReference<>
        softDeletesDatetime(const QString &column = Orm::Constants::DELETED_AT,
                            std::optional<int> precision = 0);

        /*! Create a new year column on the table. */
        ColumnDefinitionReference<> year(const QString &column);

        /*! Create a new tiny binary column on the table. */
        ColumnDefinitionReference<> tinyBinary(const QString &column);
        /*! Create a new binary column on the table. */
        ColumnDefinitionReference<> binary(const QString &column);
        /*! Create a new medium binary column on the table. */
        ColumnDefinitionReference<> mediumBinary(const QString &column);
        /*! Create a new long binary column on the table. */
        ColumnDefinitionReference<> longBinary(const QString &column);

        /*! Create a new uuid column on the table. */
        ColumnDefinitionReference<>
        uuid(const QString &column = QStringLiteral("uuid"));

        /*! Create a new IP address column on the table. */
        ColumnDefinitionReference<>
        ipAddress(const QString &column = QStringLiteral("ip_address"));
        /*! Create a new MAC address column on the table. */
        ColumnDefinitionReference<>
        macAddress(const QString &column = QStringLiteral("mac_address"));

        /*! Create a new geometry column on the table. */
        ColumnDefinitionReference<> geometry(const QString &column);
        /*! Create a new point column on the table. */
        ColumnDefinitionReference<> point(const QString &column,
                                          std::optional<quint32> srid = std::nullopt);
        /*! Create a new linestring column on the table. */
        ColumnDefinitionReference<> lineString(const QString &column);
        /*! Create a new polygon column on the table. */
        ColumnDefinitionReference<> polygon(const QString &column);
        /*! Create a new geometrycollection column on the table. */
        ColumnDefinitionReference<> geometryCollection(const QString &column);
        /*! Create a new multipoint column on the table. */
        ColumnDefinitionReference<> multiPoint(const QString &column);
        /*! Create a new multilinestring column on the table. */
        ColumnDefinitionReference<> multiLineString(const QString &column);
        /*! Create a new multipolygon column on the table. */
        ColumnDefinitionReference<> multiPolygon(const QString &column);
        /*! Create a new multipolygon column on the table. */
        ColumnDefinitionReference<> multiPolygonZ(const QString &column);

        /*! Create a new generated, computed column on the table. */
        ColumnDefinitionReference<> computed(const QString &column,
                                             const QString &expression);

        /*! Adds the `remember_token` column to the table. */
        ColumnDefinitionReference<> rememberToken();

        /*! Add a comment to the table. */
        const TableCommentCommand &comment(QString comment);

        /*! Add a new column to the blueprint. */
        ColumnDefinitionReference<> addColumn(ColumnType type, const QString &name,
                                              ColumnDefinition &&definition = {});

        /* Others */
        /*! Get the columns on the blueprint that should be added. */
        QList<ColumnDefinition> getAddedColumns() const;
        /*! Get the columns on the blueprint that should be changed. */
        QList<ColumnDefinition> getChangedColumns() const;

        /*! Determine if the blueprint has a create command. */
        bool creating() const;

        /*! Remove a column from the schema blueprint. */
        Blueprint &removeColumn(const QString &name);

        /* Getters */
        /*! Get the table the blueprint describes. */
        inline const QString &getTable() const noexcept;
        /*! Get the columns on the blueprint. */
        inline const QList<ColumnDefinition> &getColumns() const noexcept;
        /*! Get the commands on the blueprint. */
        inline const std::deque<std::shared_ptr<CommandDefinition>> &
        getCommands() const noexcept;
        /*! Determine whether the blueprint describes temporary table. */
        inline bool isTemporary() const noexcept;

        /* Others */
        /*! Indicates that the table needs to be temporary. */
        inline void temporary() noexcept;

        /*! Set the default string length for migrations. */
        static void defaultStringLength(int length) noexcept;

        /*! The default string length for migrations. */
        static int DefaultStringLength;

        /*! The storage engine that should be used for the table (MySQL). */
        QString engine;
        /*! The default character set that should be used for the table (MySQL). */
        QString charset;
        /*! The collation that should be used for the table (MySQL). */
        QString collation;

    protected:
        /*! Expose column types. */
//        using enum ColumnType;

        /*! Add a new column definition to the blueprint. */
        ColumnDefinitionReference<> addColumnDefinition(ColumnDefinition &&definition);

        /*! Add a new command to the blueprint. */
        template<CommandDefinitionConcept T = BasicCommand>
        T &addCommand(T &&definition = {});
        /*! Create a new Fluent command. */
        template<CommandDefinitionConcept T>
        std::shared_ptr<T> createCommand(T &&definition = {});

        /*! Add the commands that are implied by the blueprint's state. */
        void addImpliedCommands(const SchemaGrammar &grammar);
        /*! Add the index commands fluently specified on columns. */
        void addFluentIndexes();
        /*! Add the fluent commands specified on any columns. */
        void addFluentCommands(const SchemaGrammar &grammar);

        /*! Add a new index command to the blueprint. */
        IndexDefinitionReference
        indexCommand(const QString &type, const QList<QString> &columns,
                     const QString &indexName, const QString &algorithm = "",
                     const QString &language = "");
        /*! Create a new drop index command on the blueprint. */
        const IndexCommand &
        dropIndexCommand(const QString &command, const QString &type,
                         const QList<QString> &columns);
        /*! Create a new drop index command on the blueprint. */
        const IndexCommand &
        dropIndexCommand(const QString &command, const QString &indexName);

        /*! Create a default index name for the table. */
        QString createIndexName(const QString &type,
                                const QList<QString> &columns) const;

        /*! The table the blueprint describes. */
        QString m_table;
        /*! The prefix of the table. */
        QString m_prefix;

        /*! The columns that should be added to the table. */
        QList<ColumnDefinition> m_columns;
        /*! The commands that should be run for the table. */
        std::deque<std::shared_ptr<CommandDefinition>> m_commands;

        /*! Whether to make the table temporary. */
        bool m_temporary = false;
        /*! The column to add new columns after. */
        QString m_after;
    };

    /* public */

#ifndef TINYORM_DISABLE_ORM
    template<Tiny::ModelConcept Model>
    ForeignIdColumnDefinitionReference
    Blueprint::foreignIdFor(const Model &model, const QString &column)
    {
        const auto column_ = column.isEmpty() ? model.getForeignKey() : column;

        // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
        return std::is_integral_v<typename Model::KeyType> && model.getIncrementing()
                ? foreignId(column_)
                : foreignUuid(column_);
    }
#endif

    template<QStringConcept ...Args>
    const DropColumnsCommand &Blueprint::dropColumns(Args &&...columns)
    {
        return dropColumns(QList<QString> {std::forward<Args>(columns)...}); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

    void Blueprint::dropTimestampsTz()
    {
        dropTimestamps();
    }

    void Blueprint::dropDatetimes()
    {
        dropTimestamps();
    }

    void Blueprint::dropSoftDeletes(const QString &column)
    {
        dropColumns(QList<QString> {column});
    }

    void Blueprint::dropSoftDeletesTz(const QString &column)
    {
        dropSoftDeletes(column);
    }

    void Blueprint::dropSoftDeletesDatetime(const QString &column)
    {
        dropSoftDeletes(column);
    }

    void Blueprint::dropRememberToken()
    {
        dropColumns({QStringLiteral("remember_token")});
    }

    template<typename>
    IndexDefinitionReference
    Blueprint::primary(const QString &column, const QString &indexName,
                       const QString &algorithm)
    {
        return primary(QList<QString> {column}, indexName, algorithm);
    }

    template<typename>
    IndexDefinitionReference
    Blueprint::unique(const QString &column, const QString &indexName,
                      const QString &algorithm)
    {
        return unique(QList<QString> {column}, indexName, algorithm);
    }

    template<typename>
    IndexDefinitionReference
    Blueprint::index(const QString &column, const QString &indexName,
                     const QString &algorithm)
    {
        return index(QList<QString> {column}, indexName, algorithm);
    }

    template<typename>
    IndexDefinitionReference
    Blueprint::fullText(const QString &column, const QString &indexName,
                        const QString &algorithm, const QString &language)
    {
        return fullText(QList<QString> {column}, indexName, algorithm, language);
    }

    template<typename>
    IndexDefinitionReference
    Blueprint::spatialIndex(const QString &column, const QString &indexName)
    {
        return spatialIndex(QList<QString> {column}, indexName);
    }

    template<typename>
    ForeignKeyDefinitionReference
    Blueprint::foreign(const QString &column, const QString &indexName)
    {
        return foreign(QList<QString> {column}, indexName);
    }

    template<typename>
    const IndexCommand &Blueprint::dropPrimary(const QString &indexName)
    {
        return dropIndexCommand(DropPrimary, indexName);
    }

    template<typename>
    const IndexCommand &Blueprint::dropUnique(const QString &indexName)
    {
        return dropIndexCommand(DropUnique, indexName);
    }

    template<typename>
    const IndexCommand &Blueprint::dropIndex(const QString &indexName)
    {
        return dropIndexCommand(DropIndex, indexName);
    }

    template<typename>
    const IndexCommand &Blueprint::dropFullText(const QString &indexName)
    {
        return dropIndexCommand(DropFullText, indexName);
    }

    template<typename>
    const IndexCommand &Blueprint::dropSpatialIndex(const QString &indexName)
    {
        return dropIndexCommand(DropSpatialIndex, indexName);
    }

    template<typename>
    const IndexCommand &Blueprint::dropForeign(const QString &indexName)
    {
        return dropIndexCommand(DropForeign, indexName);
    }

    ColumnDefinitionReference<> Blueprint::id(const QString &column)
    {
        return bigIncrements(column);
    }

    ColumnDefinitionReference<> Blueprint::increments(const QString &column)
    {
        return unsignedInteger(column, true);
    }

    ColumnDefinitionReference<> Blueprint::integerIncrements(const QString &column)
    {
        return unsignedInteger(column, true);
    }

    ColumnDefinitionReference<> Blueprint::tinyIncrements(const QString &column)
    {
        return unsignedTinyInteger(column, true);
    }

    ColumnDefinitionReference<> Blueprint::smallIncrements(const QString &column)
    {
        return unsignedSmallInteger(column, true);
    }

    ColumnDefinitionReference<> Blueprint::mediumIncrements(const QString &column)
    {
        return unsignedMediumInteger(column, true);
    }

    ColumnDefinitionReference<> Blueprint::bigIncrements(const QString &column)
    {
        return unsignedBigInteger(column, true);
    }

    ColumnDefinitionReference<>
    Blueprint::unsignedInteger(const QString &column, const bool autoIncrement)
    {
        return integer(column, autoIncrement, true);
    }

    ColumnDefinitionReference<>
    Blueprint::unsignedTinyInteger(const QString &column, const bool autoIncrement)
    {
        return tinyInteger(column, autoIncrement, true);
    }

    ColumnDefinitionReference<>
    Blueprint::unsignedSmallInteger(const QString &column, const bool autoIncrement)
    {
        return smallInteger(column, autoIncrement, true);
    }

    ColumnDefinitionReference<>
    Blueprint::unsignedMediumInteger(const QString &column, const bool autoIncrement)
    {
        return mediumInteger(column, autoIncrement, true);
    }

    ColumnDefinitionReference<>
    Blueprint::unsignedBigInteger(const QString &column, const bool autoIncrement)
    {
        return bigInteger(column, autoIncrement, true);
    }

    ColumnDefinitionReference<>
    Blueprint::unsignedFloat(const QString &column, const std::optional<int> total,
                             const std::optional<int> places)
    {
        return Float(column, total, places, true);
    }

    ColumnDefinitionReference<>
    Blueprint::unsignedDouble(const QString &column, const std::optional<int> total,
                              const std::optional<int> places)
    {
        return Double(column, total, places, true);
    }

    ColumnDefinitionReference<>
    Blueprint::unsignedDecimal(const QString &column, const std::optional<int> total,
                               const std::optional<int> places)
    {
        return decimal(column, total, places, true);
    }

    void Blueprint::datetimes(const std::optional<int> precision)
    {
        datetime(Orm::Constants::CREATED_AT, precision).nullable();

        datetime(Orm::Constants::UPDATED_AT, precision).nullable();
    }

    ColumnDefinitionReference<>
    Blueprint::softDeletesDatetime(const QString &column,
                                   const std::optional<int> precision)
    {
        return datetime(column, precision).nullable();
    }

    const QString &Blueprint::getTable() const noexcept
    {
        return m_table;
    }

    const QList<ColumnDefinition> &Blueprint::getColumns() const noexcept
    {
        return m_columns;
    }

    const std::deque<std::shared_ptr<CommandDefinition>> &
    Blueprint::getCommands() const noexcept
    {
        return m_commands;
    }

    bool Blueprint::isTemporary() const noexcept
    {
        return m_temporary;
    }

    void Blueprint::temporary() noexcept
    {
        m_temporary = true;
    }

    /* protected */

    template<CommandDefinitionConcept T>
    T &Blueprint::addCommand(T &&definition)
    {
        auto command = createCommand(std::forward<T>(definition));

        m_commands.emplace_back(std::move(command));

        return reinterpret_cast<T &>(*m_commands.back()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    }

    template<CommandDefinitionConcept T>
    std::shared_ptr<T>
    Blueprint::createCommand(T &&definition)
    {
        return std::make_shared<T>(std::forward<T>(definition));
    }

} // namespace SchemaNs
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_BLUEPRINT_HPP
