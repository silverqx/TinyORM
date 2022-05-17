#pragma once
#ifndef TOM_COMMANDS_MAKE_SUPPORT_TABLEGUESSER_HPP
#define TOM_COMMANDS_MAKE_SUPPORT_TABLEGUESSER_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <optional>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Support
{

    /*! Guess the table name by the passed argument on the command line. */
    class TableGuesser
    {
        Q_DISABLE_COPY(TableGuesser)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        TableGuesser() = delete;
        /*! Deleted destructor. */
        ~TableGuesser() = delete;

        /*! Attempt to guess the table name and creation status of the given migration. */
        static std::tuple<QString, bool> guess(const QString &migration);

    private:
        /*! RegEx pattern for guessing migration creation. */
        static const QString CreatePatterns;
        /*! RegEx pattern for guessing migration update. */
        static const QString ChangePatterns;
    };

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_SUPPORT_TABLEGUESSER_HPP
