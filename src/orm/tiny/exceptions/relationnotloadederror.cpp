#include "orm/tiny/exceptions/relationnotloadederror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Exceptions
{

RelationNotLoadedError::RelationNotLoadedError(const QString &model,
                                               const QString &relation)
    : RuntimeError(formatMessage(model, relation))
    , m_model(model)
    , m_relation(relation)
{}

QString RelationNotLoadedError::formatMessage(const QString &model,
                                              const QString &relation) const
{
    return QStringLiteral("Undefined relation '%1' (in the m_relation data memeber) "
                          "on model '%2', the relation was not loaded.")
            .arg(relation, model);
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
