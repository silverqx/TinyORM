#pragma once
#ifndef ORM_EXCEPTIONS_MULTIPLECOLUMNSSELECTEDERROR_HPP
#define ORM_EXCEPTIONS_MULTIPLECOLUMNSSELECTEDERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! Found more that one column (used by DatabaseConnection::scalar()). */
    class MultipleColumnsSelectedError : public RuntimeError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Constructor. */
        inline explicit MultipleColumnsSelectedError(int count, QString &&functionName);

        /*! Get the number of records found. */
        inline int count() const noexcept;
        /*! Get a function name where the exception occurred. */
        inline const QString &functionName() const noexcept;

    protected:
        /*! The number of records found. */
        int m_count;
        /*! Function name where the exception occurred. */
        QString m_functionName;
    };

    /* public */

    MultipleColumnsSelectedError::MultipleColumnsSelectedError(const int count,
                                                               QString &&functionName)
        : RuntimeError(QStringLiteral("%1 columns were selected in %2(), select only "
                                      "one column with the scalar() method.")
                       .arg(count).arg(functionName)
                       .toUtf8().constData())
        , m_count(count)
        , m_functionName(std::move(functionName))
    {}

    int MultipleColumnsSelectedError::count() const noexcept
    {
        return m_count;
    }

    const QString &MultipleColumnsSelectedError::functionName() const noexcept
    {
        return m_functionName;
    }

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_MULTIPLECOLUMNSSELECTEDERROR_HPP
