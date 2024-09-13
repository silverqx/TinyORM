#pragma once
#ifndef TOM_MIGRATION_HPP
#define TOM_MIGRATION_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <filesystem> // IWYU pragma: export

#include <orm/schema.hpp>

/*! Filename of the migration file (populated by the __FILE__ macro excluding
    the filepath part). */
#define T_MIGRATION                                                                 \
    /*! Filename of the migration file. */                                          \
    inline static const auto FileName = []                                          \
    {                                                                               \
        return QString::fromStdString(                                              \
                    std::filesystem::path(__FILE__).stem().string());               \
    }();

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

    /*! Migrations base class. */
    class Migration
    {
        Q_DISABLE_COPY_MOVE(Migration)

    public:
        /*! Filename of the migration file. */
        inline static const QString FileName;

        /*! Default constructor. */
        Migration() = default;
        /*! Pure virtual destructor. */
        inline virtual ~Migration() = 0;

        /*! Run the migrations. */
        virtual void up() const = 0;
        /*! Reverse the migrations. */
        virtual void down() const = 0;

        /*! The name of the database connection to use. */
        QString connection;

        /*! Wrapping the migration within a transaction, if supported. */
        bool withinTransaction = true;
    };

    Migration::~Migration() = default;

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

// Import the most used constants into Migrations namespace so the user doesn't have to
namespace Migrations
{

    /*! Expose all Qt user-defined literals. */
    using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

    /*! Alias for the Schema Blueprint. */
    using TINYORM_PREPEND_NAMESPACE(Orm::SchemaNs::Blueprint); // NOLINT(misc-unused-using-decls)
    /*! Alias for the Tom Migration. */
    using TINYORM_PREPEND_NAMESPACE(Tom::Migration); // NOLINT(misc-unused-using-decls)
    /*! Alias for the Orm Schema. */
    using TINYORM_PREPEND_NAMESPACE(Orm::Schema); // NOLINT(misc-unused-using-decls)

    // Aliases for the most used string constants
    /*! Alias for the string constant "". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::EMPTY); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "id". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::ID); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "name". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::NAME); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "size". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::SIZE_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "created_at". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::CREATED_AT); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "updated_at". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::UPDATED_AT); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "deleted_at". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::DELETED_AT); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "null". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::null_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "note". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::NOTE); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "pivot". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::pivot_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "hash". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::HASH_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "progress". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::Progress); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "added_on". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::AddedOn); // NOLINT(misc-unused-using-decls)

    /*! Alias for the string constant "MySQL". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::MYSQL_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "PostgreSQL". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::POSTGRESQL); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "SQLite". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::SQLITE); // NOLINT(misc-unused-using-decls)

    /*! Alias for the string constant "driver". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::driver_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "host". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::host_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "port". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::port_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "database". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::database_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "schema". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::search_path); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "username". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::username_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "password". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::password_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "charset". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::charset_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "collation". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::collation_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "timezone". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::timezone_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "prefix". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::prefix_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "options". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::options_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "strict". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::strict_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "engine". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::engine_); // NOLINT(misc-unused-using-decls)

    /*! Alias for the string constant "127.0.0.1". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::H127001); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "localhost". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::LOCALHOST); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "3306". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::P3306); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "5432". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::P5432); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "root". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::ROOT); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "UTC". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::UTC); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "LOCAL". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::LOCAL); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "DEFAULT". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::DEFAULT); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "SYSTEM". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::SYSTEM); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "+00:00". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::TZ00); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "public". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::PUBLIC); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::UTF8); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8mb4". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::UTF8MB4); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "InnoDB". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::InnoDB); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "MyISAM". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::MyISAM); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "postgres". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::postgres_); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant ":memory:". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::in_memory); // NOLINT(misc-unused-using-decls)

    /*! Alias for the string constant "utf8_general_ci". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::UTF8Generalci); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8_unicode_ci". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::UTF8Unicodeci); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8mb4_unicode_520_ci". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::UTF8MB4Unicode520ci); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8mb4_0900_ai_ci". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::UTF8MB40900aici); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8mb4_uca1400_ai_ci". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::UTF8MB4Uca1400aici); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "ucs_basic". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::UcsBasic); // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "POSIX". */
    using TINYORM_PREPEND_NAMESPACE(Orm::Constants::POSIX_); // NOLINT(misc-unused-using-decls)

} // namespace Migrations

#endif // TOM_MIGRATION_HPP
