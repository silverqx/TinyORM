#pragma once
#ifndef TOM_COMMANDS_MAKE_MODELCOMMANDCONCEPTS_HPP
#define TOM_COMMANDS_MAKE_MODELCOMMANDCONCEPTS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QStringList>

#include <concepts>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

    /*! Concept for the btm relation prepared values (result). */
    template<typename T>
    concept BtmPreparedValuesConcept = std::convertible_to<T, QStringList> ||
                                       std::convertible_to<T, std::vector<QStringList>> ||
                                       std::convertible_to<T, std::vector<bool>>;

    /*! Concept for the btm relation values to prepare. */
    template<typename T>
    concept BtmValuesConcept = std::convertible_to<T, QStringList> ||
                               std::convertible_to<T, std::vector<bool>>;

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_MODELCOMMANDCONCEPTS_HPP
