#pragma once
#ifndef TOM_SEEDERCREATOR_HPP
#define TOM_SEEDERCREATOR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <filesystem>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

    /*! Seeder file generator (used by the make:seeder command). */
    class SeederCreator
    {
        Q_DISABLE_COPY(SeederCreator)

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

    public:
        /*! Default constructor. */
        inline SeederCreator() = default;
        /*! Default destructor. */
        inline ~SeederCreator() = default;

        /*! Create a new seeder at the given path. */
        fspath create(const QString &className, fspath &&seedersPath) const;

    protected:
        /*! Get the seeder stub file. */
        static QString getStub();

        /*! Get the full path to the seeder. */
        static fspath getPath(const QString &basename, const fspath &path);

        /*! Ensure a directory exists. */
        static void ensureDirectoryExists(const fspath &path);

        /*! Populate the place-holders in the seeder stub. */
        static std::string
        populateStub(const QString &className, QString &&table, QString &&stub);

        /*! Get the table name from the seeder class name. */
        static QString getTableName(QString className);

    private:
        /*! Ensure that a seeder with the given name doesn't already exist. */
        void throwIfSeederAlreadyExists(
                    const QString &className, const QString &basename,
                    const fspath &seedersPath) const;
    };

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_SEEDERCREATOR_HPP
