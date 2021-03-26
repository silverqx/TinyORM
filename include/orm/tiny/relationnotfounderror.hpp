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
        from BaseModel::validateUserRelation(). */
    class SHAREDLIB_EXPORT RelationNotFoundError : public RuntimeError
    {
    public:
        RelationNotFoundError(const QString &model, const QString &relation);

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

    private:
        /*! Format the error message. */
        QString formatMessage(const QString &model, const QString &relation) const;
    };

} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // RELATIONNOTFOUNDERROR_H
