# Determine whether the minimum Qt version was satisfied using and set the internal cache
# variable TINY_QT_VERSION
# Command used to obtain a Qt version: "${QT_QMAKE_EXECUTABLE}" -query QT_VERSION
# This check is needed because eg. QtCreator sets the QT_QMAKE_EXECUTABLE based on the
# selected KIT, but there can be other Qt versions on the system/user path so even if
# the QT_QMAKE_EXECUTABLE contains qmake from eg. Qt v5.15 then the find_package()
# function is still able to find the correct Qt version, so this check is specifically
# for the QtCreator and informs about wrongly select KIT.
# If the QT_QMAKE_EXECUTABLE command can't be executed or it returns a non-zero exit code
# then continue a normal execution and leave the decision logic up to the find_package()
# function.
function(tiny_satisfies_minimum_required_qt_version out_variable)

    # Nothing to do, Qt version was already populated (cache hit)
    if(DEFINED TINY_QT_VERSION AND NOT TINY_QT_VERSION STREQUAL "")
        if(TINY_QT_VERSION VERSION_GREATER_EQUAL minReqQtVersion) # Automatic Variable Expansion
            set(${out_variable} TRUE PARENT_SCOPE)

        # There is a very low chance that this code branch will be invoked, but I can't
        # remove it 😎
        else()
            set(${out_variable} FALSE PARENT_SCOPE)
        endif()

        return()
    endif()

    execute_process(
        COMMAND "${QT_QMAKE_EXECUTABLE}" -query QT_VERSION
        RESULT_VARIABLE exitCode
        OUTPUT_VARIABLE qtVersion
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    # qmake can't be executed or qmake returned non-zero exit code, don't fail
    # in these cases and even don't cache the TINY_QT_VERSION as we don't want to cache
    # the wrong version value and leave the decision logic up to find_package()
    if(exitCode STREQUAL "no such file or directory" OR NOT exitCode EQUAL 0)
        message(VERBOSE "Qt version could not be determined because the \
'${QT_QMAKE_EXECUTABLE}' command can't be executed or it returned a non-zero exit code, \
continuing a normal execution and leaving decision logic up to the find_package() \
function, in ${CMAKE_CURRENT_FUNCTION}()")

        set(${out_variable} TRUE PARENT_SCOPE)
        return()
    endif()

    # Detect a normal tag version like eg. 6.7.2
    set(regexpVersion "^[0-9]+\.[0-9]+\.[0-9]+(\.[0-9]+)?$")

    # This should never happen :/
    if(NOT qtVersion MATCHES "${regexpVersion}")
        message(FATAL_ERROR "Parsing of the 'qmake -query QT_VERSION' failed \
in ${CMAKE_CURRENT_FUNCTION}().")
    endif()

    set(TINY_QT_VERSION "${CMAKE_MATCH_0}" CACHE INTERNAL
        "Qt version used to determine whether a minimum required Qt version was \
satisfied (also used by tiny_configure_test_pch()).")

    if(TINY_QT_VERSION VERSION_GREATER_EQUAL minReqQtVersion) # Automatic Variable Expansion
        set(${out_variable} TRUE PARENT_SCOPE)
    else()
        set(${out_variable} FALSE PARENT_SCOPE)
    endif()

endfunction()

# Make minimum toolchain version a requirement
function(tiny_toolchain_requirement)

    set(oneValueArgs MSVC CLANG_CL GCC CLANG QT)
    cmake_parse_arguments(PARSE_ARGV 0 TINY "" "${oneValueArgs}" "")

    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "The ${CMAKE_CURRENT_FUNCTION}() was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_MSVC)
            message(FATAL_ERROR "Minimum required MSVC version was not satisfied, \
required version >=${TINY_MSVC}, your version is ${CMAKE_CXX_COMPILER_VERSION}, upgrade \
Visual Studio.")
        endif()
    endif()

    # Clang-cl
    if(MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
            CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"
    )
        if(CMAKE_CXX_SIMULATE_VERSION VERSION_LESS TINY_MSVC)
            message(FATAL_ERROR "Minimum required MSVC version was not satisfied, \
required version >=${TINY_MSVC}, your version is ${CMAKE_CXX_SIMULATE_VERSION}, upgrade \
Visual Studio.")
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_CLANG_CL)
            message(FATAL_ERROR "Minimum required Clang-cl version was not satisfied, \
required version >=${TINY_CLANG_CL}, your version is ${CMAKE_CXX_COMPILER_VERSION}, \
upgrade LLVM.")
        endif()
    endif()

    if(NOT MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_GCC)
            message(STATUS "Minimum recommended GCC version was not satisfied, \
recommended version >=${TINY_GCC}, your version is ${CMAKE_CXX_COMPILER_VERSION}, \
upgrade the GCC compiler")
        endif()
    endif()

    if(NOT MSVC AND
            (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
                CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    )
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS TINY_CLANG)
            message(STATUS "Minimum recommended Clang version was not satisfied, \
recommended version >=${TINY_CLANG}, your version is ${CMAKE_CXX_COMPILER_VERSION}, \
upgrade Clang compiler")
        endif()
    endif()

    # Minimum required Qt version (minReqQtVersion)
    set(satisfiedMinReqQtVersion)
    tiny_satisfies_minimum_required_qt_version(satisfiedMinReqQtVersion)

    if(NOT satisfiedMinReqQtVersion)
        # Should never happen that the TINY_QT_VERSION is undefined or empty
        message(FATAL_ERROR "Minimum required Qt version was not satisfied, \
required version >=${TINY_QT}, your version is ${TINY_QT_VERSION}, \
upgrade Qt Framework.")
    endif()

endfunction()

# Throw a fatal error for unsupported environments
function(tiny_check_unsupported_build)

    # Fixed in Clang v18 🎉
    # Related issue: https://github.com/llvm/llvm-project/issues/55938

    if(MINGW AND NOT BUILD_SHARED_LIBS AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
            CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18"
    )
        message(FATAL_ERROR "MinGW Clang <18 static build is not supported, it has \
problems with inline constants :/.")
    endif()

    if(MINGW AND BUILD_SHARED_LIBS AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
            INLINE_CONSTANTS AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18"
    )
        message(FATAL_ERROR "MinGW Clang <18 shared build crashes with inline constants, \
don't enable the INLINE_CONSTANTS cmake option :/.")
    endif()

    if(TINY_VCPKG AND TINY_IS_MULTI_CONFIG)
        message(FATAL_ERROR "Multi-configuration generators are not supported in vcpkg \
ports.")
    endif()

    if(TINY_VCPKG AND TINY_BUILD_LOADABLE_DRIVERS)
        message(FATAL_ERROR "Loadable SQL drivers are not supported in vcpkg ports.")
    endif()

    if(BUILD_DRIVERS AND NOT BUILD_MYSQL_DRIVER)
        message(FATAL_ERROR "If the BUILD_DRIVERS option is enabled, at least one \
driver implementation must be enabled, please enable BUILD_MYSQL_DRIVER.")
    endif()

endfunction()
