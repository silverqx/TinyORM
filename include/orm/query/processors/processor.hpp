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

    class SHAREDLIB_EXPORT Processor
    {
        Q_DISABLE_COPY(Processor)

    public:
        // CUR add comments silverqx
        Processor() = default;
        inline virtual ~Processor() = default;

        /*! Process the results of a column listing query. */
        virtual QStringList processColumnListing(QSqlQuery &query) const;
    };

} // namespace Orm::Query::Processors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // PROCESSOR_H
