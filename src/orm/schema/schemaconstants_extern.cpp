#include "orm/schema/schemaconstants_extern.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Qt::StringLiterals::operator""_s;

namespace Orm::SchemaNs::Constants
{

    // Command names
    const QString Create            = u"create"_s;
    const QString Add               = u"add"_s;
    const QString Change            = u"change"_s;
    const QString Drop              = u"drop"_s;
    const QString DropIfExists      = u"dropIfExists"_s;
    const QString Rename            = u"rename"_s;
    const QString DropColumn        = u"dropColumn"_s;
    const QString RenameColumn      = u"renameColumn"_s;
    const QString DropPrimary       = u"dropPrimary"_s;
    const QString DropUnique        = u"dropUnique"_s;
    const QString DropIndex         = u"dropIndex"_s;
    const QString DropFullText      = u"dropFullText"_s;
    const QString DropSpatialIndex  = u"dropSpatialIndex"_s;
    const QString DropForeign       = u"dropForeign"_s;
    const QString RenameIndex       = u"renameIndex"_s;

    // PostgreSQL specific command
    const QString Comment           = u"comment"_s;

    // MySQL and PostgreSQL specific commands
    const QString
    AutoIncrementStartingValue      = u"autoIncrementStartingValue"_s;
    const QString TableComment      = u"tableComment"_s;

    // Indexes
    const QString Primary      = u"primary"_s;
    const QString Unique       = u"unique"_s;
    const QString Index        = u"index"_s;
    const QString Fulltext     = u"fulltext"_s;
    const QString SpatialIndex = u"spatialIndex"_s;
    const QString Foreign      = u"foreign"_s;

    // Foreign constraints
    const QString Cascade  = u"cascade"_s;
    const QString Restrict = u"restrict"_s;
    const QString SetNull  = u"set null"_s;

    // Column types
    const QString integer_ = u"integer"_s;
    const QString varchar_ = u"varchar"_s;
    const QString float_   = u"float"_s;
    const QString blob_    = u"blob"_s;
    const QString bytea_   = u"bytea"_s;

    // Common strings
    const QString TRIM_QUOTES = u"'\""_s;

    // Column names
    const QString Uuid          = u"uuid"_s;
    const QString IpAddress     = u"ip_address"_s;
    const QString MacAddress    = u"mac_address"_s;
    const QString RememberToken = u"remember_token"_s;

} // namespace Orm::SchemaNs::Constants

TINYORM_END_COMMON_NAMESPACE
