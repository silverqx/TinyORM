#pragma once
#ifndef TOM_COMMANDS_MAKE_SUPPORT_MODELCREATOR_HPP
#define TOM_COMMANDS_MAKE_SUPPORT_MODELCREATOR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <filesystem>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Support
{

    /*! Model file generator (used by the make:model command). */
    class ModelCreator
    {
        Q_DISABLE_COPY(ModelCreator)

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

    public:
        /*! Default constructor. */
        inline ModelCreator() = default;
        /*! Default destructor. */
        inline ~ModelCreator() = default;

        /*! Struct to hold command line option values. */
        struct CmdOptions
        {
            /*! The connection name for the model. */
            QString connection;
            /*! The table associated with the model. */
            QString table;
            /*! Disable timestamping of the model. */
            bool disableTimestamps;
        };

        /*! Create a new model at the given path. */
        fspath create(const QString &className, CmdOptions &&cmdOptions,
                      fspath &&modelsPath) const;

    protected:
        /*! Get the full path to the model. */
        static fspath getPath(const QString &basename, const fspath &path);

        /*! Ensure a directory exists. */
        static void ensureDirectoryExists(const fspath &path);

        /*! Populate the place-holders in the model stub. */
        static std::string populateStub(const QString &className,
                                        CmdOptions &&cmdOptions);

        /*! Create model's private section. */
        static QString createPrivateSection(const CmdOptions &cmdOptions);

    private:
        /*! Ensure that a model with the given name doesn't already exist. */
        void throwIfModelAlreadyExists(
                    const QString &className, const QString &basename,
                    const fspath &modelsPath) const;
    };

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_SUPPORT_MODELCREATOR_HPP
