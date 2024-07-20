#include "orm/drivers/concerns/selectsallcolumnswithlimit0.hpp"

#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/sqlquery.hpp"
#include "orm/drivers/sqlresult.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

namespace Orm::Drivers::Concerns
{

/* By extracting this method to own Concern, the SqlQuery and SqlResult dependency
   for SqlDriver was dropped. These classes are required to make a database query
   and the SqlDriver class doesn't need them to function properly, in short,
   they have nothing to do there. 😮🕺 */

/* public */

SqlQuery
SelectsAllColumnsWithLimit0::selectAllColumnsWithLimit0(
        const QString &table, const std::weak_ptr<SqlDriver> &driver) const
{
    const auto &sqlDriver = this->sqlDriver();

    SqlQuery query(sqlDriver.createResult(driver));

    /* Don't check if a table exists in the currently selected database because
       it doesn't make sense, leave the defaults on the database server.
       The user can select from any database if the database server allows it. */

    static const auto queryStringTmpl = u"select * from %1 limit 0"_s;

    /*! Alias for the TableName. */
    constexpr static auto TableName = SqlDriver::IdentifierType::TableName;

    query.exec(queryStringTmpl.arg(sqlDriver.escapeIdentifier(table, TableName)));

    return query;
}

/* private */

/* Others */

const SqlDriver &SelectsAllColumnsWithLimit0::sqlDriver() const
{
    return dynamic_cast<const SqlDriver &>(*this);
}

} // namespace Orm::Drivers::Concerns

TINYORM_END_COMMON_NAMESPACE
