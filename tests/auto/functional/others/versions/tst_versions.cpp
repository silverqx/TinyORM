#include <QCoreApplication>
#include <QStringBuilder>
#include <QTest>

#if defined(_WIN32) && defined(TINYTEST_VERSIONS_IS_SHARED_BUILD)
#  include <qt_windows.h>
#endif

#ifdef _WIN32
#  include "fs.hpp"
#endif

// TinyDrivers
#if defined(TINYORM_USING_TINYDRIVERS) && defined(TINYDRIVERS_LINKING_SHARED)
#  include "orm/drivers/version.hpp"
#endif
// TinyMySql (loadable shared library)
#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
#  include "orm/drivers/mysql/version.hpp"
#endif
// TinyORM
#include "orm/version.hpp"
// TinyUtils
#include "version.hpp"
// Tom example (defined on windows only)
#ifdef TINYTOM_EXAMPLE
#  include "tom/version.hpp"
#endif

// Used by checkFileVersion_*() tests
#if defined(_WIN32) && defined(TINYTEST_VERSIONS_IS_SHARED_BUILD)
#  ifdef TINYTEST_VERSIONS_IS_CMAKE
#    include TINYTEST_VERSIONS_VERSIONSDEBUG
#  elif defined(TINYTEST_VERSIONS_IS_QMAKE)
#    include "versionsdebug_qmake.hpp"
#  endif
#else
#  define TINYTEST_VERSIONS_TINYDRIVERS_PATH
#  define TINYTEST_VERSIONS_TINYMYSQL_PATH
#  define TINYTEST_VERSIONS_TINYORM_PATH
#  define TINYTEST_VERSIONS_TINYUTILS_PATH
#  define TINYTEST_VERSIONS_TOMEXAMPLE_PATH
#endif

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

#ifdef _WIN32
using TestUtils::Fs;
#endif

class tst_Versions : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
#if defined(TINYORM_USING_TINYDRIVERS) && defined(TINYDRIVERS_LINKING_SHARED)
    void versions_TinyDrivers() const;
#endif
#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
    void versions_TinyMySql() const;
#endif
    void versions_TinyOrm() const;
    void versions_TinyUtils() const;
#ifdef TINYTOM_EXAMPLE
    void versions_TomExample() const;
#endif

#if defined(TINYORM_USING_TINYDRIVERS) && defined(TINYDRIVERS_LINKING_SHARED)
    void checkFileVersion_TinyDrivers() const;
#endif
#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
    void checkFileVersion_TinyMySql() const;
#endif
    void checkFileVersion_TinyOrm() const;
    void checkFileVersion_TinyUtils() const;
#ifdef TINYTOM_EXAMPLE
    void checkFileVersion_TomExample() const;
#endif

#if defined(_WIN32) && defined(TINYTEST_VERSIONS_IS_SHARED_BUILD)
// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Return value for the getExeVersionString(). */
    struct FileVersions
    {
        /*! ProductVersion. */
        QString productVersion;
        /*! FileVersion. */
        QString fileVersion;
        /*! LegalCopyright. */
        QString copyright;
    };

    /*! Obtain a ProductVersion and FileVersion strings from an exe/dll. */
    [[nodiscard]] static FileVersions getExeVersionString(const QString &fileName);
#endif
};

#if defined(_WIN32) && defined(TINYTEST_VERSIONS_IS_SHARED_BUILD)
/*! Executables copyright, all executables have the same LegalCopyright. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, CopyRight, ("Copyright (©) 2024 Silver Zachara")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
#endif

namespace
{
    /*! QChar constant '.'. */
    constexpr QChar DOT(QLatin1Char('.'));
} // namespace

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
#if defined(TINYORM_USING_TINYDRIVERS) && defined(TINYDRIVERS_LINKING_SHARED)
void tst_Versions::versions_TinyDrivers() const
{
    // Test types
    QCOMPARE(typeid (TINYDRIVERS_VERSION_MAJOR),  typeid (int));
    QCOMPARE(typeid (TINYDRIVERS_VERSION_MINOR),  typeid (int));
    QCOMPARE(typeid (TINYDRIVERS_VERSION_BUGFIX), typeid (int));
    QCOMPARE(typeid (TINYDRIVERS_VERSION_BUILD),  typeid (int));

    // Individual version numbers have to be greater than zero
    QVERIFY(TINYDRIVERS_VERSION_MAJOR  >= 0);
    QVERIFY(TINYDRIVERS_VERSION_MINOR  >= 0);
    QVERIFY(TINYDRIVERS_VERSION_BUGFIX >= 0);
    QVERIFY(TINYDRIVERS_VERSION_BUILD  >= 0);

    // Project and File Version strings
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif
    QString versionStr = QString::number(TINYDRIVERS_VERSION_MAJOR) % DOT %
                         QString::number(TINYDRIVERS_VERSION_MINOR) % DOT %
                         QString::number(TINYDRIVERS_VERSION_BUGFIX);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif
    QString fileVersionStr = versionStr % DOT %
                             QString::number(TINYDRIVERS_VERSION_BUILD);
    if constexpr (TINYDRIVERS_VERSION_BUILD > 0)
        versionStr += DOT + QString::number(TINYDRIVERS_VERSION_BUILD);
    versionStr += TINYDRIVERS_VERSION_STATUS;

    QCOMPARE(TINYDRIVERS_FILEVERSION_STR, fileVersionStr);
    QCOMPARE(TINYDRIVERS_VERSION_STR, versionStr);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif
    QCOMPARE(TINYDRIVERS_VERSION_STR_2, QLatin1Char('v') + versionStr);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

    // Project Version number, to check API compatibility
    const auto version = TINYDRIVERS_VERSION_MAJOR * 10000 +
                         TINYDRIVERS_VERSION_MINOR * 100 +
                         TINYDRIVERS_VERSION_BUGFIX;
    QCOMPARE(TINYDRIVERS_VERSION, version);
}
#endif

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
void tst_Versions::versions_TinyMySql() const
{
    // Test types
    QCOMPARE(typeid (TINYMYSQL_VERSION_MAJOR),  typeid (int));
    QCOMPARE(typeid (TINYMYSQL_VERSION_MINOR),  typeid (int));
    QCOMPARE(typeid (TINYMYSQL_VERSION_BUGFIX), typeid (int));
    QCOMPARE(typeid (TINYMYSQL_VERSION_BUILD),  typeid (int));

    // Individual version numbers have to be greater than zero
    QVERIFY(TINYMYSQL_VERSION_MAJOR  >= 0);
    QVERIFY(TINYMYSQL_VERSION_MINOR  >= 0);
    QVERIFY(TINYMYSQL_VERSION_BUGFIX >= 0);
    QVERIFY(TINYMYSQL_VERSION_BUILD  >= 0);

    // Project and File Version strings
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif
    QString versionStr = QString::number(TINYMYSQL_VERSION_MAJOR) % DOT %
                         QString::number(TINYMYSQL_VERSION_MINOR) % DOT %
                         QString::number(TINYMYSQL_VERSION_BUGFIX);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif
    QString fileVersionStr = versionStr % DOT %
                             QString::number(TINYMYSQL_VERSION_BUILD);
    if constexpr (TINYMYSQL_VERSION_BUILD > 0)
        versionStr += DOT + QString::number(TINYMYSQL_VERSION_BUILD);
    versionStr += TINYMYSQL_VERSION_STATUS;

    QCOMPARE(TINYMYSQL_FILEVERSION_STR, fileVersionStr);
    QCOMPARE(TINYMYSQL_VERSION_STR, versionStr);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif
    QCOMPARE(TINYMYSQL_VERSION_STR_2, QLatin1Char('v') + versionStr);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

    // Project Version number, to check API compatibility
    const auto version = TINYMYSQL_VERSION_MAJOR * 10000 +
                         TINYMYSQL_VERSION_MINOR * 100 +
                         TINYMYSQL_VERSION_BUGFIX;
    QCOMPARE(TINYMYSQL_VERSION, version);
}
#endif

void tst_Versions::versions_TinyOrm() const
{
    // Test types
    QCOMPARE(typeid (TINYORM_VERSION_MAJOR),  typeid (int));
    QCOMPARE(typeid (TINYORM_VERSION_MINOR),  typeid (int));
    QCOMPARE(typeid (TINYORM_VERSION_BUGFIX), typeid (int));
    QCOMPARE(typeid (TINYORM_VERSION_BUILD),  typeid (int));

    // Individual version numbers have to be greater than zero
    QVERIFY(TINYORM_VERSION_MAJOR  >= 0);
    QVERIFY(TINYORM_VERSION_MINOR  >= 0);
    QVERIFY(TINYORM_VERSION_BUGFIX >= 0);
    QVERIFY(TINYORM_VERSION_BUILD  >= 0);

    // Project and File Version strings
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif
    QString versionStr = QString::number(TINYORM_VERSION_MAJOR) % DOT %
                         QString::number(TINYORM_VERSION_MINOR) % DOT %
                         QString::number(TINYORM_VERSION_BUGFIX);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif
    QString fileVersionStr = versionStr % DOT %
                             QString::number(TINYORM_VERSION_BUILD);
    if constexpr (TINYORM_VERSION_BUILD > 0)
        versionStr += DOT + QString::number(TINYORM_VERSION_BUILD);
    versionStr += TINYORM_VERSION_STATUS;

    QCOMPARE(TINYORM_FILEVERSION_STR, fileVersionStr);
    QCOMPARE(TINYORM_VERSION_STR, versionStr);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif
    QCOMPARE(TINYORM_VERSION_STR_2, QLatin1Char('v') + versionStr);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

    // Project Version number, to check API compatibility
    const auto version = TINYORM_VERSION_MAJOR * 10000 +
                         TINYORM_VERSION_MINOR * 100 +
                         TINYORM_VERSION_BUGFIX;
    QCOMPARE(TINYORM_VERSION, version);
}

void tst_Versions::versions_TinyUtils() const
{
    // Test types
    QCOMPARE(typeid (TINYUTILS_VERSION_MAJOR),  typeid (int));
    QCOMPARE(typeid (TINYUTILS_VERSION_MINOR),  typeid (int));
    QCOMPARE(typeid (TINYUTILS_VERSION_BUGFIX), typeid (int));
    QCOMPARE(typeid (TINYUTILS_VERSION_BUILD),  typeid (int));

    // Individual version numbers have to be greater than zero
    QVERIFY(TINYUTILS_VERSION_MAJOR  >= 0);
    QVERIFY(TINYUTILS_VERSION_MINOR  >= 0);
    QVERIFY(TINYUTILS_VERSION_BUGFIX >= 0);
    QVERIFY(TINYUTILS_VERSION_BUILD  >= 0);

    // Project and File Version strings
    QString versionStr = QString::number(TINYUTILS_VERSION_MAJOR) % DOT %
                         QString::number(TINYUTILS_VERSION_MINOR) % DOT %
                         QString::number(TINYUTILS_VERSION_BUGFIX);
    QString fileVersionStr = versionStr % DOT %
                             QString::number(TINYUTILS_VERSION_BUILD);
    if constexpr (TINYUTILS_VERSION_BUILD > 0)
        versionStr += DOT + QString::number(TINYUTILS_VERSION_BUILD);

    QCOMPARE(TINYUTILS_FILEVERSION_STR, fileVersionStr);
    QCOMPARE(TINYUTILS_VERSION_STR, versionStr);
    QCOMPARE(TINYUTILS_VERSION_STR_2, QLatin1Char('v') + versionStr);

    // Project Version number, to check API compatibility
    const auto version = TINYUTILS_VERSION_MAJOR * 10000 +
                         TINYUTILS_VERSION_MINOR * 100 +
                         TINYUTILS_VERSION_BUGFIX;
    QCOMPARE(TINYUTILS_VERSION, version);
}

#ifdef TINYTOM_EXAMPLE
void tst_Versions::versions_TomExample() const
{
    // Test types
    QCOMPARE(typeid (TINYTOM_VERSION_MAJOR),  typeid (int));
    QCOMPARE(typeid (TINYTOM_VERSION_MINOR),  typeid (int));
    QCOMPARE(typeid (TINYTOM_VERSION_BUGFIX), typeid (int));
    QCOMPARE(typeid (TINYTOM_VERSION_BUILD),  typeid (int));

    // Individual version numbers have to be greater than zero
    QVERIFY(TINYTOM_VERSION_MAJOR  >= 0);
    QVERIFY(TINYTOM_VERSION_MINOR  >= 0);
    QVERIFY(TINYTOM_VERSION_BUGFIX >= 0);
    QVERIFY(TINYTOM_VERSION_BUILD  >= 0);

    // Project and File Version strings
    QString versionStr = QString::number(TINYTOM_VERSION_MAJOR) % DOT %
                         QString::number(TINYTOM_VERSION_MINOR) % DOT %
                         QString::number(TINYTOM_VERSION_BUGFIX);
    QString fileVersionStr = versionStr % DOT %
                             QString::number(TINYTOM_VERSION_BUILD);
    if constexpr (TINYTOM_VERSION_BUILD > 0)
        versionStr += DOT + QString::number(TINYTOM_VERSION_BUILD);
    versionStr += TINYTOM_VERSION_STATUS;

    QCOMPARE(TINYTOM_FILEVERSION_STR, fileVersionStr);
    QCOMPARE(TINYTOM_VERSION_STR, versionStr);
    QCOMPARE(TINYTOM_VERSION_STR_2, QLatin1Char('v') + versionStr);

    // Project Version number, to check API compatibility
    const auto version = TINYTOM_VERSION_MAJOR * 10000 +
                         TINYTOM_VERSION_MINOR * 100 +
                         TINYTOM_VERSION_BUGFIX;
    QCOMPARE(TINYTOM_VERSION, version);
}
#endif

#if defined(TINYORM_USING_TINYDRIVERS) && defined(TINYDRIVERS_LINKING_SHARED)
void tst_Versions::checkFileVersion_TinyDrivers() const
{
#ifndef _WIN32
    QSKIP("checkFileVersion_*() related tests are supported on MSVC only.", );
#elif !defined(TINYTEST_VERSIONS_IS_SHARED_BUILD)
    QSKIP("checkFileVersion_*() related tests are enabled for shared builds only.", );
#else
    const auto fileVersions = getExeVersionString(
                                  Fs::absolutePath(TINYTEST_VERSIONS_TINYDRIVERS_PATH));

    // Project and File Version strings
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif
    const QString versionStr = QString::number(TINYDRIVERS_VERSION_MAJOR)  % DOT %
                               QString::number(TINYDRIVERS_VERSION_MINOR)  % DOT %
                               QString::number(TINYDRIVERS_VERSION_BUGFIX) % DOT %
                               QString::number(TINYDRIVERS_VERSION_BUILD);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

    QCOMPARE(fileVersions.productVersion, versionStr);
    QCOMPARE(fileVersions.fileVersion, fileVersions.productVersion);
    QCOMPARE(fileVersions.copyright, *CopyRight);
#endif
}
#endif

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
void tst_Versions::checkFileVersion_TinyMySql() const
{
#ifndef _WIN32
    QSKIP("checkFileVersion_*() related tests are supported on MSVC only.", );
#elif !defined(TINYTEST_VERSIONS_IS_SHARED_BUILD)
    QSKIP("checkFileVersion_*() related tests are enabled for shared builds only.", );
#else
    const auto fileVersions = getExeVersionString(
                                  Fs::absolutePath(TINYTEST_VERSIONS_TINYMYSQL_PATH));

    // Project and File Version strings
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif
    const QString versionStr = QString::number(TINYMYSQL_VERSION_MAJOR)  % DOT %
                               QString::number(TINYMYSQL_VERSION_MINOR)  % DOT %
                               QString::number(TINYMYSQL_VERSION_BUGFIX) % DOT %
                               QString::number(TINYMYSQL_VERSION_BUILD);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

    QCOMPARE(fileVersions.productVersion, versionStr);
    QCOMPARE(fileVersions.fileVersion, fileVersions.productVersion);
    QCOMPARE(fileVersions.copyright, *CopyRight);
#endif
}
#endif

void tst_Versions::checkFileVersion_TinyOrm() const
{
#ifndef _WIN32
    QSKIP("checkFileVersion_*() related tests are supported on MSVC only.", );
#elif !defined(TINYTEST_VERSIONS_IS_SHARED_BUILD)
    QSKIP("checkFileVersion_*() related tests are enabled for shared builds only.", );
#else
    const auto fileVersions = getExeVersionString(
                                  Fs::absolutePath(TINYTEST_VERSIONS_TINYORM_PATH));

    // Project and File Version strings
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4127)
#endif
    const QString versionStr = QString::number(TINYORM_VERSION_MAJOR)  % DOT %
                               QString::number(TINYORM_VERSION_MINOR)  % DOT %
                               QString::number(TINYORM_VERSION_BUGFIX) % DOT %
                               QString::number(TINYORM_VERSION_BUILD);
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

    QCOMPARE(fileVersions.productVersion, versionStr);
    QCOMPARE(fileVersions.fileVersion, fileVersions.productVersion);
    QCOMPARE(fileVersions.copyright, *CopyRight);
#endif
}

void tst_Versions::checkFileVersion_TinyUtils() const
{
#ifndef _WIN32
    QSKIP("checkFileVersion_*() related tests are supported on MSVC only.", );
#elif !defined(TINYTEST_VERSIONS_IS_SHARED_BUILD)
    QSKIP("checkFileVersion_*() related tests are enabled for shared builds only.", );
#else
    const auto fileVersions = getExeVersionString(
                                  Fs::absolutePath(TINYTEST_VERSIONS_TINYUTILS_PATH));

    // Project and File Version strings
    const QString versionStr = QString::number(TINYUTILS_VERSION_MAJOR)  % DOT %
                               QString::number(TINYUTILS_VERSION_MINOR)  % DOT %
                               QString::number(TINYUTILS_VERSION_BUGFIX) % DOT %
                               QString::number(TINYUTILS_VERSION_BUILD);

    QCOMPARE(fileVersions.productVersion, versionStr);
    QCOMPARE(fileVersions.fileVersion, fileVersions.productVersion);
    QCOMPARE(fileVersions.copyright, *CopyRight);
#endif
}

#ifdef TINYTOM_EXAMPLE
void tst_Versions::checkFileVersion_TomExample() const
{
#ifndef _WIN32
    QSKIP("checkFileVersion_*() related tests are supported on MSVC only.", );
#elif !defined(TINYTEST_VERSIONS_IS_SHARED_BUILD)
    QSKIP("checkFileVersion_*() related tests are enabled for shared builds only.", );
#else
    const auto fileVersions = getExeVersionString(
                                  Fs::absolutePath(TINYTEST_VERSIONS_TOMEXAMPLE_PATH));

    // Project and File Version strings
    const QString versionStr = QString::number(TINYTOM_VERSION_MAJOR)  % DOT %
                               QString::number(TINYTOM_VERSION_MINOR)  % DOT %
                               QString::number(TINYTOM_VERSION_BUGFIX) % DOT %
                               QString::number(TINYTOM_VERSION_BUILD);

    QCOMPARE(fileVersions.productVersion, versionStr);
    QCOMPARE(fileVersions.fileVersion, fileVersions.productVersion);
    QCOMPARE(fileVersions.copyright, *CopyRight);
#endif
}
#endif
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

#if defined(_WIN32) && defined(TINYTEST_VERSIONS_IS_SHARED_BUILD)
tst_Versions::FileVersions
tst_Versions::getExeVersionString(const QString &fileName)
{
    // first of all, GetFileVersionInfoSize
    DWORD dwHandle = 1;
    DWORD dwLen = GetFileVersionInfoSize(fileName.toStdWString().c_str(), &dwHandle);
    if (dwLen == 0) {
        qWarning() << "Error in GetFileVersionInfoSize().";
        return {};
    }

    // GetFileVersionInfo
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    auto lpData = std::make_unique<BYTE[]>(dwLen);
    if (!GetFileVersionInfo(fileName.toStdWString().c_str(), 0, dwLen,
                            static_cast<LPVOID>(lpData.get()))) {
        qWarning() << "Error in GetFileVersionInfo().";
        return {};
    }

    // Get the copyright
    LPTSTR copyrightPtr = nullptr;
    UINT uLen = 0;

    if (!VerQueryValue(lpData.get(), LR"(\StringFileInfo\040904b0\LegalCopyright)",
                       reinterpret_cast<LPVOID *>(&copyrightPtr), &uLen)
    ) {
        qWarning() << "Error in VerQueryValue() for the LegalCopyright.";
        return {};
    }

    // -1 to exclude null character
    auto copyright = QString::fromWCharArray(copyrightPtr,
                                             static_cast<QString::size_type>(uLen) - 1);

    // VerQueryValue
    VS_FIXEDFILEINFO *lpBuffer = nullptr;
    uLen = 0;

    if (!VerQueryValue(lpData.get(), QString("\\").toStdWString().c_str(),
                       reinterpret_cast<LPVOID *>(&lpBuffer), &uLen)
    ) {
        qWarning() << "Error in VerQueryValue() for the version-information resource.";
        return {};
    }

    return {
        // Product Version
        sl("%1.%2.%3.%4").arg(HIWORD(lpBuffer->dwProductVersionMS))
                         .arg(LOWORD(lpBuffer->dwProductVersionMS))
                         .arg(HIWORD(lpBuffer->dwProductVersionLS))
                         .arg(LOWORD(lpBuffer->dwProductVersionLS)),
        // File Version
        sl("%1.%2.%3.%4").arg(HIWORD(lpBuffer->dwFileVersionMS))
                         .arg(LOWORD(lpBuffer->dwFileVersionMS))
                         .arg(HIWORD(lpBuffer->dwFileVersionLS))
                         .arg(LOWORD(lpBuffer->dwFileVersionLS)),
        // LegalCopyright
        std::move(copyright)
    };
}
#endif

QTEST_MAIN(tst_Versions)

#include "tst_versions.moc"
