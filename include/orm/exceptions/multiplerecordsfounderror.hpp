#pragma once
#ifndef ORM_EXCEPTIONS_MULTIPLERECORDSFOUNDERROR_HPP
#define ORM_EXCEPTIONS_MULTIPLERECORDSFOUNDERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! Found more that one record (used by Builder::sole()). */
    class MultipleRecordsFoundError : public RuntimeError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Constructor. */
        inline explicit MultipleRecordsFoundError(int count, QString &&functionName);

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

    MultipleRecordsFoundError::MultipleRecordsFoundError(const int count,
                                                         QString &&functionName)
        : RuntimeError(QStringLiteral("%1 records were found in %2().")
                       .arg(count).arg(functionName)
                       .toUtf8().constData())
        , m_count(count)
        , m_functionName(std::move(functionName))
    {}

    int MultipleRecordsFoundError::count() const noexcept
    {
        return m_count;
    }

    const QString &MultipleRecordsFoundError::functionName() const noexcept
    {
        return m_functionName;
    }

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_MULTIPLERECORDSFOUNDERROR_HPP
