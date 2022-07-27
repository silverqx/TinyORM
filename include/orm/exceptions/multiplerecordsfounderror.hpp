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
        inline explicit MultipleRecordsFoundError(int count);

        /*! Get the number of records found. */
        inline int count() const noexcept;

    protected:
        /*! The number of records found. */
        int m_count;
    };

    /* public */

    MultipleRecordsFoundError::MultipleRecordsFoundError(const int count)
        : RuntimeError(QStringLiteral("%1 records were found.").arg(count)
                       .toUtf8().constData())
        , m_count(count)
    {}

    int MultipleRecordsFoundError::count() const noexcept
    {
        return m_count;
    }

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_MULTIPLERECORDSFOUNDERROR_HPP
