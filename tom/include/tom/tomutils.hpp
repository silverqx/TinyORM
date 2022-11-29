#pragma once
#ifndef TOM_TOMUTILS_HPP
#define TOM_TOMUTILS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <filesystem>
#include <optional>

#include <orm/macros/commonnamespace.hpp>

#include "tom/types/commandlineoption.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

    /*! Tom common utilities library class. */
    class Utils
    {
        Q_DISABLE_COPY(Utils)

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Utils() = delete;
        /*! Deleted destructor. */
        ~Utils() = delete;

        /*! Check whether a migration name starts with the datetime prefix. */
        static bool startsWithDatetimePrefix(const QString &migrationName);

        /*! Get the default value text (quotes the string type). */
        static QString defaultValueText(const QString &value);

        /*! Try to guess a path for "make:" commands based on the pwd or use
            the default path. */
        static fspath
        guessPathForMakeByPwd(const fspath &defaultPath,
                              std::optional<
                                  std::reference_wrapper<const fspath>
                              > &&defaultModelsPath = std::nullopt);

        /*! Convert our Tom::CommandLineOption list to the QCommandLineOption list. */
        static QList<QCommandLineOption>
        convertToQCommandLineOptionList(const QList<CommandLineOption> &options);
        /*! Convert our Tom::CommandLineOption list to the QCommandLineOption list. */
        static QList<QCommandLineOption>
        convertToQCommandLineOptionList(QList<CommandLineOption> &&options);

    private:
        /*! Check whether all datetime parts are equal to the DateTimePrefix constant. */
        static bool areDatetimePartsEqual(const QList<QStringView> &prefixParts);

        /*! Determine whether the parent paths are equal. */
        static bool areParentPathsEqual(QStringList defaultPathList,
                                        const fspath &defaultModelsPath);
    };

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_TOMUTILS_HPP
