#pragma once
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QVariant>
#include <QVector>

#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query
{

    // FEATURE expressions, rework saving Expressions to the "BindingsMap m_bindings", see also todo at BindingsMap definition in ormtypes.hpp silverqx
    class SHAREDLIB_EXPORT Expression
    {
    public:
        /*! Default constructor, needed by Q_DECLARE_METATYPE. */
        Expression() = default;
        /*! Destructor. */
        ~Expression() = default;

        /*! Converting constructor from QVariant type. */
        explicit Expression(const QVariant &value);
        /*! Converting constructor from QVariant type. */
        explicit Expression(QVariant &&value);

        /*! Copy constructor. */
        Expression(const Expression &) = default;
        /*! Copy assignment operator. */
        Expression &operator=(const Expression &) = default;

        /*! Move constructor. */
        Expression(Expression &&) = default;
        /*! Move assignment operator. */
        Expression &operator=(Expression &&) = default;

        /*! Converting operator, QVariant(Expression). */
        operator QVariant() const;

        /*! Obtain expression's value. */
        QVariant getValue() const;

    private:
        /*! Expression's value. */
        QVariant m_value;
    };

    inline QVariant Expression::getValue() const
    {
        return m_value;
    }

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#ifdef TINYORM_COMMON_NAMESPACE
Q_DECLARE_METATYPE(TINYORM_COMMON_NAMESPACE::Orm::Query::Expression)
#else
Q_DECLARE_METATYPE(Orm::Query::Expression)
#endif

#endif // EXPRESSION_H
