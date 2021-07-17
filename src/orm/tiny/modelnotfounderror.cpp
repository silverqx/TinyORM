#include "orm/tiny/modelnotfounderror.hpp"

#include "orm/constants.hpp"

using namespace Orm::Constants;

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{

ModelNotFoundError::ModelNotFoundError(const char *model,
                                       const QVector<QVariant> &ids)
    : RuntimeError(formatMessage(model, ids))
    , m_model(model)
    , m_ids(ids)
{}

ModelNotFoundError::ModelNotFoundError(const QString &model,
                                       const QVector<QVariant> &ids)
    : ModelNotFoundError(model.toUtf8().constData(), ids)
{}

QString ModelNotFoundError::formatMessage(const char *model,
                                          const QVector<QVariant> &ids) const
{
    QString result = QStringLiteral("No query results for model '%1'")
                     .arg(model);

    if (!ids.isEmpty()) {
        const auto joinIds = [&ids]
        {
            QStringList result;
            result.reserve(ids.size());

            for (const auto &id : ids)
                result << id.value<QString>();

            return result.join(COMMA);
        };

        result += QStringLiteral(" with ids : %2").arg(joinIds());
    }

    result += QChar('.');

    return result;
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
