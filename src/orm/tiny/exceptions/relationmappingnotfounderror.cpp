#include "orm/tiny/exceptions/relationmappingnotfounderror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Exceptions
{

/* public */

RelationMappingNotFoundError::RelationMappingNotFoundError(
        const QString &model, const QString &relation, const From from
)
    : RuntimeError(formatMessage(model, relation, from))
    , m_model(model)
    , m_relation(relation)
    , m_from(from)
{}

/* private */

QString
RelationMappingNotFoundError::formatMessage(
        const QString &model, const QString &relation, const From from)
{
    /* I was confused when I looked at this method after some time, but it's correct,
       it shows two kind of messages, one for the belongsTo/belongsToMany and one
       in the relationship was not found in the u_relations data member. So the default
       section is ok and is called by the validateUserRelation() without the second
       argument (UNDEFINED). */
    static const auto belongsToMessage =
            QStringLiteral(
                "Can not guess the relationship name for '%2' relation type, "
                "currently guessed relationship name is '%3', but it does not exist "
                "in the '%1::u_relation' data member. Please pass the 'relation' "
                "argument to the 'Model::%2' method called from the '%1' model, "
                "the __func__ predefined identifier is ideal for this "
                "(eg. QString::fromUtf8(__func__)).");

    switch (from) {
    case From::BELONGS_TO:
        return belongsToMessage.arg(model, QStringLiteral("belongsTo"), relation);

    case From::BELONGS_TO_MANY:
        return belongsToMessage.arg(model, QStringLiteral("belongsToMany"), relation);

    default:
        return QStringLiteral(
                    "The relationship mapping '%1' doesn't exist in the 'u_relations' "
                    "map static data member for the '%2' model.")
                .arg(relation, model);
    }
}

} // namespace Orm::Tiny::Exceptions

TINYORM_END_COMMON_NAMESPACE
