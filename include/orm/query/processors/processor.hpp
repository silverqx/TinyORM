#pragma once
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QStringList>
#include <QtGlobal>

#include "orm/utils/export.hpp"

class QSqlQuery;

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Processors
{

    /*! Base processor class, process sql result. */
    class SHAREDLIB_EXPORT Processor
    {
        Q_DISABLE_COPY(Processor)

    public:
        /*! Default constructor. */
        Processor() = default;
        /*! Virtual destructor. */
        inline virtual ~Processor() = default;

        /*! Process the results of a column listing query. */
        virtual QStringList processColumnListing(QSqlQuery &query) const;
    };

} // namespace Orm::Query::Processors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // PROCESSOR_H
