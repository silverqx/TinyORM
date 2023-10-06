#pragma once
#ifndef TOM_COMMANDS_MAKE_SUPPORT_SEEDERCREATOR_HPP
#define TOM_COMMANDS_MAKE_SUPPORT_SEEDERCREATOR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <filesystem>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Support
{

    /*! Seeder file generator (used by the make:seeder command). */
    class SeederCreator
    {
        Q_DISABLE_COPY_MOVE(SeederCreator)

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

    public:
        /*! Deleted default constructor, this is a pure library class. */
        SeederCreator() = delete;
        /*! Deleted destructor. */
        ~SeederCreator() = delete;

        /*! Create a new seeder at the given path. */
        static fspath create(const QString &className, const fspath &seedersPath);

    protected:
        /*! Get the full path to the seeder. */
        static fspath getPath(const QString &basename, const fspath &path);

        /*! Populate the place-holders in the seeder stub. */
        static std::string
        populateStub(const QString &className, const QString &table);

        /*! Get the table name from the seeder class name. */
        static QString getTableName(QString className);
    };

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_SUPPORT_SEEDERCREATOR_HPP
