#pragma once
#ifndef ORM_DRIVERS_SQLRESULT_HPP
#define ORM_DRIVERS_SQLRESULT_HPP

#include <QtSql/qtsqlglobal.h>

#include <memory>

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

class QString;
class QVariant;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlRecord;
    class SqlDriver;
    class SqlDriverError;
    class SqlResultPrivate;

    class SHAREDLIB_EXPORT SqlResult
    {
        Q_DISABLE_COPY_MOVE(SqlResult)
        Q_DECLARE_PRIVATE(SqlResult)

        friend class SqlQuery1;

    protected:
        /*! Constructor. */
        explicit SqlResult(std::unique_ptr<SqlResultPrivate> &&dd);

    public:
        /*! Pure virtual destructor. */
        virtual ~SqlResult() = 0;

        /*! Returns the low-level database result set handle (database specific). */
        virtual QVariant handle() const = 0;

    protected:
        enum BindingSyntax {
            PositionalBinding,
            NamedBinding,
        };

        /* Getters / Setters */
        bool isValid() const;

        QString query() const;
        virtual void setQuery(const QString &query);

        SqlDriverError lastError() const;
        virtual bool setLastError(const SqlDriverError &error);

        int at() const;
        virtual void setAt(int index);

        bool isActive() const;
        virtual void setActive(bool value);

        bool isSelect() const;
        virtual void setSelect(bool value);

        QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const;
        void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precision);

        const SqlDriver *driver() const noexcept;

        /* Normal queries */
        virtual bool exec(const QString &query) = 0;

        /* Prepared queries */
        virtual bool exec() = 0;
        virtual bool prepare(const QString &query) = 0;

        virtual void bindValue(int index, const QVariant &value,
                               QSql::ParamType /*unused*/);
        void addBindValue(const QVariant &value, QSql::ParamType /*unused*/);

        QVariant boundValue(int index) const;

        int boundValuesCount() const;

        QVariantList boundValues() const;
        QVariantList &boundValues();

        void clearValues();

        inline BindingSyntax bindingSyntax() const;

        /* Result sets */
        virtual SqlRecord record() const = 0;
        virtual QVariant lastInsertId() const = 0;

        virtual bool fetch(int index) = 0;
        virtual bool fetchFirst() = 0;
        virtual bool fetchLast() = 0;
        virtual bool fetchNext() = 0;
        virtual bool fetchPrevious();

        virtual QVariant data(int index) = 0;
        virtual bool isNull(int index) = 0;

        virtual int size() = 0;
        virtual int numRowsAffected() = 0;

        virtual void detachFromResultSet() = 0;

        inline virtual bool nextResult();

        /* Data members */
        std::unique_ptr<SqlResultPrivate> d_ptr;
    };

    /* protected */

    /* Prepared queries */

    SqlResult::BindingSyntax SqlResult::bindingSyntax() const
    {
        return SqlResult::PositionalBinding;
    }

    /* Result sets */

    bool SqlResult::nextResult()
    {
        throw std::exception("Not implemented :/.");
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLRESULT_HPP
