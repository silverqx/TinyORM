#include "orm/query/processors/processor.hpp"

#include "orm/exceptions/runtimeerror.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Processors
{

QStringList Processor::processColumnListing(QSqlQuery &) const
{
    throw Exceptions::RuntimeError(QStringLiteral("Method %1() is not implemented.")
                                   .arg(__tiny_func__));
}

} // namespace Orm::Query::Processors

TINYORM_END_COMMON_NAMESPACE
