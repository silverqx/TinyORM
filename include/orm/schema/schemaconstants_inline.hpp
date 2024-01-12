#pragma once
#ifndef ORM_SCHEMA_SCHEMACONSTANTS_INLINE_HPP
#define ORM_SCHEMA_SCHEMACONSTANTS_INLINE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

/*! Namespace contains common chars and strings used in the Schema namespace. */
namespace Orm::SchemaNs
{
namespace Constants
{

    // Command names
    inline const QString Create           = QStringLiteral("create");
    inline const QString Add              = QStringLiteral("add");
    inline const QString Change           = QStringLiteral("change");
    inline const QString Drop             = QStringLiteral("drop");
    inline const QString DropIfExists     = QStringLiteral("dropIfExists");
    inline const QString Rename           = QStringLiteral("rename");
    inline const QString DropColumn       = QStringLiteral("dropColumn");
    inline const QString RenameColumn     = QStringLiteral("renameColumn");
    inline const QString DropPrimary      = QStringLiteral("dropPrimary");
    inline const QString DropUnique       = QStringLiteral("dropUnique");
    inline const QString DropIndex        = QStringLiteral("dropIndex");
    inline const QString DropFullText     = QStringLiteral("dropFullText");
    inline const QString DropSpatialIndex = QStringLiteral("dropSpatialIndex");
    inline const QString DropForeign      = QStringLiteral("dropForeign");
    inline const QString RenameIndex      = QStringLiteral("renameIndex");

    // PostgreSQL specific command
    inline const QString Comment          = QStringLiteral("comment");

    // MySQL and PostgreSQL specific commands
    inline const QString
    AutoIncrementStartingValue            = QStringLiteral("autoIncrementStartingValue");
    inline const QString TableComment     = QStringLiteral("tableComment");

    // Indexes
    inline const QString Primary      = QStringLiteral("primary");
    inline const QString Unique       = QStringLiteral("unique");
    inline const QString Index        = QStringLiteral("index");
    // CUR schema, I fucked up case in fulltext and spatialIndex during replacement, have to check all occurences :( silverqx
    inline const QString Fulltext     = QStringLiteral("fulltext");
    inline const QString SpatialIndex = QStringLiteral("spatialIndex");
    inline const QString Foreign      = QStringLiteral("foreign");

    // Foreign constraints
    inline const QString Cascade  = QStringLiteral("cascade");
    inline const QString Restrict = QStringLiteral("restrict");
    inline const QString SetNull  = QStringLiteral("set null");

    // Column types
    inline const QString integer_ = QStringLiteral("integer");
    inline const QString varchar_ = QStringLiteral("varchar");
    inline const QString float_   = QStringLiteral("float");
    inline const QString blob_    = QStringLiteral("blob");
    inline const QString bytea_   = QStringLiteral("bytea");

    // Common strings
    inline const QString TRIM_QUOTES  = QStringLiteral("'\"");

} // namespace Constants

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace Orm::SchemaNs::Constants;

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_SCHEMACONSTANTS_INLINE_HPP
