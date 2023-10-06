#pragma once
#ifndef TOM_COMMANDS_MAKE_MAKECOMMAND_HPP
#define TOM_COMMANDS_MAKE_MAKECOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <filesystem>

#include "tom/commands/command.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

    /*! Base class for the make-related commands. */
    class MakeCommand : public Command
    {
        Q_DISABLE_COPY(MakeCommand)

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

    public:
        /*! Constructor. */
        MakeCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~MakeCommand() override = default;

    protected:
        /*! Check whether the created file already exists and create folder if needed. */
        void prepareFileSystem(
                    const QString &type, const fspath &folder, const QString &basename,
                    const QString &className = {}) const;

        /*! Throw if the given classname constains a namespace or path. */
        static void throwIfContainsNamespaceOrPath(
                    const QString &type, const QString &className,
                    const QString &source);

        /*! Try to guess a path for "make:" commands based on the pwd or use
            the default path. */
        static fspath
        guessPathForMakeByPwd(const fspath &defaultPath,
                              const std::optional<
                                  std::reference_wrapper<const fspath>
                              > &defaultModelsPath = std::nullopt);

    private:
        /*! Ensure that a file in the given folder doesn't already exist. */
        void throwIfFileAlreadyExists(
                    const QString &type, const fspath &folder, const QString &basename,
                    const QString &className) const;

        /*! Ensure a directory exists. */
        static void ensureDirectoryExists(const fspath &path);

        /*! Determine whether the parent paths are equal. */
        static bool areParentPathsEqual(QStringList defaultPathList,
                                        const fspath &defaultModelsPath);
    };

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_MAKECOMMAND_HPP
