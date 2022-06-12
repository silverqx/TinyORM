#pragma once
#ifndef TOM_TOMUTILS_HPP
#define TOM_TOMUTILS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

    /*! Tom common utilities library class. */
    class Utils
    {
        Q_DISABLE_COPY(Utils)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Utils() = delete;
        /*! Deleted destructor. */
        ~Utils() = delete;

        /*! Check whether a migration name starts with the datetime prefix. */
        static bool startsWithDatetimePrefix(const QString &migrationName);

        /*! Get the default value text (quotes the string type). */
        static QString defaultValueText(const QString &value);

    private:
        /*! Check whether all datetime parts are equal to the DateTimePrefix constant. */
        static bool areDatetimePartsEqual(const QList<QStringView> &prefixParts);
    };

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TOMUTILS_HPP
