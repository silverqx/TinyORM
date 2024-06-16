#pragma once
#ifndef TINYUTILS_MACROS_HPP
#define TINYUTILS_MACROS_HPP

#include <QtGlobal>

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

#ifndef TVERIFY_THROWS_EXCEPTION
#  if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
/*! Alias for QVERIFY_THROWS_EXCEPTION() (temporary workaround for Qt v6.8). */
#    define TVERIFY_THROWS_EXCEPTION(exceptiontype, ...) \
            QVERIFY_THROWS_EXCEPTION(exceptiontype, __VA_ARGS__)
#  else
/*! Alias for QVERIFY_EXCEPTION_THROWN(). */
#    define TVERIFY_THROWS_EXCEPTION(exceptiontype, expression) \
            QVERIFY_EXCEPTION_THROWN(expression, exceptiontype)
#  endif
#endif

#endif // TINYUTILS_MACROS_HPP
