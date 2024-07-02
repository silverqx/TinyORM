#include "orm/tiny/exceptions/modelnotfounderror.hpp"

#include "orm/constants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::COMMA;
using Orm::Constants::DOT;

namespace Orm::Tiny::Exceptions
{

/* public */

ModelNotFoundError::ModelNotFoundError(const char *model,
                                       const QList<QVariant> &ids)
    : RuntimeError(formatMessage(model, ids))
    , m_model(model)
    , m_ids(ids)
{}

ModelNotFoundError::ModelNotFoundError(const QString &model,
                                       const QList<QVariant> &ids)
    : ModelNotFoundError(model.toUtf8().constData(), ids)
{}

/* private */

QString ModelNotFoundError::formatMessage(const char *model,
                                          const QList<QVariant> &ids)
{
    QString result = QStringLiteral("No query results for model '%1'")
                     .arg(model);

    if (!ids.isEmpty()) {
        const auto joinIds = [&ids]
        {
            QStringList idsList;
            idsList.reserve(ids.size());

            for (const auto &id : ids)
                idsList << id.value<QString>();

            return idsList.join(COMMA);
        };

        result += QStringLiteral(" with IDs : %2").arg(joinIds());
    }

    result += DOT;

    return result;
}

} // namespace Orm::Tiny::Exceptions

TINYORM_END_COMMON_NAMESPACE
