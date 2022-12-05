#pragma once
#ifndef ORM_TINY_EXCEPTIONS_MODELNOTFOUNDERROR_HPP
#define ORM_TINY_EXCEPTIONS_MODELNOTFOUNDERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Exceptions
{

    /*! Model not found exception. */
    class SHAREDLIB_EXPORT ModelNotFoundError : public Orm::Exceptions::RuntimeError // clazy:exclude=copyable-polymorphic
    {
    public:
        // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
        // TODO perf add overload ctor for QVector<quint64>, better QVector<Model::KeyType> silverqx
        /*! const char * constructor. */
        explicit ModelNotFoundError(const char *model,
                                    const QVector<QVariant> &ids = {});
        /*! QString constructor. */
        explicit ModelNotFoundError(const QString &model,
                                    const QVector<QVariant> &ids = {});

        /*! Get the affected TinyORM model. */
        inline const QString &getModel() const;
        /*! Get the affected TinyORM model IDs. */
        inline const QVector<QVariant> &getIds() const;

    protected:
        /*! Name of the affected TinyORM model. */
        QString m_model;
        /*! The affected model IDs. */
        QVector<QVariant> m_ids;

    private:
        /*! Format the error message. */
        static QString formatMessage(const char *model,
                                     const QVector<QVariant> &ids = {});
    };

    /* public */

    const QString &
    ModelNotFoundError::getModel() const
    {
        return m_model;
    }

    const QVector<QVariant> &
    ModelNotFoundError::getIds() const
    {
        return m_ids;
    }

} // namespace Orm::Tiny::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_EXCEPTIONS_MODELNOTFOUNDERROR_HPP
