#pragma once
#ifndef RELATIONNOTFOUNDERROR_H
#define RELATIONNOTFOUNDERROR_H

#include "orm/exceptions/runtimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Exceptions
{

    /*! Relation not found exception, check whether the relation was defined
        in the u_relation data member, called from Model::validateUserRelation(). */
    class SHAREDLIB_EXPORT RelationNotFoundError : public Orm::Exceptions::RuntimeError
    {
    public:
        /*! Exception message will be generated on the base of this enum struct. */
        enum struct From
        {
            UNDEFINED       = -1,
            BELONGS_TO      = 0,
            BELONGS_TO_MANY = 1,
        };

        /*! Constructor. */
        RelationNotFoundError(const QString &model, const QString &relation,
                              From from = From::UNDEFINED);

        /*! Get the affected TinyORM model. */
        const QString &getModel() const;
        /*! Get the name of the relation. */
        const QString &getRelation() const;

    protected:
        /*! The name of the affected TinyORM model.. */
        const QString m_model;
        /*! The name of the relation. */
        const QString m_relation;
        /*! Exception message will be generated on the base of this data member. */
        const From m_from;

    private:
        /*! Format the error message. */
        QString formatMessage(const QString &model, const QString &relation,
                              const From from) const;
    };

    inline const QString &
    RelationNotFoundError::getModel() const
    {
        return m_model;
    }

    inline const QString &
    RelationNotFoundError::getRelation() const
    {
        return m_relation;
    }

} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // RELATIONNOTFOUNDERROR_H
