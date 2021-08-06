#pragma once
#ifndef BASEGRAMMAR_H
#define BASEGRAMMAR_H

#include "orm/ormtypes.hpp"
#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
namespace Query
{
    class Expression;
}

    /*! QString container concept (QStringList or QVector<QString>). */
    template<typename T>
    concept ColumnContainer = std::convertible_to<T, const QStringList &> ||
                              std::convertible_to<T, const QVector<QString> &> ||
                              std::convertible_to<T, const QVector<Column> &>;

    /*! Concept for container passed to the parametrize() method (QVariantMap
        or QVector<QString>). */
    template<typename T>
    concept Parametrize = std::same_as<T, QVariantMap> ||
                          std::same_as<T, QVector<QVariant>>;

    /*! Base class for the database grammar. */
    class SHAREDLIB_EXPORT BaseGrammar
    {
        Q_DISABLE_COPY(BaseGrammar)

        /*! Expression alias. */
        using Expression = Query::Expression;

    public:
        /*! Default constructor. */
        BaseGrammar() = default;
        /*! Virtual destructor. */
        inline virtual ~BaseGrammar() = default;

        /*! Get the format for database stored dates. */
        virtual const QString &getDateFormat() const;

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
        QVector<QString> wrapArray(const T &values) const;

        /*! Quote the given string literal. */
        QString quoteString(const QString &value) const;
        /*! Quote the given string literal. */
        template<typename = void>
        QString quoteString(const QVector<QString> &values) const;

        /*! Determine if the given value is a raw expression. */
        bool isExpression(const QVariant &value) const;

        /*! Get the value of a raw expression. */
        QVariant getValue(const QVariant &expression) const;
        /*! Get the value of a raw expression. */
        QVariant getValue(const Expression &expression) const;

        /*! Get the grammar's table prefix. */
        QString getTablePrefix() const;
        /*! Set the grammar's table prefix. */
        BaseGrammar &setTablePrefix(const QString &prefix);

        /*! Get the column name without the table name, a string after last dot. */
        QString unqualifyColumn(const QString &column) const;

    protected:
        /*! Convert the vector of column names into a delimited string. */
        template<ColumnContainer T>
        QString columnize(const T &columns) const;

        /*! Create query parameter place-holders for the vector. */
        template<Parametrize Container>
        QString parametrize(const Container &values) const;
        /*! Get the appropriate query parameter place-holder for a value. */
        QString parameter(const QVariant &value) const;

        /*! Wrap a value that has an alias. */
        QString wrapAliasedValue(const QString &value, bool prefixAlias = false) const;
        /*! Wrap a single string in keyword identifiers. */
        virtual QString wrapValue(QString value) const;

        /*! Wrap the given value segments. */
        QString wrapSegments(QStringList segments) const;

        /*! Get individual segments from the 'from' clause. */
        QStringList getSegmentsFromFrom(const QString &from) const;
        /*! Get the table name without an alias. */
        QString getFromWithoutAlias(const QString &from) const;
        /*! Get an alias from the 'from' clause. */
        QString getAliasFromFrom(const QString &from) const;

        // FEATURE qt6, use everywhere QLatin1String("") instead of = "", BUT Qt6 has char8_t ctor, so u"" can be used, I will wait with this problem silverqx
        /*! The grammar table prefix. */
        QString m_tablePrefix = "";

    private:
        /*! Convert the vector of column names into a delimited string. */
        QString columnizeInternal(const QVector<QString> &columns) const;
    };

    template<ColumnContainer T>
    inline QString BaseGrammar::columnize(const T &columns) const
    {
        return columnizeInternal(wrapArray(columns));
    }

    inline QString BaseGrammar::getTablePrefix() const
    {
        return m_tablePrefix;
    }

    template<ColumnContainer T>
    QVector<QString> BaseGrammar::wrapArray(const T &values) const
    {
        QVector<QString> wrapped;
        wrapped.reserve(values.size());

        for (const auto &value : values)
            wrapped << wrap(value);

        return wrapped;
    }

    template<typename>
    QString BaseGrammar::quoteString(const QVector<QString> &values) const
    {
        QString quoted;

        if (values.isEmpty())
            return quoted;

        const auto end = values.cend() - 1;
        auto it = values.begin();

        for (; it < end; ++it)
            quoted += QStringLiteral("'%1', ").arg(*it);

        if (it == end)
            quoted += QStringLiteral("'%1'").arg(*it);

        return quoted;
    }

    template<Parametrize Container>
    QString BaseGrammar::parametrize(const Container &values) const
    {
        QStringList compiledParameters;

        for (const auto &value : values)
            compiledParameters << parameter(value);

        // CUR1 QString allocation 😟 solve everywhere 😭 silverqx
        return compiledParameters.join(COMMA);
    }

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // BASEGRAMMAR_H
