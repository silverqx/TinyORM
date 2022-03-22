#pragma once
#ifndef ORM_SCHEMA_FOREIGNKEYDEFINITIONREFERENCE_HPP
#define ORM_SCHEMA_FOREIGNKEYDEFINITIONREFERENCE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/export.hpp"
#include "orm/schema/columndefinitionreference.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    /*! Reference class to the ColumnDefinition, provides setters with a nice API
        for the foreign key column. */
    class SHAREDLIB_EXPORT ForeignKeyDefinitionReference
    {
    public:
        /*! Constructor. */
        ForeignKeyDefinitionReference( // NOLINT(google-explicit-constructor)
                    ColumnDefinitionReference<> columnDefinitionReference);
        /*! Constructor. */
        ForeignKeyDefinitionReference( // NOLINT(google-explicit-constructor)
                    ForeignIdColumnDefinitionReference foreignIdColumnReference);
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
        std::reference_wrapper<ColumnDefinition> m_columnDefinition;
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
