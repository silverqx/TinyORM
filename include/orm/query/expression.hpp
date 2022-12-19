#pragma once
#ifndef ORM_QUERY_EXPRESSION_HPP
#define ORM_QUERY_EXPRESSION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>
#include <QVector>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query
{

    /*! Expression in sql query. */
    class Expression
    {
    public:
        /*! Default constructor, needed by Q_DECLARE_METATYPE. */
        inline Expression() noexcept = default;
        /*! Default destructor. */
        inline ~Expression() = default;

        /*! Converting constructor from the QVariant type. */
        inline explicit Expression(const QVariant &value);
        /*! Converting constructor from the QVariant type. */
        inline explicit Expression(QVariant &&value) noexcept;

        /*! Copy constructor. */
        inline Expression(const Expression &) = default;
        /*! Copy assignment operator. */
        inline Expression &operator=(const Expression &) = default;

        /*! Move constructor. */
        inline Expression(Expression &&) noexcept = default;
        /*! Move assignment operator. */
        inline Expression &operator=(Expression &&) noexcept = default;

        /*! Converting operator, QVariant(Expression). */
        inline operator QVariant() const; // NOLINT(google-explicit-constructor)

        /*! Obtain expression's value. */
        inline const QVariant &getValue() const noexcept;

        /*! Equality comparison operator for the Expression. */
        inline bool operator==(const Expression &) const = default;

    private:
        /*! Expression's value. */
        QVariant m_value {};
    };

    /* public */

    // NOLINTNEXTLINE(modernize-pass-by-value)
    Expression::Expression(const QVariant &value)
        : m_value(value)
    {}

    Expression::Expression(QVariant &&value) noexcept
        : m_value(std::move(value))
    {}

    Expression::operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }

    const QVariant &Expression::getValue() const noexcept
    {
        return m_value;
    }

} // namespace Orm::Query

TINYORM_END_COMMON_NAMESPACE

#ifdef TINYORM_COMMON_NAMESPACE
Q_DECLARE_METATYPE(TINYORM_COMMON_NAMESPACE::Orm::Query::Expression) // NOLINT(performance-no-int-to-ptr, misc-no-recursion)
#else
Q_DECLARE_METATYPE(Orm::Query::Expression) // NOLINT(performance-no-int-to-ptr, misc-no-recursion)
#endif

#endif // ORM_QUERY_EXPRESSION_HPP
