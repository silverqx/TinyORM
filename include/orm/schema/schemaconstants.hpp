#pragma once
#ifndef ORM_SCHEMA_SCHEMACONSTANTS_HPP
#define ORM_SCHEMA_SCHEMACONSTANTS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/config.hpp" // IWYU pragma: keep

#ifdef TINYORM_EXTERN_CONSTANTS
#  include "orm/schema/schemaconstants_extern.hpp" // IWYU pragma: export
#else
#  include "orm/schema/schemaconstants_inline.hpp" // IWYU pragma: export
#endif

#endif // ORM_SCHEMA_SCHEMACONSTANTS_HPP
