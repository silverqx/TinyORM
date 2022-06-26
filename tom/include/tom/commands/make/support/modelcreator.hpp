#pragma once
#ifndef TOM_COMMANDS_MAKE_SUPPORT_MODELCREATOR_HPP
#define TOM_COMMANDS_MAKE_SUPPORT_MODELCREATOR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <filesystem>
#include <set>

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
            /*! Related class name for one-to-one relationship. */
            QString oneToOne;
            /*! Related class name for one-to-many relationship. */
            QString oneToMany;
            /*! The connection name for the model. */
            QString connection;
            /*! The table associated with the model. */
            QString table;
            /*! Disable timestamping of the model. */
            bool disableTimestamps;
        };

        /*! Create a new model at the given path. */
        fspath create(const QString &className, const CmdOptions &cmdOptions,
                      fspath &&modelsPath);

    protected:
        /*! Get the full path to the model. */
        static fspath getPath(const QString &basename, const fspath &path);

        /*! Ensure a directory exists. */
        static void ensureDirectoryExists(const fspath &path);

        /*! Populate the place-holders in the model stub. */
        std::string populateStub(const QString &className, const CmdOptions &cmdOptions);

        /*! Create model's public section (relations). */
        static QString createPublicSection(const QString &className,
                                           const CmdOptions &cmdOptions);

        /*! Create one-to-one relationship method. */
        static QString createOneToOneRelation(const QString &parentClass,
                                              const QString &relatedClass);
        /*! Create one-to-many relationship method. */
        static QString createOneToManyRelation(const QString &parentClass,
                                               const QString &relatedClass);

        /*! Convert the given class name for usage in the comment (singular). */
        static QString guessSingularComment(const QString &className);
        /*! Convert the given class name for usage in the comment (plural). */
        static QString guessPluralComment(const QString &className);
        /*! Guess the to-one relationship name (singular). */
        static QString guessOneTypeRelationName(const QString &className);
        /*! Guess the to-many relationship name (plural). */
        static QString guessManyTypeRelationName(const QString &className);

        /*! Create model's private section. */
        QString createPrivateSection(
                    const QString &className, const CmdOptions &cmdOptions,
                    bool hasPublicSection);

        /*! Create model's u_relations hash. */
        QString createRelationsHash(const QString &className,
                                    const CmdOptions &cmdOptions);

        /*! Create one-to-one relation mapping item for u_relations hash. */
        QString createOneToOneRelationItem(
                    const QString &parentClass, const QString &relatedClass,
                    QString::size_type relationsMaxSize);
        /*! Create one-to-many relation mapping item for u_relations hash. */
        QString createOneToManyRelationItem(
                    const QString &parentClass, const QString &relatedClass,
                    QString::size_type relationsMaxSize);

        /*! Create model's includes section. */
        QString createIncludesSection();
        /*! Create model's usings section. */
        QString createUsingsSection();

        /*! Include paths for the generated model. */
        std::set<QString> m_includesList {};
        /*! Using directives for the generated model. */
        std::set<QString> m_usingsList {};

    private:
        /*! Ensure that a model with the given name doesn't already exist. */
        static void throwIfModelAlreadyExists(
                    const QString &className, const QString &basename,
                    const fspath &modelsPath);
    };

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_SUPPORT_MODELCREATOR_HPP
