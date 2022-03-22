#pragma once
#ifndef ORM_SCHEMA_FOREIGNIDCOLUMNDEFINITIONREFERENCE_HPP
#define ORM_SCHEMA_FOREIGNIDCOLUMNDEFINITIONREFERENCE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/constants.hpp"
#include "orm/schema/foreignkeydefinitionreference.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    class Blueprint;

    /*! Reference class to the ColumnDefinition provides setters with a nice API using
        terser syntax for the foreign key column. */
    class SHAREDLIB_EXPORT ForeignIdColumnDefinitionReference :
            public ColumnDefinitionReference<ForeignIdColumnDefinitionReference>
    {
        // To access m_columnDefinition data member
        friend ForeignKeyDefinitionReference;

    public:
        /*! Constructor. */
        ForeignIdColumnDefinitionReference( // NOLINT(google-explicit-constructor)
                    Blueprint &blueprint,
                    ColumnDefinitionReference<> columnDefinitionReference);
        /*! Default destructor. */
        inline ~ForeignIdColumnDefinitionReference() = default;

        /*! Create a foreign key constraint on this column referencing the "id" column
            of the conventionally related table. */
        ForeignKeyDefinitionReference
        constrained(const QString &table = "", const QString &column = Constants::ID);

        /*! Specify the referenced columns. */
        ForeignKeyDefinitionReference references(const QVector<QString> &columns);
        /*! Specify the referenced column. */
        template<typename = void>
        ForeignKeyDefinitionReference references(const QString &column);

    private:
        /*! Reference to the schema builder blueprint instance. */
        Blueprint &m_blueprint;
        /*! Reference to a column definition. */
        std::reference_wrapper<ColumnDefinition> m_columnDefinition;
    };

    template<typename>
    ForeignKeyDefinitionReference
    ForeignIdColumnDefinitionReference::references(const QString &column)
    {
        return references(QVector<QString> {column});
    }

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_FOREIGNIDCOLUMNDEFINITIONREFERENCE_HPP
