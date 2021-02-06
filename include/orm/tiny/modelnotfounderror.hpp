#ifndef MODELNOTFOUNDERROR_H
#define MODELNOTFOUNDERROR_H

#include "orm/ormruntimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{

    class SHAREDLIB_EXPORT ModelNotFoundError : public OrmRuntimeError
    {
    public:
        // TODO primarykey dilema, Model::KeyType vs QVariant silverqx
        // TODO perf add overload ctor for QVector<quint64>, better QVector<Model::KeyType> silverqx
        explicit ModelNotFoundError(const char *model,
                                    const QVector<QVariant> &ids = {});
        explicit ModelNotFoundError(const QString &model,
                                    const QVector<QVariant> &ids = {});

        /*! Get the affected Eloquent model. */
        inline const QString &getModel() const
        { return m_model; }
        /*! Get the affected Eloquent model IDs. */
        inline const QVector<QVariant> getIds() const
        { return m_ids; }

    protected:
        /*! Name of the affected Eloquent model. */
        const QString m_model;
        /*! The affected model IDs. */
        const QVector<QVariant> m_ids;

    private:
        /*! Format the error message. */
        QString formatMessage(const char *model,
                              const QVector<QVariant> &ids = {}) const;
    };

} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MODELNOTFOUNDERROR_H
