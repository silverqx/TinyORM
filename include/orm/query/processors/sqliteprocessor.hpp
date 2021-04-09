#ifndef SQLITEPROCESSOR_H
#define SQLITEPROCESSOR_H

#include "orm/query/processors/processor.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Processors
{

    class SHAREDLIB_EXPORT SQLiteProcessor : public Processor
    {
        Q_DISABLE_COPY(SQLiteProcessor)

    public:
        SQLiteProcessor() = default;
    };

} // namespace Orm::Query::Processors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SQLITEPROCESSOR_H
