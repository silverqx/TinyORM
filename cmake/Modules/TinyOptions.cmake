include(TinyFeatureOptions)

# Initialize INLINE_CONSTANTS CMake feature dependent option.
# MinGW Clang shared build crashes with inline constants (fixed in Clang v18).
# clang-cl shared build crashes with extern constants so force to inline constants 😕🤔
# (also fixed in Clang v18), only one option with the clang-cl is inline constants
# for both shared/static builds.
# Look at NOTES.txt[inline constants] how this funckin machinery works. 😎
# Related issue: https://github.com/llvm/llvm-project/issues/55938
# Possible reason why extern constants don't work with static linking (already fixed,
# it started working mysteriously 😅):
# https://stackoverflow.com/a/3187324/2266467
function(tiny_initialize_inline_constants_option)

    if(MINGW AND BUILD_SHARED_LIBS AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
            CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18"
    )
        set(tinyInlineConstantsForceValue OFF)
    else()
        set(tinyInlineConstantsForceValue ON)
    endif()

    feature_option_dependent(INLINE_CONSTANTS
        "Use inline constants instead of extern constants in the shared build. \
OFF is highly recommended for the shared build; is always ON for the static build"
        # Default value for shared builds is OFF and for static builds ON
        OFF
        # Always TRUE for Clang >18 and for everything else
        "NOT (CMAKE_CXX_COMPILER_ID STREQUAL \"Clang\" AND \
CMAKE_CXX_COMPILER_VERSION VERSION_LESS \"18\" AND \
(MSVC OR (MINGW AND BUILD_SHARED_LIBS)))"
        # When the above condition is FALSE then always force INLINE_CONSTANTS to ON excepting
        # for MinGW Clang <18 shared build, in this case we need extern constants 😅
        ${tinyInlineConstantsForceValue}
    )

    unset(tinyInlineConstantsForceValue)

endfunction()
