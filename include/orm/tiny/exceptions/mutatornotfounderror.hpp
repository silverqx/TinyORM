#pragma once
#ifndef ORM_TINY_EXCEPTIONS_MUTATORNOTFOUNDERROR_HPP
#define ORM_TINY_EXCEPTIONS_MUTATORNOTFOUNDERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Exceptions
{

    /*! Mutator mapping not found exception, check whether the mutator/accessor name was
        defined in the u_mutators data member. */
    class MutatorNotFoundError : public Orm::Exceptions::RuntimeError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Constructor. */
        inline MutatorNotFoundError(QString model, const QString &mutator);

        /*! Get the affected TinyORM model. */
        inline const QString &getModel() const noexcept;
        /*! Get a mutator/accessor name. */
        inline const QString &getMutator() const noexcept;

    protected:
        /*! The name of the affected TinyORM model. */
        QString m_model;
        /*! The mutator/accessor name. */
        QString m_mutator;

    private:
        /*! Format the error message. */
        inline static QString
        formatMessage(const QString &model, const QString &mutator);
    };

    /* public */

    MutatorNotFoundError::MutatorNotFoundError(QString model, const QString &mutator)
        : RuntimeError(formatMessage(model, mutator))
        , m_model(std::move(model))
        , m_mutator(mutator)
    {}

    const QString &
    MutatorNotFoundError::getModel() const noexcept
    {
        return m_model;
    }

    const QString &
    MutatorNotFoundError::getMutator() const noexcept
    {
        return m_mutator;
    }

    /* private */

    QString
    MutatorNotFoundError::formatMessage(const QString &model, const QString &mutator)
    {
        return QStringLiteral(
                    "The mutator mapping '%1' doesn't exist in the 'u_mutators' map "
                    "static data member for the '%2' model.")
                .arg(mutator, model);
    }

} // namespace Orm::Tiny::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_EXCEPTIONS_MUTATORNOTFOUNDERROR_HPP
