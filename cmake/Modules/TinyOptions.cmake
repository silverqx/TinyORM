include(TinyFeatureOptions)

# Initialize INLINE_CONSTANTS CMake feature dependent option.
# MinGW Clang <18 shared build crashes with inline constants (fixed in Clang v18).
# Clang-cl <18 shared build crashes with extern constants so force to inline constants
# (also fixed in Clang v18) 😕🤔, only one option with the Clang-cl is inline constants
# for both shared/static builds (not true from Clang v18).
# Look at NOTES.txt[inline constants] how this funckin machinery works. 😎
# Related issue: https://github.com/llvm/llvm-project/issues/55938
# Possible reason why extern constants don't work with static linking (fixed in some
# cases, but doesn't always work):
# https://stackoverflow.com/a/3187324/2266467
macro(tiny_initialize_inline_constants_option)

    # Summary how this works:
    # - INLINE_CONSTANT feature option is available for shared builds only so you can
    #   select if you want to use inline or extern constants
    # - INLINE_CONSTANT isn't available:
    #   - for static builds, in this case is always forced to ON (inline)
    #   - for MinGW Clang <18 shared build, in this case is always forced to OFF (extern)
    #     because shared build crashes with inline constants
    #   - Clang-cl <18 shared build, in this case is always forced to ON (inline)
    #     because shared build crashes with extern constants
    #
    # Next, based on the BUILD_SHARED_LIBS and INLINE_CONSTANTS values will be decided
    # if inline or extern constants will be used in tiny_init_tiny_variables() that sets
    # the TINY_EXTERN_CONSTANTS INTERNAL CACHE CMake variable and based on this variable
    # will be set TINYORM_EXTERN_CONSTANTS or TINYORM_INLINE_CONSTANTS
    # -D compiler definition later in the main CMakeLists.txt. 😅
    # Also, based on the TINY_EXTERN_CONSTANTS will be decided which source files
    # for inline/extern constants will be used constants_extern/_p.hpp/.cpp or
    # constants_inline/_p.hpp for in all projects.

    # MinGW Clang <18 shared build crashes with inline constants so force extern constants
    # if <depends>=FALSE
    if(MINGW AND BUILD_SHARED_LIBS AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
            CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18"
    )
        set(tinyInlineConstantsForceValue OFF)
    # All other cases if <depends>=FALSE force the INLINE_CONSTANTS to ON
    else()
        set(tinyInlineConstantsForceValue ON)
    endif()

    # Default value for shared builds is OFF and for static builds ON (with exceptions
    # described all around)
    feature_option_dependent(INLINE_CONSTANTS
        "Use inline constants instead of extern constants in the shared build. \
OFF is highly recommended for the shared build; is always ON for the static build"
        OFF
        # Always TRUE if BUILD_SHARED_LIBS=ON excluding Clang-cl <18 with MSVC or
        # MinGW Clang <18
        "BUILD_SHARED_LIBS AND NOT ((MSVC OR MINGW) AND \
CMAKE_CXX_COMPILER_ID STREQUAL \"Clang\" AND \
CMAKE_CXX_COMPILER_VERSION VERSION_LESS \"18\")"
        # When the above <depends> condition is FALSE then always force
        # the INLINE_CONSTANTS to ON except for MinGW Clang <18 shared build,
        # in this case we need extern constants 😅 (so force INLINE_CONSTANT to OFF)
        ${tinyInlineConstantsForceValue}
    )

    unset(tinyInlineConstantsForceValue)

endmacro()
