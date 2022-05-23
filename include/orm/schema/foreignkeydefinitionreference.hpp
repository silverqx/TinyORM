#pragma once
#ifndef ORM_SCHEMA_FOREIGNKEYDEFINITIONREFERENCE_HPP
#define ORM_SCHEMA_FOREIGNKEYDEFINITIONREFERENCE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVector>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    class ForeignKeyCommand;

    /*! Reference class to the ColumnDefinition, provides setters with a nice API
        for the foreign key column. */
    class SHAREDLIB_EXPORT ForeignKeyDefinitionReference
    {
    public:
        /*! Constructor. */
        ForeignKeyDefinitionReference(ForeignKeyCommand &foreignKeyCommandDefinition); // NOLINT(google-explicit-constructor)
        /*! Default destructor. */
        inline ~ForeignKeyDefinitionReference() = default;

        /*! Specify the referenced columns. */
        ForeignKeyDefinitionReference &references(const QVector<QString> &columns);
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
        /*! Indicate that updates should cascade. */
        ForeignKeyDefinitionReference &cascadeOnUpdate();
        /*! Indicate that updates should be restricted. */
        ForeignKeyDefinitionReference &restrictOnUpdate();
        /*! Indicate that deletes should cascade. */
        ForeignKeyDefinitionReference &cascadeOnDelete();
        /*! Indicate that deletes should be restricted. */
        ForeignKeyDefinitionReference &restrictOnDelete();
        /*! Indicate that deletes should set the foreign key value to null. */
        ForeignKeyDefinitionReference &nullOnDelete();

    private:
        /*! Reference to a column definition. */
        std::reference_wrapper<ForeignKeyCommand> m_foreignKeyCommandDefinition;
    };

    template<typename>
    ForeignKeyDefinitionReference &
    ForeignKeyDefinitionReference::references(const QString &column)
    {
        return references(QVector<QString> {column});
    }

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_FOREIGNKEYDEFINITIONREFERENCE_HPP
