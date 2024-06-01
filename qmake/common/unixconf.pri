# Compiler and Linker options
# ---

# Abort compiling on warnings for Debug builds only, Release builds must go on as far as possible
CONFIG(debug, debug|release): \
    QMAKE_CXXFLAGS_WARN_ON *= -Werror -Wfatal-errors -pedantic-errors

QMAKE_CXXFLAGS_WARN_ON *= \
    -Wall \
    -Wextra \
    # Weffc++ is outdated, it warnings about bullshits 🤬, even word about this in docs:
    # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=110186
    # -Weffc++ \
    -pedantic \
    -Winvalid-pch \
    -Wcast-qual \
    -Wcast-align \
    -Woverloaded-virtual \
    -Wold-style-cast \
    -Wshadow \
    -Wundef \
    -Wfloat-equal \
    -Wformat-security \
    -Wdouble-promotion \
    -Wconversion \
    -Wzero-as-null-pointer-constant \
    -Wuninitialized \
    -Wdeprecated-copy-dtor \

clang {
    # Has the potential to catch weird code
    QMAKE_CXXFLAGS_WARN_ON *= -Wdeprecated

    # The -fno-pch-timestamp option is needed for Clang with ccache, this option will be
    # used even when the ccache isn't used because is very hard to make 100% logic that
    # will detect the ccache with qmake but it doesn't interfere with anything so it's ok
    precompile_header {
        # qmake on MSYS2 ignores the QMAKE_CXXFLAGS_PRECOMPILE variable
        mingw: \
            QMAKE_CXXFLAGS = -Xclang -fno-pch-timestamp $$QMAKE_CXXFLAGS

        else: \
            QMAKE_CXXFLAGS_PRECOMPILE = -Xclang -fno-pch-timestamp \
                $$QMAKE_CXXFLAGS_PRECOMPILE
    }
}

# Clang 12 still doesn't support -Wstrict-null-sentinel
!clang: QMAKE_CXXFLAGS_WARN_ON *= -Wstrict-null-sentinel

# Use 64-bit off_t on 32-bit Linux, ensure 64bit offsets are used for filesystem
# accesses for 32bit compilation
linux:contains(QT_ARCH, x86_64): \
    DEFINES *= _FILE_OFFSET_BITS=64

# Allow to enable UBSan with Clang
clang:ubsan {
    QMAKE_CXXFLAGS += -O1

    QMAKE_CXXFLAGS *= \
        -fsanitize=undefined \
        -fsanitize=nullability \
        -fsanitize=float-divide-by-zero \
        -fsanitize=unsigned-integer-overflow \
        -fsanitize=implicit-conversion \
        -fsanitize=local-bounds \

    QMAKE_LFLAGS *= \
        -fsanitize=undefined \
        -fsanitize=nullability \
        -fsanitize=float-divide-by-zero \
        -fsanitize=unsigned-integer-overflow \
        -fsanitize=implicit-conversion \
        -fsanitize=local-bounds
}

# Optimized for MSYS2, at 100% will not work with mingw-w64, it doesn't contain the lld
# linker and has outdated ld linker (bfd v2.40) that is buggy and is missing a lot of
# new fixes that are already merged in >v2.42.
mingw {
    # Confirm that we want a type with an extended alignment
    DEFINES *= _ENABLE_EXTENDED_ALIGNED_STORAGE

    # Disable std::type_info inline comparison because it throws duplicate
    # std::type_info::operator==() for static builds/linkage.
    # Related issue: https://github.com/msys2/MINGW-packages/issues/17730
    CONFIG(static, dll|shared|static|staticlib) | \
    CONFIG(staticlib, dll|shared|static|staticlib): \
        DEFINES *= __GXX_TYPEINFO_EQUALITY_INLINE=0

    # Reduce I/O operations (use pipes between commands when possible)
    gcc|clang: QMAKE_CXXFLAGS *= -pipe

    # Set the hidden symbol visibility for all global definitions and inline C++ member
    # functions, the CONFIG+=hide_symbols doesn't work on mingw.
    gcc|clang {
        QMAKE_CFLAGS   *= -fvisibility=hidden
        QMAKE_CXXFLAGS *= -fvisibility=hidden
    }
    clang: \
        QMAKE_CXXFLAGS *= -fvisibility-inlines-hidden

    # Target lld linker
    contains(QMAKE_LFLAGS, "-fuse-ld=lld") {
        # Enable colors in diagnostics and use ANSI escape codes for diagnostics
        llvm: QMAKE_LFLAGS *= -fansi-escape-codes -fcolor-diagnostics

        # Enable all possible security flags and features
        QMAKE_LFLAGS += -Xlinker --dynamicbase
        QMAKE_LFLAGS += -Xlinker --high-entropy-va
        QMAKE_LFLAGS += -Xlinker --nxcompat
        QMAKE_LFLAGS += -Xlinker --default-image-base-high

        # The QMAKE_LFLAGS_CONSOLE -= -Wl,-subsystem,console is correct, lld recognizes
        # this correctly, but eg. cmake isn't using this linker parameter, the reason
        # is that the console subsystem is the default value.
        # I could use QMAKE_LFLAGS_CONSOLE = to reset it but I leave it as it's now. 😁
    }

    # Don't use QMAKE_COMPILER_DEFINES, CMake is adding both as -DWIN64 on compiler line,
    # so do it the same way, if it's good for CMake then it's also good for us.
    contains(QT_ARCH, x86_64): \
        DEFINES *= WIN64 _WIN64
}
