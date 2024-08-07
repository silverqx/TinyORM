#pragma once
#ifndef ORM_TINY_EXCEPTIONS_RELATIONNOTFOUNDERROR_HPP
#define ORM_TINY_EXCEPTIONS_RELATIONNOTFOUNDERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Exceptions
{

    /*! Relation mapping not found exception, check whether the relation was defined
        in the u_relations data member, called from Model::validateUserRelation(). */
    class TINYORM_EXPORT RelationMappingNotFoundError : public Orm::Exceptions::RuntimeError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Exception message will be generated on the base of this enum struct. */
        enum struct From : qint8
        {
            /*! Tags undefined relation (used in exception messages). */
            UNDEFINED       = -1,
            /*! Tags Belongs to relation (used in exception messages). */
            BELONGS_TO      = 0,
            /*! Tags Belongs to many relation (used in exception messages). */
            BELONGS_TO_MANY = 1,
        };

        /*! Constructor. */
        RelationMappingNotFoundError(const QString &model, const QString &relation,
                                     From from = From::UNDEFINED);

        /*! Get the affected TinyORM model. */
        inline const QString &getModel() const noexcept;
        /*! Get the name of the relation. */
        inline const QString &getRelation() const noexcept;

    protected:
        /*! The name of the affected TinyORM model. */
        QString m_model;
        /*! The name of the relation. */
        QString m_relation;
        /*! Exception message will be generated on the base of this data member. */
        From m_from;

    private:
        /*! Format the error message. */
        static QString formatMessage(const QString &model, const QString &relation,
                                     From from);
    };

    /* public */

    const QString &
    RelationMappingNotFoundError::getModel() const noexcept
    {
        return m_model;
    }

    const QString &
    RelationMappingNotFoundError::getRelation() const noexcept
    {
        return m_relation;
    }

} // namespace Orm::Tiny::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_EXCEPTIONS_RELATIONNOTFOUNDERROR_HPP
