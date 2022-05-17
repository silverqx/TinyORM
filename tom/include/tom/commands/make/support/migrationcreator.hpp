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
        /*! Default constructor. */
        inline MigrationCreator() = default;
        /*! Default destructor. */
        inline ~MigrationCreator() = default;

        /*! Create a new migration at the given path. */
        fspath create(std::string &&datetimePrefix, const QString &name,
                      std::string &&extension, fspath &&migrationsPath,
                      const QString &table = "", bool create = false) const;

    protected:
        /*! Get the migration stub file. */
        static QString getStub(const QString &table, bool create);

        /*! Get the full path to the migration. */
        fspath getPath(std::string &&datetimePrefix, const QString &name,
                       std::string &&extension, const fspath &path) const;
        /*! Get the date prefix for the migration. */
        static std::string getDatePrefix();

        /*! Populate the place-holders in the migration stub. */
        static std::string
        populateStub(const QString &name, QString &&stub, const QString &table);
        /*! Get the class name of a migration name. */
        static QString getClassName(const QString &name);
        /*! Ensure a directory exists. */
        static void ensureDirectoryExists(const fspath &path);

    private:
        /*! Ensure that a migration with the given name doesn't already exist. */
        static void throwIfMigrationAlreadyExists(const QString &name,
                                                  const fspath &migrationsPath);
    };

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_SUPPORT_MIGRATIONCREATOR_HPP
