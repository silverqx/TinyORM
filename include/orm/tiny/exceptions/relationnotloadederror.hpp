#pragma once
#ifndef ORM_RELATIONNOTLOADEDERROR_HPP
#define ORM_RELATIONNOTLOADEDERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Exceptions
{

    /*! Relation not loaded exception, called from Model::getRelation(), when
        the relation was not loaded. */
    class SHAREDLIB_EXPORT RelationNotLoadedError : public Orm::Exceptions::RuntimeError
    {
    public:
        /*! Constructor. */
        RelationNotLoadedError(const QString &model, const QString &relation);

        /*! Get the affected TinyORM model. */
        const QString &getModel() const;
        /*! Get the name of the relation. */
        const QString &getRelation() const;

    protected:
        /*! The name of the affected TinyORM model.. */
        const QString m_model;
        /*! The name of the relation. */
        const QString m_relation;

    private:
        /*! Format the error message. */
        QString formatMessage(const QString &model, const QString &relation) const;
    };

    inline const QString &
    RelationNotLoadedError::getModel() const
    {
        return m_model;
    }

    inline const QString &
    RelationNotLoadedError::getRelation() const
    {
        return m_relation;
    }

} // namespace Orm::Tiny::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_RELATIONNOTLOADEDERROR_HPP
