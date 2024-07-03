#pragma once
#ifndef ORM_SCHEMA_INDEXDEFINITIONREFERENCE_HPP
#define ORM_SCHEMA_INDEXDEFINITIONREFERENCE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{
    class IndexCommand;

    /*! Reference class to the ColumnDefinition, provides setters with a nice API
        for the database index. */
    class TINYORM_EXPORT IndexDefinitionReference
    {
    public:
        /*! Constructor. */
        IndexDefinitionReference(IndexCommand &indexCommand); // NOLINT(google-explicit-constructor)
        /*! Default destructor. */
        ~IndexDefinitionReference() = default;

        /*! Copy constructor. */
        IndexDefinitionReference(
                    const IndexDefinitionReference &) = default;
        /*! Move constructor. */
        IndexDefinitionReference(
                    IndexDefinitionReference &&) noexcept = default;

        /*! Deleted copy assignment operator. */
        IndexDefinitionReference &
        operator=(const IndexDefinitionReference &) = delete;
        /*! Deleted move assignment operator. */
        IndexDefinitionReference &
        operator=(IndexDefinitionReference &&) = delete;

        /*! Return the reference to underlying index command. */
        constexpr const IndexCommand &get() const noexcept;
        /*! Return the reference to underlying index command. */
        constexpr operator const IndexCommand &() const noexcept; // NOLINT(google-explicit-constructor)

        /*! Specify an algorithm for the index (MySQL/PostgreSQL). */
        IndexDefinitionReference &algorithm(const QString &algorithm);
        /*! Specify a language for the full text index (PostgreSQL). */
        IndexDefinitionReference &language(const QString &language);

    private:
        /*! Reference to an index command definition. */
        std::reference_wrapper<IndexCommand> m_indexCommand;
    };

    /* public */

    constexpr const IndexCommand &IndexDefinitionReference::get() const noexcept
    {
        return m_indexCommand.get();
    }

    constexpr IndexDefinitionReference::operator const IndexCommand &() const noexcept
    {
        return get();
    }

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_INDEXDEFINITIONREFERENCE_HPP
