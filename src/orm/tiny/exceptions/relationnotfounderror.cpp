#include "orm/tiny/exceptions/relationnotfounderror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Exceptions
{

RelationNotFoundError::RelationNotFoundError(const QString &model,
                                             const QString &relation,
                                             const From from)
    : RuntimeError(formatMessage(model, relation, from))
    , m_model(model)
    , m_relation(relation)
    , m_from(from)
{}

QString RelationNotFoundError::formatMessage(const QString &model,
                                             const QString &relation,
                                             const From from) const
{
    static const auto belongsToMessage =
            QStringLiteral(
                "Can not guess the relationship name for '%2' relation type, "
                "currently guessed relationship name is '%3', but it does not exist "
                "in the '%1::u_relation' data member. Please pass the third parameter "
                "to the 'Model::%2' method called from the '%1' model, the __func__ "
                "predefined identifier is ideal for this.");

    switch (from) {
    case From::BELONGS_TO:
        return belongsToMessage.arg(model, QStringLiteral("belongsTo"), relation);

    case From::BELONGS_TO_MANY:
        return belongsToMessage.arg(model, QStringLiteral("belongsToMany"), relation);

    default:
        return QStringLiteral(
                    "Call to undefined relationship '%1' (in the u_relation data "
                    "member) on model '%2'.")
                .arg(relation, model);
    }
}

} // namespace Orm::Tiny::Exceptions

TINYORM_END_COMMON_NAMESPACE
