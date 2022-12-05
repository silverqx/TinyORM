#include "orm/tiny/exceptions/relationnotloadederror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Exceptions
{

/* public */

RelationNotLoadedError::RelationNotLoadedError(const QString &model,
                                               const QString &relation)
    : RuntimeError(formatMessage(model, relation))
    , m_model(model)
    , m_relation(relation)
{}

/* private */

QString RelationNotLoadedError::formatMessage(const QString &model,
                                              const QString &relation)
{
    return QStringLiteral("Undefined relation '%1' (in the m_relation data memeber) "
                          "on model '%2', the relation was not loaded.")
            .arg(relation, model);
}

} // namespace Orm::Tiny::Exceptions

TINYORM_END_COMMON_NAMESPACE
