# Set common variables and create interface-only library target so all other targets
# will be able to link to, either directly or transitively, to consume common compile
# options/definitions
function(tiny_common target)

    set(options EXPORT)
    set(oneValueArgs NAMESPACE NAME)
    cmake_parse_arguments(PARSE_ARGV 1 TINY "${options}" "${oneValueArgs}" "")

    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} was passed extra arguments: \
${TINY_UNPARSED_ARGUMENTS}")
    endif()

    add_library(${target} INTERFACE)
    add_library(${TINY_NAMESPACE}::${TINY_NAME} ALIAS ${target})

    if(TINY_EXPORT)
        set_target_properties(${target} PROPERTIES EXPORT_NAME ${TINY_NAME})
    endif()

    # Full C++ 20 support is required
    target_compile_features(${target} INTERFACE cxx_std_20)

    # Qt defines
    # ---

    target_compile_definitions(${target} INTERFACE
        # You can also make your code fail to compile if it uses deprecated APIs.
        # In order to do so, uncomment the following line.
        # You can also select to disable deprecated APIs only up to a certain version of Qt.
        # Disables all the APIs deprecated before Qt 6.0.0
        QT_DISABLE_DEPRECATED_BEFORE=0x060000

        #QT_ASCII_CAST_WARNINGS
        #QT_NO_CAST_FROM_ASCII
        #QT_RESTRICTED_CAST_FROM_ASCII
        QT_NO_CAST_TO_ASCII
        QT_NO_CAST_FROM_BYTEARRAY
        QT_USE_QSTRINGBUILDER
        QT_STRICT_ITERATORS
    )

    # Platform specific configurations
    # ---

    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_compile_definitions(${target} INTERFACE
            # All have to be defined because of checks at the beginning of <qt_windows.h>
            # Windows 10 1903 "19H1" - 0x0A000007
            WINVER=_WIN32_WINNT_WIN10
            NTDDI_VERSION=NTDDI_WIN10_19H1
            _WIN32_WINNT=_WIN32_WINNT_WIN10
            # Internet Explorer 11
            _WIN32_IE=_WIN32_IE_IE110
            UNICODE _UNICODE
            # Exclude unneeded header files
            WIN32_LEAN_AND_MEAN
            NOMINMAX
        )
    endif()

    # Compiler and Linker options
    # ---

    if(MSVC)
        target_compile_options(${target} INTERFACE
            # Suppress banner and info messages
            /nologo
            # Is safer to provide this explicitly, qmake do it for msvc too
            /EHsc
            /guard:cf
            /utf-8
            # Set by default by c++20 but from VS 16.11, can be removed when
            # minMsvcReqVersion will be >= 16.11
            /permissive-
            /bigobj
            # Has to be enabled explicitly
            # https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
            /Zc:__cplusplus
            # Standards-conforming behavior
            /Zc:wchar_t,rvalueCast,inline,strictStrings
            /Zc:throwingNew,referenceBinding,ternary
            /external:anglebrackets /external:W0
            /WX /W4 /wd4702
        )

        target_link_options(${target} INTERFACE
            /guard:cf
            $<$<NOT:$<CONFIG:Debug>>:/OPT:REF,ICF=5>
            # /OPT:REF,ICF does not support incremental linking
            $<$<CONFIG:RelWithDebInfo>:/INCREMENTAL:NO>
            /WX
        )
    endif()

    if(MINGW)
        target_compile_options(${target} INTERFACE
            $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-ignored-attributes>
        )

        target_link_options(${target} INTERFACE
            $<$<CONFIG:Debug,RelWithDebInfo>:
                LINKER:--dynamicbase,--high-entropy-va,--nxcompat
                LINKER:--default-image-base-high>
            # Use faster linker ( GNU ld linker doesn't work with the Clang )
            LINKER:-fuse-ld=lld
        )
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
            OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
            OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"
    )
        target_compile_options(${target} INTERFACE
            # -fexceptions for linux is not needed, it is on by default
            -Wall
            -Wextra
            -Weffc++
            -Werror
            -Wfatal-errors
            -Wcast-qual
            -Wcast-align
            -Woverloaded-virtual
            -Wold-style-cast
            -Wshadow
            -Wundef
            -Wfloat-equal
            -Wformat-security
            -Wdouble-promotion
            -Wconversion
            -Wzero-as-null-pointer-constant
            -Wuninitialized
            -pedantic
            -pedantic-errors
            # Reduce I/O operations
            -pipe
        )

        # Clang 12 still doesn't support -Wstrict-null-sentinel
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag(-Wstrict-null-sentinel SNS_SUPPORT)
        if(SNS_SUPPORT)
            target_compile_options(${target} INTERFACE -Wstrict-null-sentinel)
        endif()
    endif()

    # Use 64-bit off_t on 32-bit Linux, ensure 64bit offsets are used for filesystem
    # accesses for 32bit compilation
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_compile_definitions(${target} INTERFACE -D_FILE_OFFSET_BITS=64)
    endif()

endfunction()
