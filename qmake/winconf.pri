# WinApi
# ---

# All have to be defined because of checks at the beginning of the qt_windows.h
# Windows 10 1903 "19H1" - 0x0A000007
DEFINES += WINVER=_WIN32_WINNT_WIN10
DEFINES += NTDDI_VERSION=NTDDI_WIN10_19H1
DEFINES += _WIN32_WINNT=_WIN32_WINNT_WIN10
# Internet Explorer 11
DEFINES += _WIN32_IE=_WIN32_IE_IE110
# Exclude unneeded header files
DEFINES += WIN32_LEAN_AND_MEAN
DEFINES += NOMINMAX

# Latest qmake's msvc fixes
# ---
win32-msvc {
    greaterThan(QMAKE_MSC_VER, 1909) {
        QMAKE_CXXFLAGS     *= -permissive-
        QMAKE_CXXFLAGS     -= -Zc:referenceBinding
    }

    greaterThan(QMAKE_MSC_VER, 1919): \
        QMAKE_CXXFLAGS     *= -Zc:externConstexpr
}

win32-msvc|win32-clang-msvc {
    greaterThan(QMAKE_MSC_VER, 1927) {
        # Visual Studio 2019 (16.8 or 16.9) / Visual C++ 19.28 and up
        MSVC_VER            = 16.8
        QMAKE_CFLAGS_C11    = /std:c11
        QMAKE_CFLAGS_C17    = /std:c17
    }

    greaterThan(QMAKE_MSC_VER, 1928) {
        # Visual Studio 2019 (16.10 or 16.11) / Visual C++ 19.29 and up
        MSVC_VER            = 16.10

        # -std:c++20 compiler option for Visual Studio 2019 16.11.0 and up
        greaterThan(QMAKE_MSC_FULL_VER, 192930132): \
            QMAKE_CXXFLAGS_CXX2A = -std:c++20
    }

    greaterThan(QMAKE_MSC_VER, 1929): \
        # Visual Studio 2022 (17.0) / Visual C++ 19.30 and up
        MSVC_VER            = 17.0

    greaterThan(QMAKE_MSC_VER, 1938) {
        # Visual Studio 2022 (17.9) / Visual C++ 19.39 and up
        MSVC_VER            = 17.9
        MSVC_TOOLSET_VER    = 143
    }

    greaterThan(QMAKE_MSC_VER, 1939) {
        # Visual Studio 2022 (17.10) / Visual C++ 19.40 and up
        MSVC_VER            = 17.10
        MSVC_TOOLSET_VER    = 144
    }
}

# Compiler and Linker options
# ---

win32-msvc {
    QMAKE_CXXFLAGS += -guard:cf -bigobj
    QMAKE_CXXFLAGS_DEBUG += -RTC1 -sdl
    QMAKE_CXXFLAGS_WARN_ON = -external:anglebrackets -external:W0 -WX -W4 -wd4702
                             # Enable and check it from time to time
#                             -external:templates-
}

# clang-cl.exe notes:
# /RTC    - https://lists.llvm.org/pipermail/cfe-commits/Week-of-Mon-20130902/088105.html
# /bigobj - clang-cl uses it by default - https://reviews.llvm.org/D12981
win32-clang-msvc: \
    QMAKE_CXXFLAGS_WARN_ON = -WX -W4

win32-msvc|win32-clang-msvc {
    # I don't use -MP flag because using the jom
    QMAKE_LFLAGS += /guard:cf /WX
    # Looks like clang-cl does know nothing about these, for now enabling
    QMAKE_LFLAGS_RELEASE += /OPT:REF,ICF=5
}

win32-clang-g++ {
    # -mthreads is unused on Clang
    QMAKE_CXXFLAGS_EXCEPTIONS_ON -= -mthreads
    QMAKE_LFLAGS_EXCEPTIONS_ON -= -mthreads
}
