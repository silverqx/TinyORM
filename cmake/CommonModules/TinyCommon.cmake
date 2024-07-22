# Set common variables and create interface-only library target so all other targets
# will be able to link to, either directly or transitively, to consume common compile
# options/definitions
function(tiny_common target)

    set(options EXPORT)
    set(oneValueArgs NAMESPACE NAME)
    cmake_parse_arguments(PARSE_ARGV 1 TINY "${options}" "${oneValueArgs}" "")

    if(DEFINED TINY_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "The ${CMAKE_CURRENT_FUNCTION}() was passed extra arguments: \
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

    # Disable deprecated APIs up to the given Qt version
    # Disable all the APIs deprecated up to Qt v6.9.0 (including)
    # Must be disabled with vcpkg because the qtbase port doesn't define it and it causes
    # incompatible API, eg. QByteArray::isNull() or QString::toLongLong() is defined
    # inline through this QT_CORE_INLINE_SINCE() deprecated macros and we end up
    # with multiple defined symbols, what means QtCore is compiled with old API without
    # inline because it doesn't define this QT_DISABLE_DEPRECATED_UP_TO and TinyORM has
    # inlined symbols. It of course works with shared DLL builds but fails with static/-md
    # triplets (builds). It's a good idea to use the same setting for this like qtbase
    # has anyway.
    # See: https://bugreports.qt.io/browse/QTBUG-127070
    if(NOT TINY_VCPKG)
        target_compile_definitions(${target} INTERFACE
            QT_DISABLE_DEPRECATED_UP_TO=0x060900
        )
    endif()

    target_compile_definitions(${target}
        INTERFACE
            #QT_NO_DEPRECATED_WARNINGS
            #QT_WARN_DEPRECATED_UP_TO=0x060900

            #QT_ASCII_CAST_WARNINGS
            #QT_NO_CAST_FROM_ASCII
            #QT_RESTRICTED_CAST_FROM_ASCII
            QT_NO_CAST_TO_ASCII
            QT_NO_CAST_FROM_BYTEARRAY
            # Don't define globally, it's better for compatibility, instead use % when needed
            #QT_USE_QSTRINGBUILDER
            QT_STRICT_ITERATORS
            QT_NO_KEYWORDS

            # Disable debug output in Release mode
            $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
    )

    # Platform specific configurations
    # ---

    # WinApi
    # For orientation in these versions, see:
    # https://developer.microsoft.com/en-us/windows/downloads/sdk-archive/
    # https://microsoft.fandom.com/wiki/List_of_Windows_codenames
    # https://en.wikipedia.org/wiki/Windows_11_version_history
    # https://en.wikipedia.org/wiki/Microsoft_Windows_SDK

    # The ideal case would be not to define these and rely on what is defined
    # in <qt_/windows.h> but Qt uses too old values for these, eg. MSYS2 patches these and
    # uses the latest versions, so we have to define these manually because the original
    # Qt code doesn't maintain these correctly.
    # All have to be defined because of checks at the beginning of <qt_windows.h> (fixed)
    # WINVER, _WIN32_WINNT, NTDDI_VERSION
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        # MSYS2 Qt 6 already defines these macros in the Qt6Targets.cmake Qt6::Platform
        # which is included in Package Config file. These C macros comes
        # from the QtBaseConfigureTests.cmake#qt_internal_ensure_latest_win_nt_api(),
        # this functions is doing compile test using the check_cxx_source_compiles(),
        # it includes the <windows.h> and checks whether these C macros are defined and
        # if they are not then it appends them to the QT_PLATFORM_DEFINITIONS, then
        # the QT_PLATFORM_DEFINITIONS is set for the Qt6::Platform interface library.
        # So these C macros are set on MSVC (in <sdkddkver.h> is advanced guess logic)
        # and are not set on mingw-w64/MSYS2 in the <windows.h> file.
        # What means we can't set them on MSYS2 because it throws [-Wmacro-redefined].
        # Flipped expression of : if(MINGW AND QT_VERSION_MAJOR GREATER_EQUAL 6)
        if(NOT MINGW OR NOT QT_VERSION_MAJOR GREATER_EQUAL 6)
            target_compile_definitions(${target} INTERFACE
                # Windows 11 "22H2" - 0x0A00000C
                WINVER=_WIN32_WINNT_WIN10
                _WIN32_WINNT=_WIN32_WINNT_WIN10
            )
        endif()

        target_compile_definitions(${target} INTERFACE
            # Windows 11 "22H2" - 0x0A00000C
            NTDDI_VERSION=NTDDI_WIN10_NI
            # Internet Explorer 11
            _WIN32_IE=_WIN32_IE_IE110
            # Exclude unneeded header files
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            # Others
            UNICODE _UNICODE
        )
    endif()

    # Compiler and Linker options
    # ---

    # clang-cl.exe notes:
    # /RTC    - https://lists.llvm.org/pipermail/cfe-commits/Week-of-Mon-20130902/088105.html
    # /bigobj - Clang-cl uses it by default - https://reviews.llvm.org/D12981
    if(MSVC)
        # Common for MSVC and Clang-cl
        target_compile_options(${target} INTERFACE
            # Suppress banner and info messages
            /nologo
            # Is safer to provide this explicitly, qmake do it for MSVC too
            /EHsc
            /utf-8
            # Has to be enabled explicitly
            # https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
            /Zc:__cplusplus
            # Standards-conforming behavior
            /Zc:strictStrings
        )

        # Abort compiling on warnings for Debug builds only (excluding vcpkg),
        # Release and vcpkg builds must go on as far as possible
        if(NOT TINY_VCPKG)
            target_compile_options(${target} INTERFACE
                /W4
                # Enable Additional Security Checks for Debug builds only
                $<$<CONFIG:Debug>:/WX /sdl>
            )
        endif()

        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            target_compile_options(${target} INTERFACE
                # Set by default by c++20 but from VS 16.11, can be removed when
                # minMsvcReqVersion will be >= 16.11
                /permissive-
                # Clang-cl 16 throws -Wunused-command-line-argument, so provide it
                # only for the MSVC
                /guard:cf
                /bigobj
                # Standards-conforming behavior
                /Zc:wchar_t,rvalueCast,inline
                /Zc:throwingNew,referenceBinding,ternary
                # C/C++ conformant preprocessor
                /Zc:preprocessor
                /external:anglebrackets /external:W0
                # Enable and check it from time to time
#                /external:templates-
                /wd4702
            )

            # TODO cmake this will not work with clang-cl problem is that CMAKE_CXX_SIMULATE_VERSION reports only short version like 19.40, so I can't fix it easily silverqx
            # Needed to suppress this because lot of new warnings on latest MSVC, also doesn't
            # matter too much because TinyORM compiles even without it with Qt v6, warnings were
            # only on Qt v5
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.38.32914.95")
                target_compile_definitions(${target} INTERFACE
                    _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
                )
            endif()
        endif()

        target_link_options(${target} INTERFACE
            /guard:cf
            $<$<NOT:$<CONFIG:Debug>>:/OPT:REF,ICF=5>
            # /OPT:REF,ICF does not support incremental linking
            $<$<CONFIG:RelWithDebInfo>:/INCREMENTAL:NO>
            # Abort linking on warnings for Debug builds only, Release builds must go on
            # as far as possible
            $<$<CONFIG:Debug>:/WX>
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
            # Use faster linker ( GNU ld linker doesn't work with the Clang;
            # for both GCC and Clang )
            # TODO use LINKER_TYPE target property when min. version will be CMake v3.29 silverqx
            -fuse-ld=lld
        )
    endif()

    if(NOT MSVC AND
            (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
                CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
                CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    )
        # Abort compiling on warnings for Debug builds only (excluding vcpkg),
        # Release and vcpkg builds must go on as far as possible
        if(NOT TINY_VCPKG)
            target_compile_options(${target} INTERFACE
                $<$<CONFIG:Debug>:-Werror -Wfatal-errors -pedantic-errors>
            )
        endif()

        target_compile_options(${target} INTERFACE
            # -fexceptions for Linux is not needed, it is on by default
            -Wall
            -Wextra
            # Weffc++ is outdated, it warnings about bullshits 🤬, even word about this
            # in docs: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=110186
            # -Weffc++
            # CMake already defines it
            # -Winvalid-pch
            -pedantic
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
            -Wdeprecated-copy-dtor
            # Reduce I/O operations (use pipes between commands when possible)
            -pipe
        )

        # Clang 12 still doesn't support -Wstrict-null-sentinel
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag(-Wstrict-null-sentinel SNS_SUPPORT)
        if(SNS_SUPPORT)
            target_compile_options(${target} INTERFACE -Wstrict-null-sentinel)
        endif()

        # Has the potential to catch weird code
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(${target} INTERFACE -Wdeprecated)
        endif()
    endif()

    # Use faster lld linker on Clang (target the Clang except Clang-cl with MSVC)
    # Don't set for MINGW to avoid duplicate setting (look a few lines above)
    # TODO use LINKER_TYPE target property when min. version will be CMake v3.29 silverqx
    if(NOT MINGW AND NOT MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_link_options(${target} INTERFACE -fuse-ld=lld)
    endif()

    # Use 64-bit off_t on 32-bit Linux, ensure 64bit offsets are used for filesystem
    # accesses for 32bit compilation
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_compile_definitions(${target} INTERFACE -D_FILE_OFFSET_BITS=64)
    endif()

endfunction()
