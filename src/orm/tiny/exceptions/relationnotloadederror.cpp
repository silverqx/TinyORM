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
    using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

    return u"Undefined relation '%1' (in the m_relation data memeber) on model '%2', "
            "the relation was not loaded."_s
            .arg(relation, model);
}

} // namespace Orm::Tiny::Exceptions

TINYORM_END_COMMON_NAMESPACE
