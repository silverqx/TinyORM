#pragma once
#ifndef TOM_MIGRATIONCREATOR_HPP
#define TOM_MIGRATIONCREATOR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <filesystem>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
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
        /*! Ensure that a migration with the given name doesn't already exist. */
        void throwIfMigrationAlreadyExists(const QString &name,
                                           const fspath &migrationsPath) const;

        /*! Get the migration stub file. */
        QString getStub(const QString &table, bool create) const;

        /*! Get the path to the stubs. */
        fspath stubPath() const;
        /*! Get the full path to the migration. */
        fspath getPath(std::string &&datetimePrefix, const QString &name,
                       std::string &&extension, const fspath &path) const;
        /*! Get the date prefix for the migration. */
        std::string getDatePrefix() const;

        /*! Populate the place-holders in the migration stub. */
        std::string populateStub(const QString &name, QString &&stub,
                                 const QString &table) const;
        /*! Get the class name of a migration name. */
        QString getClassName(const QString &name) const;
        /*! Ensure a directory exists. */
        void ensureDirectoryExists(const fspath &path) const;
    };

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_MIGRATIONCREATOR_HPP
