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
    concept QStringContainer = std::is_same_v<T, QStringList> ||
                               std::is_same_v<T, QVector<QString>>;

    /*! Concept for container passed to the parametrize() method (QVariantMap
        or QVector<QString>). */
    template<typename T>
    concept Parametrize = std::is_same_v<T, QVariantMap> ||
                          std::is_same_v<T, QVector<QVariant>>;

    class SHAREDLIB_EXPORT BaseGrammar
    {
        Q_DISABLE_COPY(BaseGrammar)

        using Expression = Query::Expression;

    public:
        BaseGrammar() = default;
        inline virtual ~BaseGrammar() = default;

        /*! Get the format for database stored dates. */
        virtual const QString &getDateFormat() const;

        /*! Wrap a value in keyword identifiers. */
        virtual QString wrap(const QString &value, bool prefixAlias = false) const;
        /*! Wrap a value in keyword identifiers. */
        virtual QString wrap(const Expression &value, bool prefixAlias = false) const;
        /*! Wrap a table in keyword identifiers. */
        virtual QString wrapTable(const QString &table) const;
        /*! Wrap a table in keyword identifiers. */
        virtual QString wrapTable(const Expression &table) const;

        /*! Wrap an array of values. */
        template<QStringContainer T>
        T wrapArray(T values) const;

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

    protected:
        /*! Convert the vector of column names into a delimited string. */
        QString columnize(const QStringList &columns) const;
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
        /*! Get the column name without the table name, a string after last dot. */
        QString unqualifyColumn(const QString &column) const;

        // FEATURE qt6, use everywhere QLatin1String("") instead of = "", BUT Qt6 has char8_t ctor, so u"" can be used, I will wait with this problem silverqx
        /*! The grammar table prefix. */
        QString m_tablePrefix = "";
    };

    inline QString BaseGrammar::getTablePrefix() const
    {
        return m_tablePrefix;
    }

    template<QStringContainer T>
    T BaseGrammar::wrapArray(T values) const
    {
        for (auto &value : values)
            value = wrap(value);

        return values;
    }

    template<Parametrize Container>
    QString BaseGrammar::parametrize(const Container &values) const
    {
        QStringList compiledParameters;

        for (const auto &value : values)
            compiledParameters << parameter(value);

        // TODO move all common QStringLiteral() to the common file as global constants silverqx
        return compiledParameters.join(", ");
    }

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // BASEGRAMMAR_H
