#include "orm/drivers/mysql/concerns/populatesfielddefaultvalues_p.hpp"

#include "orm/drivers/mysql/mysqlconstants_p.hpp"
#include "orm/drivers/mysql/mysqldriver_p.hpp"
#include "orm/drivers/mysql/mysqlresult_p.hpp"
#include "orm/drivers/sqlquery.hpp"
#include "orm/drivers/sqlresult.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Drivers::MySql::Constants::EMPTY;
using Orm::Drivers::MySql::MySqlResultPrivate;

namespace Orm::Drivers::MySql::Concerns
{

/* By extracting this method to own Concern, the SqlQuery and SqlResult dependency
   for MySqlResultPrivate was dropped. These classes are required to make a database query
   and the MySqlResultPrivate class doesn't need them to function properly, in short,
   they have nothing to do there. 😮🕺 */

/* private */

/* Result sets */

void PopulatesFieldDefaultValuesPrivate::populateFieldDefaultValuesInternal(
        SqlRecord &record, const bool allColumns) const
{
    // Nothing to do
    if (record.isEmpty())
        return;

    const auto &mySqlResultPrivate = this->mySqlResultPrivate();
    const auto &sqldriver = mySqlResultPrivate.sqldriver;

    // Ownership of the shared_ptr()
    const auto driver = sqldriver.lock();

    SqlQuery sqlQuery(driver->createResult());

    // Prepare the query string and field names based on the allColumns
    const auto [queryString, fieldNames] = prepareQueryStringAndFieldNames(
                                               record, allColumns);

    sqlQuery.prepare(queryString);

    sqlQuery.addBindValue(mySqlResultPrivate.drv_d_func()->databaseName);
    // This should never happen :/
    Q_ASSERT(record.contains(0));
    // Table name from the field 0 is guaranteed
    sqlQuery.addBindValue(record.field(0).tableName());
    // Bind all field names
    if (!allColumns && fieldNames) // fieldNames check to suppress Clang Tidy
        for (const auto &fieldName : *fieldNames)
            sqlQuery.addBindValue(fieldName); // Don't move because QVariant(fieldName)

    sqlQuery.exec();

    constexpr static SqlQuery::size_type COLUMN_NAME = 0;
    constexpr static SqlQuery::size_type COLUMN_DEFAULT = 1;

    /* The sqlQuery.value(1) QVariant's type is database dependent, MySQL returns
       QByteArray because it has set the BINARY attribute on the COLUMN_DEFAULT column
       because it uses the utf8mb3_bin collation, the flags=144 (BLOB_FLAG, BINARY_FLAG).
       I spent a lot of time on this to find out. 🤔
       MariaDB uses the utf8mb3_general_ci so it returns the QString,
       flags=4112 (BLOB_FLAG, NO_DEFAULT_VALUE_FLAG). */
    while (sqlQuery.next())
        record.fieldInternal(sqlQuery.value(COLUMN_NAME).value<QString>())
              .setDefaultValue(sqlQuery.value(COLUMN_DEFAULT));
}

PopulatesFieldDefaultValuesPrivate::QueryStringAndFieldNames
PopulatesFieldDefaultValuesPrivate::prepareQueryStringAndFieldNames(
        const SqlRecord &record, const bool allColumns)
{
    // The order by ORDINAL_POSITION not needed (also it's better for performance w/o it)
    static const auto
    queryStringTmpl = u"select `COLUMN_NAME`, `COLUMN_DEFAULT` "
                       "from `information_schema`.`columns` "
                       "where `TABLE_SCHEMA` = ? and `TABLE_NAME` = ?%1"_s;
    static const auto columnNamesTmpl = u" and `COLUMN_NAME` in (%1)"_s;

    // Select the Default Column Values for all columns
    if (allColumns)
        return {queryStringTmpl.arg(EMPTY), std::nullopt};

    /* Get all field names in the given record and select the Default Column Values only
       for them as we know these field names in advance. */
    auto fieldNames = record.fieldNames();

    return {queryStringTmpl.arg(columnNamesTmpl.arg(u"?, "_s.repeated(fieldNames.size())
                                                            .chopped(2))),
            std::move(fieldNames)};
}

/* Others */

const MySqlResultPrivate &
PopulatesFieldDefaultValuesPrivate::mySqlResultPrivate() const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
    return static_cast<const MySqlResultPrivate &>(*this);
}

} // namespace Orm::Drivers::MySql::Concerns

TINYORM_END_COMMON_NAMESPACE
