#pragma once
#ifndef ORM_SCHEMA_FOREIGNIDCOLUMNDEFINITIONREFERENCE_HPP
#define ORM_SCHEMA_FOREIGNIDCOLUMNDEFINITIONREFERENCE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/constants.hpp"
#include "orm/schema/columndefinitionreference.hpp"
#include "orm/schema/foreignkeydefinitionreference.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    class Blueprint;

    /*! Reference class to the ColumnDefinition provides setters with a nice API using
        terser syntax for the foreign key column. */
    class TINYORM_EXPORT ForeignIdColumnDefinitionReference :
            public ColumnDefinitionReference<ForeignIdColumnDefinitionReference>
    {
    public:
        /*! Constructor. */
        ForeignIdColumnDefinitionReference( // NOLINT(google-explicit-constructor)
                Blueprint &blueprint,
                ColumnDefinitionReference<> columnDefinitionReference);
        /*! Default destructor. */
        ~ForeignIdColumnDefinitionReference() = default;

        /*! Copy constructor. */
        ForeignIdColumnDefinitionReference(
                    const ForeignIdColumnDefinitionReference &) = default;
        /*! Move constructor. */
        ForeignIdColumnDefinitionReference(
                    ForeignIdColumnDefinitionReference &&) noexcept = default;

        /*! Deleted copy assignment operator. */
        ForeignIdColumnDefinitionReference &
        operator=(const ForeignIdColumnDefinitionReference &) = delete;
        /*! Deleted move assignment operator. */
        ForeignIdColumnDefinitionReference &
        operator=(ForeignIdColumnDefinitionReference &&) = delete;

        /*! Create a foreign key constraint on this column referencing the "id" column
            of the conventionally related table. */
        ForeignKeyDefinitionReference
        constrained(const QString &table = Orm::Constants::EMPTY,
                    const QString &column = Orm::Constants::ID);

        /*! Specify the referenced columns. */
        ForeignKeyDefinitionReference references(const QList<QString> &columns);
        /*! Specify the referenced column. */
        template<typename = void>
        ForeignKeyDefinitionReference references(const QString &column);

    private:
        /*! Reference to the schema builder blueprint instance. */
        std::reference_wrapper<Blueprint> m_blueprint;
        /*! Reference to a column definition. */
        std::optional<ForeignKeyDefinitionReference>
                m_foreignKeyDefinitionReference = std::nullopt;
    };

    template<typename>
    ForeignKeyDefinitionReference
    ForeignIdColumnDefinitionReference::references(const QString &column)
    {
        return references(QList<QString> {column});
    }

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_FOREIGNIDCOLUMNDEFINITIONREFERENCE_HPP
