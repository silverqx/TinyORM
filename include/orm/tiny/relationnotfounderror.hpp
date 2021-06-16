#pragma once
#ifndef RELATIONNOTFOUNDERROR_H
#define RELATIONNOTFOUNDERROR_H

#include "orm/runtimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{

    /*! Check whether the relation was defined in the u_relation data member, called
        from Model::validateUserRelation(). */
    class SHAREDLIB_EXPORT RelationNotFoundError : public RuntimeError
    {
    public:
        /*! Exception message will be generated on the base of this enum struct. */
        enum struct From
        {
            UNDEFINED       = -1,
            BELONGS_TO      = 0,
            BELONGS_TO_MANY = 1,
        };

        RelationNotFoundError(const QString &model, const QString &relation);
        RelationNotFoundError(const QString &model, const QString &relation, From from);

        /*! Get the affected Eloquent model. */
        inline const QString &getModel() const
        { return m_model; }
        /*! Get the name of the relation. */
        inline const QString &getRelation() const
        { return m_relation; }

    protected:
        /*! The name of the affected Eloquent model.. */
        const QString m_model;
        /*! The name of the relation. */
        const QString m_relation;
        /*! Exception message will be generated on the base of this data member. */
        const From m_from;

    private:
        /*! Format the error message. */
        QString formatMessage(const QString &model, const QString &relation) const;
    };

} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // RELATIONNOTFOUNDERROR_H
