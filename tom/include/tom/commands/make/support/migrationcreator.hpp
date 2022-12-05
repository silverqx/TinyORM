#pragma once
#ifndef TOM_COMMANDS_MAKE_SUPPORT_MIGRATIONCREATOR_HPP
#define TOM_COMMANDS_MAKE_SUPPORT_MIGRATIONCREATOR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <filesystem>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Support
{

    /*! Migration file generator (used by the make:migration command). */
    class MigrationCreator
    {
        Q_DISABLE_COPY(MigrationCreator)

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

    public:
        /*! Deleted default constructor, this is a pure library class. */
        MigrationCreator() = delete;
        /*! Deleted destructor. */
        ~MigrationCreator() = delete;

        /*! Create a new migration at the given path. */
        static fspath create(std::string &&datetimePrefix, const QString &name,
                             std::string &&extension, fspath &&migrationsPath,
                             const QString &table = "", bool create = false);

    protected:
        /*! Get the migration stub file. */
        static QString getStub(const QString &table, bool create);

        /*! Get the full path to the migration. */
        static fspath getPath(std::string &&datetimePrefix, const QString &name,
                              std::string &&extension, const fspath &path);
        /*! Get the date prefix for the migration. */
        static std::string getDatePrefix();

        /*! Populate the place-holders in the migration stub. */
        static std::string
        populateStub(const QString &name, QString &&stub, const QString &table);
        /*! Get the class name of a migration name. */
        static QString getClassName(const QString &name);
    };

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_SUPPORT_MIGRATIONCREATOR_HPP
