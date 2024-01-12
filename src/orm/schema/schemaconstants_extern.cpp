#include "orm/schema/schemaconstants_extern.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs::Constants
{

    // Command names
    const QString Create            = QStringLiteral("create");
    const QString Add               = QStringLiteral("add");
    const QString Change            = QStringLiteral("change");
    const QString Drop              = QStringLiteral("drop");
    const QString DropIfExists      = QStringLiteral("dropIfExists");
    const QString Rename            = QStringLiteral("rename");
    const QString DropColumn        = QStringLiteral("dropColumn");
    const QString RenameColumn      = QStringLiteral("renameColumn");
    const QString DropPrimary       = QStringLiteral("dropPrimary");
    const QString DropUnique        = QStringLiteral("dropUnique");
    const QString DropIndex         = QStringLiteral("dropIndex");
    const QString DropFullText      = QStringLiteral("dropFullText");
    const QString DropSpatialIndex  = QStringLiteral("dropSpatialIndex");
    const QString DropForeign       = QStringLiteral("dropForeign");
    const QString RenameIndex       = QStringLiteral("renameIndex");

    // PostgreSQL specific command
    const QString Comment           = QStringLiteral("comment");

    // MySQL and PostgreSQL specific commands
    const QString
    AutoIncrementStartingValue      = QStringLiteral("autoIncrementStartingValue");
    const QString TableComment      = QStringLiteral("tableComment");

    // Indexes
    const QString Primary      = QStringLiteral("primary");
    const QString Unique       = QStringLiteral("unique");
    const QString Index        = QStringLiteral("index");
    const QString Fulltext     = QStringLiteral("fulltext");
    const QString SpatialIndex = QStringLiteral("spatialIndex");
    const QString Foreign      = QStringLiteral("foreign");

    // Foreign constraints
    const QString Cascade  = QStringLiteral("cascade");
    const QString Restrict = QStringLiteral("restrict");
    const QString SetNull  = QStringLiteral("set null");

    // Column types
    const QString integer_ = QStringLiteral("integer");
    const QString varchar_ = QStringLiteral("varchar");
    const QString float_   = QStringLiteral("float");
    const QString blob_    = QStringLiteral("blob");
    const QString bytea_   = QStringLiteral("bytea");

    // Common strings
    const QString TRIM_QUOTES  = QStringLiteral("'\"");

} // namespace Orm::SchemaNs::Constants

TINYORM_END_COMMON_NAMESPACE
