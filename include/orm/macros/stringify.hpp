#pragma once
#ifndef ORM_MACROS_STRINGIFY_HPP
#define ORM_MACROS_STRINGIFY_HPP

// Excluded for the Resource compiler
#ifndef RC_INVOKED
#  include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER
#endif

#ifndef TINY_STRINGIFY
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#  define TINY__STRINGIFY(x) #x // clazy:exclude=ifndef-define-typo
#  define TINY_STRINGIFY(x) TINY__STRINGIFY(x)
#endif

#endif // ORM_MACROS_STRINGIFY_HPP
