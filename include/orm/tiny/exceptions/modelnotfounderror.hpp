#pragma once
#ifndef MODELNOTFOUNDERROR_H
#define MODELNOTFOUNDERROR_H

#include <QVariant>

#include "orm/exceptions/runtimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Exceptions
{

    class SHAREDLIB_EXPORT ModelNotFoundError : public Orm::Exceptions::RuntimeError
    {
    public:
        // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
        // TODO perf add overload ctor for QVector<quint64>, better QVector<Model::KeyType> silverqx
        explicit ModelNotFoundError(const char *model,
                                    const QVector<QVariant> &ids = {});
        explicit ModelNotFoundError(const QString &model,
                                    const QVector<QVariant> &ids = {});

        /*! Get the affected TinyORM model. */
        const QString &getModel() const;
        /*! Get the affected TinyORM model IDs. */
        const QVector<QVariant> getIds() const;

    protected:
        /*! Name of the affected TinyORM model. */
        const QString m_model;
        /*! The affected model IDs. */
        const QVector<QVariant> m_ids;

    private:
        /*! Format the error message. */
        QString formatMessage(const char *model,
                              const QVector<QVariant> &ids = {}) const;
    };

    inline const QString &
    ModelNotFoundError::getModel() const
    {
        return m_model;
    }

    inline const QVector<QVariant>
    ModelNotFoundError::getIds() const
    {
        return m_ids;
    }

} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MODELNOTFOUNDERROR_H
