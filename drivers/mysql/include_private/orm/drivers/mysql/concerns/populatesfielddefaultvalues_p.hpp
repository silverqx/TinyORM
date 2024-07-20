#pragma once
#ifndef ORM_DRIVERS_MYSQL_CONCERNS_POPULATESFIELDDEFAULTVALUES_P_HPP
#define ORM_DRIVERS_MYSQL_CONCERNS_POPULATESFIELDDEFAULTVALUES_P_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QStringList>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{
    class SqlRecord;

namespace MySql
{
    class MySqlResultPrivate;

namespace Concerns
{

    /*! Concept for the SqlRecord passed to the populateFieldDefaultValues() method. */
    template<typename T>
    concept PopulateRecordConcept = std::convertible_to<T, SqlRecord &&> ||
                                    std::convertible_to<T, SqlRecord &>;

    /*! Populate the Default Column Values for the given SqlRecord. */
    class PopulatesFieldDefaultValuesPrivate
    {
        Q_DISABLE_COPY_MOVE(PopulatesFieldDefaultValuesPrivate)

    public:
        /*! Default constructor. */
        PopulatesFieldDefaultValuesPrivate() = default;
        /*! Default destructor. */
        ~PopulatesFieldDefaultValuesPrivate() = default;

        /* Result sets */
        /*! Populate the Default Column Values for the given SqlRecord. */
        template<PopulateRecordConcept T>
        T &&populateFieldDefaultValues(T &&record, bool allColumns = false) const;

    private:
        /* Result sets */
        /*! Populate the Default Column Values for the given SqlRecord. */
        void populateFieldDefaultValuesInternal(SqlRecord &record, bool allColumns) const;

        /*! The return type for query string and field names based on the allColumns. */
        struct QueryStringAndFieldNames
        {
            /*! Query string to select the Default Column Values. */
            QString queryString;
            /*! Field names for which to select the Default Column Values. */
            std::optional<QStringList> fieldNames;
        };

        /*! Prepare the query string and field names based on the allColumns. */
        static QueryStringAndFieldNames
        prepareQueryStringAndFieldNames(const SqlRecord &record, bool allColumns);

        /* Others */
        /*! Static cast *this to the MySqlResultPrivate & derived type. */
        const MySqlResultPrivate &mySqlResultPrivate() const noexcept;
    };

    /* public */

    /* Result sets */

    template<PopulateRecordConcept T>
    T &&
    PopulatesFieldDefaultValuesPrivate::populateFieldDefaultValues(
            T &&record, const bool allColumns) const
    {
        populateFieldDefaultValuesInternal(record, allColumns);

        return std::forward<T>(record);
    }

} // namespace Concerns
} // namespace MySql
} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_CONCERNS_POPULATESFIELDDEFAULTVALUES_P_HPP
