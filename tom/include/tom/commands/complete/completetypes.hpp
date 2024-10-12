#pragma once
#ifndef TOM_COMMANDS_COMPLETE_COMPLETETYPES_HPP
#define TOM_COMMANDS_COMPLETE_COMPLETETYPES_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/types/guessedcommandname.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Complete
{

    /* Common for all */

    /*! Alias for the QList command-line option size type. */
    using SizeType = Command::SizeType;

    /*! Tom command positional argument position on the command-line. */
    inline constexpr qint8 ArgTomCommand = 1;
    /*! Second positional argument position on the command-line. */
    inline constexpr qint8 Arg2 = 2;
    /*! Third positional argument position on the command-line. */
    inline constexpr qint8 Arg3 = 3;

    /*! Cursor positions on the command-line. */
    enum TomCursorPostion : qint8
    {
        /*! Undefined cursor position (used as an initial value for the for-loops). */
        kUndefinedPosition = -1, // Must be -1 because initializing loops index!
        /*! The current word is a long/short option. */
        kOnOptionArgument  = -2,
        /*! Cursor is on the multi-value long option. */
//        kOnMultiValueOptionArgument = -3, // CUR1 finish, search all kOnOptionArgument, comments too silverqx
    };

    /*! Context for the tab-completion. */
    struct CompleteContext
    {
        /*! Currently processed/guessed Tom command. */
        GuessedCommandName guessedTomCommand;
        /*! The current word that is being completed. */
        QStringView wordArg;
        /*! Number of all positional arguments on the command-line. */
        SizeType argumentsCount = 0;
        /*! Tom command (positional argument) position under the cursor (0-based) or
            kOnOptionArgument if the cursor is on the long/short option. */
        SizeType currentArgumentPosition = kUndefinedPosition;
        /*! Maximum number of positional arguments for all commands. (from signature). */
        SizeType maxArgumentsCount = 0;
        /*! Is known/our or ambiguous Tom command on the command-line? (!kNotFound) */
        bool hasAnyTomCommand = false; // true = kFound || kAmbiguous; false = kNotFound
        /*! Determine if the cursor is at the end on the command-line, in this case
            positionArg > commandlineArgSize (pwsh only, all others are false
            in all cases). */
        bool isNewArgPositionAtEnd = false; // pwsh only; !isNewArgPositionAtEnd implies positionArg <= commandlineArgSize
        /*! Multi-value option value position without the long option prefix,
            eg. for --only=env,mac|ros,versions the position will be 7 (0-based). */
        SizeType multiValueOptionPosition = kUndefinedPosition; // pwsh only
    };

    /*! Option type (long/short). */
    enum struct OptionType : quint8
    {
        /*! Consider/match both long and short option arguments. */
        kAnyOption,
        /*! Long option argument. */
        kLongOption,
        /*! Short option argument. */
        kShortOption,
    };

    /* Pwsh only */

    /*! Return type for getWordArgOptionValue() and findCurrentWord(). */
    struct MultiValueOptionType
    {
        /*! The current word that is being completed. */
        QStringView wordArg;
        /*! Get a value position or multi-value option position.
            getWordArgOptionValue() - value position:
            Multi-value option value position without the long option prefix,
            eg. for --only=env,mac|¦ros,versions the position will be 7 (0-based).
            findCurrentWord() - multi-value option position:
            wordArg position in the commandlineArg eg. for
            tom about ¦--only=env,mac|ros,versions the position will be 10 (0-based).
            Legend: | - tab hit; ¦ - result. */
        SizeType multiValueOptionPosition = kUndefinedPosition;
    };

    /*! Return type for initializePrintArrayOptionValues() (multi-value options). */
    struct PrintMultiValueOptionType
    {
        /*! Option value to guess/complete (given on the command-line). */
        QStringView currentOptionValue = nullptr;
        /*! All values for printing (excluding already printed/completed values). */
        QList<QStringView> optionValues;
        /*! Determine whether guessing/printing the first value (need by pwsh). */
        bool isFirstOptionValue = false;
        /*! Print all option values? (if the current option value is empty) */
        bool printAllValues = false;
    };

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_COMPLETE_COMPLETETYPES_HPP
