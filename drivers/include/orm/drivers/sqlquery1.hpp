#pragma once
#ifndef ORM_DRIVERS_SQLQUERY1_HPP
#define ORM_DRIVERS_SQLQUERY1_HPP

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

#include "orm/drivers/sqldatabase.hpp"
#include "orm/drivers/sqlresult.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDatabase;
    class SqlDriver;
    class SqlDriverError;
    class SqlRecord;
    class SqlQueryPrivate;

    class SHAREDLIB_EXPORT SqlQuery1
    {
        Q_DISABLE_COPY(SqlQuery1)

    public:
        /*! Default constructor. */
        SqlQuery1();
        /*! Converting constructor from the SqlDatabase. */
        explicit SqlQuery1(const SqlDatabase &connection);
        /*! Converting constructor from the std::unique_ptr<SqlResult>. */
        explicit SqlQuery1(std::unique_ptr<SqlResult> &&result);

        /*! Move constructor. */
        inline SqlQuery1(SqlQuery1 &&) noexcept = default;
        /*! Move assignment operator. */
        inline SqlQuery1 &operator=(SqlQuery1 &&) noexcept = default;

        /*! Default destructor. */
        ~SqlQuery1();

        // CUR drivers swap silverqx

        /* Getters / Setters */
        bool isValid() const;

        QString executedQuery() const;
        SqlDriverError lastError() const;

        int at() const;

        bool isActive() const noexcept;
        bool isSelect() const noexcept;

        QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const;
        void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precision);

        inline const SqlDriver *driver() const noexcept;

        /* Normal queries */
        bool exec(const QString &query);

        /* Prepared queries */
        bool prepare(const QString &query);
        bool exec();

        void bindValue(int index, const QVariant &value,
                       QSql::ParamType /*unused*/ = QSql::In);
        void addBindValue(const QVariant &value, QSql::ParamType /*unused*/ = QSql::In);

        QVariant boundValue(int index) const;
        QVariantList boundValues() const;

        /* Result sets */
        SqlRecord record() const;
        QVariant lastInsertId() const;

        bool next();
        bool previous();
        bool first();
        bool last();
        bool seek(int index, bool relative = false);

        QVariant value(int index) const;
        QVariant value(const QString &name) const;

        bool isNull(int index) const;
        bool isNull(const QString &name) const;

        int size() const;
        int numRowsAffected() const;

        /* Others */
        void finish();
        void clear();

    private:
        /* Result sets */
        /*! Normal seek. */
        bool seekArbitrary(int index, int &actualIdx);
        /*! Relative seek. */
        bool seekRelative(int index, int &actualIdx);
        /*! Map the given index to the fetch-related methods that are available. */
        bool mapSeekToFetch(int actualIdx);

        /* Constructors */
        static std::unique_ptr<SqlResult> initSqlResult();
        static std::unique_ptr<SqlResult> initSqlResult(const SqlDatabase &connection);

        /* Data members */
        std::unique_ptr<SqlResult> m_sqlResult;
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLQUERY1_HPP
