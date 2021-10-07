#pragma once
#ifndef ORM_CONFIG_HPP
#define ORM_CONFIG_HPP

// Enforce extern constants in shared build/linking when a user did not define it
#if !defined(TINYORM_EXTERN_CONSTANTS) && !defined(TINYORM_INLINE_CONSTANTS) && \
    (defined(TINYORM_BUILDING_SHARED) || defined(TINYORM_LINKING_SHARED))
#  define TINYORM_EXTERN_CONSTANTS
#endif

#endif // ORM_CONFIG_HPP
