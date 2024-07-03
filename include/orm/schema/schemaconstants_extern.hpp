#pragma once
#ifndef ORM_SCHEMA_SCHEMACONSTANTS_EXTERN_HPP
#define ORM_SCHEMA_SCHEMACONSTANTS_EXTERN_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace contains common chars and strings used in the Schema namespace. */
namespace Orm::SchemaNs
{
namespace Constants
{

    // Command names
    TINYORM_EXPORT extern const QString Create;
    TINYORM_EXPORT extern const QString Add;
    TINYORM_EXPORT extern const QString Change;
    TINYORM_EXPORT extern const QString Rename;
    TINYORM_EXPORT extern const QString Drop;
    TINYORM_EXPORT extern const QString DropIfExists;
    TINYORM_EXPORT extern const QString DropColumn;
    TINYORM_EXPORT extern const QString RenameColumn;
    TINYORM_EXPORT extern const QString DropPrimary;
    TINYORM_EXPORT extern const QString DropUnique;
    TINYORM_EXPORT extern const QString DropIndex;
    TINYORM_EXPORT extern const QString DropFullText;
    TINYORM_EXPORT extern const QString DropSpatialIndex;
    TINYORM_EXPORT extern const QString DropForeign;
    TINYORM_EXPORT extern const QString RenameIndex;

    // PostgreSQL specific command
    TINYORM_EXPORT extern const QString Comment;

    // MySQL and PostgreSQL specific commands
    TINYORM_EXPORT extern const QString AutoIncrementStartingValue;
    TINYORM_EXPORT extern const QString TableComment;

    // Indexes
    TINYORM_EXPORT extern const QString Primary;
    TINYORM_EXPORT extern const QString Unique;
    TINYORM_EXPORT extern const QString Index;
    TINYORM_EXPORT extern const QString Fulltext;
    TINYORM_EXPORT extern const QString SpatialIndex;
    TINYORM_EXPORT extern const QString Foreign;

    // Foreign constraints
    TINYORM_EXPORT extern const QString Cascade;
    TINYORM_EXPORT extern const QString Restrict;
    TINYORM_EXPORT extern const QString SetNull;

    // Column types
    TINYORM_EXPORT extern const QString integer_;
    TINYORM_EXPORT extern const QString varchar_;
    TINYORM_EXPORT extern const QString float_;
    TINYORM_EXPORT extern const QString blob_;
    TINYORM_EXPORT extern const QString bytea_;

    // Common strings
    TINYORM_EXPORT extern const QString TRIM_QUOTES;

} // namespace Constants

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace Orm::SchemaNs::Constants;

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_SCHEMACONSTANTS_EXTERN_HPP
