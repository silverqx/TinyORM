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
    /*! Alias for the literal operator that creates a QString. */
    using Qt::StringLiterals::operator""_s;

    // Command names
    inline const QString Create           = u"create"_s;
    inline const QString Add              = u"add"_s;
    inline const QString Change           = u"change"_s;
    inline const QString Drop             = u"drop"_s;
    inline const QString DropIfExists     = u"dropIfExists"_s;
    inline const QString Rename           = u"rename"_s;
    inline const QString DropColumn       = u"dropColumn"_s;
    inline const QString RenameColumn     = u"renameColumn"_s;
    inline const QString DropPrimary      = u"dropPrimary"_s;
    inline const QString DropUnique       = u"dropUnique"_s;
    inline const QString DropIndex        = u"dropIndex"_s;
    inline const QString DropFullText     = u"dropFullText"_s;
    inline const QString DropSpatialIndex = u"dropSpatialIndex"_s;
    inline const QString DropForeign      = u"dropForeign"_s;
    inline const QString RenameIndex      = u"renameIndex"_s;

    // PostgreSQL specific command
    inline const QString Comment          = u"comment"_s;

    // MySQL and PostgreSQL specific commands
    inline const QString
    AutoIncrementStartingValue            = u"autoIncrementStartingValue"_s;
    inline const QString TableComment     = u"tableComment"_s;

    // Indexes
    inline const QString Primary      = u"primary"_s;
    inline const QString Unique       = u"unique"_s;
    inline const QString Index        = u"index"_s;
    // CUR schema, I fucked up case in fulltext and spatialIndex during replacement, have to check all occurrences :( silverqx
    inline const QString Fulltext     = u"fulltext"_s;
    inline const QString SpatialIndex = u"spatialIndex"_s;
    inline const QString Foreign      = u"foreign"_s;

    // Foreign constraints
    inline const QString Cascade  = u"cascade"_s;
    inline const QString Restrict = u"restrict"_s;
    inline const QString SetNull  = u"set null"_s;

    // Column types
    inline const QString integer_ = u"integer"_s;
    inline const QString varchar_ = u"varchar"_s;
    inline const QString float_   = u"float"_s;
    inline const QString blob_    = u"blob"_s;
    inline const QString bytea_   = u"bytea"_s;

    // Common strings
    inline const QString TRIM_QUOTES  = u"'\""_s;

    // Column names
    inline const QString Uuid          = u"uuid"_s;
    inline const QString IpAddress     = u"ip_address"_s;
    inline const QString MacAddress    = u"mac_address"_s;
    inline const QString RememberToken = u"remember_token"_s;

} // namespace Constants

using namespace Orm::SchemaNs::Constants; // NOLINT(google-build-using-namespace)

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_SCHEMACONSTANTS_INLINE_HPP
