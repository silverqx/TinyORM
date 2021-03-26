#include "orm/tiny/relationnotfounderror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{

RelationNotFoundError::RelationNotFoundError(const QString &model,
                                             const QString &relation)
    : RuntimeError(formatMessage(model, relation))
    , m_model(model)
    , m_relation(relation)
{}

QString RelationNotFoundError::formatMessage(const QString &model,
                                             const QString &relation) const
{
    return QStringLiteral(
                "Call to undefined relationship '%1' (in the u_relation data member) "
                "on model '%2'.")
            .arg(relation, model);
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
