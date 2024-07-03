#pragma once
#ifndef ORM_BASEGRAMMAR_HPP
#define ORM_BASEGRAMMAR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/export.hpp"
#include "orm/ormconcepts.hpp"
#include "orm/ormtypes.hpp"
#include "orm/utils/container.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Query
{
    class Expression;
}

    /*! Concept for container passed to the parametrize() method (QVariantMap
        or QList<QString>). */
    template<typename T>
    concept Parametrize = std::same_as<T, QVariantMap> ||
                          std::same_as<T, QList<QVariant>>;

    /*! Base class for the database grammar. */
    class TINYORM_EXPORT BaseGrammar
    {
        Q_DISABLE_COPY_MOVE(BaseGrammar)

    protected:
        /*! Alias for the container utils. */
        using ContainerUtils = Orm::Utils::Container;
        /*! Alias for the Expression. */
        using Expression = Query::Expression;

    public:
        /*! Default constructor. */
        BaseGrammar() = default;
        /*! Pure virtual destructor. */
        inline virtual ~BaseGrammar() = 0;

        /*! Get the format for database stored dates. */
        virtual const QString &getDateFormat() const;
        /*! Get the format for database stored times. */
        virtual const QString &getTimeFormat() const;

        /*! Wrap a value in keyword identifiers. */
        QString wrap(const QString &value, bool prefixAlias = false) const;
        /*! Wrap a value in keyword identifiers. */
        QString wrap(const Column &value) const;

        /*! Wrap a table in keyword identifiers. */
        QString wrapTable(const QString &table) const;
        /*! Wrap a table in keyword identifiers. */
        QString wrapTable(const FromClause &table) const;

        /*! Wrap an array of values. */
        template<ColumnContainer T>
        QList<QString> wrapArray(const T &values) const;

        /*! Quote the given string literal. */
        static QString quoteString(const QString &value);
        /*! Quote the given string literal. */
        template<typename = void>
        static QString quoteString(const QList<QString> &values);

        /*! Determine if the given value is a raw expression. */
        static bool isExpression(const QVariant &value);

        /*! Get the value of a raw expression. */
        static QVariant getValue(const QVariant &expression);
        /*! Get the value of a raw expression. */
        static QVariant getValue(const Expression &expression) noexcept;

        /*! Get the grammar's table prefix. */
        inline QString getTablePrefix() const;
        /*! Set the grammar's table prefix. */
        BaseGrammar &setTablePrefix(const QString &prefix);

        /*! Get the column name without the table name, a string after last dot. */
        static QString unqualifyColumn(const QString &column);

        /*! Get the table name without an alias. */
        static QString getFromWithoutAlias(const QString &from);
        /*! Get an alias from the 'from' clause. */
        static QString getAliasFromFrom(const QString &from);
        /*! Get an alias from the column name (select expression). */
        inline static QString getAliasFromColumn(const QString &column);

    protected:
        /*! Convert the vector of column names into a wrapped comma delimited string. */
        template<ColumnContainer T>
        QString columnize(T &&columns) const;
        /*! Convert the vector of column names into a comma delimited string. */
        template<ColumnContainer T>
        QString columnizeWithoutWrap(T &&columns) const;

        /*! Create query parameter place-holders for the vector. */
        template<Parametrize Container>
        QString parametrize(const Container &values) const;
        /*! Get the appropriate query parameter place-holder for a value. */
        static QString parameter(const QVariant &value);

        /*! Wrap a value that has an alias. */
        QString wrapAliasedValue(const QString &value, bool prefixAlias = false) const;
        /*! Wrap a single string in keyword identifiers. */
        virtual QString wrapValue(QString value) const;

        /*! Wrap the given value segments. */
        QString wrapSegments(QStringList segments) const;

        /*! Get individual segments from the aliased identifier ('from' clause or
            column alias (select expression)). */
        static QStringList getSegmentsFromAlias(const QString &aliasedExpression);

        // FEATURE qt6, use everywhere QLatin1String("") instead of = "", BUT Qt6 has char8_t ctor, so u"" can be used, I will wait with this problem silverqx
        /*! The grammar table prefix. */
        QString m_tablePrefix;
    };

    /* public */

    BaseGrammar::~BaseGrammar() = default;

    /* protected */

    template<ColumnContainer T>
    QString BaseGrammar::columnize(T &&columns) const
    {
        return columnizeWithoutWrap(wrapArray(std::forward<T>(columns)));
    }

    /* I leave this method here because it has meaningful name, not make it inline to avoid
       utils/container.hpp include in the header file. */
    template<ColumnContainer T>
    QString BaseGrammar::columnizeWithoutWrap(T &&columns) const
    {
        return ContainerUtils::join(std::forward<T>(columns));
    }

    QString BaseGrammar::getTablePrefix() const
    {
        return m_tablePrefix;
    }

    QString BaseGrammar::getAliasFromColumn(const QString &column)
    {
        // The algorithm is the same but define this method anyway for better naming
        return getAliasFromFrom(column);
    }

    template<ColumnContainer T>
    QList<QString> BaseGrammar::wrapArray(const T &values) const
    {
        QList<QString> wrapped;
        wrapped.reserve(values.size());

        for (const auto &value : values)
            wrapped << wrap(value);

        return wrapped;
    }

    template<typename>
    QString BaseGrammar::quoteString(const QList<QString> &values)
    {
        // Nothing to quote
        if (values.isEmpty())
            return {};

        QString quoted;
        quoted.reserve(ContainerUtils::countStringSizes(values, 4) + 8);

        auto it = values.constBegin();
        const auto end = values.constEnd();

        // Don't prepend a comma before the first item
        if (it != end) {
            quoted.append(SQUOTE)
                  .append(*it)
                  .append(SQUOTE);
            ++it;
        }

        while (it != end) {
            // These append-s() are better for performance
            quoted.append(QStringLiteral(", '"))
                  .append(*it)
                  .append(SQUOTE);
            ++it;
        }

        Q_ASSERT(it == end);

        return quoted;
    }

    template<Parametrize Container>
    QString BaseGrammar::parametrize(const Container &values) const
    {
        QStringList compiledParameters;
        compiledParameters.reserve(values.size());

        for (const auto &value : values)
            compiledParameters << parameter(value);

        return columnizeWithoutWrap(compiledParameters);
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_BASEGRAMMAR_HPP
