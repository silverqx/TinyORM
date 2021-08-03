#pragma once
#ifndef RELATIONNOTLOADEDERROR_H
#define RELATIONNOTLOADEDERROR_H

#include "orm/runtimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{

    /*! Called from Model::getRelation(), when the relation was not loaded. */
    class SHAREDLIB_EXPORT RelationNotLoadedError : public RuntimeError
    {
    public:
        RelationNotLoadedError(const QString &model, const QString &relation);

        /*! Get the affected TinyORM model. */
        inline const QString &getModel() const
        { return m_model; }
        /*! Get the name of the relation. */
        inline const QString &getRelation() const
        { return m_relation; }

    protected:
        /*! The name of the affected TinyORM model.. */
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

#endif // RELATIONNOTLOADEDERROR_H
