#pragma once
#ifndef TOM_TOMCONSTANTS_HPP
#define TOM_TOMCONSTANTS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <orm/config.hpp> // IWYU pragma: keep

#ifdef TINYORM_EXTERN_CONSTANTS
#  include <tom/tomconstants_extern.hpp> // IWYU pragma: export
#else
#  include <tom/tomconstants_inline.hpp> // IWYU pragma: export
#endif

#endif // TOM_TOMCONSTANTS_HPP
