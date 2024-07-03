#pragma once
#ifndef ORM_SCHEMA_FOREIGNKEYDEFINITIONREFERENCE_HPP
#define ORM_SCHEMA_FOREIGNKEYDEFINITIONREFERENCE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QList>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    class ForeignKeyCommand;

    /*! Reference class to the ColumnDefinition, provides setters with a nice API
        for the foreign key column. */
    class TINYORM_EXPORT ForeignKeyDefinitionReference
    {
    public:
        /*! Constructor. */
        ForeignKeyDefinitionReference(ForeignKeyCommand &foreignKeyCommandDefinition); // NOLINT(google-explicit-constructor)

        /*! Specify the referenced columns. */
        ForeignKeyDefinitionReference &references(const QList<QString> &columns);
        /*! Specify the referenced column. */
        template<typename = void>
        ForeignKeyDefinitionReference &references(const QString &column);

        /*! Specify the referenced table. */
        ForeignKeyDefinitionReference &on(const QString &table);

        /*! Add an ON DELETE action. */
        ForeignKeyDefinitionReference &onDelete(const QString &action);
        /*! Add an ON UPDATE action. */
        ForeignKeyDefinitionReference &onUpdate(const QString &action);

        /*! Set the foreign key as deferrable (PostgreSQL). */
        ForeignKeyDefinitionReference &deferrable(bool value = true);
        /*! Set the default time to check the constraint (PostgreSQL). */
        ForeignKeyDefinitionReference &initiallyImmediate(bool value = true);
        /*! Set skip check that all existing rows in the table satisfy the new
            constraint (PostgreSQL). */
        ForeignKeyDefinitionReference &notValid(bool value = true);

        /* Shortcuts */
        /*! Indicates that updates should cascade. */
        ForeignKeyDefinitionReference &cascadeOnUpdate();
        /*! Indicates that updates should be restricted. */
        ForeignKeyDefinitionReference &restrictOnUpdate();
        /*! Indicates that deletes should cascade. */
        ForeignKeyDefinitionReference &cascadeOnDelete();
        /*! Indicates that deletes should be restricted. */
        ForeignKeyDefinitionReference &restrictOnDelete();
        /*! Indicates that deletes should set the foreign key value to null. */
        ForeignKeyDefinitionReference &nullOnDelete();

    private:
        /*! Reference to a column definition. */
        std::reference_wrapper<ForeignKeyCommand> m_foreignKeyCommandDefinition;
    };

    /* public */

    template<typename>
    ForeignKeyDefinitionReference &
    ForeignKeyDefinitionReference::references(const QString &column)
    {
        return references(QList<QString> {column});
    }

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_FOREIGNKEYDEFINITIONREFERENCE_HPP
