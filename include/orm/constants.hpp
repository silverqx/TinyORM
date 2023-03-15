#pragma once
#ifndef ORM_CONSTANTS_HPP
#define ORM_CONSTANTS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/config.hpp" // IWYU pragma: keep

#ifdef TINYORM_EXTERN_CONSTANTS
#  include "orm/constants_extern.hpp" // IWYU pragma: export
#else
#  include "orm/constants_inline.hpp" // IWYU pragma: export
#endif

#endif // ORM_CONSTANTS_HPP
