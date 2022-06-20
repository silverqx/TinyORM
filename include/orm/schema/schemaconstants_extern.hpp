#pragma once
#ifndef ORM_SCHEMA_SCHEMACONSTANTS_EXTERN_HPP
#define ORM_SCHEMA_SCHEMACONSTANTS_EXTERN_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace constains common chars and strings used in the Schema. */
namespace Orm::SchemaNs
{
namespace Constants
{

    // Command names
    SHAREDLIB_EXPORT extern const QString Create;
    SHAREDLIB_EXPORT extern const QString Add;
    SHAREDLIB_EXPORT extern const QString Change;
    SHAREDLIB_EXPORT extern const QString Rename;
    SHAREDLIB_EXPORT extern const QString Drop;
    SHAREDLIB_EXPORT extern const QString DropIfExists;
    SHAREDLIB_EXPORT extern const QString DropColumn;
    SHAREDLIB_EXPORT extern const QString RenameColumn;
    SHAREDLIB_EXPORT extern const QString DropPrimary;
    SHAREDLIB_EXPORT extern const QString DropUnique;
    SHAREDLIB_EXPORT extern const QString DropIndex;
    SHAREDLIB_EXPORT extern const QString DropFullText;
    SHAREDLIB_EXPORT extern const QString DropSpatialIndex;
    SHAREDLIB_EXPORT extern const QString DropForeign;
    SHAREDLIB_EXPORT extern const QString RenameIndex;

    // PostgreSQL specific command
    SHAREDLIB_EXPORT extern const QString Comment;

    // Indexes
    SHAREDLIB_EXPORT extern const QString Primary;
    SHAREDLIB_EXPORT extern const QString Unique;
    SHAREDLIB_EXPORT extern const QString Index;
    SHAREDLIB_EXPORT extern const QString Fulltext;
    SHAREDLIB_EXPORT extern const QString SpatialIndex;
    SHAREDLIB_EXPORT extern const QString Foreign;

    // Foreign constraints
    SHAREDLIB_EXPORT extern const QString Cascade;
    SHAREDLIB_EXPORT extern const QString Restrict;
    SHAREDLIB_EXPORT extern const QString SetNull;

    // Column types
    SHAREDLIB_EXPORT extern const QString integer_;
    SHAREDLIB_EXPORT extern const QString varchar_;
    SHAREDLIB_EXPORT extern const QString float_;

} // namespace Constants

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace Orm::SchemaNs::Constants;

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_SCHEMACONSTANTS_EXTERN_HPP
