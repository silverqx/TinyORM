#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "export.hpp"

class QSqlQuery;

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Processors
{

    // CUR implement missing methods silverqx
    class SHAREDLIB_EXPORT Processor
    {
        Q_DISABLE_COPY(Processor)

    public:
        Processor() = default;
        virtual ~Processor() = default;

        /*! Process the results of a column listing query. */
        QStringList processColumnListing(QSqlQuery &query) const;
    };

} // namespace Orm::Query::Processors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // PROCESSOR_H
