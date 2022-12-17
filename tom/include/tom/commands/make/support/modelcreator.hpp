#pragma once
#ifndef TOM_COMMANDS_MAKE_SUPPORT_MODELCREATOR_HPP
#define TOM_COMMANDS_MAKE_SUPPORT_MODELCREATOR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QStringList>

#include <filesystem>
#include <set>

#include <orm/macros/commonnamespace.hpp>

#include "tom/commands/make/modelcommandtypes.hpp"

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

        /*! Create a new model at the given path. */
        fspath create(const QString &className, const CmdOptions &cmdOptions,
                      const fspath &modelsPath, bool isSetPreserveOrder);

    protected:
        /*! Get the full path to the model. */
        static fspath getPath(const QString &basename, const fspath &path);

        /*! Populate the place-holders in the model stub. */
        std::string populateStub(const QString &className, const CmdOptions &cmdOptions,
                                 bool isSetPreserveOrder);

        /* Public model section */
        /*! Create model's public section (relations). */
        QString createPublicSection(
                const QString &className, const CmdOptions &cmdOptions,
                bool isSetPreserveOrder);

        /*! One relationship method with order for the public section list. */
        struct RelationWithOrder
        {
            /*! Relationship method order. */
            std::size_t relationOrder;
            /*! Relationship method content. */
            QString     content;
        };
        /*! Prepared public sections list with preserved order on the command-line. */
        using RelationsWithOrder = std::vector<RelationWithOrder>;

        /*! Create one-to-one relationship method. */
        RelationsWithOrder createOneToOneRelation(
                const QString &parentClass, const QStringList &relatedClasses,
                const QStringList &foreignKeys,
                const std::vector<std::size_t> &orderList);
        /*! Create one-to-many relationship method. */
        RelationsWithOrder createOneToManyRelation(
                const QString &parentClass, const QStringList &relatedClasses,
                const QStringList &foreignKeys,
                const std::vector<std::size_t> &orderList);
        /*! Create belongs-to relationship method. */
        RelationsWithOrder createBelongsToRelation(
                const QString &parentClass, const QStringList &relatedClasses,
                const QStringList &foreignKeys,
                const std::vector<std::size_t> &orderList);

        /*! Create arguments list for the relation factory method (for oto, otm, bto). */
        static QString createRelationArguments(const QString &foreignKey);

        /*! Create belongs-to-many relationship method. */
        RelationsWithOrder createBelongsToManyRelation(
                const QString &parentClass, const QStringList &relatedClasses,
                const std::vector<BelongToManyForeignKeys> &foreignKeys,
                const std::vector<std::size_t> &orderList,
                const QStringList &pivotTables, const QStringList &pivotClasses,
                const std::vector<QStringList> &pivotInverseClasses,
                const QStringList &asList,
                const std::vector<QStringList> &withPivotList,
                const std::vector<bool> &withTimestampsList);

        /*! Create arguments list for the relation factory method (for btm). */
        static QString createRelationArgumentsBtm(
                const QString &pivotTable, const BelongToManyForeignKeys &foreignKey);
        /*! Pivot class logic for the belongs-to-many relation (--pivot option). */
        void handlePivotClass(const QString &pivotClass, bool isPivotClassEmpty);
        /*! Pivot class logic for inverse belongs-to-many relation (--pivot-inverse). */
        void handlePivotInverseClass(const QStringList &pivotInverseClasses);
        /*! Create method calls on the belongs-to-many relation. */
        static QString createRelationCalls(
                const QString &as, const QStringList &withPivot,
                bool withTimestamps);

        /*! Convert the given class name for usage in the comment (singular). */
        static QString guessSingularComment(const QString &className);
        /*! Convert the given class name for usage in the comment (plural). */
        static QString guessPluralComment(const QString &className);
        /*! Guess the to-one relationship name (singular). */
        static QString guessOneTypeRelationName(const QString &className);
        /*! Guess the to-many relationship name (plural). */
        static QString guessManyTypeRelationName(const QString &className);

        /* Private model section */
        /*! Create model's private section. */
        QString createPrivateSection(
                const QString &className, const CmdOptions &cmdOptions,
                bool hasPublicSection, bool isSetPreserveOrder);

        /*! Determine whether any model related option was given on the command-line. */
        static bool anyModelOptionGiven(const CmdOptions &cmdOptions);
        /*! Prepare string values for the initializer list. */
        static QString prepareInitializerListValues(const QStringList &list);

        /*! Create model's u_relations hash. */
        QString createRelationsHash(
                const QString &className, const CmdOptions &cmdOptions,
                bool isSetPreserveOrder) const;
        /*! Get max. size of relation names for align. */
        static QString::size_type getRelationNamesMaxSize(const CmdOptions &cmdOptions);

        /*! Create one-to-one relation mapping item for u_relations hash. */
        static RelationsWithOrder createOneToOneRelationItem(
                const QString &parentClass, const QStringList &relatedClasses,
                QString::size_type relationsMaxSize,
                const std::vector<std::size_t> &orderList);
        /*! Create one-to-many relation mapping item for u_relations hash. */
        static RelationsWithOrder createOneToManyRelationItem(
                const QString &parentClass, const QStringList &relatedClasses,
                QString::size_type relationsMaxSize,
                const std::vector<std::size_t> &orderList);
        /*! Create belongs-to relation mapping item for u_relations hash. */
        static RelationsWithOrder createBelongsToRelationItem(
                const QString &parentClass, const QStringList &relatedClasses,
                QString::size_type relationsMaxSize,
                const std::vector<std::size_t> &orderList);
        /*! Create belongs-to-many relation mapping item for u_relations hash. */
        static RelationsWithOrder createBelongsToManyRelationItem(
                const QString &parentClass, const QStringList &relatedClasses,
                QString::size_type relationsMaxSize,
                const std::vector<std::size_t> &orderList);

        /* Global */
        /*! Create model's TinyORM includes section. */
        QString createIncludesOrmSection(const CmdOptions &cmdOptions);
        /*! Create model's includes section. */
        QString createIncludesSection() const;
        /*! Create model's usings section. */
        QString createUsingsSection(const CmdOptions &cmdOptions);
        /*! Create model's relations list for the Model base class. */
        QString createRelationsList() const;
        /*! Create model's pivots list for the Model base class. */
        QString createPivotsList() const;
        /*! Create model's forward declarations section. */
        QString createForwardsSection() const;

        /* Common for public/private sections */
        /*! Compute reserve value for the public section list. */
        std::size_t computeReserveForRelationsList(
                const QStringList &oneToOne, const QStringList &oneToMany,
                const QStringList &belongsTo, const QStringList &belongsToMany);
        /*! Join relations list. */
        static QString joinRelationsList(RelationsWithOrder &&relationsList);

        /*! TinyORM include paths for the generated model. */
        std::set<QString> m_includesOrmList {};
        /*! Include paths for the generated model. */
        std::set<QString> m_includesList {};
        /*! Using directives for the generated model. */
        std::set<QString> m_usingsList {};
        /*! Relations list for the generated model's base class (all related classes). */
        std::set<QString> m_relationsList {};
        /*! Pivots list for the generated model's base class. */
        std::set<QString> m_pivotsList {};
        /*! Forward declarations list for related models. */
        std::set<QString> m_forwardsList {};

        /*! Cached relations list size to avoid recomputation. */
        std::size_t m_relationsListSize = 0;
    };

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_SUPPORT_MODELCREATOR_HPP
